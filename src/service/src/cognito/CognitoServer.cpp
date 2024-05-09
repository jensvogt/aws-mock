//
// Created by vogje01 on 04/01/2023.
//

#include "awsmock/service/cognito/CognitoServer.h"

namespace AwsMock::Service {

    CognitoServer::CognitoServer(Core::Configuration &configuration)
        : AbstractServer("cognito", 10), _configuration(configuration), _module("cognito"), _moduleDatabase(Database::ModuleDatabase::instance()),
          _cognitoDatabase(Database::CognitoDatabase::instance()) {

        // Get HTTP configuration values
        _port = _configuration.getInt("awsmock.service.cognito.http.port", COGNITO_DEFAULT_PORT);
        _host = _configuration.getString("awsmock.service.cognito.http.host", COGNITO_DEFAULT_HOST);
        _maxQueueLength = _configuration.getInt("awsmock.service.cognito.http.max.queue", COGNITO_DEFAULT_QUEUE_SIZE);
        _maxThreads = _configuration.getInt("awsmock.service.cognito.http.max.threads", COGNITO_DEFAULT_MAX_THREADS);
        _requestTimeout = _configuration.getInt("awsmock.service.cognito.http.timeout", COGNITO_DEFAULT_TIMEOUT);
        _monitoringPeriod = _configuration.getInt("awsmock.service.cognito.monitoring.period", COGNITO_DEFAULT_MONITORING_PERIOD);

        // Monitoring
        _cognitoMonitoring = std::make_unique<CognitoMonitoring>(_monitoringPeriod);

        log_debug << "Cognito module initialized, endpoint: " << _host << ":" << _port;
    }

    void CognitoServer::Initialize() {

        // Check module active
        if (!IsActive("cognito")) {
            log_info << "Cognito module inactive";
            return;
        }
        log_info << "Cognito module starting";

        // Start monitoring
        _cognitoMonitoring->Start();

        // Start REST module
        StartHttpServer(_maxQueueLength, _maxThreads, _requestTimeout, _host, _port, new CognitoHandlerFactory(_configuration));
    }

    void CognitoServer::Run() {
        log_trace << "Cognito processing started";
    }

    void CognitoServer::Shutdown() {
        StopHttpServer();
        _cognitoMonitoring->Stop();
    }

}// namespace AwsMock::Service
