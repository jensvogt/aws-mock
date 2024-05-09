//
// Created by vogje01 on 04/01/2023.
//

#include <awsmock/controller/Router.h>

namespace AwsMock::Controller {

    Router::Router(Service::ServerMap &serverMap) : _serverMap(serverMap) {

        // Add routes
        AddRoute("module", "AwsMock::Resource::Factory::ModuleFactory");
        log_debug << "Router initialized";
    }

    Poco::Net::HTTPRequestHandler *Router::createRequestHandler(const Poco::Net::HTTPServerRequest &request) {

        // Get the authorization header
        std::string scheme, authInfo;
        request.getCredentials(scheme, authInfo);
        log_debug << "Schema: " << scheme << " Authorization: " << authInfo << "URI: " << request.getURI() << " Method: " + request.getMethod();

        // Get the module from the request authorization header. Currently, no credentials checks are made.
        std::string service = GetModule(authInfo);

        return GetResource(service, request.getURI());
    }

    Poco::Net::HTTPRequestHandler *Router::GetResource(const std::string &service, const std::string &route) {

        Poco::URI uri = Poco::URI(route);

        // Get the resource factory index for the module
        auto factoryIndex = _routingTable.find(service);
        if (factoryIndex == _routingTable.end()) {
            log_error << "No routing found, module: " + service;
            return new AwsMock::Core::ResourceNotFound();
        }

        // Get the resource factory for the module
        Resource::Factory::IFactory *factory = Resource::Factory::Factory::createResourceFactory(factoryIndex->second);
        if (!factory) {
            log_error << "Request handler for route: " << route << " not found";
            return new AwsMock::Core::ResourceNotFound();
        }
        log_debug << "Found request handler for route: " << route << " factory: " << factoryIndex->second;
        return factory->createResource(_serverMap);
    }

    void Router::AddRoute(const std::string &route, const std::string &factory) {

        log_debug << "Route added, route: " << route << " factory: " << factory;
        _routingTable[route] = factory;
    }

    std::string Router::GetModule(const std::string &authorization) {

        Poco::RegularExpression::MatchVec posVec;
        Poco::RegularExpression pattern(R"(Credential=[a-zA-Z0-9]+\/[0-9]{8}\/[a-zA-Z0-9\-]+\/([a-zA-Z0-9]+)\/aws4_request,.*$)");
        if (!pattern.match(authorization, 0, posVec)) {
            log_error << "Could not extract module, authorization: " << authorization;
            throw Core::ResourceNotFoundException("Could not extract module");
        }

        std::string module = authorization.substr(posVec[1].offset, posVec[1].length);
        log_debug << "Found module: " << module;

        return module;
    }
}// namespace AwsMock::Controller
