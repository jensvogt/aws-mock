//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_DB_LAMBDADATABASE_H
#define AWSMOCK_DB_LAMBDADATABASE_H

// C++ standard includes
#include <string>
#include <vector>
#include <iostream>

// Poco includes
#include "Poco/Logger.h"
#include "Poco/LogStream.h"

// AwsMock includes
#include <awsmock/core/Logger.h>
#include <awsmock/core/Configuration.h>
#include <awsmock/core/DatabaseException.h>
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/db/Database.h>
#include <awsmock/entity/lambda/Lambda.h>

namespace AwsMock::Database {

    class LambdaDatabase : public Database {

    public:

      /**
       * Constructor
       *
       * @param configuration configuration properties
       */
      explicit LambdaDatabase(const Core::Configuration &configuration);

      /**
       * Check existence of lambda
       *
       * @param function AWS function
       * @param runtime lambda runtime
       * @return true if lambda already exists
       * @throws DatabaseException
       */
      bool LambdaExists(const std::string &function, const std::string &runtime);

      /**
       * Check existence of lambda
       *
       * @param lambda AWS function
       * @return true if lambda already exists
       * @throws DatabaseException
       */
      bool LambdaExists(const Entity::Lambda::Lambda &lambda);

      /**
       * Check existence of lambda
       *
       * @param function AWS function
       * @return true if lambda already exists
       * @throws DatabaseException
       */
      bool LambdaExists(const std::string &function);

      /**
       * Create a new lambda function
       *
       * @param lambda lambda entity
       * @return created lambda entity.
       */
      Entity::Lambda::Lambda CreateLambda(const Entity::Lambda::Lambda &lambda);


      /**
       * Updates an existing lambda lambda function
       *
       * @param lambda lambda entity
       * @return updated lambda entity.
       */
      Entity::Lambda::Lambda UpdateLambda(const Entity::Lambda::Lambda &lambda);


      /**
       * Created or updates an existing lambda function
       *
       * @param lambda lambda entity
       * @return created or updated lambda entity.
       */
      Entity::Lambda::Lambda CreateOrUpdateLambda(const Entity::Lambda::Lambda &lambda);

      /**
       * Returns a lambda entity by primary key
       *
       * @param oid lambda primary key
       * @return lambda entity
       * @throws DatabaseException
       */
      Entity::Lambda::Lambda GetLambdaById(bsoncxx::oid oid);

      /**
       * Returns a lambda entity by primary key
       *
       * @param oid lambda primary key
       * @return lambda entity
       * @throws DatabaseException
       */
      Entity::Lambda::Lambda GetLambdaById(const std::string &oid);

      /**
       * Returns a lambda entity by ARN
       *
       * @param arn lambda ARN
       * @return lambda entity
       * @throws DatabaseException
       */
      Entity::Lambda::Lambda GetLambdaByArn(const std::string &arn);

      /**
       * Returns a list of lambda functions.
       *
       * @param region AWS region name
       * @return list of lambda functions
       */
      std::vector<Entity::Lambda::Lambda> ListLambdas(const std::string &region);

      /**
       * Deletes an existing lambda function
       *
       * @param functionName lambda function name
       * @throws DatabaseException
       */
      void DeleteLambda(const std::string &functionName);

      /**
       * Deletes all existing lambda functions
       *
       * @throws DatabaseException
       */
      void DeleteAllLambdas();

    private:

      /**
       * Logger
       */
      Poco::LogStream _logger;

      /**
       * Lambda collection
       */
      mongocxx::collection _lambdaCollection{};

    };

} // namespace AwsMock::Database

#endif // AWSMOCK_DB_LAMBDADATABASE_H
