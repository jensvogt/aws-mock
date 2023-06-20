//
// Created by vogje01 on 04/01/2023.
//

#include "awsmock/service/SNSServer.h"

namespace AwsMock::Service {

    SNSServer::SNSServer(Core::Configuration &configuration, Core::MetricService &metricService)
        : _logger(Poco::Logger::get("SNSServer")), _configuration(configuration), _metricService(metricService) {

        Core::Logger::SetDefaultConsoleLogger("SNSServer");

        _port = _configuration.getInt("awsmock.service.sns.port", SNS_DEFAULT_PORT);
        _host = _configuration.getString("awsmock.service.sns.host", SNS_DEFAULT_HOST);
        _maxQueueLength = _configuration.getInt("awsmock.service.sns.max.queue", 250);
        _maxThreads = _configuration.getInt("awsmock.service.sns.max.threads", 50);
        log_debug_stream(_logger) << "SNS rest service initialized, endpoint: " << _host << ":" << _port << std::endl;
    }

    SNSServer::~SNSServer() {
        if(_httpServer) {
            _httpServer->stopAll(true);
            delete _httpServer;
            log_info_stream(_logger) << "SNS rest service stopped" << std::endl;
        }
    }

    void SNSServer::start() {

        // Set HTTP server parameter
        auto *httpServerParams = new Poco::Net::HTTPServerParams();
        httpServerParams->setMaxQueued(_maxQueueLength);
        httpServerParams->setMaxThreads(_maxThreads);
        log_debug_stream(_logger) << "HTTP server parameter set, maxQueue: " << _maxQueueLength << " maxThreads: " << _maxThreads << std::endl;

        _httpServer =
            new Poco::Net::HTTPServer(new SNSRequestHandlerFactory(_configuration, _metricService), Poco::Net::ServerSocket(Poco::UInt16(_port)), httpServerParams);

        _httpServer->start();
        log_info_stream(_logger) << "SNS rest service started, endpoint: http://" << _host << ":" << _port << std::endl;
    }
}
