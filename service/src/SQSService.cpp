//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/SQSService.h>

namespace AwsMock::Service {

    using namespace Poco::Data::Keywords;

    SQSService::SQSService(const Core::Configuration &configuration) : _logger(Poco::Logger::get("SQSService")), _configuration(configuration) {
        Initialize();
    }

    void SQSService::Initialize() {

        // Set console logger
        Core::Logger::SetDefaultConsoleLogger("SQSService");

        // Initialize environment
        _database = std::make_unique<Database::SQSDatabase>(_configuration);
        _accountId = DEFAULT_ACCOUNT_ID;
    }

    Dto::SQS::CreateQueueResponse SQSService::CreateQueue(const Dto::SQS::CreateQueueRequest &request) {

        // Check existence
        if (_database->QueueExists(request.region, request.name)) {
            throw Core::ServiceException("SQS Queue exists already", 400);
        }

        try {
            // Set default attributes
            std::string queueArn = Core::AwsUtils::CreateSQSQueueArn(request.region, _accountId, request.name);
            Database::Entity::SQS::QueueAttribute attribute{};

            // Update database
            Database::Entity::SQS::Queue
                queue = _database->CreateQueue({.region=request.region, .name=request.name, .owner=request.owner, .queueUrl=request.queueUrl, .queueArn=queueArn});
            log_trace_stream(_logger) << "SQS queue created: " << queue.ToString() << std::endl;

            return {.region=queue.region, .name=queue.name, .owner=queue.owner, .queueUrl=queue.queueUrl};

        } catch (Core::DatabaseException &exc) {
            log_error_stream(_logger) << "SQS create queue failed, message: " << exc.message() << std::endl;
            throw Core::ServiceException(exc.message(), 400);
        }
    }

    Dto::SQS::ListQueueResponse SQSService::ListQueues(const std::string &region) {
        log_trace_stream(_logger) << "List all queues request, region: " << region << std::endl;

        try {

            Database::Entity::SQS::QueueList queueList = _database->ListQueues(region);
            auto listQueueResponse = Dto::SQS::ListQueueResponse(queueList);
            log_trace_stream(_logger) << "SQS create queue list response: " << listQueueResponse.ToXml() << std::endl;

            return listQueueResponse;

        } catch (Poco::Exception &ex) {
            log_error_stream(_logger) << "SQS list queues failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500, "SQS", Poco::UUIDGenerator().createRandom().toString().c_str());
        }
    }

    Dto::SQS::PurgeQueueResponse SQSService::PurgeQueue(const Dto::SQS::PurgeQueueRequest &request) {
        log_trace_stream(_logger) << "Purge queue request, region: " << request.region << " queueUrl: " << request.queueUrl << std::endl;

        Dto::SQS::PurgeQueueResponse response = {.resource=request.resource, .requestId=request.requestId};
        try {
            // Check existence
            if (!_database->QueueUrlExists(request.queueUrl)) {
                throw Core::ServiceException("SQS queue does not exists", 500, request.resource.c_str(), request.requestId.c_str());
            }

            _database->PurgeQueue(request.region, request.queueUrl);
            log_trace_stream(_logger) << "SQS queue purged, region: " << request.region << " queueUrl: " << request.queueUrl << std::endl;

        } catch (Poco::Exception &ex) {
            log_error_stream(_logger) << "SQS purge queue failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500, request.resource.c_str(), request.requestId.c_str());
        }
        return response;
    }

    Dto::SQS::GetQueueAttributesResponse SQSService::GetQueueAttributes(const Dto::SQS::GetQueueAttributesRequest &request) {
        log_trace_stream(_logger) << "Get queue attributes request, request: " << request.ToString() << std::endl;

        Database::Entity::SQS::Queue queue = _database->GetQueueByUrl(request.queueUrl);
        _logger.debug() << "Got queue: " << queue.ToString() << std::endl;

        Dto::SQS::GetQueueAttributesResponse response;
        if (request.attributeNames.size() == 1 && request.attributeNames[0] == "All") {
            response.attributes.emplace_back("ApproximateNumberOfMessages", std::to_string(queue.attributes.approximateNumberOfMessages));
            response.attributes.emplace_back("ApproximateNumberOfMessagesDelayed", std::to_string(queue.attributes.approximateNumberOfMessagesDelayed));
            response.attributes.emplace_back("ApproximateNumberOfMessagesNotVisible", std::to_string(queue.attributes.approximateNumberOfMessagesNotVisible));
            response.attributes.emplace_back("CreatedTimestamp", std::to_string(queue.created.timestamp().epochTime()));
            response.attributes.emplace_back("DelaySeconds", std::to_string(queue.attributes.delaySeconds));
            response.attributes.emplace_back("LastModifiedTimestamp", std::to_string(queue.modified.timestamp().epochTime()));
            response.attributes.emplace_back("MaximumMessageSize", std::to_string(queue.attributes.maxMessageSize));
            response.attributes.emplace_back("MessageRetentionPeriod", std::to_string(queue.attributes.messageRetentionPeriod));
            response.attributes.emplace_back("Policy", queue.attributes.policy);
            response.attributes.emplace_back("QueueArn", queue.queueArn);
            response.attributes.emplace_back("ReceiveMessageWaitTimeSeconds", std::to_string(queue.attributes.receiveMessageWaitTime));
            response.attributes.emplace_back("VisibilityTimeout", std::to_string(queue.attributes.visibilityTimeout));
        }
        return response;
    }

    Dto::SQS::SetQueueAttributesResponse SQSService::SetQueueAttributes(Dto::SQS::SetQueueAttributesRequest &request) {
        log_trace_stream(_logger) << "Put queue sqs request, request: " << request.ToString() << std::endl;

        Dto::SQS::SetQueueAttributesResponse response;
        try {
            // Check existence
            if (!_database->QueueUrlExists(request.queueUrl)) {
                throw Core::ServiceException("Queue does not exist", 500);
            }

            // Get the queue
            Database::Entity::SQS::Queue queue = _database->GetQueueByUrl(request.queueUrl);
            _logger.debug() << "Got queue: " << queue.ToString() << std::endl;

            // Reset all attributes
            queue.attributes.policy = request.attributes["Policy"];
            queue.attributes.redrivePolicy.FromJson(request.attributes["RedrivePolicy"]);
            queue.attributes.redriveAllowPolicy = request.attributes["RedriveAllowPolicy"];
            queue.attributes.messageRetentionPeriod = std::stoi(request.attributes["MessageRetentionPeriod"]);
            queue.attributes.visibilityTimeout = std::stoi(request.attributes["VisibilityTimeout"]);

            // Update database
            queue = _database->UpdateQueue(queue);
            _logger.debug() << "Queue updated: " << queue.ToString() << std::endl;

        } catch (Poco::Exception &ex) {
            log_error_stream(_logger) << "SQS delete queue failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500);
        }
        return response;
    }

    Dto::SQS::DeleteQueueResponse SQSService::DeleteQueue(const Dto::SQS::DeleteQueueRequest &request) {
        log_trace_stream(_logger) << "Delete queue request, request: " << request.ToString() << std::endl;

        Dto::SQS::DeleteQueueResponse response;
        try {
            // Check existence
            if (!_database->QueueUrlExists(request.queueUrl)) {
                throw Core::ServiceException("Queue does not exist", 500);
            }

            // Delete all messages in queue
            _database->DeleteMessages(request.queueUrl);

            // Update database
            _database->DeleteQueue({.queueUrl=request.queueUrl});

        } catch (Poco::Exception &ex) {
            log_error_stream(_logger) << "SQS delete queue failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500);
        }
        return response;
    }

    Dto::SQS::CreateMessageResponse SQSService::CreateMessage(const Dto::SQS::CreateMessageRequest &request) {

        Database::Entity::SQS::Message message;
        try {
            if (!request.queueUrl.empty() && !_database->QueueUrlExists(request.queueUrl)) {
                throw Core::ServiceException("SQS queue does not exists", 500);
            } else if (!request.queueArn.empty() && !_database->QueueArnExists(request.queueArn)) {
                throw Core::ServiceException("SQS queue does not exists", 500);
            }

            // Get queue in case of ARN
            Database::Entity::SQS::Queue queue;
            if (!request.queueUrl.empty()) {
                queue = _database->GetQueueByUrl(request.queueUrl);
            } else if (!request.queueArn.empty()) {
                queue = _database->GetQueueByArn(request.queueArn);
            }

            // Update database
            std::string messageId = Core::StringUtils::GenerateRandomString(100);
            std::string receiptHandle = Core::StringUtils::GenerateRandomString(512);
            std::string md5Body = GetMd5Body(request.body);
            std::string md5Attr = GetMd5Attributes(request.messageAttributes);
            message =
                _database->CreateMessage({.region= request.region, .queueUrl=queue.queueUrl, .body=request.body, .messageId=messageId, .receiptHandle=receiptHandle,
                                             .md5Body=md5Body, .md5Attr=md5Attr});

        } catch (Poco::Exception &ex) {
            log_error_stream(_logger) << "SQS create message failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500);
        }
        return {.queueUrl=message.queueUrl, .messageId=message.messageId, .receiptHandle=message.receiptHandle, .md5Body=message.md5Body, .md5Attr=message.md5Attr};
    }

    Dto::SQS::ReceiveMessageResponse SQSService::ReceiveMessages(const Dto::SQS::ReceiveMessageRequest &request) {

        Dto::SQS::ReceiveMessageResponse response;
        try {
            Database::Entity::SQS::MessageList messageList;

            Database::Entity::SQS::Queue queue = _database->GetQueueByUrl(request.queueUrl);

            long elapsed = 0;
            auto begin = std::chrono::high_resolution_clock::now();

            while (elapsed < request.waitTimeSeconds * 1000) {

                _database->ReceiveMessages(request.region, request.queueUrl, queue.attributes.visibilityTimeout, messageList);

                if(!messageList.empty()) {
                    break;
                }

                auto end = std::chrono::high_resolution_clock::now();
                elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
                Poco::Thread::sleep(500);
            }

            if (!messageList.empty()) {
                response.messageList = messageList;
                response.requestId = Poco::UUIDGenerator().createRandom().toString();
            }

        } catch (Poco::Exception &ex) {
            log_error_stream(_logger) << "SQS create message failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500);
        }
        return {response};
    }

    Dto::SQS::DeleteMessageResponse SQSService::DeleteMessage(const Dto::SQS::DeleteMessageRequest &request) {
        log_trace_stream(_logger) << "Delete message request, url: " << request.receiptHandle << std::endl;

        Dto::SQS::DeleteMessageResponse response;
        try {
            // TODO: Check existence
            /*if (!_database->MessageExists(request.url)) {
                throw Core::ServiceException("Queue does not exist", 500);
            }*/

            // Delete from database
            _database->DeleteMessage({.queueUrl=request.queueUrl, .receiptHandle=request.receiptHandle});

        } catch (Poco::Exception &ex) {
            log_error_stream(_logger) << "SQS delete message failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500);
        }
        return response;
    }

    std::string SQSService::GetMd5Body(const std::string &body) {
        std::string md5sum = Core::Crypto::GetMd5FromString(body);
        log_trace_stream(_logger) << "MD5 of body: " << md5sum << std::endl;
        return md5sum;
    }

    std::string SQSService::GetMd5Attributes(const Dto::SQS::MessageAttributeList &attributes) {
        std::stringstream attrString;
        for (const auto &s : attributes) {
            attrString << s.attributeName << s.type << s.transportType << s.attributeValue;
        }
        std::string md5sum = Core::Crypto::GetMd5FromString(attrString.str());
        log_trace_stream(_logger) << "MD5 of attributes: " << md5sum << std::endl;
        return md5sum;
    }
} // namespace AwsMock::Service
