//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_DB_DATABASE_H
#define AWSMOCK_DB_DATABASE_H

// C++ standard includes
#include <string>

// Poco includes
#include "Poco/Logger.h"
#include "Poco/LogStream.h"

// MongoDB includes
#include <mongocxx/client.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/exception/exception.hpp>

// AwsMock includes
#include <awsmock/core/Logger.h>
#include <awsmock/core/Configuration.h>

namespace AwsMock::Database {

    class Database {

    public:

      /**
       * Constructor
       *
       * @param configuration configuration properties
       */
      explicit Database(const Core::Configuration &configuration);

      /**
       * Returns a MongoDB connection from the pool
       *
       * @return MongoDB database client
       */
      mongocxx::database GetConnection();

    private:

      /**
       * Logger
       */
      Poco::LogStream _logger;

      /**
       * Application configuration
       */
      const Core::Configuration &_configuration;

      /**
       * Database name
       */
      std::string _name;

      /**
       * Database host
       */
      std::string _host;

      /**
       * Database port
       */
      int _port;

      /**
       * Database user
       */
      std::string _user;

      /**
       * Database password
       */
      std::string _password;

      /**
       * MongoDB URI
       */
      mongocxx::uri _uri;

      /**
       * Database client
       */
      mongocxx::client _client;

    };

} // namespace AwsMock::Database

#endif //AWSMOCK_DB_DATABASE_H
