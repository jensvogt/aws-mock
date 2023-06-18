
#include "awsmock/resource/handler/S3Handler.h"

namespace AwsMock {

    S3Handler::S3Handler(Core::Configuration &configuration, Core::MetricService &metricService)
        : AbstractResource(), _logger(Poco::Logger::get("S3Handler")), _configuration(configuration), _metricService(metricService), _s3Service(configuration) {

        // Set console logger
        Core::Logger::SetDefaultConsoleLogger("S3Handler");

        _s3ServiceHost = _configuration.getString("awsmock.service.s3.host", "localhost");
        _s3ServicePort = _configuration.getInt("awsmock.service.s3.port", 9500);
    }

    void S3Handler::handleGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {

        Core::MetricServiceTimer measure(_metricService, HTTP_GET_TIMER);
        _logger.debug() << "S3 GET request, URI: " + request.getURI() << " region: " << region << " user: " + user << std::endl;

        SetHeaders(request, region, user);
        ForwardRequest(request, response, _s3ServiceHost, _s3ServicePort);
    }

    void S3Handler::handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
        Core::MetricServiceTimer measure(_metricService, HTTP_PUT_TIMER);
        _logger.debug() << "S3 PUT request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl << std::endl;

        SetHeaders(request, region, user);
        ForwardRequest(request, response, _s3ServiceHost, _s3ServicePort);
    }

    void S3Handler::handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
        Core::MetricServiceTimer measure(_metricService, HTTP_POST_TIMER);
        _logger.debug() << "S3 POST request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl << std::endl;

        SetHeaders(request, region, user);
        ForwardRequest(request, response, _s3ServiceHost, _s3ServicePort);
    }

    void S3Handler::handleDelete(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
        Core::MetricServiceTimer measure(_metricService, HTTP_DELETE_TIMER);
        _logger.debug() << "S3 DELETE request, URI: " + request.getURI() << " region: " << region << " user: " << user << std::endl << std::endl;

        SetHeaders(request, region, user);
        ForwardRequest(request, response, _s3ServiceHost, _s3ServicePort);
    }

    void S3Handler::handleHead(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
        Core::MetricServiceTimer measure(_metricService, HTTP_HEAD_TIMER);
        _logger.debug() << "S3 HEAD request, address: " << request.clientAddress().toString() << std::endl << std::endl;

        ForwardRequest(request, response, _s3ServiceHost, _s3ServicePort);
    }

    void S3Handler::handleOptions(Poco::Net::HTTPServerResponse &response) {
        Core::MetricServiceTimer measure(_metricService, HTTP_OPTIONS_TIMER);
        _logger.debug() << "S3 OPTIONS request" << std::endl << std::endl;

        response.set("Allow", "GET, PUT, POST, DELETE, HEAD, OPTIONS");
        response.setContentType("text/plain; charset=utf-8");

        handleHttpStatusCode(response, 200);
        std::ostream &outputStream = response.send();
        outputStream.flush();
    }

}
