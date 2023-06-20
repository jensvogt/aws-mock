
#include "awsmock/service/SNSHandler.h"

namespace AwsMock::Service {

    SNSHandler::SNSHandler(Core::Configuration &configuration, Core::MetricService &metricService)
        : AbstractHandler(), _logger(Poco::Logger::get("SNSServiceHandler")), _configuration(configuration), _metricService(metricService), _snsService(configuration) {

        // Set default console logger
        Core::Logger::SetDefaultConsoleLogger("SNSServiceHandler");
    }

    void SNSHandler::handleGet(Poco::Net::HTTPServerRequest &request,
                               Poco::Net::HTTPServerResponse &response,
                               [[maybe_unused]]const std::string &region,
                               [[maybe_unused]]const std::string &user) {
        Core::MetricServiceTimer measure(_metricService, HTTP_GET_TIMER);
        log_debug_stream(_logger) << "SNS GET request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;
        DumpRequest(request);
    }

    void SNSHandler::handlePut(Poco::Net::HTTPServerRequest &request,
                               Poco::Net::HTTPServerResponse &response,
                               [[maybe_unused]]const std::string &region,
                               [[maybe_unused]]const std::string &user) {
        Core::MetricServiceTimer measure(_metricService, HTTP_PUT_TIMER);
        log_debug_stream(_logger) << "SNS PUT request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;
        DumpRequest(request);
    }

    void SNSHandler::handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
        Core::MetricServiceTimer measure(_metricService, HTTP_POST_TIMER);
        log_trace_stream(_logger) << "SNS POST request, URI: " << request.getURI() << " region: " << region << " user: " << user << " length: " << response.getContentLength() << std::endl;

        try {
            std::string payload = GetPayload(request);
            std::string action, version;

            GetActionVersion(payload, action, version);

            if (action == "CreateTopic") {

                std::string name = GetStringParameter(payload, "Name");
                log_debug_stream(_logger) << "Topic name: " << name << std::endl;

                Dto::SNS::CreateTopicRequest snsRequest = {.region=region, .topicName = name, .owner=user};
                Dto::SNS::CreateTopicResponse snsResponse = _snsService.CreateTopic(snsRequest);
                SendOkResponse(response, snsResponse.ToXml());

            } else if(action == "ListTopics") {

                Dto::SNS::ListTopicsResponse snsResponse = _snsService.ListTopics(region);
                SendOkResponse(response, snsResponse.ToXml());

            } else if(action == "Publish") {

                std::string topicArn = GetStringParameter(payload, "TopicArn");
                std::string targetArn = GetStringParameter(payload, "TargetArn");
                std::string message = GetStringParameter(payload, "Message");

                Dto::SNS::PublishResponse snsResponse = _snsService.Publish({.region=region, .topicArn=topicArn, .targetArn=targetArn, .message=message});
                SendOkResponse(response, snsResponse.ToXml());

            } else if(action == "Subscribe") {

                std::string topicArn = GetStringParameter(payload, "TopicArn");
                std::string protocol = GetStringParameter(payload, "Protocol");
                std::string endpoint = GetStringParameter(payload, "Endpoint");

                Dto::SNS::SubscribeResponse
                    snsResponse = _snsService.Subscribe({.region=region, .topicArn=topicArn, .protocol=protocol, .endpoint=endpoint, .owner=user});
                SendOkResponse(response, snsResponse.ToXml());

            } else if(action == "DeleteTopic") {

                std::string topicArn = GetStringParameter(payload, "TopicArn");
                log_debug_stream(_logger) << "Topic ARN: " << topicArn << std::endl;

                Dto::SNS::DeleteTopicResponse snsResponse = _snsService.DeleteTopic(region, topicArn);
                SendOkResponse(response, snsResponse.ToXml());
            }

        } catch (Core::ServiceException &exc) {
            log_error_stream(_logger) << "Service exception: " << exc.message() << std::endl;
            SendErrorResponse("SNS", response, exc);
        }
    }

    void SNSHandler::handleDelete(Poco::Net::HTTPServerRequest &request,
                                  Poco::Net::HTTPServerResponse &response,
                                  [[maybe_unused]]const std::string &region,
                                  [[maybe_unused]]const std::string &user) {
        Core::MetricServiceTimer measure(_metricService, HTTP_DELETE_TIMER);
        log_debug_stream(_logger) << "SNS DELETE request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;
        log_debug_stream(_logger) << "SNS DELETE request, URI: " << response.getContentLength() << std::endl;
        DumpRequest(request);
    }

    void SNSHandler::handleOptions(Poco::Net::HTTPServerResponse &response) {
        Core::MetricServiceTimer measure(_metricService, HTTP_OPTIONS_TIMER);
        log_debug_stream(_logger) << "SNS OPTIONS request" << std::endl;

        response.set("Allow", "GET, PUT, POST, DELETE, OPTIONS");
        response.setContentType("text/plain; charset=utf-8");

        handleHttpStatusCode(response, 200);
        std::ostream &outputStream = response.send();
        outputStream.flush();
    }

    void SNSHandler::handleHead([[maybe_unused]]Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
        Core::MetricServiceTimer measure(_metricService, HTTP_OPTIONS_TIMER);
        log_debug_stream(_logger) << "SNS HEAD request, address: " << request.clientAddress().toString() << std::endl;

        handleHttpStatusCode(response, 200);
        std::ostream &outputStream = response.send();
        outputStream.flush();
    }
}
