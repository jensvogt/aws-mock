//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_SQSSERVER_H
#define AWSMOCK_SERVICE_SQSSERVER_H

// Poco includes
#include "Poco/Logger.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServer.h"

// AwsMock includes
#include "awsmock/core/Logger.h"
#include "awsmock/core/LogStream.h"
#include "awsmock/core/Configuration.h"
#include "awsmock/core/MetricService.h"
#include "awsmock/service/SQSHandlerFactory.h"

#define SQS_DEFAULT_PORT 9501
#define SQS_DEFAULT_HOST "localhost"

namespace AwsMock::Service {

    /**
     * S3 REST service
     */
    class SQSServer {

    public:
      /**
       * Constructor
       *
       * @param configuration application configuration
       * @param metricService monitoring service
       */
      explicit SQSServer(Core::Configuration &configuration, Core::MetricService &metricService);

      /**
       * Destructor
       */
      ~SQSServer();

      /**
       * Start the restfull service.
       */
      void start();

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
      Core::Configuration &_configuration;

      /**
       * Metric service
       */
      Core::MetricService &_metricService;

      /**
       * HTTP server instance
       */
      Poco::Net::HTTPServer *_httpServer;

      /**
       * HTTP max message queue length
       */
      int _maxQueueLength;

      /**
       * HTTP max concurrent connections
       */
      int _maxThreads;

    };

} // namespace AwsMock::Service

#endif //AWSMOCK_SERVICE_SQSSERVER_H
