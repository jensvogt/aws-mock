//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_SERVICE_ABSTRACT_SERVER_H
#define AWSMOCK_SERVICE_ABSTRACT_SERVER_H

// C++ standard includes
#include <iostream>
#include <map>
#include <string>
#include <utility>

// Poco includes
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Task.h>

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/CurlUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/Timer.h>
#include <awsmock/entity/module/ModuleState.h>
#include <awsmock/repository/ModuleDatabase.h>

namespace AwsMock::Service {

    /**
     * Abstract HTTP request server
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class AbstractServer : public Core::Timer {

      public:

        /**
         * Constructor
         *
         * @param name manager name
         * @param timeout run timeout in seconds
         */
        explicit AbstractServer(std::string name, int timeout);

        /**
         * Destructor
         */
        virtual ~AbstractServer();

        /**
         * Checks whether the module is active
         *
         * @param name module name
         */
        bool IsActive(const std::string &name);

        /**
         * Returns the running flag
         */
        [[nodiscard]] bool IsRunning() const;

        /**
         * Stop the manager
         */
        void StopServer();

        /**
         * Start the HTTP manager
         *
         * @param maxQueueLength maximal request queue length
         * @param maxThreads maximal number of worker threads
         * @param requestTimeout request timeout in seconds
         * @param host HTTP host name
         * @param port HTTP port
         * @param requestFactory HTTP request factory
         */
        void StartHttpServer(int maxQueueLength, int maxThreads, int requestTimeout, const std::string &host, int port, Poco::Net::HTTPRequestHandlerFactory *requestFactory);

        /**
         * Stops the HTTP manager
         */
        void StopHttpServer();

      protected:

        /**
         * Main loop
         */
        void Run() override = 0;

      private:

        /**
         * Configuration
         */
        Core::Configuration &_configuration;

        /**
         * Service name
         */
        std::string _name;

        /**
         * Service database
         */
        Database::ModuleDatabase &_moduleDatabase;

        /**
         * Shutdown mutex
         */
        Poco::Mutex _mutex;

        /**
         * Running flag
         */
        bool _running;

        /**
         * HTTP manager instance
         */
        std::shared_ptr<Poco::Net::HTTPServer> _httpServer;
    };

    /**
     * Server map
     */
    typedef std::map<std::string, std::shared_ptr<Service::AbstractServer>> ServerMap;

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_ABSTRACT_SERVER_H
