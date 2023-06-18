//
// Created by vogje01 on 04/01/2023.
//

#include "awsmock/service/SQSServer.h"

namespace AwsMock::Service {

    SQSServer::SQSServer(Core::Configuration &configuration, Core::MetricService &metricService)
        : _logger(Poco::Logger::get("SQSServer")), _configuration(configuration), _metricService(metricService) {

        Core::Logger::SetDefaultConsoleLogger("SQSServer");

        _port = _configuration.getInt("awsmock.service.sqs.port", SQS_DEFAULT_PORT);
        _host = _configuration.getString("awsmock.service.sqs.host", SQS_DEFAULT_HOST);
        _maxQueueLength = _configuration.getInt("awsmock.service.sqs.max.queue", 250);
        _maxThreads = _configuration.getInt("awsmock.service.sqs.max.threads", 50);
        _logger.debug() << "SQS rest service initialized, endpoint: " << _host << ":" << _port << std::endl;
    }

    SQSServer::~SQSServer() {
        if(_httpServer) {
            _httpServer->stopAll(true);
            delete _httpServer;
            _logger.information() << "SQS rest service stopped" << std::endl;
        }
    }

    void SQSServer::start() {

        // Set HTTP server parameter
        auto *httpServerParams = new Poco::Net::HTTPServerParams();
        httpServerParams->setMaxQueued(250);
        httpServerParams->setMaxThreads(50);
        _logger.debug() << "HTTP server parameter set, maxQueue: " << _maxQueueLength << " maxThreads: " << _maxThreads << std::endl;

        _httpServer =
            new Poco::Net::HTTPServer(new SQSRequestHandlerFactory(_configuration, _metricService), Poco::Net::ServerSocket(Poco::UInt16(_port)), httpServerParams);

        _httpServer->start();
        _logger.information() << "SQS rest service started, endpoint: http://" << _host << ":" << _port << std::endl;
    }
}
