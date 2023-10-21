//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_SERVICE_TRANSFERSERVER_H
#define AWSMOCK_SERVICE_TRANSFERSERVER_H

// C++ standard includes
#include <string>

// Poco includes
#include <Poco/Logger.h>
#include <Poco/Runnable.h>

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/s3/CreateBucketConstraint.h>
#include <awsmock/ftpserver/FtpServer.h>
#include <awsmock/repository/ServiceDatabase.h>
#include <awsmock/repository/TransferDatabase.h>
#include <awsmock/worker/AbstractWorker.h>

#define DEFAULT_TRANSFER_BUCKET "transfer-server"
#define DEFAULT_BASE_DIR "transfer"
#define CONTENT_TYPE_JSON "application/json"

namespace AwsMock::Service {

    class TransferServer : public Poco::Runnable, public AbstractWorker {

    public:

      /**
       * Constructor
       *
       * @param AwsMock configuration
       */
      explicit TransferServer(Core::Configuration &configuration);

      /**
       * Destructor
       */
      ~TransferServer() override;

      /**
       * Main method
       */
      void run() override;

    private:

      /**
       * Start the restfull service.
       */
      void StartHttpServer();

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
       * Sends a create bucket request to the S3 service
       *
       * @param bucket S3 bucket name
       */
      void SendCreateBucketRequest(const std::string &bucket);

      /**
       * Sends a exists bucket request to the S3 service
       *
       * @param bucket S3 bucket name
       * @param contentType content type
       * @return true when bucket exists
       */
      bool SendExistsBucketRequest(const std::string &bucket);

      /**
       * Logger
       */
      Core::LogStream _logger;

      /**
       * Configuration
       */
      Core::Configuration &_configuration;

      /**
       * Service database
       */
      std::unique_ptr<Database::ServiceDatabase> _serviceDatabase;

      /**
       * Lambda database
       */
      std::unique_ptr<Database::TransferDatabase> _transferDatabase;

      /**
       * Directory _watcher thread
       */
      Poco::Thread _watcherThread;

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
       * AWS S3 bucket
       */
      std::string _bucket;

      /**
       * Base dir for all FTP users
       */
      std::string _baseDir;

      /**
       * Base URL for all S3 request
       */
      std::string _baseUrl;

      /**
       * Server id
       */
      std::string _serverId;

      /**
       * List of transfer servers
       */
      std::map<std::string, std::shared_ptr<FtpServer::FtpServer>> _transferServerList;

      /**
       * Actual FTP server
       */
      std::shared_ptr<FtpServer::FtpServer> _ftpServer;

      /**
       * Directory _watcher
       */
      std::shared_ptr<Core::DirectoryWatcher> _watcher;

      /**
       * S3 service host
       */
      std::string _s3ServiceHost;

      /**
       * S3 service port
       */
      int _s3ServicePort;
    };

} // namespace AwsMock::Service

#endif // AWSMOCK_SERVICE_TRANSFERSERVER_H
