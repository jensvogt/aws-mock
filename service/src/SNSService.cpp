//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/SNSService.h>

namespace AwsMock::Service {

    using namespace Poco::Data::Keywords;

    SNSService::SNSService(const Core::Configuration &configuration) : _logger(Poco::Logger::get("SNSService")), _configuration(configuration) {
        Initialize();
    }

    void SNSService::Initialize() {

        // Set console logger
        Core::Logger::SetDefaultConsoleLogger("SNSService");

        // Initialize environment
        _snsDatabase = std::make_unique<Database::SNSDatabase>(_configuration);
        _sqsDatabase = std::make_unique<Database::SQSDatabase>(_configuration);
        _sqsService = std::make_unique<SQSService>(_configuration);
        _accountId = Core::AwsUtils::GetDefaultAccountId();
    }

    Dto::SNS::CreateTopicResponse SNSService::CreateTopic(const Dto::SNS::CreateTopicRequest &request) {
        _logger.trace() << "Create topic request: " << request.ToString() << std::endl;

        Database::Entity::SNS::Topic topic;
        try {
            // Check existence
            if (_snsDatabase->TopicExists(request.region, request.topicName)) {
                throw Core::ServiceException("SQS Queue exists already", 500);
            }

            // Update database
            std::string topicArn = Core::AwsUtils::CreateSNSTopicArn(request.region, _accountId, request.topicName);
            topic = _snsDatabase->CreateTopic({.region=request.region, .topicName=request.topicName, .owner=request.owner, .topicArn=topicArn});
            _logger.trace() << "SNS topic created: " << topic.ToString() << std::endl;

        } catch (Poco::Exception &exc) {
            _logger.error() << "SNS create topic failed, message: " << exc.message() << std::endl;
            throw Core::ServiceException(exc.message(), 500);
        }
        return {.region=topic.region, .name=topic.topicName, .owner=topic.owner, .topicArn=topic.topicArn};
    }

    Dto::SNS::ListTopicsResponse SNSService::ListTopics(const std::string &region) {
        _logger.trace() << "List all topics request, region: " << region << std::endl;

        try {

            Database::Entity::SNS::TopicList topicList = _snsDatabase->ListTopics(region);
            auto listTopicsResponse = Dto::SNS::ListTopicsResponse(topicList);
            _logger.trace() << "SNS list topics response: " << listTopicsResponse.ToXml() << std::endl;

            return listTopicsResponse;

        } catch (Poco::Exception &ex) {
            _logger.error() << "SNS list topics request failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500, "SQS", Poco::UUIDGenerator().createRandom().toString().c_str());
        }
    }

    Dto::SNS::DeleteTopicResponse SNSService::DeleteTopic(const std::string &region, const std::string &topicArn) {
        _logger.trace() << "Delete topic request, region: " << region << " topicArn: " << topicArn << std::endl;

        Dto::SNS::DeleteTopicResponse response;
        try {
            // Check existence
            if (!_snsDatabase->TopicExists(topicArn)) {
                throw Core::ServiceException("Topic does not exist", 500);
            }

            // Update database
            _snsDatabase->DeleteTopic({.region=region, .topicArn=topicArn});

        } catch (Poco::Exception &ex) {
            _logger.error() << "SNS delete topic failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500);
        }
        return response;
    }

    Dto::SNS::PublishResponse SNSService::Publish(const Dto::SNS::PublishRequest &request) {

        Database::Entity::SNS::Message message;
        try {
            // Check topic/target ARN
            if (request.topicArn.empty() && request.targetArn.empty()) {
                throw Core::ServiceException("Either topicARN or targetArn must exist", 500);
            }

            // Check existence
            if (!_snsDatabase->TopicExists(request.topicArn)) {
                throw Core::ServiceException("SNS topic does not exists", 500);
            }

            // Update database
            std::string messageId = Core::StringUtils::GenerateRandomString(100);
            message =
                _snsDatabase->CreateMessage({.region=request.region, .topicArn=request.topicArn, .targetArn=request.targetArn, .message=request.message, .messageId=messageId});

            // Check subscriptions
            CheckSubscriptions(request);

            return {.messageId=message.messageId};

        } catch (Poco::Exception &ex) {
            _logger.error() << "SNS create message failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500);
        }
    }

    Dto::SNS::SubscribeResponse SNSService::Subscribe(const Dto::SNS::SubscribeRequest &request) {

        try {

            // Check topic/target ARN
            if (request.topicArn.empty()) {
                throw Core::ServiceException("Topic ARN missing", 500);
            }

            // Check existence
            if (!_snsDatabase->TopicExists(request.topicArn)) {
                throw Core::ServiceException("SNS topic does not exists", 500);
            }

            // Create new subscription
            Database::Entity::SNS::Topic topic = _snsDatabase->GetTopicByArn(request.topicArn);
            std::string subscriptionArn = Core::AwsUtils::CreateSNSSubscriptionArn(request.region, _accountId, topic.topicName);

            Database::Entity::SNS::Subscription subscription = {.protocol=request.protocol, .endpoint=request.endpoint};
            if (!topic.HasSubscription(subscription)) {

                // Add subscription
                topic.subscriptions.push_back({.protocol=request.protocol, .endpoint=request.endpoint});

                // Save to database
                topic = _snsDatabase->UpdateTopic(topic);
                _logger.debug() << "Subscription added, topic: " << topic.ToString() << std::endl;
            }

            return {.subscriptionArn=subscriptionArn};

        } catch (Poco::Exception &ex) {
            _logger.error() << "SNS subscription failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500);
        }
    }

    void SNSService::CheckSubscriptions(const Dto::SNS::PublishRequest &request) {

        Database::Entity::SNS::Topic topic = _snsDatabase->GetTopicByArn(request.topicArn);
        if (!topic.subscriptions.empty()) {

            for (const auto &it : topic.subscriptions) {

                if (it.protocol == SQS_PROTOCOL) {
                    SendSQSMessage(it, request.message);

                }
            }
        }
    }

    void SNSService::SendSQSMessage(const Database::Entity::SNS::Subscription &subscription, const std::string &message) {

        Database::Entity::SQS::Queue sqsQueue = _sqsDatabase->GetQueueByArn(subscription.endpoint);
        _sqsService->CreateMessage({.queueUrl = sqsQueue.queueUrl, .body=message});
    }

} // namespace AwsMock::Service
