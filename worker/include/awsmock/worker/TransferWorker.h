//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_WORKER_TRANSFERWORKER_H
#define AWSMOCK_WORKER_TRANSFERWORKER_H

// C++ standard includes
#include <string>

// Poco includes
#include <Poco/Logger.h>
#include <Poco/Runnable.h>

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/Logger.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/db/ServiceDatabase.h>
#include <awsmock/db/TransferDatabase.h>
#include <awsmock/service/FtpServer.h>

namespace AwsMock::Worker {

    class TransferWorker : public Poco::Runnable {

    public:

      /**
       * Constructor
       */
      [[maybe_unused]] explicit TransferWorker(const Core::Configuration &configuration);

      /**
       * Main method
       */
      void run() override;

    private:

      /**
       * Initialization
       */
      void Initialize();

      /**
       * Starts a single transfer server
       *
       * @param server transfer server entity
       */
      void StartTransferServer(Database::Entity::Transfer::Transfer &server);

      /**
       * Stops a single transfer server
       *
       * @param server transfer server entity
       */
      void StopTransferServer(Database::Entity::Transfer::Transfer &server);

      /**
       * Start all transfer servers, if they are not existing
       */
      void StartTransferServers();

      /**
       * Check transfer servers
       */
      void CheckTransferServers();

      /**
       * Logger
       */
      Core::LogStream _logger;

      /**
       * Configuration
       */
      const Core::Configuration &_configuration;

      /**
       * Service database
       */
      std::unique_ptr<Database::ServiceDatabase> _serviceDatabase;

      /**
       * Lambda database
       */
      std::unique_ptr<Database::TransferDatabase> _transferDatabase;

      /**
       * Running flag
       */
      bool _running;

      /**
       * Sleeping period in ms
       */
      int _period;

      /**
       * AWS region
       */
      std::string _region;

      /**
       * AWS client ID
       */
      std::string _clientId;

      /**
       * AWS user
       */
      std::string _user;

      /**
       * List of transfer servers
       */
      std::map<std::string, std::shared_ptr<Service::FtpServer>> _transferServerList;
    };

} // namespace AwsMock::Worker

#endif // AWSMOCK_WORKER_TRANSFERWORKER_H
