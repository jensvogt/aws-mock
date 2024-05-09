//
// Created by vogje01 on 03/06/2023.
//

#include "awsmock/service/gateway/GatewayServer.h"

namespace AwsMock::Service {

    GatewayServer::GatewayServer() : AbstractServer("gateway", 10) {

        // Get HTTP configuration values
        Core::Configuration& configuration = Core::Configuration::instance();
        _port = configuration.getInt("awsmock.service.gateway.http.port", GATEWAY_DEFAULT_PORT);
        _host = configuration.getString("awsmock.service.gateway.http.host", GATEWAY_DEFAULT_HOST);
        _maxQueueLength = configuration.getInt("awsmock.service.gateway.http.max.queue", GATEWAY_MAX_QUEUE);
        _maxThreads = configuration.getInt("awsmock.service.gateway.http.max.threads", GATEWAY_MAX_THREADS);
        _requestTimeout = configuration.getInt("awsmock.service.gateway.http.timeout", GATEWAY_TIMEOUT);

        // Sleeping period
        _period = configuration.getInt("awsmock.worker.lambda.period", 10000);
        log_debug << "Gateway worker period: " << _period;

        // Create environment
        _region = configuration.getString("awsmock.region");
        log_debug << "GatewayServer initialized";
    }

    GatewayServer::~GatewayServer() {
        StopServer();
    }

    void GatewayServer::Initialize() {

        // Check module active
        if (!IsActive("gateway")) {
            log_info << "Gateway module inactive";
            return;
        }
        log_info << "Gateway server starting";

        // Start HTTP manager
        StartHttpServer(_maxQueueLength, _maxThreads, _requestTimeout, _host, _port, new GatewayRouter());
    }

    void GatewayServer::Run() {
    }

    void GatewayServer::Shutdown() {
        StopHttpServer();
    }

}// namespace AwsMock::Service
