//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_DB_SNSDATABASE_H
#define AWSMOCK_DB_SNSDATABASE_H

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
#include <awsmock/core/Logger.h>
#include <awsmock/core/Configuration.h>
#include <awsmock/core/DatabaseException.h>
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/db/Database.h>
#include <awsmock/db/SNSDatabase.h>
#include <awsmock/entity/sns/Message.h>
#include <awsmock/entity/sns/Topic.h>

namespace AwsMock::Database {

    class SNSDatabase : public Database {

    public:

      /**
       * Constructor
       *
       * @param configuration configuration properties
       */
      explicit SNSDatabase(const Core::Configuration &configuration);

      /**
       * Check existence of topic
       *
       * @param region AWS region
       * @param name topic name
       * @return true if topic already exists
       * @throws DatabaseException
       */
      bool TopicExists(const std::string &region, const std::string &name);

      /**
       * Check existence of topic
       *
       * @param topicName topic ARN
       * @return true if topic already exists
       * @throws DatabaseException
       */
      bool TopicExists(const std::string &topicName);

      /**
       * Create a new topic in the SNS topic table
       *
       * @param topic topic entity
       * @return created SNS topic entity
       * @throws DatabaseException
       */
      Entity::SNS::Topic CreateTopic(const Entity::SNS::Topic& topic);

      /**
       * Returns a topic by primary key
       *
       * @param oid topic primary key
       * @return topic entity
       * @throws DatabaseException
       */
      Entity::SNS::Topic GetTopicById(bsoncxx::oid oid);

      /**
       * Returns a topic by primary key
       *
       * @param oid topic primary key
       * @return topic entity
       * @throws DatabaseException
       */
      Entity::SNS::Topic GetTopicById(const std::string &oid);

      /**
       * Returns a topic by is ARN
       *
       * @param topicArn topic ARN
       * @return topic entity
       * @throws DatabaseException
       */
      Entity::SNS::Topic GetTopicByArn(const std::string &topicArn);

      /**
       * Updates an existing topic in the SNS topic table
       *
       * @param topic topic entity
       * @return updated SNS topic entity
       * @throws DatabaseException
       */
      Entity::SNS::Topic UpdateTopic(const Entity::SNS::Topic& topic);

      /**
       * List all available topics
       *
       * @param region AWS region
       * @return list of SNS topics
       * @throws DatabaseException
       */
      Entity::SNS::TopicList ListTopics(const std::string &region);

      /**
       * Counts the number of topics
       *
       * @param region AWS region
       * @return number of topics
       */
      long CountTopics(const std::string &region);

      /**
       * Deletes a topic.
       *
       * @param topic topic entity
       * @throws DatabaseException
       */
      void DeleteTopic(const Entity::SNS::Topic &topic);

      /**
       * Deletes all topics
       */
      void DeleteAllTopics();

      /**
       * Creates a new message in the SQS message table
       *
       * @param message SQS message entity
       * @return saved message entity
       * @throws Core::DatabaseException
       */
      Entity::SNS::Message CreateMessage(const Entity::SNS::Message &message);

      /**
       * Returns a message by ID.
       *
       * @param oid message objectId
       * @return message entity
       * @throws Core::DatabaseException
       */
      Entity::SNS::Message GetMessageById(bsoncxx::oid oid);

      /**
       * Returns a message by ID.
       *
       * @param oid message objectId
       * @return message entity
       * @throws Core::DatabaseException
       */
      [[maybe_unused]] Entity::SNS::Message GetMessageById(const std::string & oid);

      /**
       * Count the number of message by status
       *
       * @param topicUrl URL of the topic
       * @param visibility visibility period in seconds
       */
      long CountMessages(const std::string &region, const std::string& topicUrl);

      /**
       * Count the number of message by status
       *
       * @param topicUrl URL of the topic
       * @param visibility visibility period in seconds
       */
      //[[maybe_unused]] long CountMessagesByStatus(const std::string &region, const std::string& topicUrl, int status);

      /**
       * Deletes a message.
       *
       * @param message message to delete
       * @throws Core::DatabaseException
       */
      void DeleteMessage(const Entity::SNS::Message &message);

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
       * SNS topic collection
       */
      mongocxx::collection _topicCollection{};

      /**
       * SNS message collection
       */
      mongocxx::collection _messageCollection{};
    };

} // namespace AwsMock::Database

#endif // AWSMOCK_DB_SNSDatabase_H
