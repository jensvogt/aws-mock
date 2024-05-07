//
// Created by vogje01 on 17/06/2023.
//

#ifndef AWSMOCK_SERVICE_LAMBDA_HANDLER_FACTORY_H
#define AWSMOCK_SERVICE_LAMBDA_HANDLER_FACTORY_H

// Poco includes
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/NotificationQueue.h>

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/MetricService.h>
#include <awsmock/service/lambda/LambdaHandler.h>

namespace AwsMock::Service {

    /**
     * Lambda request handler factory
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class LambdaRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {

      public:

        /**
         * Constructor
         *
         * @param configuration application configuration
         */
        explicit LambdaRequestHandlerFactory(Core::Configuration &configuration) : _configuration(configuration) {}

        /**
         * Create new lambda request handler
         *
         * @param request HTTP request
         * @return lambda request handler
         */
        Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest &request) override {
            if (request.getURI().empty()) {
                return nullptr;
            }
            return new LambdaHandler(_configuration);
        }

      private:

        /**
         * Configuration
         */
        Core::Configuration &_configuration;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_LAMBDA_HANDLER_FACTORY_H
