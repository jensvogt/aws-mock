//
// Created by vogje01 on 04/01/2023.
//

#include "awsmock/service/LambdaServer.h"

namespace AwsMock::Service {

    LambdaServer::LambdaServer(Core::Configuration &configuration, Core::MetricService &metricService)
        : _logger(Poco::Logger::get("LambdaServer")), _configuration(configuration), _metricService(metricService) {

        // Set default console logger
        Core::Logger::SetDefaultConsoleLogger("LambdaServer");

        _port = _configuration.getInt("awsmock.service.lambda.port", LAMBDA_DEFAULT_PORT);
        _host = _configuration.getString("awsmock.service.lambda.host", LAMBDA_DEFAULT_HOST);
        _maxQueueLength = _configuration.getInt("awsmock.service.lambda.max.queue", 250);
        _maxThreads = _configuration.getInt("awsmock.service.lambda.max.threads", 50);
        _logger.debug() << "Lambda rest service initialized, endpoint: " << _host << ":" << _port << std::endl;
    }

    LambdaServer::~LambdaServer() {
        if(_httpServer) {
            _httpServer->stopAll(true);
            delete _httpServer;
            _logger.information() << "Lambda rest service stopped" << std::endl;
        }
    }

    void LambdaServer::start() {

        // Set HTTP server parameter
        auto *httpServerParams = new Poco::Net::HTTPServerParams();
        httpServerParams->setMaxQueued(_maxQueueLength);
        httpServerParams->setMaxThreads(_maxThreads);
        _logger.debug() << "HTTP server parameter set, maxQueue: " << _maxQueueLength << " maxThreads: " << _maxThreads << std::endl;

        _httpServer =
            new Poco::Net::HTTPServer(new LambdaRequestHandlerFactory(_configuration, _metricService), Poco::Net::ServerSocket(Poco::UInt16(_port)), httpServerParams);

        _httpServer->start();
        _logger.information() << "Lambda rest service started, endpoint: http://" << _host << ":" << _port << std::endl;
    }
}
