
#include "awsmock/resource/handler/SQSHandler.h"

namespace AwsMock {

    SQSHandler::SQSHandler(Core::Configuration &configuration, Core::MetricService &metricService)
        : AbstractResource(), _logger(Poco::Logger::get("SQSHandler")), _configuration(configuration), _metricService(metricService) {

        // Set default console logger
        Core::Logger::SetDefaultConsoleLogger("SQSHandler");

        _sqsServiceHost = _configuration.getString("awsmock.service.sqs.host", "localhost");
        _sqsServicePort = _configuration.getInt("awsmock.service.sqs.port", 9501);
    }

    void SQSHandler::handleGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {

        Core::MetricServiceTimer measure(_metricService, HTTP_GET_TIMER);
        _logger.debug() << "SQS GET request, URI: " << request.getURI() << " region: " << region << " user: " + user << std::endl;

        SetHeaders(request, region, user);
        DumpRequest(request);
        DumpResponse(response);
    }

    void SQSHandler::handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {

        Core::MetricServiceTimer measure(_metricService, HTTP_PUT_TIMER);
        _logger.debug() << "SQS PUT request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

        SetHeaders(request, region, user);
        DumpRequest(request);
        DumpResponse(response);
    }

    void SQSHandler::handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {

        Core::MetricServiceTimer measure(_metricService, HTTP_POST_TIMER);
        _logger.debug() << "SQS POST request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

        SetHeaders(request, region, user);
        ForwardRequest(request, response, _sqsServiceHost, _sqsServicePort);
    }

    void SQSHandler::handleDelete(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {

        Core::MetricServiceTimer measure(_metricService, HTTP_DELETE_TIMER);
        _logger.debug() << "SQS DELETE request, URI: " + request.getURI() << " region: " << region << " user: " << user << std::endl;

        SetHeaders(request, region, user);
        ForwardRequest(request, response, _sqsServiceHost, _sqsServicePort);
    }

    void SQSHandler::handleOptions(Poco::Net::HTTPServerResponse &response) {
        Core::MetricServiceTimer measure(_metricService, HTTP_OPTIONS_TIMER);
        _logger.debug() << "SQS OPTIONS request" << std::endl;

        response.set("Allow", "GET, PUT, POST, DELETE, HEAD, OPTIONS");
        response.setContentType("text/plain; charset=utf-8");

        handleHttpStatusCode(response, 200);
        std::ostream &outputStream = response.send();
        outputStream.flush();
    }

    void SQSHandler::handleHead([[maybe_unused]]Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
        Core::MetricServiceTimer measure(_metricService, HTTP_OPTIONS_TIMER);
        _logger.debug() << "SQS HEAD request, address: " << request.clientAddress().toString() << std::endl;

        handleHttpStatusCode(response, 200);
        std::ostream &outputStream = response.send();
        outputStream.flush();
    }
}
