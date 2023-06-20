//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_SQSSERVICE_H
#define AWSMOCK_SERVICE_SQSSERVICE_H

// C++ standard includes
#include <string>
#include <chrono>
#include <ctime>

// AwsMock includes
#include "awsmock/core/AwsUtils.h"
#include "awsmock/core/LogStream.h"
#include "awsmock/core/CryptoUtils.h"
#include "awsmock/core/ServiceException.h"
#include "awsmock/dto/sqs/CreateMessageRequest.h"
#include "awsmock/dto/sqs/CreateMessageResponse.h"
#include "awsmock/dto/sqs/CreateQueueRequest.h"
#include "awsmock/dto/sqs/CreateQueueResponse.h"
#include "awsmock/dto/sqs/GetQueueAttributesRequest.h"
#include "awsmock/dto/sqs/GetQueueAttributesResponse.h"
#include "awsmock/dto/sqs/DeleteMessageRequest.h"
#include "awsmock/dto/sqs/DeleteMessageResponse.h"
#include "awsmock/dto/sqs/DeleteQueueRequest.h"
#include "awsmock/dto/sqs/DeleteQueueResponse.h"
#include "awsmock/dto/sqs/ListQueueResponse.h"
#include "awsmock/dto/sqs/SetQueueAttributesRequest.h"
#include "awsmock/dto/sqs/SetQueueAttributesResponse.h"
#include "awsmock/dto/sqs/PurgeQueueRequest.h"
#include "awsmock/dto/sqs/PurgeQueueResponse.h"
#include "awsmock/dto/sqs/ReceiveMessageRequest.h"
#include "awsmock/dto/sqs/ReceiveMessageResponse.h"
#include "awsmock/db/SQSDatabase.h"

#define DEFAULT_ACCOUNT_ID "000000000000"

namespace AwsMock::Service {

    class SQSService {

    public:

      /**
       * Constructor
       *
       * @param configuration service configuration
       */
      explicit SQSService(const Core::Configuration &configuration);

      /**
       * Creates a new queue
       *
       * @param request create queue request
       * @return CreateQueueResponse
       */
      Dto::SQS::CreateQueueResponse CreateQueue(const Dto::SQS::CreateQueueRequest & request);

      /**
       * Returns a list of all available queues
       *
       * @param region AWS region
       * @return ListQueuesResponse
       */
      Dto::SQS::ListQueueResponse ListQueues(const std::string &region);

      /**
       * Purge a queue.
       *
       * @param request purge queue request
       * @return PurgeQueueResponse
       * @throws ServiceException
       */
      Dto::SQS::PurgeQueueResponse PurgeQueue(const Dto::SQS::PurgeQueueRequest &request);

      /**
       * Return the queue attributes
       *
       * @param request get queue sqs request
       * @return GetQueueAttributesResponse
       * @throws ServiceException
       */
      Dto::SQS::GetQueueAttributesResponse GetQueueAttributes(const Dto::SQS::GetQueueAttributesRequest &request);

      /**
       * Put queue attributes
       *
       * @param request put queue sqs request
       * @return SetQueueAttributesResponse
       * @throws ServiceException
       */
      Dto::SQS::SetQueueAttributesResponse SetQueueAttributes(Dto::SQS::SetQueueAttributesRequest &request);

      /**
       * Delete a queue
       *
       * @param request delete request DTO
       * @return DeleteQueueResponse
       * @throws ServiceException
       */
      Dto::SQS::DeleteQueueResponse DeleteQueue(const Dto::SQS::DeleteQueueRequest &request);

      /**
       * Creates a new queue
       *
       * @param request create message request
       * @return CreateMessageResponse
       * @throws ServiceException
       */
      Dto::SQS::CreateMessageResponse CreateMessage(const Dto::SQS::CreateMessageRequest &request);

      /**
       * Receive a list of messages
       *
       * @param request receive message request
       * @return ReceiveMessageResponse
       * @throws ServiceException
       */
      Dto::SQS::ReceiveMessageResponse ReceiveMessages(const Dto::SQS::ReceiveMessageRequest &request);

      /**
       * Delete a message
       *
       * @param request delete message request DTO
       * @return DeleteMessageResponse
       * @throws ServiceException
       */
      Dto::SQS::DeleteMessageResponse DeleteMessage(const Dto::SQS::DeleteMessageRequest &request);

    private:

      /**
       * Initialize the service
       */
      void Initialize();

      /**
       * Returns the MD5 sum of the message body.
       *
       * @param body message body
       * @return MD5 sum of message body
       */
      std::string GetMd5Body(const std::string &body);

      /**
       * Returns the MD5 sum of all attributes.
       *
       * @param attributes vector of attributes
       * @return MD5 sum of attributes string
       */
      std::string GetMd5Attributes(const Dto::SQS::MessageAttributeList &attributes);

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
       * Database connection
       */
      std::unique_ptr<Database::SQSDatabase> _database;
    };

} // namespace AwsMock::Service

#endif // AWSMOCK_SERVICE_SQSSERVICE_H
