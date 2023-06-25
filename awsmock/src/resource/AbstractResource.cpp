
#include "awsmock/resource/AbstractResource.h"

namespace AwsMock::Resource {

    AbstractResource::AbstractResource() : _logger(Poco::Logger::get("AbstractResource")), _baseUrl(), _requestURI(), _requestHost() {
        Core::Logger::SetDefaultConsoleLogger("AbstractResource");
    }

    AbstractResource::~AbstractResource() = default;

    void AbstractResource::handleHttpHeaders(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {

        if (request.getMethod() != Poco::Net::HTTPRequest::HTTP_GET && request.getMethod() != Poco::Net::HTTPRequest::HTTP_PUT
            && request.getMethod() != Poco::Net::HTTPRequest::HTTP_POST && request.getMethod() != Poco::Net::HTTPRequest::HTTP_DELETE
            && request.getMethod() != Poco::Net::HTTPRequest::HTTP_OPTIONS && request.getMethod() != Poco::Net::HTTPRequest::HTTP_HEAD) {
            log_error_stream(_logger) << "Invalid request method, method: " << request.getMethod() << std::endl;
            throw HandlerException("Not Implemented", "The request method is not supported by the server and cannot be handled.", 501);
        }

        if (request.has("Accept")) {
            if (!request.get("Accept").empty()) {
                response.setContentType(request.get("Accept"));
            }
        }
    }

    void AbstractResource::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {

        try {
            handleHttpHeaders(request, response);
        } catch (HandlerException &exception) {

            log_error_stream(_logger) << "Exception: msg: " << exception.message() << std::endl;

            handleHttpStatusCode(response, exception.code());
            return;
        }

        Poco::URI uri = Poco::URI(request.getURI());

        std::string scheme, authInfo, region, user;
        request.getCredentials(scheme, authInfo);
        GetRegionUser(authInfo, region, user);

        _queryStringParameters = uri.getQueryParameters();
        uri.getPathSegments(_pathParameter);

        if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
            this->handleGet(request, response, region, user);
        }

        if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_PUT) {
            this->handlePut(request, response, region, user);
        }

        if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST) {
            this->handlePost(request, response, region, user);
        }

        if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_DELETE) {
            this->handleDelete(request, response, region, user);
        }

        if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_OPTIONS) {
            this->handleOptions(response);
        }

        if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_HEAD) {
            this->handleHead(request, response);
        }
    }

    void AbstractResource::handleGet([[maybe_unused]]Poco::Net::HTTPServerRequest &request,
                                     Poco::Net::HTTPServerResponse &response,
                                     [[maybe_unused]]const std::string &region,
                                     [[maybe_unused]]const std::string &user) {
        log_trace_stream(_logger) << "Request, method: " + request.getMethod() << " region: " << region << " user: " << user << std::endl;
        handleHttpStatusCode(response, 501);
        std::ostream &errorStream = response.send();
        errorStream.flush();
    }

    void AbstractResource::handlePut([[maybe_unused]]Poco::Net::HTTPServerRequest &request,
                                     Poco::Net::HTTPServerResponse &response,
                                     [[maybe_unused]]const std::string &region,
                                     [[maybe_unused]]const std::string &user) {
        log_trace_stream(_logger) << "Request, method: " + request.getMethod() << " region: " << region << " user: " << user << std::endl;
        handleHttpStatusCode(response, 501);
        std::ostream &errorStream = response.send();
        errorStream.flush();
    }

    void AbstractResource::handlePost([[maybe_unused]]Poco::Net::HTTPServerRequest &request,
                                      Poco::Net::HTTPServerResponse &response,
                                      [[maybe_unused]]const std::string &region,
                                      [[maybe_unused]]const std::string &user) {
        log_trace_stream(_logger) << "Request, method: " + request.getMethod() << " region: " << region << " user: " << user << std::endl;
        handleHttpStatusCode(response, 501);
        std::ostream &errorStream = response.send();
        errorStream.flush();
    }

    void AbstractResource::handleDelete([[maybe_unused]]Poco::Net::HTTPServerRequest &request,
                                        Poco::Net::HTTPServerResponse &response,
                                        [[maybe_unused]]const std::string &region,
                                        [[maybe_unused]]const std::string &user) {
        log_trace_stream(_logger) << "Request, method: " + request.getMethod() << " region: " << region << " user: " << user << std::endl;
        handleHttpStatusCode(response, 501);
        std::ostream &errorStream = response.send();
        errorStream.flush();
    }

    void AbstractResource::handleOptions(Poco::Net::HTTPServerResponse &response) {
        log_trace_stream(_logger) << "Request, method: OPTIONS" << std::endl;
        response.setStatusAndReason(
            Poco::Net::HTTPResponse::HTTP_NOT_IMPLEMENTED,
            Poco::Net::HTTPResponse::HTTP_REASON_NOT_IMPLEMENTED
        );

        std::ostream &errorStream = response.send();
        errorStream.flush();
    }

    void AbstractResource::handleHead([[maybe_unused]]Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
        log_trace_stream(_logger) << "Request, method: " << request.getMethod() << std::endl;
        response.setStatusAndReason(
            Poco::Net::HTTPResponse::HTTP_NOT_IMPLEMENTED,
            Poco::Net::HTTPResponse::HTTP_REASON_NOT_IMPLEMENTED
        );

        std::ostream &errorStream = response.send();
        errorStream.flush();
    }

    void AbstractResource::handleHttpStatusCode(Poco::Net::HTTPServerResponse &response, int statusCode, const char *reason) {

        switch (statusCode) {

        case 200:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_OK, reason != nullptr ? reason : Poco::Net::HTTPResponse::HTTP_REASON_OK);
            break;

        case 201:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_CREATED, reason != nullptr ? reason : Poco::Net::HTTPResponse::HTTP_REASON_CREATED);
            break;

        case 202:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_ACCEPTED);
            break;

        case 204:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NO_CONTENT);
            break;

        case 205:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_RESET_CONTENT);
            break;

        case 206:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_PARTIAL_CONTENT);
            break;

        case 400:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
            break;

        case 401:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED);
            break;

        case 403:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_FORBIDDEN);
            break;

        case 404:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
            break;

        case 405:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_METHOD_NOT_ALLOWED);
            break;

        case 406:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_ACCEPTABLE);
            break;

        case 409:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_CONFLICT);
            break;

        case 410:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_GONE);
            break;

        case 415:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_UNSUPPORTEDMEDIATYPE);
            break;

        case 500:
            response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR,
                                        reason != nullptr ? reason : Poco::Net::HTTPResponse::HTTP_REASON_INTERNAL_SERVER_ERROR);
            break;

        case 501:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_IMPLEMENTED);
            break;

        case 503:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_SERVICE_UNAVAILABLE);
            break;

            // Validating routines throw exceptions all over the program, but are not able to specify
            // an exception code compatible with HTTP. So, the code is left zero. This routine can catch this.
        default:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
            break;
        }
    }

    std::string AbstractResource::GetQueryParameter(const std::string &parameterKey, bool optional) {

        auto iterator = std::find_if(_queryStringParameters.begin(), _queryStringParameters.end(),
                                     [&parameterKey](const std::pair<std::string, std::string> &item) {
                                       return item.first == parameterKey;
                                     }
        );

        if (iterator == _queryStringParameters.end()) {
            if (optional) {
                return {};
            } else {
                throw HandlerException(Poco::Net::HTTPResponse::HTTP_REASON_BAD_REQUEST,
                                       "MessageAttribute '" + parameterKey + "' is missing at URL.",
                                       Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
            }
        }
        return iterator->second;
    }

    bool AbstractResource::QueryParameterExists(const std::string &parameterKey) {

        auto iterator = std::find_if(_queryStringParameters.begin(), _queryStringParameters.end(),
                                     [&parameterKey](const std::pair<std::string, std::string> &item) {
                                       return item.first == parameterKey;
                                     }
        );

        return iterator != _queryStringParameters.end();
    }

    std::string AbstractResource::GetPathParameter(int pos) {
        return _pathParameter[pos];
    }

    void AbstractResource::GetRegionUser(const std::string &authorization, std::string &region, std::string &user) {
        Poco::RegularExpression::MatchVec posVec;

        Poco::RegularExpression pattern(R"(Credential=([a-zA-Z0-9]+)\/[0-9]{8}\/([a-zA-Z0-9\-]+)\/[a-zA-Z0-9]+\/aws4_request,.*$)");
        if (!pattern.match(authorization, 0, posVec)) {
            throw Core::ResourceNotFoundException("Could not extract region and user");
        }

        user = authorization.substr(posVec[1].offset, posVec[1].length);
        region = authorization.substr(posVec[2].offset, posVec[2].length);
        log_info_stream(_logger) << "Found user: " << user << " region: " << region << std::endl;
    }

    std::string AbstractResource::GetPayload(Poco::Net::HTTPServerRequest &request) {
        std::string payload;
        Poco::StreamCopier::copyToString(request.stream(), payload);
        log_trace_stream(_logger) << "Request payload: " << payload << std::endl;
        return payload;
    }

    void AbstractResource::SendOkResponse(Poco::Net::HTTPServerResponse &response, const std::string &payload, HeaderMap *extraHeader) {
        log_trace_stream(_logger) << "Sending OK response, status: 200 payload: " << payload << std::endl;

        // Get content length
        unsigned long contentLength = 0;
        if (!payload.empty()) {
            contentLength = payload.length();
        }

        // Set headers
        SetHeaders(response, contentLength, extraHeader);

        // Send response
        handleHttpStatusCode(response, 200);
        std::ostream &outputStream = response.send();
        if (!payload.empty()) {
            outputStream << payload;
        }
        outputStream.flush();
    }

    void AbstractResource::SendOkResponse(Poco::Net::HTTPServerResponse &response, const std::string &fileName, long contentLength, HeaderMap *extraHeader) {
        log_trace_stream(_logger) << "Sending OK response, status: 200, filename: " << fileName << " contentLength: " << contentLength << std::endl;
        try {

            // Set headers
            SetHeaders(response, contentLength, extraHeader);

            std::ifstream ifs(fileName);

            // Send response
            handleHttpStatusCode(response, 200);
            std::ostream &outputStream = response.send();
            outputStream << ifs.rdbuf();
            outputStream.flush();
            ifs.close();

        } catch (Poco::Exception &exc) {
            log_error_stream(_logger) << "Exception: " << exc.message() << std::endl;
        }
    }

    void AbstractResource::SendErrorResponse(const std::string &service, Poco::Net::HTTPServerResponse &response, Poco::Exception &exc) {

        Core::ServiceException serviceException = Core::ServiceException(exc.message(), exc.code());
        SendErrorResponse(service, response, serviceException);
    }

    void AbstractResource::SendErrorResponse(const std::string &service, Poco::Net::HTTPServerResponse &response, Core::ServiceException &exc) {

        std::string payload;
        if (service == "SQS") {
            payload = Dto::SQS::RestErrorResponse(exc).ToXml();
        } else if(service == "S3") {
            payload = Dto::S3::RestErrorResponse(exc).ToXml();
        }
        SetHeaders(response, payload.length());

        log_error_stream(_logger) << "Exception, code: " << exc.code() << " message: " << exc.message() << std::endl;
        handleHttpStatusCode(response, exc.code());
        std::ostream &outputStream = response.send();
        outputStream << payload;
        outputStream.flush();
    }

    void AbstractResource::SendErrorResponse(Poco::Net::HTTPServerResponse &response, const std::string &payload) {

        SetHeaders(response, payload.length());

        log_error_stream(_logger) << "Exception, code: " << response.getStatus() << " message: " << payload << std::endl;
        handleHttpStatusCode(response, response.getStatus());
        std::ostream &outputStream = response.send();
        outputStream << payload;
        outputStream.flush();
    }

    void AbstractResource::ForwardRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &host, int port) {

        // Create HTTP request and set headers
        Poco::Net::HTTPClientSession session(host, port);
        log_info_stream(_logger) << "Forward session, host: " << host << " port: " << port << std::endl;

        // Send request with body
        Poco::StreamCopier::copyStream(request.stream(), session.sendRequest(request));
        log_info_stream(_logger) << "Forward request send" << std::endl;

        // Get the response
        std::stringstream body;
        Poco::StreamCopier::copyStream(session.receiveResponse(response), body);
        log_info_stream(_logger) << "Got response from backend service" << std::endl;

        Resource::HeaderMap headerMap;
        auto i = response.begin();
        while (i != response.end()) {
            headerMap.emplace_back(std::make_pair(i->first, i->second));
            ++i;
        }

        if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
            SendOkResponse(response, body.str(), &headerMap);
        } else {
            SendErrorResponse(response, body.str());
        }
        log_info_stream(_logger) << "Backend service response send back to client" << std::endl;
    }

    void AbstractResource::SetHeaders(Poco::Net::HTTPServerResponse &response, unsigned long contentLength, HeaderMap *extraHeader) {
        log_trace_stream(_logger) << "Setting header values, contentLength: " << contentLength << std::endl;

        // Default headers
        response.set("Date", Poco::DateTimeFormatter::format(Poco::DateTime(), Poco::DateTimeFormat::HTTP_FORMAT));
        response.set("Content-Length", std::to_string(contentLength));
        response.set("Content-Type", "text/html; charset=utf-8");
        response.set("Connection", "close");
        response.set("Server", "AmazonS3");

        // Extra headers
        if (extraHeader != nullptr) {
            log_trace_stream(_logger) << "Setting extra header values, count: " << extraHeader->size() << std::endl;
            for (auto &it : *extraHeader) {
                response.set(it.first, it.second);
            }
        }
    }

    void AbstractResource::SetHeaders(Poco::Net::HTTPServerRequest &request, const std::string &region, const std::string &user) {
        log_trace_stream(_logger) << "Setting request header values, region: " << region << " user: " << user << std::endl;

        // Default headers
        request.set("Region", region);
        request.set("User", user);
        request.set("RequestId", Core::AwsUtils::GetRequestId());
        request.set("WriteFile", "true");
    }

    void AbstractResource::DumpRequest(Poco::Net::HTTPServerRequest &request) {
        log_trace_stream(_logger) << "Dump request" << std::endl;
        std::cerr << "==================== Request =====================" << std::endl;
        request.write(std::cerr);
        std::cerr << "==================================================" << std::endl;
    }

    void AbstractResource::DumpResponse(Poco::Net::HTTPServerResponse &response) {
        log_trace_stream(_logger) << "Dump response" << std::endl;
        std::cerr << "==================== Response ====================" << std::endl;
        response.write(std::cerr);
        std::cerr << "==================================================" << std::endl;
    }

    [[maybe_unused]] void AbstractResource::DumpBody(Poco::Net::HTTPServerRequest &request) {
        log_trace_stream(_logger) << "Dump request body" << std::endl;
        std::cerr << "================== Request Body ==================" << std::endl;
        std::cerr << request.stream().rdbuf() << std::endl;
        std::cerr << "==================================================" << std::endl;
    }

    void AbstractResource::DumpBodyToFile(Poco::Net::HTTPServerRequest &request, const std::string &filename) {
        log_trace_stream(_logger) << "Dump request body to file: " + filename << std::endl;
        std::ofstream ofs(filename);
        ofs << request.stream().rdbuf();
        ofs.close();
    }
}
