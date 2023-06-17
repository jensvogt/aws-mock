//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_DB_SQSDATABASE_H
#define AWSMOCK_DB_SQSDATABASE_H

// C++ standard includes
#include <string>
#include <vector>
#include <iostream>

// Poco includes
#include "Poco/Logger.h"
#include "Poco/LogStream.h"
#include "Poco/Data/Session.h"
#include "Poco/Data/RecordSet.h"

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/Logger.h>
#include <awsmock/core/Configuration.h>
#include <awsmock/core/DatabaseException.h>
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/db/Database.h>
#include <awsmock/entity/sqs/Message.h>
#include <awsmock/entity/sqs/Queue.h>

namespace AwsMock::Database {

    class SQSDatabase : public Database {

    public:

      /**
       * Constructor
       *
       * @param configuration configuration properties
       */
      explicit SQSDatabase(const Core::Configuration &configuration);

      /**
       * Check existence of queue
       *
       * @param region AWS region
       * @param name queue name
       * @return true if queue already exists
       * @throws DatabaseException
       */
      bool QueueExists(const std::string &region, const std::string &name);

      /**
       * Check existence of queue
       *
       * @param queueUrl AWS region
       * @return true if queue already exists
       * @throws DatabaseException
       */
      bool QueueExists(const std::string &queueUrl);

      /**
       * Create a new queue in the SQS queue table
       *
       * @param queue queue entity
       * @return created SQS queue entity
       * @throws DatabaseException
       */
      Entity::SQS::Queue CreateQueue(const Entity::SQS::Queue& queue);

      /**
       * Returns a queue by primary key
       *
       * @param oid queue primary key
       * @return queue entity
       * @throws DatabaseException
       */
      Entity::SQS::Queue GetQueueById(bsoncxx::oid oid);

      /**
       * Returns a queue by primary key
       *
       * @param oid queue primary key
       * @return queue entity
       * @throws DatabaseException
       */
      Entity::SQS::Queue GetQueueById(const std::string &oid);

      /**
       * Returns a queue by ARN
       *
       * @param queueArn queue ARN
       * @return queue entity
       * @throws DatabaseException
       */
      Entity::SQS::Queue GetQueueByArn(const std::string &queueArn);

      /**
       * Returns a queue by URL
       *
       * @param queueArn queue URL
       * @return queue entity
       * @throws DatabaseException
       */
      Entity::SQS::Queue GetQueueByUrl(const std::string &queueUrl);

      /**
       * List all available queues
       *
       * @param region AWS region
       * @return List of SQS queues
       * @throws DatabaseException
       */
      Entity::SQS::QueueList ListQueues(const std::string &region);

      /**
       * Purge a given queueUrl.
       *
       * @param region AWS region
       * @param queueUrl queueUrl name
       */
      void PurgeQueue(const std::string &region, const std::string &queueUrl);

      /**
       * Updates a given queue.
       *
       * @param queue AWS region
       * @return updated queue
       */
      Entity::SQS::Queue UpdateQueue(const Entity::SQS::Queue &queue);

      /**
       * Count the number of queues for a given region.
       *
       * @param queueUrl URL of the queue
       * @return number of queues in the given region.
       */
      long CountQueues(const std::string &region);

      /**
       * Deletes a queue.
       *
       * @param queue queue entity
       * @throws DatabaseException
       */
      void DeleteQueue(const Entity::SQS::Queue &queue);

      /**
       * Deletes all queues
       */
      void DeleteAllQueues();

      /**
       * Creates a new message in the SQS message table
       *
       * @param message SQS message entity
       * @return saved message entity
       * @throws Core::DatabaseException
       */
      Entity::SQS::Message CreateMessage(const Entity::SQS::Message &message);

      /**
       * Returns a message by ID.
       *
       * @param oid message objectId
       * @return message entity
       * @throws Core::DatabaseException
       */
      Entity::SQS::Message GetMessageById(bsoncxx::oid oid);

      /**
       * Returns a message by ID.
       *
       * @param oid message objectId
       * @return message entity
       * @throws Core::DatabaseException
       */
      Entity::SQS::Message GetMessageById(const std::string & oid);

      /**
       * Returns a message by receipt handle.
       *
       * @param receiptHandle message receipt handle
       * @return message entity
       * @throws Core::DatabaseException
       */
      Entity::SQS::Message GetMessageByReceiptHandle(const std::string &receiptHandle);

      /**
       * Receive messages from an queue.
       *
       * @param region AWS region
       * @param queueUrl queue URL
       * @param visibility in seconds
       * @param messageList message list
       * @return MessageList
       */
      void ReceiveMessages(const std::string &region, const std::string &queueUrl, int visibility, Entity::SQS::MessageList &messageList);

      /**
       * Reset expired messages
       *
       * @param queueUrl URL of the queue
       * @param visibility visibility period in seconds
       */
      [[maybe_unused]] void ResetMessages(const std::string& queueUrl, long visibility);

      /**
       * Redrive expired messages.
       *
       * @param queueUrl URL of the queue
       * @param visibility visibility period in seconds
       */
      [[maybe_unused]] void RedriveMessages(const std::string& queueUrl, const Entity::SQS::RedrivePolicy &redrivePolicy);

      /**
       * Count the number of message by status
       *
       * @param queueUrl URL of the queue
       * @param visibility visibility period in seconds
       */
      [[maybe_unused]] long CountMessages(const std::string &region, const std::string& queueUrl);

      /**
       * Count the number of message by status
       *
       * @param queueUrl URL of the queue
       * @param visibility visibility period in seconds
       */
      [[maybe_unused]] long CountMessagesByStatus(const std::string &region, const std::string& queueUrl, int status);

      /**
       * Deletes a message.
       *
       * @param message message to delete
       * @throws Core::DatabaseException
       */
      void DeleteMessage(const Entity::SQS::Message &message);

      /**
       * Deletes a messages.
       *
       * @throws Core::DatabaseException
       */
      void DeleteAllMessages();

    private:

      /**
       * Logger
       */
      Poco::LogStream _logger;

      /**
       * SQS queue collection
       */
      mongocxx::collection _queueCollection{};

      /**
       * SQS message collection
       */
      mongocxx::collection _messageCollection{};
    };

} // namespace AwsMock::Database

#endif // AWSMOCK_DB_SQSDATABASE_H
