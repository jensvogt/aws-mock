#include "awsmock/resource/AbstractResource.h"

namespace AwsMock::Resource {

    AbstractResource::AbstractResource() : _logger(Poco::Logger::get("AbstractResource")), _baseUrl(), _requestURI(), _requestHost() {
        Core::Logger::SetDefaultConsoleLogger("AbstractResource");
    }

    AbstractResource::~AbstractResource() = default;

    void AbstractResource::handleHttpHeaders(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {

        response.setContentType("application/json; charset=utf-8");
        /*std::streamsize contentLength = request.getContentLength();
        std::string contentType = request.getContentType();
        if (contentLength > 0) {
            std::string body = GetBody(request);
            if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_PUT || request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST) {
                if (request.getContentType() != "application/json") {
                    poco_error(_logger, "Invalid content type, contentType: " + request.getContentType());
                    throw HandlerException("Unsupported Media Type", "The only media type supported is application/json and image/jpg.", 415);
                }
            }
        }*/

        if (request.getMethod() != Poco::Net::HTTPRequest::HTTP_GET && request.getMethod() != Poco::Net::HTTPRequest::HTTP_PUT
            && request.getMethod() != Poco::Net::HTTPRequest::HTTP_POST && request.getMethod() != Poco::Net::HTTPRequest::HTTP_DELETE
            && request.getMethod() != Poco::Net::HTTPRequest::HTTP_OPTIONS) {
            poco_error(_logger, "Invalid request method, method: " + request.getMethod());
            throw HandlerException("Not Implemented", "The request method is not supported by the server and cannot be handled.", 501);
        }

        if(request.has("Accept")) {
            if (!request.get("Accept").empty()) {
                response.setContentType(request.get("Accept"));
            }
        }
        poco_debug(_logger, "Header checked");
    }

    void AbstractResource::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {

        try {
            handleHttpHeaders(request, response);
            poco_debug(_logger, "Header checked");
        } catch (HandlerException &exception) {

            poco_error(_logger, "Exception: msg: " + exception.message());

            handleHttpStatusCode(exception.code(), response);

            AwsMock::JsonAPIErrorBuilder errorBuilder = AwsMock::JsonAPIErrorBuilder(request.getHost());

            errorBuilder.sourceAt(request.getURI());
            errorBuilder.withType(exception.type());
            errorBuilder.withStatusCode(exception.code());
            errorBuilder.withDetails(exception.message());

            std::ostream &errorStream = response.send();
            errorStream << errorBuilder.build().toString();

            errorStream.flush();
            return;
        }

        Poco::URI uri = Poco::URI(request.getURI());

        _requestURI = request.getURI();
        _requestHost = request.getHost();
        _baseUrl = "http://" + _requestHost + _requestURI;
        //_queryStringParameters = uri.getQueryParameters();
        uri.getPathSegments(_pathParameter);

        if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
            this->handleGet(request, response);
        }

        if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_PUT) {
            this->handlePut(request, response);
        }

        if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST) {
            this->handlePost(request, response);
        }

        if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_DELETE) {
            this->handleDelete(request, response);
        }

        if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_OPTIONS) {
            this->handleOptions(request, response);
        }

    }

    void AbstractResource::handleGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
        handleHttpStatusCode(501, response);
        std::ostream &errorStream = response.send();
        errorStream.flush();
    }

    void AbstractResource::handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
        handleHttpStatusCode(501, response);
        std::ostream &errorStream = response.send();
        errorStream.flush();
    }

    void AbstractResource::handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
        handleHttpStatusCode(501, response);
        std::ostream &errorStream = response.send();
        errorStream.flush();
    }

    void AbstractResource::handleDelete(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
        handleHttpStatusCode(501, response);
        std::ostream &errorStream = response.send();
        errorStream.flush();
    }

    void AbstractResource::handleOptions(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
        response.setStatusAndReason(
            Poco::Net::HTTPResponse::HTTP_NOT_IMPLEMENTED,
            Poco::Net::HTTPResponse::HTTP_REASON_NOT_IMPLEMENTED
        );

        std::ostream &errorStream = response.send();
        errorStream.flush();
    }

    Poco::JSON::Object::Ptr AbstractResource::getJsonAttributesSectionObject(const std::string &payload) {

        Poco::JSON::Parser jsonParser;
        Poco::Dynamic::Var parsingResult = jsonParser.parse(payload);
        auto jsonObject = parsingResult.extract<Poco::JSON::Object::Ptr>();

        if (jsonObject->isArray("data")) {
            throw HandlerException(Poco::Net::HTTPResponse::HTTP_REASON_BAD_REQUEST,
                                   "This payload can not be represented as a collection.",
                                   Poco::Net::HTTPResponse::HTTP_BAD_REQUEST
            );
        }

        //
        Poco::JSON::Object::Ptr dataObject = jsonObject->getObject("data");

        if (!dataObject->has("attributes")) {
            throw HandlerException(Poco::Net::HTTPResponse::HTTP_REASON_BAD_REQUEST,
                                   "The payload has no an 'attributes' section.",
                                   Poco::Net::HTTPResponse::HTTP_BAD_REQUEST
            );
        }

        return dataObject->getObject("attributes");

    }

    void AbstractResource::assertPayloadAttributes(
        const Poco::JSON::Object::Ptr &payloadObject, const std::list<std::string> &attributes) {

        for (auto const &attribute : attributes) {

            if (!payloadObject->has(attribute)) {
                throw HandlerException(
                    Poco::Net::HTTPResponse::HTTP_REASON_BAD_REQUEST,
                    "One or more attributes are is missing at the payload.",
                    Poco::Net::HTTPResponse::HTTP_BAD_REQUEST
                );
            }
        }
    }

    void AbstractResource::handleHttpStatusCode(int statusCode, Poco::Net::HTTPServerResponse &response) {

        switch (statusCode) {

        case 200:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_OK);
            break;

        case 201:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_CREATED);
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

        case 500:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);

        case 501:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_IMPLEMENTED);

        case 503:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_SERVICE_UNAVAILABLE);

            // Validating routines throw exceptions all over the program, but are not able to specify
            // an exception code compatible with HTTP. So, the code is left zero. This routine can catch this.
        default:response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
        }
    }

    std::string AbstractResource::getUrl(const std::string &fragment) {
        return _baseUrl + fragment;
    }

    std::string AbstractResource::toJson(const HandlerException &exception) {
        AwsMock::JsonAPIErrorBuilder errorBuilder(_requestHost);

        errorBuilder.withType(exception.type());
        errorBuilder.sourceAt(_requestURI);
        errorBuilder.withStatusCode(exception.code());
        errorBuilder.withDetails(exception.message());

        return errorBuilder.build().toString();
    }

    std::string AbstractResource::toJson(const AwsMock::Core::ResourceNotFoundException &exception) {
        AwsMock::JsonAPIErrorBuilder errorBuilder(_requestHost);

        errorBuilder.withType("Server exception");
        errorBuilder.sourceAt(_requestURI);
        errorBuilder.withStatusCode(500);
        errorBuilder.withDetails("Resource not found");

        return errorBuilder.build().toString();
    }

    std::string AbstractResource::getQueryParameter(const std::string &parameterKey) {

        auto iterator = std::find_if(_queryStringParameters.begin(), _queryStringParameters.end(),
                                     [&parameterKey](const std::pair<std::string, std::string> &item) {
                                       return item.first == parameterKey;
                                     }
        );

        if (iterator == _queryStringParameters.end()) {
            throw HandlerException(Poco::Net::HTTPResponse::HTTP_REASON_BAD_REQUEST,
                                   "Attribute '" + parameterKey + "' is missing at URL.",
                                   Poco::Net::HTTPResponse::HTTP_BAD_REQUEST
            );
        }
        return iterator->second;
    }

    std::string AbstractResource::getPathParameter(int pos) {
        return _pathParameter[pos];
    }

    std::string AbstractResource::GetBody(Poco::Net::HTTPServerRequest &request) {
        std::istream &i = request.stream();
        int len = request.getContentLength();
        char *buffer = new char[len];
        i.read(buffer, len);
        return {buffer};
    }
}
