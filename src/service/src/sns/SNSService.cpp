//
// Created by vogje01 on 30/05/2023.
//

#include "awsmock/service/sns/SNSService.h"

namespace AwsMock::Service {

    SNSService::SNSService(Core::Configuration &configuration) : _configuration(configuration), _snsDatabase(Database::SNSDatabase::instance()), _sqsDatabase(Database::SQSDatabase::instance()) {

        // Initialize environment
        _sqsService = std::make_unique<SQSService>(_configuration);
        _accountId = _configuration.getString("awsmock.account.userPoolId", DEFAULT_SQS_ACCOUNT_ID);
    }

    Dto::SNS::CreateTopicResponse SNSService::CreateTopic(const Dto::SNS::CreateTopicRequest &request) {
        log_trace << "Create topic request: " << request.ToString();

        // Check existence
        if (_snsDatabase.TopicExists(request.region, request.topicName)) {
            log_warning << "SNS topic '" + request.topicName + "' exists already";
            Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByName(request.region, request.topicName);
            log_debug << "Got topic: " << topic.topicArn;
            return {
                    .region = topic.region,
                    .name = topic.topicName,
                    .owner = topic.owner,
                    .topicArn = topic.topicArn};
        }

        try {
            // Update database
            std::string topicArn = Core::AwsUtils::CreateSNSTopicArn(request.region, _accountId, request.topicName);
            Database::Entity::SNS::Topic topic = _snsDatabase.CreateTopic({.region = request.region, .topicName = request.topicName, .owner = request.owner, .topicArn = topicArn});
            log_trace << "SNS topic created: " << topic.ToString();

            return {.region = topic.region, .name = topic.topicName, .owner = topic.owner, .topicArn = topic.topicArn};

        } catch (Core::DatabaseException &exc) {
            log_error << "SNS create topic failed, message: " << exc.message();
            throw Core::ServiceException(exc.message(), 400);
        }
    }

    Dto::SNS::ListTopicsResponse SNSService::ListTopics(const std::string &region) {
        log_trace << "List all topics request, region: " << region;

        try {

            Database::Entity::SNS::TopicList topicList = _snsDatabase.ListTopics(region);
            auto listTopicsResponse = Dto::SNS::ListTopicsResponse(topicList);
            log_trace << "SNS list topics response: " << listTopicsResponse.ToXml();

            return listTopicsResponse;

        } catch (Poco::Exception &ex) {
            log_error << "SNS list topics request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message(), Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR, "SQS", Poco::UUIDGenerator().createRandom().toString().c_str());
        }
    }

    Dto::SNS::DeleteTopicResponse SNSService::DeleteTopic(const std::string &region, const std::string &topicArn) {
        log_trace << "Delete topic request, region: " << region << " topicArn: " << topicArn;

        Dto::SNS::DeleteTopicResponse response;
        try {

            // Check existence
            if (!_snsDatabase.TopicExists(topicArn)) {
                log_warning << "Topic does not exist, arn: " << topicArn;
                throw Core::ServiceException("Topic does not exist", Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            }

            // Update database
            _snsDatabase.DeleteTopic({.region = region, .topicArn = topicArn});

        } catch (Poco::Exception &ex) {
            log_error << "SNS delete topic failed, message: " << ex.message();
            throw Core::ServiceException(ex.message(), Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        }
        return response;
    }

    Dto::SNS::PublishResponse SNSService::Publish(const Dto::SNS::PublishRequest &request) {

        // Check topic/target ARN
        if (request.topicArn.empty()) {
            log_error << "Either topicARN or targetArn must exist";
            throw Core::ServiceException("Either topicARN or targetArn must exist", Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        }

        // Check existence
        if (!_snsDatabase.TopicExists(request.topicArn)) {
            log_error << "Topic does not exist: " << request.topicArn;
            throw Core::ServiceException("SNS topic does not exists", Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        }

        Database::Entity::SNS::Message message;
        try {

            // Update database
            std::string messageId = Core::AwsUtils::CreateMessageId();
            message = _snsDatabase.CreateMessage({.region = request.region,
                                                  .topicArn = request.topicArn,
                                                  .targetArn = request.targetArn,
                                                  .message = request.message,
                                                  .messageId = messageId});

            // Check subscriptions
            CheckSubscriptions(request);

            return {.messageId = message.messageId};

        } catch (Poco::Exception &ex) {
            log_error << "SNS create message failed, message: " << ex.message();
            throw Core::ServiceException(ex.message(), Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        }
    }

    Dto::SNS::SubscribeResponse SNSService::Subscribe(const Dto::SNS::SubscribeRequest &request) {

        try {

            // Check topic/target ARN
            if (request.topicArn.empty()) {
                throw Core::ServiceException("Topic ARN missing", Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            }

            // Check existence
            if (!_snsDatabase.TopicExists(request.topicArn)) {
                throw Core::ServiceException("SNS topic does not exists", Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            }

            // Create new subscription
            Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn);
            std::string subscriptionArn = Core::AwsUtils::CreateSNSSubscriptionArn(request.region, _accountId, topic.topicName);

            Database::Entity::SNS::Subscription subscription = {.protocol = request.protocol, .endpoint = request.endpoint};
            if (!topic.HasSubscription(subscription)) {

                // Add subscription
                topic.subscriptions.push_back({.protocol = request.protocol,
                                               .endpoint = request.endpoint,
                                               .subscriptionArn = subscriptionArn});

                // Save to database
                topic = _snsDatabase.UpdateTopic(topic);
                log_debug << "Subscription added, topic: " << topic.ToString();
            }

            return {.subscriptionArn = subscriptionArn};

        } catch (Poco::Exception &ex) {
            log_error << "SNS subscription failed, message: " << ex.message();
            throw Core::ServiceException(ex.message(), Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        }
    }

    Dto::SNS::UnsubscribeResponse SNSService::Unsubscribe(const Dto::SNS::UnsubscribeRequest &request) {

        try {

            // Check topic/target ARN
            if (request.subscriptionArn.empty()) {
                throw Core::ServiceException("Subscription ARN missing", Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
            }

            // Create new subscription
            Database::Entity::SNS::TopicList topics = _snsDatabase.GetTopicsBySubscriptionArn(request.subscriptionArn);

            for (auto &topic: topics) {

                // Add subscription
                topic.subscriptions.erase(std::remove_if(topic.subscriptions.begin(), topic.subscriptions.end(), [&request](const auto &item) {
                                              return item.subscriptionArn == request.subscriptionArn;
                                          }),
                                          end(topic.subscriptions));

                // Save to database
                topic = _snsDatabase.UpdateTopic(topic);
                log_debug << "Subscription added, topic: " << topic.ToString();
            }

            return {.subscriptionArn = request.subscriptionArn};

        } catch (Poco::Exception &ex) {
            log_error << "SNS subscription failed, message: " << ex.message();
            throw Core::ServiceException(ex.message(), Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        }
    }

    Dto::SNS::GetTopicAttributesResponse SNSService::GetTopicAttributes(const Dto::SNS::GetTopicAttributesRequest &request) {

        try {

            // Check existence
            if (!_snsDatabase.TopicExists(request.topicArn)) {
                throw Core::ServiceException("SNS topic does not exists", Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            }

            Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn);
            return {
                    .region = topic.region,
                    .topicArn = topic.topicArn,
                    .owner = topic.owner};

        } catch (Poco::Exception &ex) {
            log_error << "SNS get topic attributes failed, message: " << ex.message();
            throw Core::ServiceException(ex.message(), Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        }
    }

    Dto::SNS::ListSubscriptionsByTopicResponse SNSService::ListSubscriptionsByTopic(const Dto::SNS::ListSubscriptionsByTopicRequest &request) {

        try {

            // Check existence
            if (!_snsDatabase.TopicExists(request.topicArn)) {
                throw Core::ServiceException("SNS topic does not exists", Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            }

            Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn);

            Dto::SNS::ListSubscriptionsByTopicResponse response;
            for (const auto &s: topic.subscriptions) {
                Dto::SNS::Subscription subscription = {.topicArn = request.topicArn, .protocol = s.protocol, .subscriptionArn = s.subscriptionArn, .endpoint = s.endpoint};
                response.subscriptions.emplace_back(subscription);
            }
            return response;

        } catch (Poco::Exception &ex) {
            log_error << "SNS get topic subscriptions failed, message: " << ex.message();
            throw Core::ServiceException(ex.message(), Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        }
    }

    Dto::SNS::TagResourceResponse SNSService::TagResource(const Dto::SNS::TagResourceRequest &request) {

        try {

            // Check existence
            if (!_snsDatabase.TopicExists(request.resourceArn)) {
                throw Core::ServiceException("SNS topic does not exists", Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            }

            // Get the topic
            Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.resourceArn);

            // Set tags and update database
            topic.tags = request.tags;
            _snsDatabase.UpdateTopic(topic);
            log_info << "SNS tags updated, count: " << request.tags.size();

            return {};

        } catch (Poco::Exception &ex) {
            log_error << "SNS tag resource failed, message: " << ex.message();
            throw Core::ServiceException(ex.message(), Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        }
    }

    void SNSService::CheckSubscriptions(const Dto::SNS::PublishRequest &request) {

        Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.topicArn);
        if (!topic.subscriptions.empty()) {

            for (const auto &it: topic.subscriptions) {

                if (it.protocol == SQS_PROTOCOL) {

                    SendSQSMessage(it, request);
                    log_debug << "Message send to SQS queue, queueArn: " << it.endpoint;
                }
            }
        }
    }

    void SNSService::SendSQSMessage(const Database::Entity::SNS::Subscription &subscription, const Dto::SNS::PublishRequest &request) {

        // Get queue URL
        Database::Entity::SQS::Queue sqsQueue = _sqsDatabase.GetQueueByArn(subscription.endpoint);

        // Create a SQS notification request
        AwsMock::Dto::SNS::SqsNotificationRequest sqsNotificationRequest = {
                .type = "Notification",
                .messageId = Core::AwsUtils::CreateMessageId(),
                .topicArn = request.topicArn,
                .message = request.message,
                .timestamp = Poco::Timestamp().epochMicroseconds() / 1000};

        // Wrap it in a SQS message request
        Dto::SQS::SendMessageRequest sendMessageRequest = {
                .region = request.region,
                .queueUrl = sqsQueue.queueUrl,
                .queueArn = sqsQueue.queueArn,
                .body = sqsNotificationRequest.ToJson(),
                .messageId = Core::AwsUtils::CreateMessageId(),
                .requestId = Core::AwsUtils::CreateRequestId(),
        };

        _sqsService->SendMessage(sendMessageRequest);
    }

}// namespace AwsMock::Service
