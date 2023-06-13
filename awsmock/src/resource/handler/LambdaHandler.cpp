
#include "awsmock/resource/handler/LambdaHandler.h"

namespace AwsMock {

    LambdaHandler::LambdaHandler(Core::Configuration &configuration, Core::MetricService &metricService)
        : AbstractResource(), _logger(Poco::Logger::get("LambdaHandler")), _configuration(configuration), _metricService(metricService), _lambdaService(configuration) {
        Core::Logger::SetDefaultConsoleLogger("LambdaHandler");
    }

    void LambdaHandler::handleGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, [[maybe_unused]]const std::string &region, [[maybe_unused]]const std::string &user) {
        Core::MetricServiceTimer measure(_metricService, HTTP_GET_TIMER);
        _logger.debug() << "Lambda GET request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

        try {

            std::string version, action;
            GetVersionActionFromUri(request.getURI(), version, action);

        } catch (Core::ServiceException &exc) {
            SendErrorResponse("S3", response, exc);
        } catch (Core::ResourceNotFoundException &exc) {
            SendErrorResponse("Lambda", response, exc);
        }
    }

    void LambdaHandler::handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, [[maybe_unused]]const std::string &region, [[maybe_unused]]const std::string &user) {
        Core::MetricServiceTimer measure(_metricService, HTTP_PUT_TIMER);
        _logger.debug() << "Lambda PUT request, URI: " << request.getURI() << " region: " << region << " user: " + user << std::endl;

        try {
            std::string version, action;
            GetVersionActionFromUri(request.getURI(), version, action);

        } catch (Poco::Exception &exc) {
            SendErrorResponse("Lambda", response, exc);
        }
    }

    void LambdaHandler::handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, [[maybe_unused]]const std::string &region, [[maybe_unused]]const std::string &user) {
        Core::MetricServiceTimer measure(_metricService, HTTP_POST_TIMER);
        _logger.debug() << "Lambda POST request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

        try {
            std::string version, action;
            GetVersionActionFromUri(request.getURI(), version, action);

            if(action == "functions") {

                Dto::Lambda::CreateFunctionRequest lambdaRequest;
                lambdaRequest.FromJson(request.stream());

                Dto::Lambda::CreateFunctionResponse lambdaResponse = _lambdaService.CreateFunctionConfiguration(lambdaRequest);
                SendOkResponse(response, lambdaResponse.ToJson());
            }

        } catch (Poco::Exception &exc) {
            SendErrorResponse("Lambda", response, exc);
        }
    }

    void LambdaHandler::handleDelete(Poco::Net::HTTPServerRequest &request,
                                     Poco::Net::HTTPServerResponse &response,
                                     [[maybe_unused]]const std::string &region,
                                     [[maybe_unused]]const std::string &user) {
        Core::MetricServiceTimer measure(_metricService, HTTP_DELETE_TIMER);
        _logger.debug() << "Lambda DELETE request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

        try {
            std::string version, action;
            GetVersionActionFromUri(request.getURI(), version, action);

            if(Core::StringUtils::StartsWith(action, "functions")) {

                Poco::RegularExpression::MatchVec posVec;
                Poco::RegularExpression pattern(R"(/([a-z0-9-.]+)?/?([a-zA-Z0-9-_/.*'()]+)?\??.*$)");
                if (!pattern.match(action, 0, posVec)) {
                    throw Core::ResourceNotFoundException("Could not extract function name");
                }

                std::string functionName, qualifier;
                if (posVec.size() > 1) {
                    functionName = action.substr(posVec[1].offset, posVec[1].length);
                }
                if (posVec.size() > 2) {
                    qualifier = action.substr(posVec[2].offset, posVec[2].length);
                }

                Dto::Lambda::DeleteFunctionRequest lambdaRequest = {.functionName=functionName, .qualifier=qualifier};
                _lambdaService.DeleteFunction(lambdaRequest);
                SendOkResponse(response);
            }
        } catch (Core::ServiceException &exc) {
            SendErrorResponse("Lambda", response, exc);
        }
    }

    void LambdaHandler::handleOptions(Poco::Net::HTTPServerResponse &response) {
        Core::MetricServiceTimer measure(_metricService, HTTP_OPTIONS_TIMER);
        _logger.debug() << "Lambda OPTIONS request" << std::endl;

        response.set("Allow", "GET, PUT, POST, DELETE, OPTIONS");
        response.setContentType("text/plain; charset=utf-8");

        handleHttpStatusCode(response, 200);
        std::ostream &outputStream = response.send();
        outputStream.flush();
    }

    void LambdaHandler::handleHead(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
        Core::MetricServiceTimer measure(_metricService, HTTP_HEAD_TIMER);
        _logger.debug() << "Lambda HEAD request, address: " << request.clientAddress().toString() << std::endl;

        try {

            std::string version, action;
            GetVersionActionFromUri(request.getURI(), version, action);

            Resource::HeaderMap headerMap;
            headerMap.emplace_back("Connection", "closed");
            headerMap.emplace_back("Server", "AmazonS3");

            SendOkResponse(response, {}, &headerMap);

        } catch (Poco::Exception &exc) {
            SendErrorResponse("Lambda", response, exc);
        }
    }

    void LambdaHandler::GetVersionActionFromUri(const std::string &uri, std::string &version, std::string &action) {

        Poco::RegularExpression::MatchVec posVec;
        Poco::RegularExpression pattern(R"(/([a-z0-9-.]+)?/?([a-zA-Z0-9-_/.*'()]+)?\??.*$)");
        if (!pattern.match(uri, 0, posVec)) {
            throw Core::ResourceNotFoundException("Could not extract version and action");
        }

        if (posVec.size() > 1) {
            version = uri.substr(posVec[1].offset, posVec[1].length);
        }
        if (posVec.size() > 2) {
            action = uri.substr(posVec[2].offset, posVec[2].length);
        }
        _logger.debug() << "Found version and action, version: " << version << " action: " << action << std::endl;
    }
}
