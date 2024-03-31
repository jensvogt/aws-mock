//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_S3SERVER_H
#define AWSMOCK_SERVICE_S3SERVER_H

// Poco includes
#include <Poco/Logger.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServer.h>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/Configuration.h>
#include <awsmock/core/MetricService.h>
#include <awsmock/core/ThreadPool.h>
#include <awsmock/service/AbstractServer.h>
#include <awsmock/service/AbstractWorker.h>
#include <awsmock/service/S3Monitoring.h>
#include <awsmock/service/S3HandlerFactory.h>

#define S3_DEFAULT_PORT 9500
#define S3_DEFAULT_HOST "localhost"
#define S3_DEFAULT_QUEUE_SIZE 250
#define S3_DEFAULT_MAX_THREADS 50
#define S3_DEFAULT_TIMEOUT 900

namespace AwsMock::Service {

  /**
   * S3 module
   */
  class S3Server : public AbstractServer, public AbstractWorker {

  public:
    /**
     * Constructor
     *
     * @param configuration application configuration
     * @param metricService monitoring module
     */
    explicit S3Server(Core::Configuration &configuration, Core::MetricService &metricService);

    /**
     * Destructor
     */
    ~S3Server() override;

    /**
     * Thread main method
     */
    void MainLoop() override;

    /**
     * Stops the monitoring module.
     */
    void StopMonitoringServer();

  private:

    /**
     * Start the monitoring module.
     */
    void StartMonitoringServer();

    /**
     * Update metric counters
     */
    void UpdateCounters();

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
     * Metric module
     */
    Core::MetricService &_metricService;

    /**
     * HTTP manager instance
     */
    std::shared_ptr<Poco::Net::HTTPServer> _httpServer;

    /**
     * S3 database
     */
    std::unique_ptr<Database::S3Database> _s3Database;

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
     * Sleeping period in ms
     */
    int _period;

    /**
     * Data directory
     */
    std::string _dataDir;

    /**
     * Thread pool
     */
    AwsMock::Core::ThreadPool<S3Monitoring> _threadPool;

    /**
     * S3 module name
     */
    std::string _module;
  };

} // namespace AwsMock::Service

#endif // AWSMOCK_SERVICE_S3SERVER_H
