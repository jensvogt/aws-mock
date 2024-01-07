//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_SNSSERVICE_H
#define AWSMOCK_SERVICE_SNSSERVICE_H

// C++ standard includes
#include <string>
#include <chrono>
#include <ctime>

// Poco includes
#include <Poco/UUIDGenerator.h>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/ServiceException.h>
#include <awsmock/dto/sns/CreateTopicRequest.h>
#include <awsmock/dto/sns/CreateTopicResponse.h>
#include <awsmock/dto/sns/ListTopicsResponse.h>
#include <awsmock/dto/sns/DeleteTopicResponse.h>
#include <awsmock/dto/sns/GetTopicAttributesRequest.h>
#include <awsmock/dto/sns/GetTopicAttributesResponse.h>
#include <awsmock/dto/sns/PublishRequest.h>
#include <awsmock/dto/sns/PublishResponse.h>
#include <awsmock/dto/sns/SqsNotificationRequest.h>
#include <awsmock/dto/sns/SubscribeRequest.h>
#include <awsmock/dto/sns/SubscribeResponse.h>
#include <awsmock/dto/sns/TagResourceRequest.h>
#include <awsmock/dto/sns/TagResourceResponse.h>
#include <awsmock/dto/sns/UnsubscribeRequest.h>
#include <awsmock/dto/sns/UnsubscribeResponse.h>
#include <awsmock/dto/sqs/SendMessageRequest.h>
#include <awsmock/dto/sqs/SendMessageResponse.h>
#include <awsmock/repository/SNSDatabase.h>
#include <awsmock/service/SQSService.h>

#define SQS_PROTOCOL "sqs"
#define DEFAULT_SQS_ACCOUNT_ID "000000000000"

namespace AwsMock::Service {

  class SNSService {

    public:

      /**
       * Constructor
       *
       * @param configuration module configuration
       * @param condition stop condition
       */
      explicit SNSService(Core::Configuration &configuration, Poco::Condition &condition);

      /**
       * Creates a new queue
       *
       * @param request create queue request
       * @return CreateQueueResponse
       */
      Dto::SNS::CreateTopicResponse CreateTopic(const Dto::SNS::CreateTopicRequest &request);

      /**
       * Returns a list of all available queues
       *
       * @param region AWS region
       * @return ListQueuesResponse
       */
      Dto::SNS::ListTopicsResponse ListTopics(const std::string &region);

      /**
       * Publish a message to a SNS topic
       *
       * @param request AWS region
       * @return PublishResponse
       */
      Dto::SNS::PublishResponse Publish(const Dto::SNS::PublishRequest &request);

      /**
       * Subscribe to a topic
       *
       * @param request subscribe request DTO
       * @return SubscribeResponse DTO
       */
      Dto::SNS::SubscribeResponse Subscribe(const Dto::SNS::SubscribeRequest &request);

      /**
       * Unsubscribe from a topic
       *
       * @param request unsubscribe request DTO
       * @return UnsubscribeResponse DTO
       */
      Dto::SNS::UnsubscribeResponse Unsubscribe(const Dto::SNS::UnsubscribeRequest &request);

      /**
       * Sets tags for a topic
       *
       * @param request tag resource request DTO
       * @return TagResourceResponse DTO
       */
      Dto::SNS::TagResourceResponse TagResource(const Dto::SNS::TagResourceRequest &request);

      /**
       * Returns the topic attributes
       *
       * @param request get topic attributes request DTO
       * @return GetTopicAttributesResponse DTO
       */
      Dto::SNS::GetTopicAttributesResponse GetTopicAttributes(const Dto::SNS::GetTopicAttributesRequest &request);

      /**
       * Delete a queue
       *
       * @param region AWS region name
       * @param topicArn topic ARN
       * @return DeleteQueueResponse
       * @throws ServiceException
       */
      Dto::SNS::DeleteTopicResponse DeleteTopic(const std::string &region, const std::string &topicArn);

    private:

      /**
       * Checks the subscriptions.
       *
       * <p>If a SQS queue subscription is found send the message to the SQS queue.</p>
       */
      void CheckSubscriptions(const Dto::SNS::PublishRequest &request);

      /**
       * Send a SNS message to an SQS queue
       *
       * @param subscription SNS subscription
       * @param request SNS publish request
       */
      void SendSQSMessage(const Database::Entity::SNS::Subscription &subscription, const Dto::SNS::PublishRequest &request);

      /**
       * Logger
       */
      Core::LogStream _logger;

      /**
       * Account ID
       */
      std::string _accountId;

      /**
       * Configuration
       */
      Core::Configuration &_configuration;

      /**
       * SNS database connection
       */
      std::unique_ptr<Database::SNSDatabase> _snsDatabase;

      /**
       * SQS database connection
       */
      std::unique_ptr<Database::SQSDatabase> _sqsDatabase;

      /**
       * SQS module
       */
      std::unique_ptr<SQSService> _sqsService;

      /**
       * Shutdown condition
       */
      Poco::Condition &_condition;
  };

} // namespace AwsMock::Service

#endif // AWSMOCK_SERVICE_SNSSERVICE_H
