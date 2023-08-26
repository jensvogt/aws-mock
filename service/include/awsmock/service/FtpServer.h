//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_FTPSERVER_H
#define AWSMOCK_SERVICE_FTPSERVER_H

// Poco includes
#include "Poco/Logger.h"
#include "Poco/Runnable.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServer.h"

// FineFTP includes
#include <fineftp/server.h>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/Configuration.h>
#include <awsmock/core/MetricService.h>
#include <awsmock/service/LambdaHandlerFactory.h>

#define FTP_DEFAULT_PORT 21
#define FTP_DEFAULT_HOST "localhost"
#define FTP_ANONYMOUS_DIR "/tmp/awsmock/data/ftp"

namespace AwsMock::Service {

    /**
     * FTP service
     */
    class FtpServer : public Poco::Runnable {

    public:
      /**
       * Constructor
       *
       * @param configuration application configuration
       */
      explicit FtpServer(const Core::Configuration &configuration);

      /**
       * Destructor
       */
      ~FtpServer();

      /**
       * Run main routine
       */
      void run();

    private:

      /**
       * Rest port
       */
      int _port;

      /**
       * Rest host
       */
      std::string _host;

      /**
       * Logger
       */
      Core::LogStream _logger;

      /**
      * Application configuration
      */
      const Core::Configuration &_configuration;

      /**
       * Metric service
       */
      //Core::MetricService &_metricService;

      /**
       * FTP server instance
       */
      std::shared_ptr<fineftp::FtpServer> _ftpServer;

      /**
       * Anonymous directory
       */
      std::string _anonymousDir;

      /**
       * FTP max concurrent connections
       */
      int _maxThreads;

      /**
       * FTP server running thread
       */
      bool _running = false;

    };

} // namespace AwsMock::Service

#endif //AWSMOCK_SERVICE_FTPSERVER_H
