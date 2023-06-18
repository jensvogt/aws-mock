
#include "awsmock/resource/handler/SNSHandler.h"

namespace AwsMock {

    SNSHandler::SNSHandler(Core::Configuration &configuration, Core::MetricService &metricService)
        : AbstractResource(), _logger(Poco::Logger::get("SNSHandler")), _configuration(configuration), _metricService(metricService) {

        // Set default console logger
        Core::Logger::SetDefaultConsoleLogger("SNSHandler");

        // Service parameter
        _snsServiceHost = _configuration.getString("awsmock.service.sns.host", "localhost");
        _snsServicePort = _configuration.getInt("awsmock.service.sns.port", 9502);
    }

    void SNSHandler::handleGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {

        Core::MetricServiceTimer measure(_metricService, HTTP_GET_TIMER);
        _logger.debug() << "SNS GET request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

        SetHeaders(request, region, user);
        DumpRequest(request);
        DumpResponse(response);
    }

    void SNSHandler::handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {

        Core::MetricServiceTimer measure(_metricService, HTTP_PUT_TIMER);
        _logger.debug() << "SNS PUT request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

        SetHeaders(request, region, user);
        DumpRequest(request);
        DumpResponse(response);
    }

    void SNSHandler::handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {

        Core::MetricServiceTimer measure(_metricService, HTTP_POST_TIMER);
        _logger.trace() << "SNS POST request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

        SetHeaders(request, region, user);
        ForwardRequest(request, response, _snsServiceHost, _snsServicePort);
    }

    void SNSHandler::handleDelete(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {

        Core::MetricServiceTimer measure(_metricService, HTTP_DELETE_TIMER);
        _logger.debug() << "SNS DELETE request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

        SetHeaders(request, region, user);
        DumpRequest(request);
        DumpResponse(response);
    }

    void SNSHandler::handleOptions(Poco::Net::HTTPServerResponse &response) {
        Core::MetricServiceTimer measure(_metricService, HTTP_OPTIONS_TIMER);
        _logger.debug() << "SNS OPTIONS request" << std::endl;

        response.set("Allow", "GET, PUT, POST, DELETE, HEAD, OPTIONS");
        response.setContentType("text/plain; charset=utf-8");

        handleHttpStatusCode(response, 200);
        std::ostream &outputStream = response.send();
        outputStream.flush();
    }
}
