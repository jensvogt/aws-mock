//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_SERVER_GATEWAY_SERVER_H
#define AWSMOCK_SERVER_GATEWAY_SERVER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/MetricService.h>
#include <awsmock/dto/lambda/model/InvocationNotification.h>
#include <awsmock/repository/LambdaDatabase.h>
#include <awsmock/repository/ModuleDatabase.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/gateway/GatewayRouter.h>
#include <awsmock/service/lambda/LambdaCreator.h>
#include <awsmock/service/lambda/LambdaExecutor.h>
#include <awsmock/service/lambda/LambdaHandlerFactory.h>
#include <awsmock/service/s3/S3Service.h>

#define GATEWAY_DEFAULT_HOST "localhost"
#define GATEWAY_MAX_QUEUE 250
#define GATEWAY_MAX_THREADS 50
#define GATEWAY_TIMEOUT 900

namespace AwsMock::Service {

    /**
     * @brief Gateway server
     *
     * All AwsMock HTTP request end here and will be routed to the corresponding handler.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class GatewayServer : public AbstractServer {

      public:

        /**
         * Constructor
         */
        explicit GatewayServer();

        /**
         * Destructor
         */
        ~GatewayServer() override;

        /**
         * Timer initialization
         */
        void Initialize() override;

        /**
         * Main method
         */
        void Run() override;

        /**
         * Shutdown
         */
        void Shutdown() override;

      private:

        /**
         * Service database
         */
        std::unique_ptr<Database::ModuleDatabase> _serviceDatabase;

        /**
         * AWS region
         */
        std::string _region;

        /**
         * Sleeping period in ms
         */
        int _period;

        /**
         * Rest port
         */
        int _port;

        /**
         * Rest host
         */
        std::string _host;

        /**
         * HTTP max message queue length
         */
        int _maxQueueLength;

        /**
         * HTTP max concurrent connection
         */
        int _maxThreads;

        /**
         * HTTP request timeout
         */
        int _requestTimeout;

        /**
         * Gateway router
         */
        std::shared_ptr<Service::GatewayRouter> _router = std::make_shared<Service::GatewayRouter>();
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVER_GATEWAY_SERVER_H
