//
// Created by vogje01 on 04/01/2023.
//

#include "awsmock/controller/Router.h"

namespace AwsMock::Controller {

    Router::Router(Configuration &configuration, Core::MetricService &metricService)
        : _logger(Poco::Logger::get("Router")), _configuration(configuration), _metricService(metricService) {
        Core::Logger::SetDefaultConsoleLogger("Router");

        AddRoute("s3", "AwsMock::Resource::Factory::S3Factory");
        AddRoute("s3api", "AwsMock::Resource::Factory::S3Factory");
        AddRoute("sqs", "AwsMock::Resource::Factory::SQSFactory");
        AddRoute("sns", "AwsMock::Resource::Factory::SNSFactory");
        AddRoute("lambda", "AwsMock::Resource::Factory::LambdaFactory");
    }

    Router::~Router() {
        _routingTable.clear();
    }

    Poco::Net::HTTPRequestHandler *Router::createRequestHandler(const Poco::Net::HTTPServerRequest &request) {
        std::string scheme, authInfo;
        request.getCredentials(scheme, authInfo);
        poco_debug(_logger, "Schema: " + scheme + " Authorization: " + authInfo);
        poco_debug(_logger, "URI: " + request.getURI() + " Method: " + request.getMethod());

        // Get the service from the request authorization header. Currently no credentials checks are made.
        std::string service = GetService(authInfo);

        return GetResource(service, request.getURI());
    }

    Poco::Net::HTTPRequestHandler *Router::GetResource(const std::string &service, const std::string &route) {

        Poco::URI uri = Poco::URI(route);

        // Get the resource factory index for the service
        auto factoryIndex = _routingTable.find(service);
        if (factoryIndex == _routingTable.end()) {
            return new AwsMock::ResourceNotFound();
        }

        // Get the resource factory for the service
        Resource::Factory::IFactory *factory = Resource::Factory::Factory::createResourceFactory(factoryIndex->second);
        if(!factory) {
            poco_error(_logger, "Request handler for route: " + route + " not found");
            return new AwsMock::ResourceNotFound();
        }
        poco_debug(_logger, "Found request handler for route: " + route + " factory: " + factoryIndex->second);

        return factory->createResource(_configuration, _metricService);
    }

    void Router::AddRoute(const std::string &route, const std::string &factory) {
        poco_debug(_logger, "Route added, route: " + route + " factory: " + factory);
        _routingTable[route] = factory;
    }

    std::string Router::GetService(const std::string &authorization) {
        Poco::RegularExpression::MatchVec posVec;

        Poco::RegularExpression pattern(R"(Credential=[a-zA-Z]+\/[0-9]{8}\/[a-zA-Z0-9\-]+\/([a-zA-Z0-9]+)\/aws4_request,.*$)");
        if (!pattern.match(authorization, 0, posVec)) {
            throw Core::ResourceNotFoundException("Could not extract service");
        }

        std::string service = authorization.substr(posVec[1].offset, posVec[1].length);
        poco_debug(_logger, "Found service: " + service);
        return service;
    }

} // namespace AwsMock::Controller
