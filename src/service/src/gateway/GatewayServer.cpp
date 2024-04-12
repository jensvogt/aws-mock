//
// Created by vogje01 on 03/06/2023.
//

#include "awsmock/service/gateway/GatewayServer.h"

namespace AwsMock::Service {

  GatewayServer::GatewayServer(Core::Configuration &configuration, Core::MetricService &metricService)
      : AbstractServer(configuration, "gateway"), _logger(Poco::Logger::get("GatewayServer")), _configuration(configuration), _metricService(metricService), _running(false) {

    // Get HTTP configuration values
    _port = _configuration.getInt("awsmock.module.gateway.port", GATEWAY_DEFAULT_PORT);
    _host = _configuration.getString("awsmock.module.gateway.host", GATEWAY_DEFAULT_HOST);
    _maxQueueLength = _configuration.getInt("awsmock.module.gateway.max.queue", GATEWAY_MAX_QUEUE);
    _maxThreads = _configuration.getInt("awsmock.module.gateway.max.threads", GATEWAY_MAX_THREADS);
    _requestTimeout = _configuration.getInt("awsmock.module.gateway.timeout", GATEWAY_TIMEOUT);

    // Sleeping period
    _period = _configuration.getInt("awsmock.worker.lambda.period", 10000);
    log_debug_stream(_logger) << "Gateway worker period: " << _period << std::endl;

    // Create environment
    _region = _configuration.getString("awsmock.region");
    log_debug_stream(_logger) << "GatewayServer initialized" << std::endl;
  }

  GatewayServer::~GatewayServer() {
    StopServer();
  }

  void GatewayServer::Initialize() {

    // Check module active
    if (!IsActive("gateway")) {
      log_info_stream(_logger) << "Gateway module inactive" << std::endl;
      return;
    }
    log_info_stream(_logger) << "Gateway server starting" << std::endl;

    // Start HTTP manager
    StartHttpServer(_maxQueueLength, _maxThreads, _requestTimeout, _host, _port, new GatewayRouter(_configuration, _metricService));
  }

  void GatewayServer::Run() {
  }

  void GatewayServer::Shutdown() {
    StopHttpServer();
  }

} // namespace AwsMock::Service
