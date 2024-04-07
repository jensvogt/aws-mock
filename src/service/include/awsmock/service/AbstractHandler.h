//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_ABSTRACT_HANDLER_H
#define AWSMOCK_SERVICE_ABSTRACT_HANDLER_H

// C++ includes
#include <string>
#include <fstream>
#include <streambuf>

// Poco includes
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/Logger.h"
#include "Poco/URI.h"
#include "Poco/StreamCopier.h"
#include <Poco/RegularExpression.h>
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPRequestHandler.h"

// AwsMock includes
#include <awsmock/core/InvalidMethodCallException.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/MemoryMappedFile.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/ServiceException.h>
#include <awsmock/core/ResourceNotFoundException.h>
#include <awsmock/dto/common/UserAgent.h>
#include <awsmock/dto/common/S3ClientCommand.h>
#include <awsmock/dto/s3/RestErrorResponse.h>
#include <awsmock/dto/sqs/RestErrorResponse.h>

namespace AwsMock::Service {

  typedef std::map<std::string, std::string> HeaderMap;

  /**
   * Abstract HTTP request handler
   */
  class AbstractHandler : public Poco::Net::HTTPRequestHandler {

  public:
    /**
     * Default User-defined Constructor
     */
    AbstractHandler();

    /**
     * Default Destructor
     */
    ~AbstractHandler() override;

    /**
     * Handle requests.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @see Poco::Net::HTTPRequestHandler
     */
    void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) override;

  protected:

    /**
     * Handles the HTTP method GET.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @param region AWS region
     * @param user current user
     */
    virtual void handleGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user);

    /**
     * Handles the HTTP method GET.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @param s3Command S3 client command
     */
    virtual void handleGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const Dto::Common::S3ClientCommand &s3Command);

    /**
     * Handles the HTTP method PUT.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @param region AWS region
     * @param user current user
     */
    virtual void handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user);

    /**
     * Handles the HTTP method PUT.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @param s3Command S3 client command
     */
    virtual void handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const Dto::Common::S3ClientCommand &s3Command);

    /**
     * Handles the HTTP method POST.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @param region AWS region
     * @param user current user
     */
    virtual void handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user);

    /**
     * Handles the HTTP method POST.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @param s3Command S3 client command
     */
    virtual void handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const Dto::Common::S3ClientCommand &s3Command);

    /**
     * Handles the HTTP method DELETE.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @param region AWS region
     * @param user current user
     */
    virtual void handleDelete(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user);

    /**
     * Handles the HTTP method DELETE.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @param s3Command S3 client command
     */
    virtual void handleDelete(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const Dto::Common::S3ClientCommand &s3Command);

    /**
     * Handles the HTTP OPTIONS method.
     *
     * @param response HTTP response
     */
    virtual void handleOptions(Poco::Net::HTTPServerResponse &response);

    /**
     * Handles the HTTP HEAD method.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @param region AWS region
     * @param user current user
     */
    virtual void handleHead(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user);

    /**
     * It validates required information into the Http headers.
     *
     * @param request HTTP request
     * @param response HTTP response
     */
    virtual void handleHttpHeaders(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);

    /**
     * It sets all the HTTP Response information based on the HTTP Code.
     *
     * @param response response to be handled.
     * @param statusCode HTTP Status Code.
     * @param reason state reason.
     */
    void handleHttpStatusCode(Poco::Net::HTTPServerResponse &response, int statusCode, const char *reason = nullptr);

    /**
     * Returns a path parameter by position.
     *
     * @param pos parameter position.
     * @return The parameter value.
     */
    std::string GetPathParameter(int pos);

    /**
     * Returns the region and the user
     *
     * @param authorization HTTP authorization string
     * @param region AWS region
     * @param user AWS user
     */
    void GetRegionUser(const std::string &authorization, std::string &region, std::string &user);

    /**
     * Returns the payload as a string
     *
     * @param request HTTP request
     * @return payload as a string,
     */
    std::string GetPayload(Poco::Net::HTTPServerRequest &request);

    /**
     * Get the action from the request body
     *
     * <p>Returns the action/version parameter from the message body. This is mainly used by SQS.</p>
     *
     * @param body HTTP request body (in)
     * @param action SQS action (out)
     * @param version SQS version (out)
     */
    void GetActionVersion(const std::string &body, std::string &action, std::string &version);

    /**
     * Get the action from the request path
     *
     * <p>Returns a string parameter from the message path. This is mainly used by SQS.</p>
     *
     * @param path HTTP request path
     * @param name parameter name
     * @return parameter value
     */
    std::string GetStringParameter(const std::string &path, const std::string &name);

    /**
     * Return an integer name.
     *
     * <p>Returns an integer parameter from the message body. This is mainly used by SQS.</p>
     *
     * @param body HTTP message body
     * @param name name name
     * @param min minimum value
     * @param max maximum value
     * @param def default value
     * @return integer name
     */
    int GetIntParameter(const std::string &body, const std::string &name, int min, int max, int def);

    /**
     * Returns the attribute count.
     *
     * <p>Returns the number of name from the message body. This is mainly used by SQS.</p>
     *
     * @param body HTTP message body
     * @param name name name
     * @return number of sqs
     */
    int GetAttributeCount(const std::string &body, const std::string &name);

    /**
     * Get the attribute name count
     *
     * @param body message body
     * @param name attribute name
     * @return number of atributes with the given name
     */
    int GetAttributeNameCount(const std::string &body, const std::string &name);

    /**
     * Returns the version and action from URI
     *
     * @param uri URI to scan
     * @param version version string
     * @param action action
     */
    void GetVersionActionFromUri(const std::string &uri, std::string &version, std::string &action);

    /**
     * Get the endpoint from the request header
     *
     * @param request HTTP request
     * @return endpoint
     */
    static std::string GetEndpoint(Poco::Net::HTTPServerRequest &request);

    /**
     * Returns the HTTP request body as string.
     *
     * @param request HTTP request
     * @return request body as string
     */
    std::string GetBodyAsString(Poco::Net::HTTPServerRequest &request);

    /**
     * Send a OK response (HTTP state code 200).
     *
     * @param response HTTP response object
     * @param payload HTTP body payload
     * @param extraHeader HTTP header map values, added to the default headers
     */
    void SendOkResponse(Poco::Net::HTTPServerResponse &response, const std::string &payload = {}, const HeaderMap &extraHeader = {});

    /**
     * Send a OK response (HTTP state code 200) with an output stream.
     *
     * @param response HTTP response object
     * @param fileName file to send
     * @param contentLength content length of the stream in bytes
     * @param extraHeader HTTP header map values, added to the default headers
     */
    void SendOkResponse(Poco::Net::HTTPServerResponse &response, const std::string &fileName, long contentLength, const HeaderMap &extraHeader = {});

    /**
     * Send a CONTINUE response (HTTP state code 100).
     *
     * @param response HTTP response object
     */
    void SendContinueResponse(Poco::Net::HTTPServerResponse &response);

    /**
     * Send a OK response (HTTP state code 200) with an part of an output.
     *
     * @param response HTTP response object
     * @param fileName file to send
     * @param min minimum position
     * @param max minimum position
     * @param size total size of the file
     * @param extraHeader HTTP header map values, added to the default headers
     */
    void SendRangeResponse(Poco::Net::HTTPServerResponse &response, const std::string &fileName, long min, long max, long size, const HeaderMap &extraHeader = {});

    /**
     * Send a HEAD response (HTTP state code 200)
     *
     * @param response HTTP response object
     * @param headerMap HTTP header map values
     */
    void SendHeadResponse(Poco::Net::HTTPServerResponse &response, const HeaderMap &headerMap);

    /**
     * Send a DELETE response (HTTP state code 204) with an output stream.
     *
     * @param response HTTP response object
     * @param extraHeader HTTP header map values, added to the default headers
     */
    void SendDeleteResponse(Poco::Net::HTTPServerResponse &response, const HeaderMap &extraHeader = {});

    /**
     * Send generic error response.
     *
     * @param response HTTP response
     * @param body HTTP body
     * @param headers HTTP headers
     * @param status HTTP status
     */
    void SendErrorResponse(Poco::Net::HTTPServerResponse &response, const std::string& body, std::map<std::string, std::string> headers, const Poco::Net::HTTPResponse::HTTPStatus& status);

    /**
     * Send an error response (HTTP state code 200).
     *
     * @param service module name
     * @param response HTTP response object
     * @param exc exception object
     */
    void SendXmlErrorResponse(const std::string &service, Poco::Net::HTTPServerResponse &response, Poco::Exception &exc);

    /**
     * Send an error response (HTTP state code 200).
     *
     * @param service module name
     * @param response HTTP response object
     * @param exc module exception object
     */
    void SendXmlErrorResponse(const std::string &service, Poco::Net::HTTPServerResponse &response, Core::ServiceException &exc);

    /**
     * Send an error response (HTTP state code 200).
     *
     * @param service module name
     * @param response HTTP response object
     * @param payload response body
     */
    void SendXmlErrorResponse(const std::string &service, Poco::Net::HTTPServerResponse &response, const std::string &payload);

    /**
     * Send an error response (HTTP state code 200).
     *
     * @param service module name
     * @param response HTTP response object
     * @param exc exception object
     */
    void SendJsonErrorResponse(const std::string &service, Poco::Net::HTTPServerResponse &response, Poco::Exception &exc);

    /**
     * Send an error response (HTTP state code 200).
     *
     * @param service module name
     * @param response HTTP response object
     * @param exc module exception object
     */
    void SendJsonErrorResponse(const std::string &service, Poco::Net::HTTPServerResponse &response, Core::ServiceException &exc);

    /**
     * Send a no content response, state: 204.
     *
     * @param response HTTP response
     * @param extraHeader extrac headers
     */
    void SendNoContentResponse(Poco::Net::HTTPServerResponse &response, const HeaderMap &extraHeader = {});

    /**
     * Checks whether the request has a header with the given key
     *
     * @param request HTTP request
     * @param name header key
     * @return true if header with the specified key exists, otherwise false
     */
    static bool HeaderExists(Poco::Net::HTTPServerRequest &request, const std::string &name);

    /**
     * Gets a header values
     *
     * @param request HTTP request
     * @param name header name
     * @param defaultValue default value
     * @return value or default value
     */
    std::string GetHeaderValue(Poco::Net::HTTPServerRequest &request, const std::string &name, const std::string &defaultValue);

    /**
     * Gets a header values
     *
     * @param request HTTP request
     * @return string map of metadata
     */
    static std::map<std::string, std::string> GetMetadata(Poco::Net::HTTPServerRequest &request);

    /**
     * Check for the existence of given header key
     * @param request HTTP request
     * @param name header key name
     * @return true of header exists
     */
    bool HasHeaderValue(Poco::Net::HTTPServerRequest &request, const std::string &name);

    /**
     * Dump the request to std::cerr
     *
     * @param request HTTP request
     */
    void DumpRequest(Poco::Net::HTTPServerRequest &request);

    /**
     * Dump the request to std::cerr
     *
     * @param request HTTP request
     */
    void DumpRequestHeaders(Poco::Net::HTTPServerRequest &request);

    /**
     * Dump the request to std::cerr
     *
     * @param response HTTP response
     */
    void DumpResponseHeaders(Poco::Net::HTTPServerResponse &response);

    /**
     * Dump the response to std::cerr
     *
     * @param response HTTP response
     */
    void DumpResponse(Poco::Net::HTTPServerResponse &response);

    /**
     * Dump the request body to std::cerr
     *
     * @param request HTTP request
     */
    [[maybe_unused]] void DumpBody(Poco::Net::HTTPServerRequest &request);

    /**
     * Dump the request body to std::cerr
     *
     * @param payload HTTP request body
     */
    [[maybe_unused]] void DumpPayload(const std::string &payload);

    /**
     * Dump the request body to a file
     *
     * @param request HTTP request
     * @param filename name fo the file
     */
    void DumpBodyToFile(Poco::Net::HTTPServerRequest &request, const std::string &filename);

  private:

    /**
     * Set the header values
     *
     * @param response HTTP response object
     * @param contentLength payload content length
     * @param headerMap vector of header key/values pairs
     * @param contentType content type
     */
    void SetHeaders(Poco::Net::HTTPServerResponse &response, unsigned long contentLength, const HeaderMap &headerMap = {}, const std::string &contentType = "application/xml");

    /**
     * Logger
     */
    Core::LogStream _logger;

    /**
     * Base URL
     */
    std::string _baseUrl;

    /**
     * Request URI
     */
    std::string _requestURI;

    /**
     * Request Host
     */
    std::string _requestHost;

    /**
     * Query parameters
     */
    Poco::URI::QueryParameters _queryStringParameters;

    /**
     * Path parameters
     */
    std::vector<std::string> _pathParameter;

    /**
     * Header map
     */
    HeaderMap _headerMap;

    /**
     * POco mutex
     */
    Poco::Mutex _mutex;
  };

} // namespace AwsMock::Service

#endif // AWSMOCK_SERVICE_ABSTRACT_HANDLER_H
