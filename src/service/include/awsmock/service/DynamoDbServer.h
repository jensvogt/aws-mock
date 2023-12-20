//
// Created by vogje01 on 20/12/2023.
//

#ifndef AWSMOCK_SERVER_DYNAMODB_SERVER_H
#define AWSMOCK_SERVER_DYNAMODB_SERVER_H

// C++ standard includes
#include <string>

// Poco includes
#include <Poco/Condition.h>
#include <Poco/Logger.h>
#include <Poco/Runnable.h>
#include <Poco/NotificationQueue.h>

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/MetricService.h>
#include <awsmock/repository/DynamoDbDatabase.h>
#include <awsmock/repository/ModuleDatabase.h>
#include <awsmock/service/AbstractWorker.h>
#include <awsmock/service/AbstractServer.h>
#include <awsmock/service/LambdaMonitoring.h>
#include <awsmock/service/DynamoDbHandlerFactory.h>

#define DYNAMODB_DEFAULT_PORT 9506
#define DYNAMODB_DEFAULT_HOST "localhost"
#define DYNAMODB_DEFAULT_QUEUE 150
#define DYNAMODB_DEFAULT_THREADS 50
#define DYNAMODB_DEFAULT_TIMEOUT 120
#define DYNAMODB_DOCKER_IMAGE std::string("dynamodblocal")
#define DYNAMODB_DOCKER_TAG std::string("latest")
#define DYNAMODB_INTERNAL_PORT 8000
#define DYNAMODB_EXTERNAL_PORT 8000
#define DYNAMODB_DOCKER_FILE "FROM amazon/dynamodb-local:latest\n" \
                             "VOLUME ./dynamodbdata /home/dynamodblocal/data\n" \
                             "WORKDIR /home/dynamodblocal\n" \
                             "EXPOSE 8000 8000\n" \
                             "ENTRYPOINT [\"java\", \"-Djava.library.path=./DynamoDBLocal_lib\", \"-jar\", \"DynamoDBLocal.jar\", \"-sharedDb\"]"

namespace AwsMock::Service {

  /**
   * AwsMock DynamoDB server
   *
   * @author jens.vogt@opitz-consulting.com
   */
  class DynamoDbServer : public AbstractServer, public AbstractWorker {

    public:

      /**
       * Constructor
       *
       * @param configuration aws-mock configuration
       * @param metricService aws-mock monitoring
       */
      explicit DynamoDbServer(Core::Configuration &configuration, Core::MetricService &metricService);

      /**
       * Destructor
       */
      ~DynamoDbServer() override;

      /**
       * Main method
       */
      void MainLoop() override;

      /**
       * Stop monitoring manager
       */
      void StopMonitoringServer();

    private:

      /**
       * Start monitoring manager
       */
      void StartMonitoringServer();

      /**
       * Delete dangling, stopped containers
       */
      void CleanupContainers();

      /**
       * Start the local DynamoDB container.
       */
      void StartLocalDynamoDb();

      /**
       * Logger
       */
      Core::LogStream _logger;

      /**
       * Configuration
       */
      Core::Configuration &_configuration;

      /**
       * Metric module
       */
      Core::MetricService &_metricService;

      /**
       * Thread pool
       */
      AwsMock::Core::ThreadPool<LambdaMonitoring> _threadPool;

      /**
       * Service database
       */
      std::unique_ptr<Database::ModuleDatabase> _moduleDatabase;

      /**
       * DynamoDb database
       */
      std::unique_ptr<Database::DynamoDbDatabase> _dynamoDbDatabase;

      /**
       * Docker module
       */
      std::unique_ptr<Service::DockerService> _dockerService;

      /**
       * AWS region
       */
      std::string _region;

      /**
       * Sleeping period in ms
       */
      int _period;

      /**
       * Rest port
       */
      int _port;

      /**
       * Rest host
       */
      std::string _host;

      /**
       * HTTP max message queue length
       */
      int _maxQueueLength;

      /**
       * HTTP max concurrent connection
       */
      int _maxThreads;

      /**
       * HTTP request timeout in seconds
       */
      int _requestTimeout;

      /**
       * Module name
       */
      std::string _module;
  };

} // namespace AwsMock::Service

#endif // AWSMOCK_SERVER_DYNAMODB_SERVER_H
