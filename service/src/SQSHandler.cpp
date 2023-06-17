
#include "awsmock/service/SQSHandler.h"

namespace AwsMock::Service {

    SQSHandler::SQSHandler(Core::Configuration &configuration, Core::MetricService &metricService)
        : AbstractHandler(), _logger(Poco::Logger::get("SQSServiceHandler")), _configuration(configuration), _metricService(metricService), _sqsService(configuration) {

        // Set console logger
        Core::Logger::SetDefaultConsoleLogger("SQSServiceHandler");

        _accountId = Core::AwsUtils::GetDefaultAccountId();
    }

    void SQSHandler::handleGet(Poco::Net::HTTPServerRequest &request,
                               Poco::Net::HTTPServerResponse &response,
                               [[maybe_unused]] const std::string &region,
                               [[maybe_unused]]const std::string &user) {
        Core::MetricServiceTimer measure(_metricService, HTTP_GET_TIMER);
        _logger.debug() << "SQS GET request, URI: " << request.getURI() << " region: " << region << " user: " + user << std::endl;
        DumpRequest(request);
        DumpResponse(response);
    }

    void SQSHandler::handlePut(Poco::Net::HTTPServerRequest &request,
                               Poco::Net::HTTPServerResponse &response,
                               [[maybe_unused]]const std::string &region,
                               [[maybe_unused]]const std::string &user) {
        Core::MetricServiceTimer measure(_metricService, HTTP_PUT_TIMER);
        _logger.debug() << "SQS PUT request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;
        DumpRequest(request);
        DumpResponse(response);
    }

    void SQSHandler::handlePost(Poco::Net::HTTPServerRequest &request,
                                Poco::Net::HTTPServerResponse &response,
                                [[maybe_unused]]const std::string &region,
                                [[maybe_unused]]const std::string &user) {
        Core::MetricServiceTimer measure(_metricService, HTTP_POST_TIMER);
        _logger.debug() << "SQS POST request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

        try {
            //DumpBody(request);

            std::string endpoint = GetEndpoint(request);
            std::string payload = GetPayload(request);
            std::string action, version;

            GetActionVersion(payload, action, version);

            if (action == "CreateQueue") {

                std::string name = GetStringParameter(payload, "QueueName");

                Dto::SQS::CreateQueueRequest sqsRequest = {.region=region, .name = name, .queueUrl="http://" + endpoint + "/" + _accountId + "/" + name, .owner=user};
                Dto::SQS::CreateQueueResponse sqsResponse = _sqsService.CreateQueue(sqsRequest);
                SendOkResponse(response, sqsResponse.ToXml());

            } else if (action == "ListQueues") {

                Dto::SQS::ListQueueResponse sqsResponse = _sqsService.ListQueues(region);
                SendOkResponse(response, sqsResponse.ToXml());

            } else if (action == "DeleteQueue") {

                std::string url = GetStringParameter(payload, "QueueUrl");

                Dto::SQS::DeleteQueueRequest sqsRequest = {.queueUrl=url};
                Dto::SQS::DeleteQueueResponse sqsResponse = _sqsService.DeleteQueue(sqsRequest);
                SendOkResponse(response, sqsResponse.ToXml());

            } else if (action == "SendMessage") {

                std::string url = GetStringParameter(payload, "QueueUrl");
                std::string body = GetStringParameter(payload, "MessageBody");

                Dto::SQS::CreateMessageRequest sqsRequest = {.region=region, .url=url, .body=body};
                Dto::SQS::CreateMessageResponse sqsResponse = _sqsService.CreateMessage(sqsRequest);
                SendOkResponse(response, sqsResponse.ToXml());

            } else if (action == "ReceiveMessage") {

                std::string queueUrl = GetStringParameter(payload, "QueueUrl");
                int maxMessages = GetIntParameter(payload, "MaxNumberOfMessages", 1, 10, 3);
                int waitTimeSeconds = GetIntParameter(payload, "WaitTimeSeconds", 1, 900, 5);
                int visibility = GetIntParameter(payload, "VisibilityTimeout", 1, 900, 30);

                Dto::SQS::ReceiveMessageRequest sqsRequest = {.region=region, .queueUrl=queueUrl, .maxMessages=maxMessages, .visibility=visibility,
                    .waitTimeSeconds=waitTimeSeconds};
                Dto::SQS::ReceiveMessageResponse sqsResponse = _sqsService.ReceiveMessages(sqsRequest);

                SendOkResponse(response, sqsResponse.ToXml());

            } else if (action == "PurgeQueue") {

                std::string queueUrl = GetStringParameter(payload, "QueueUrl");

                Dto::SQS::PurgeQueueRequest sqsRequest = {.queueUrl=queueUrl, .region=region};
                Dto::SQS::PurgeQueueResponse sqsResponse = _sqsService.PurgeQueue(sqsRequest);

                SendOkResponse(response, sqsResponse.ToXml());

            } else if (action == "GetQueueAttributes") {

                std::string queueUrl = GetStringParameter(payload, "QueueUrl");

                int count = GetAttributeCount(payload, "AttributeName");
                _logger.trace() << "Got attribute names count: " << count << std::endl;

                std::vector<std::string> attributeNames;
                for(int i = 1; i <= count; i++) {
                    std::string attributeName = GetStringParameter(payload, "AttributeName." + std::to_string(i));
                    attributeNames.emplace_back(attributeName);
                }

                Dto::SQS::GetQueueAttributesRequest sqsRequest = {.queueUrl=queueUrl, .attributeNames=attributeNames};
                Dto::SQS::GetQueueAttributesResponse sqsResponse = _sqsService.GetQueueAttributes(sqsRequest);

                SendOkResponse(response, sqsResponse.ToXml());

            } else if (action == "SetQueueAttributes") {

                std::string queueUrl = GetStringParameter(payload, "QueueUrl");

                int count = GetAttributeCount(payload, "Attribute");
                _logger.trace() << "Got attribute count, count: " << count << std::endl;

                AttributeList attributes;
                for(int i = 1; i <= count; i++) {
                    std::string attributeName = GetStringParameter(payload, "Attribute." + std::to_string(i) + ".Name");
                    std::string attributeValue = GetStringParameter(payload, "Attribute." + std::to_string(i) + ".Value");
                    attributes[attributeName] = attributeValue;
                }

                Dto::SQS::SetQueueAttributesRequest sqsRequest = {.queueUrl=queueUrl, .attributes=attributes};
                Dto::SQS::SetQueueAttributesResponse sqsResponse = _sqsService.SetQueueAttributes(sqsRequest);

                SendOkResponse(response, sqsResponse.ToXml());

            } else if (action == "DeleteMessage") {

                std::string queueUrl = GetStringParameter(payload, "QueueUrl");
                std::string receiptHandle = GetStringParameter(payload, "ReceiptHandle");

                Dto::SQS::DeleteMessageRequest sqsRequest = {.queueUrl=queueUrl, .receiptHandle=receiptHandle};
                Dto::SQS::DeleteMessageResponse sqsResponse = _sqsService.DeleteMessage(sqsRequest);

                SendOkResponse(response, sqsResponse.ToXml());
            }

        } catch (Core::ServiceException &exc) {
            _logger.error() << "Service exception: " << exc.message() << std::endl;
            SendErrorResponse("SQS", response, exc);
        }
    }

    void SQSHandler::handleDelete(Poco::Net::HTTPServerRequest &request,
                                  Poco::Net::HTTPServerResponse &response,
                                  [[maybe_unused]]const std::string &region,
                                  [[maybe_unused]]const std::string &user) {
        Core::MetricServiceTimer measure(_metricService, HTTP_DELETE_TIMER);
        _logger.debug() << "SQS DELETE request, URI: " + request.getURI() << " region: " << region << " user: " << user << std::endl;
        DumpRequest(request);
        DumpResponse(response);
    }

    void SQSHandler::handleOptions(Poco::Net::HTTPServerResponse &response) {
        Core::MetricServiceTimer measure(_metricService, HTTP_OPTIONS_TIMER);
        _logger.debug() << "SQS OPTIONS request" << std::endl;

        response.set("Allow", "GET, PUT, POST, DELETE, OPTIONS");
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

    void SQSHandler::GetActionVersion(const std::string &body, std::string &action, std::string &version) {
        std::vector<std::string> bodyParts = Core::StringUtils::Split(body, '&');
        for (auto &it : bodyParts) {
            std::vector<std::string> parts = Core::StringUtils::Split(it, '=');
            if (parts.size() < 2) {
                throw Core::ServiceException("Invalid request body", 400);
            }
            if (parts[0] == "Action") {
                action = parts[1];
            }
            if (parts[0] == "Version") {
                version = parts[1];
            }
        }
        _logger.debug() << "Found action: " << action << " version: " << version << std::endl;
    }

    std::string SQSHandler::GetStringParameter(const std::string &body, const std::string &name) {
        std::string value;
        std::vector<std::string> bodyParts = Core::StringUtils::Split(body, '&');
        for (auto &it : bodyParts) {
            std::vector<std::string> parts = Core::StringUtils::Split(it, '=');
            if (parts[0] == name) {
                value = Core::StringUtils::UrlDecode(parts[1]);
            }
        }
        _logger.debug() << "Found string parameter, name: " << name << " value: " << value << std::endl;
        return value;
    }

    int SQSHandler::GetIntParameter(const std::string &body, const std::string &name, int min, int max, int def) {
        int value = def;
        std::string parameterValue = GetStringParameter(body, name);
        if (!parameterValue.empty()) {
            value = std::stoi(parameterValue);
            value = value > min && value < max ? value : def;
        }
        _logger.debug() << "Found integer name, name: " << name << " value: " << value << std::endl;
        return value;
    }

    int SQSHandler::GetAttributeCount(const std::string &body, const std::string &parameter) {
        int count = 0;
        std::vector<std::string> bodyParts = Core::StringUtils::Split(body, '&');
        for (auto &it : bodyParts) {
            if(it.starts_with(parameter)) {
                count++;
            }
        }
        return count / 2;
    }

    std::string SQSHandler::GetEndpoint(Poco::Net::HTTPServerRequest &request) {
        return request.get("Host");
    }
}
