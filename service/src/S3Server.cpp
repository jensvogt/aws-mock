//
// Created by vogje01 on 04/01/2023.
//

#include "awsmock/service/S3Server.h"

namespace AwsMock::Service {

    S3Server::S3Server(Core::Configuration &configuration, Core::MetricService &metricService)
        : _logger(Poco::Logger::get("S3Server")), _configuration(configuration), _metricService(metricService) {

        // Set default console logger
        Core::Logger::SetDefaultConsoleLogger("S3Server");

        _port = _configuration.getInt("awsmock.service.s3.port", S3_DEFAULT_PORT);
        _host = _configuration.getString("awsmock.service.s3.host", S3_DEFAULT_HOST);
        _maxQueueLength = _configuration.getInt("awsmock.service.s3.max.queue", 250);
        _maxThreads = _configuration.getInt("awsmock.service.s3.max.threads", 50);
        log_debug_stream(_logger) << "S3 rest service initialized, endpoint: " << _host << ":" << _port << std::endl;
    }

    S3Server::~S3Server() {
        if(_httpServer) {
            _httpServer->stopAll(true);
            delete _httpServer;
            log_info_stream(_logger) << "S3 rest service stopped" << std::endl;
        }
    }

    void S3Server::start() {

        // Set HTTP server parameter
        auto *httpServerParams = new Poco::Net::HTTPServerParams();
        httpServerParams->setMaxQueued(_maxQueueLength);
        httpServerParams->setMaxThreads(_maxThreads);
        log_debug_stream(_logger) << "HTTP server parameter set, maxQueue: " << _maxQueueLength << " maxThreads: " << _maxThreads << std::endl;

        _httpServer =
            new Poco::Net::HTTPServer(new S3RequestHandlerFactory(_configuration, _metricService), Poco::Net::ServerSocket(Poco::UInt16(_port)), httpServerParams);

        _httpServer->start();
        log_info_stream(_logger) << "S3 rest service started, endpoint: http://" << _host << ":" << _port << std::endl;
    }
}
