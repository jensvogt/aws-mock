//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_MANAGER_ROUTER_H
#define AWSMOCK_MANAGER_ROUTER_H

// C++ standard includes
#include <iostream>
#include <map>
#include <string>

// Poco includes
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/URI.h>

// AwsMock includes
#include <awsmock/controller/ResourceNotFound.h>
#include <awsmock/core/Configuration.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/resource/factory/Factory.h>
#include <awsmock/resource/factory/IFactory.h>
#include <awsmock/service/common/AbstractServer.h>

namespace AwsMock::Controller {

    /**
     * Management router
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class Router : public Poco::Net::HTTPRequestHandlerFactory {

      public:

        /**
         * Constructor
         *
         * @param serverMap map of services
         */
        explicit Router(Service::ServerMap &serverMap);

        /**
         * Add a route
         *
         * @param route request route
         * @param factory resource factory
         */
        void AddRoute(const std::string &route, const std::string &factory);

        /**
         * HTTP request handler
         *
         * @param request HTTP request
         * @return request handler or null in case of failure
         */
        Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest &request) override;

      private:

        /**
         * Return HTTP restful resource.
         *
         * @param service AWS module name
         * @param uri request URI
         * @return restfull resource
         */
        Poco::Net::HTTPRequestHandler *GetResource(const std::string &service, const std::string &uri);

        /**
         * Returns the AWS module, region and user from the authorization string.
         *
         * @param authorization authorization string
         * @return module name
         */
        static std::string GetModule(const std::string &authorization);

        /**
         * Server map
         */
        Service::ServerMap &_serverMap;

        /**
         * Routing table
         */
        std::map<std::string, std::string> _routingTable;
    };

}// namespace AwsMock::Controller

#endif// AWSMOCK_MANAGER_ROUTER_H
