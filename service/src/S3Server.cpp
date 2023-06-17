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
        _logger.debug() << "S3 rest service initialized, endpoint: " << _host << ":" << _port << std::endl;
    }

    S3Server::~S3Server() {
        _httpServer->stopAll(true);
        delete _httpServer;
    }

    void S3Server::start() {
        auto *httpServerParams = new Poco::Net::HTTPServerParams();

        httpServerParams->setMaxQueued(250);
        httpServerParams->setMaxThreads(50);

        _httpServer =
            new Poco::Net::HTTPServer(new S3RequestHandlerFactory(_configuration, _metricService), Poco::Net::ServerSocket(Poco::UInt16(_port)), httpServerParams);

        _httpServer->start();
        _logger.information() << "S3 rest service started, endpoint: http://" << _host << ":" << _port << std::endl;
    }
}