//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_DYNAMODB_DATABASE_H
#define AWSMOCK_REPOSITORY_DYNAMODB_DATABASE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/Configuration.h>
#include <awsmock/core/DatabaseException.h>
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/memorydb/DynamoDbMemoryDb.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

  class DynamoDbDatabase : public Database {

    public:

      /**
       * Constructor
       *
       * @param configuration configuration properties
       */
      explicit DynamoDbDatabase(Core::Configuration &configuration);

      /**
       * Check existence of DynamoDb table
       *
       * @param region AWS region name
       * @param tableName table name
       * @return true if DynamoDb table already exists
       * @throws DatabaseException
       */
      bool TableExists(const std::string &region, const std::string &tableName);

      /**
       * Create a new DynamoDb table
       *
       * @param table DynamoDb table
       * @return created DynamoDb table.
       */
      Entity::DynamoDb::Table CreateTable(const Entity::DynamoDb::Table &table);

      /**
       * Returns a table entity by primary key
       *
       * @param oid table primary key
       * @return table entity
       * @throws DatabaseException
       */
      Entity::DynamoDb::Table GetTableById(bsoncxx::oid oid);

      /**
       * Returns a table entity by primary key
       *
       * @param oid table primary key
       * @return table entity
       * @throws DatabaseException
       */
      Entity::DynamoDb::Table GetTableById(const std::string &oid);

      /**
       * Returns a list of DynamoDB tables
       *
       * @param region AWS region name
       * @return list of DynamoDB tables
       */
      Entity::DynamoDb::TableList ListTables(const std::string &region = {});

      /**
       * Deletes an existing DynamoDB table
       *
       * @param tableName name of the table
       * @throws DatabaseException
       */
      void DeleteTable(const std::string &tableName);

      /**
       * Deletes all existing DynamoDB tables
       *
       * @throws DatabaseException
       */
      void DeleteAllTables();

      /**
       * Checks the existence of an item.
       *
       * @param region AWS region.
       * @param tableName name of the table
       * @param key primary key of the item
       * @return true if database exists, otherwise false
       */
      bool ItemExists(const std::string &region, const std::string &tableName, const std::string & key);

      /**
       * Returns a list of DynamoDB items
       *
       * @param region AWS region.
       * @param tableName table name
       * @return list of DynamoDB tables
       */
      Entity::DynamoDb::ItemList ListItems(const std::string &region = {}, const std::string &tableName = {});

      /**
       * Deletes an item
       *
       * @param region AWS region.
       * @param tableName name of the table
       * @param key primary key of the item
       * @return true if database exists, otherwise false
       */
      void DeleteItem(const std::string &region, const std::string &tableName, const std::string &key);

      /**
       * Deletes all items
       */
      void DeleteAllItems();

    private:

      /**
       * Logger
       */
      Core::LogStream _logger;

      /**
       * DynamoDb table collection
       */
      mongocxx::collection _tableCollection{};

      /**
       * DynamoDb item collection
       */
      mongocxx::collection _itemCollection{};

      /**
       * DynamoDB in-memory database
       */
      DynamoDbMemoryDb &_memoryDb;

  };

} // namespace AwsMock::Database

#endif // AWSMOCK_REPOSITORY_DYNAMODB_DATABASE_H