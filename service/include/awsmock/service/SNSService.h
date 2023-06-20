//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_SNSSERVICE_H
#define AWSMOCK_SERVICE_SNSSERVICE_H

// C++ standard includes
#include <string>
#include <chrono>
#include <ctime>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/ServiceException.h>
#include <awsmock/dto/sns/CreateTopicRequest.h>
#include <awsmock/dto/sns/CreateTopicResponse.h>
#include <awsmock/dto/sns/ListTopicsResponse.h>
#include <awsmock/dto/sns/DeleteTopicResponse.h>
#include <awsmock/dto/sns/PublishRequest.h>
#include <awsmock/dto/sns/PublishResponse.h>
#include <awsmock/dto/sns/SubscribeRequest.h>
#include <awsmock/dto/sns/SubscribeResponse.h>
#include <awsmock/dto/sqs/CreateMessageRequest.h>
#include <awsmock/dto/sqs/CreateMessageResponse.h>
#include <awsmock/db/SNSDatabase.h>
#include <awsmock/service/SQSService.h>

#define SQS_PROTOCOL "sqs"

namespace AwsMock::Service {

    class SNSService {

    public:

      /**
       * Constructor
       *
       * @param configuration service configuration
       */
      explicit SNSService(const Core::Configuration &configuration);

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
       * Publish a message to a SNS topic
       *
       * @param request AWS region
       * @return PublishResponse
       */
      Dto::SNS::SubscribeResponse Subscribe(const Dto::SNS::SubscribeRequest &request);

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
       * Initialize the service
       */
      void Initialize();

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
       * @param message message body
       */
      void SendSQSMessage(const Database::Entity::SNS::Subscription &subscription, const std::string &message);

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
      const Core::Configuration &_configuration;

      /**
       * SNS database connection
       */
      std::unique_ptr<Database::SNSDatabase> _snsDatabase;

      /**
       * SQS database connection
       */
      std::unique_ptr<Database::SQSDatabase> _sqsDatabase;

      /**
       * SQS service
       */
      std::unique_ptr<SQSService> _sqsService;
    };

} // namespace AwsMock::Service

#endif // AWSMOCK_SERVICE_SNSSERVICE_H
