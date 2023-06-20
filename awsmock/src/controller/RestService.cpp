//
// Created by vogje01 on 04/01/2023.
//

#include "awsmock/controller/RestService.h"

namespace AwsMock {

    RestService::RestService(Core::Configuration &configuration)
        : _port(DEFAULT_PORT), _host(DEFAULT_HOST), _logger(Poco::Logger::get("RestService")), _configuration(configuration)  {

        Core::Logger::SetDefaultConsoleLogger("RestService");

        _port = _configuration.getInt("awsmock.rest.port", DEFAULT_PORT);
        _host = _configuration.getString("awsmock.rest.host", DEFAULT_HOST);
        log_debug_stream(_logger) << "Rest service initialized, endpoint: " << _host << ":" << _port << std::endl;
    }

    RestService::~RestService() {
        if (_router) {
            delete _router;
        }
        if (_httpServer) {
            _httpServer->stopAll(true);
            delete _httpServer;
        }
    }

    void RestService::setPort(int port) {
        _port = port;
    }

    void RestService::setRouter(Poco::Net::HTTPRequestHandlerFactory *router) {
        _router = router;
    }

    Poco::Net::HTTPRequestHandlerFactory *RestService::getRouter() {
        return _router;
    }

    void RestService::start() {
        auto *httpServerParams = new Poco::Net::HTTPServerParams();

        httpServerParams->setMaxQueued(250);
        httpServerParams->setMaxThreads(50);

        _httpServer = new Poco::Net::HTTPServer(getRouter(), Poco::Net::ServerSocket(Poco::UInt16(_port)), httpServerParams);

        _httpServer->start();
        log_info_stream(_logger) << "AwsMock gateway started, endpoint: http://" << _host << ":" << _port << std::endl;
    }

    void RestService::start(Poco::Net::HTTPRequestHandlerFactory *router, int port) {
        setPort(port);
        setRouter(router);
        start();
    }
}
