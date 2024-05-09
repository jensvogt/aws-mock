//
// Created by vogje01 on 17/06/2023.
//

#ifndef AWSMOCK_SERVICE_GATEWAY_HANDLER_FACTORY_H
#define AWSMOCK_SERVICE_GATEWAY_HANDLER_FACTORY_H

// Poco includes
#include <Poco/Net/HTTPRequestHandlerFactory.h>

// AwsMock includes
#include <GatewayHandler.h>
#include <awsmock/core/Configuration.h>
#include <awsmock/core/MetricService.h>

namespace AwsMock::Service {

    /**
     * @brief Gateway request handler factory
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class GatewayRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {

      public:

        /**
         * Constructor
         *
         * @param configuration application configuration
         */
        GatewayRequestHandlerFactory(Core::Configuration &configuration) : _configuration(configuration) {}

        /**
         * Creates a new request handler
         *
         * @param request HTTP request
         * @param host module host
         * @param port module port
         * @return request HTTP request handler
         */
        virtual Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest &request, const std::string &host, int port) {
            return new GatewayHandler(_configuration, host, port);
        }

      private:

        /**
         * Configuration
         */
        Core::Configuration &_configuration;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_GATEWAYHANDLERFACTORY_H
