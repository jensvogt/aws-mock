//
// Created by vogje01 on 04/01/2023.
//

#include "awsmock/service/cognito/CognitoServer.h"

namespace AwsMock::Service {

  CognitoServer::CognitoServer(Core::Configuration &configuration, Core::MetricService &metricService)
      : AbstractWorker(configuration), AbstractServer(configuration, "cognito"), _logger(Poco::Logger::get("CognitoServer")), _configuration(configuration), _metricService(metricService), _module("cognito"), _running(false),_moduleDatabase(Database::ModuleDatabase::instance()),_cognitoDatabase(Database::CognitoDatabase::instance()) {

    // Get HTTP configuration values
    _port = _configuration.getInt("awsmock.service.cognito.port", COGNITO_DEFAULT_PORT);
    _host = _configuration.getString("awsmock.service.cognito.host", COGNITO_DEFAULT_HOST);
    _maxQueueLength = _configuration.getInt("awsmock.service.cognito.max.queue", COGNITO_DEFAULT_QUEUE_SIZE);
    _maxThreads = _configuration.getInt("awsmock.service.cognito.max.threads", COGNITO_DEFAULT_MAX_THREADS);
    _requestTimeout = _configuration.getInt("awsmock.service.cognito.timeout", COGNITO_DEFAULT_TIMEOUT);

    // Sleeping period
    _period = _configuration.getInt("awsmock.worker.cognito.period", 10000);
    log_debug_stream(_logger) << "Worker period: " << _period << std::endl;

    // Create environment
    _region = _configuration.getString("awsmock.region", "eu-central_1");
    _accountId = _configuration.getString("awsmock.account.userPoolId", "000000000000");
    _clientId = _configuration.getString("awsmock.client.userPoolId", "00000000");
    _user = _configuration.getString("awsmock.user", "none");
    log_debug_stream(_logger) << "Cognito module initialized, endpoint: " << _host << ":" << _port << std::endl;
  }

  void CognitoServer::Initialize() {

    // Check module active
    if (!IsActive("cognito")) {
      log_info_stream(_logger) << "Cognito module inactive" << std::endl;
      return;
    }
    log_info_stream(_logger) << "Cognito module starting" << std::endl;

    // Start REST module
    StartHttpServer(_maxQueueLength, _maxThreads, _requestTimeout, _host, _port, new CognitoHandlerFactory(_configuration, _metricService));
  }

  void CognitoServer::Run() {
      log_trace_stream(_logger) << "Cognito processing started" << std::endl;
      UpdateCounters();
  }

  void CognitoServer::Shutdown() {
    StopHttpServer();
  }

  void CognitoServer::UpdateCounters() {
    long userPools = _cognitoDatabase.CountUserPools();
    _metricService.SetGauge("cognito_userpool_count_total", userPools);
  }

}
