
#include "awsmock/resource/handler/LambdaHandler.h"

namespace AwsMock {

    LambdaHandler::LambdaHandler(Core::Configuration &configuration, Core::MetricService &metricService)
        : AbstractResource(), _logger(Poco::Logger::get("LambdaHandler")), _configuration(configuration), _metricService(metricService) {

        // Set default console logger
        Core::Logger::SetDefaultConsoleLogger("LambdaHandler");

        _lambdaServiceHost = _configuration.getString("awsmock.service.lambda.host", "localhost");
        _lambdaServicePort = _configuration.getInt("awsmock.service.lambda.port", 9503);
    }

    void LambdaHandler::handleGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {

        Core::MetricServiceTimer measure(_metricService, HTTP_GET_TIMER);
        _logger.debug() << "Lambda GET request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

        SetHeaders(request, region, user);
        ForwardRequest(request, response, _lambdaServiceHost, _lambdaServicePort);
    }

    void LambdaHandler::handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, [[maybe_unused]]const std::string &region, [[maybe_unused]]const std::string &user) {

        Core::MetricServiceTimer measure(_metricService, HTTP_PUT_TIMER);
        _logger.debug() << "Lambda PUT request, URI: " << request.getURI() << " region: " << region << " user: " + user << std::endl;

        SetHeaders(request, region, user);
        ForwardRequest(request, response, _lambdaServiceHost, _lambdaServicePort);
    }

    void LambdaHandler::handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, [[maybe_unused]]const std::string &region, [[maybe_unused]]const std::string &user) {

        Core::MetricServiceTimer measure(_metricService, HTTP_POST_TIMER);
        _logger.debug() << "Lambda POST request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

        SetHeaders(request, region, user);
        ForwardRequest(request, response, _lambdaServiceHost, _lambdaServicePort);
    }

    void LambdaHandler::handleDelete(Poco::Net::HTTPServerRequest &request,
                                     Poco::Net::HTTPServerResponse &response,
                                     [[maybe_unused]]const std::string &region,
                                     [[maybe_unused]]const std::string &user) {

        Core::MetricServiceTimer measure(_metricService, HTTP_DELETE_TIMER);
        _logger.debug() << "Lambda DELETE request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;
        SetHeaders(request, region, user);
        ForwardRequest(request, response, _lambdaServiceHost, _lambdaServicePort);
    }

    void LambdaHandler::handleOptions(Poco::Net::HTTPServerResponse &response) {
        Core::MetricServiceTimer measure(_metricService, HTTP_OPTIONS_TIMER);
        _logger.debug() << "Lambda OPTIONS request" << std::endl;

        response.set("Allow", "GET, PUT, POST, DELETE, HEAD, OPTIONS");
        response.setContentType("text/plain; charset=utf-8");

        handleHttpStatusCode(response, 200);
        std::ostream &outputStream = response.send();
        outputStream.flush();
    }

    void LambdaHandler::handleHead(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
        Core::MetricServiceTimer measure(_metricService, HTTP_HEAD_TIMER);
        _logger.debug() << "Lambda HEAD request, address: " << request.clientAddress().toString() << std::endl;

        try {

            Resource::HeaderMap headerMap;
            headerMap.emplace_back("Connection", "closed");
            headerMap.emplace_back("Server", "AmazonS3");

            SendOkResponse(response, {}, &headerMap);

        } catch (Poco::Exception &exc) {
            SendErrorResponse("Lambda", response, exc);
        }
    }
}
