//
// Created by vogje01 on 04/01/2023.
//

#include "awsmock/service/SQSServer.h"

namespace AwsMock::Service {

    SQSServer::SQSServer(Core::Configuration &configuration, Core::MetricService &metricService)
        : _logger(Poco::Logger::get("S3Server")), _configuration(configuration), _metricService(metricService) {

        Core::Logger::SetDefaultConsoleLogger("S3Server");

        _port = _configuration.getInt("awsmock.service.s3.port", S3_DEFAULT_PORT);
        _host = _configuration.getString("awsmock.service.s3.host", S3_DEFAULT_HOST);
        _logger.debug() << "S3 rest service initialized, endpoint: " << _host << ":" << _port << std::endl;
    }

    SQSServer::~SQSServer() {
        _httpServer->stopAll(true);
        delete _httpServer;
    }

    void SQSServer::start() {
        auto *httpServerParams = new Poco::Net::HTTPServerParams();

        httpServerParams->setMaxQueued(250);
        httpServerParams->setMaxThreads(50);

        _httpServer =
            new Poco::Net::HTTPServer(new SQSRequestHandlerFactory(_configuration, _metricService), Poco::Net::ServerSocket(Poco::UInt16(_port)), httpServerParams);

        _httpServer->start();
        _logger.information() << "SQS rest service started, endpoint: http://" << _host << ":" << _port << std::endl;
    }
}
