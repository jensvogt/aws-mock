
#include <awsmock/service/gateway/GatewayHandlerOld.h>

namespace AwsMock::Service {

    void GatewayHandlerOld::handleGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
        log_trace << "Gateway GET request, URI: " << request.getURI() << " region: " << region << " user: " << user << " remoteAddress: " << request.clientAddress().toString();

        Core::MetricServiceTimer measure(GATEWAY_HTTP_TIMER, "GET");
        Core::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "GET");
        log_debug << "Gateway GET request, URI: " + request.getURI() << " region: " << region << " user: " << user;

        SetHeaders(request, region, user);
        AbstractHandler *handler = _route._handler;
        std::make_unique<std::future<void>>((std::async(std::launch::async, [&handler, &request, &response, &region, &user]() {
            handler->handleGet(request, response, region, user);
        }))).reset();
    }

    void GatewayHandlerOld::handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
        log_trace << "Gateway PUT request, URI: " << request.getURI() << " region: " << region << " user: " << user << " remoteAddress: " << request.clientAddress().toString();

        Core::MetricServiceTimer measure(GATEWAY_HTTP_TIMER, "PUT");
        Core::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "PUT");
        log_debug << "Gateway PUT request, URI: " + request.getURI() << " region: " << region << " user: " + user;

        SetHeaders(request, region, user);
        AbstractHandler *handler = _route._handler;
        std::make_unique<std::future<void>>((std::async(std::launch::async, [&handler, &request, &response, &region, &user]() {
            handler->handlePut(request, response, region, user);
        }))).reset();
    }

    void GatewayHandlerOld::handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
        log_trace << "Gateway POST request, URI: " << request.getURI() << " region: " << region << " user: " << user << " remoteAddress: " << request.clientAddress().toString();

        Core::MetricServiceTimer measure(GATEWAY_HTTP_TIMER, "POST");
        Core::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "POST");
        log_trace << "Gateway POST request, URI: " + request.getURI() << " region: " << region << " user: " + user;

        SetHeaders(request, region, user);
        AbstractHandler *handler = _route._handler;
        std::make_unique<std::future<void>>((std::async(std::launch::async, [&handler, &request, &response, &region, &user]() {
            handler->handlePost(request, response, region, user);
        }))).reset();
    }

    void GatewayHandlerOld::handleDelete(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
        log_trace << "Gateway DELETE request, URI: " << request.getURI() << " region: " << region << " user: " << user << " remoteAddress: " << request.clientAddress().toString();

        Core::MetricServiceTimer measure(GATEWAY_HTTP_TIMER, "DELETE");
        Core::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "DELETE");
        log_debug << "Gateway DELETE request, URI: " + request.getURI() << " region: " << region << " user: " + user;

        SetHeaders(request, region, user);
        AbstractHandler *handler = _route._handler;
        std::make_unique<std::future<void>>((std::async(std::launch::async, [&handler, &request, &response, &region, &user]() {
            handler->handleDelete(request, response, region, user);
        }))).reset();
    }

    void GatewayHandlerOld::handleHead(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
        log_trace << "Gateway HEAD request, URI: " << request.getURI() << " region: " << region << " user: " << user << " remoteAddress: " << request.clientAddress().toString();

        Core::MetricServiceTimer measure(GATEWAY_HTTP_TIMER, "HEAD");
        Core::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "HEAD");
        log_debug << "Gateway HEAD request, URI: " + request.getURI() << " region: " << region << " user: " + user;

        SetHeaders(request, region, user);
        AbstractHandler *handler = _route._handler;
        std::make_unique<std::future<void>>((std::async(std::launch::async, [&handler, &request, &response, &region, &user]() {
            handler->handleHead(request, response, region, user);
        }))).reset();
    }

    void GatewayHandlerOld::handleOptions(Poco::Net::HTTPServerResponse &response) {
        log_debug << "Gateway OPTIONS request";

        Core::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "OPTIONS");
        response.set("Allow", "GET, PUT, POST, DELETE, OPTIONS");
        response.setContentType("text/plain; charset=utf-8");

        handleHttpStatusCode(response, Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        std::ostream &outputStream = response.send();
        outputStream.flush();
    }

    void GatewayHandlerOld::SetHeaders(Poco::Net::HTTPServerRequest &request, const std::string &region, const std::string &user) {
        log_trace << "Setting request header values, region: " << region << " user: " << user;

        // Default headers
        request.set("Region", region);
        request.set("User", user);
        request.set("RequestId", Core::AwsUtils::CreateRequestId());
    }

}// namespace AwsMock::Service