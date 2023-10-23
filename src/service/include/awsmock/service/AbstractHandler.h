//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_ABSTRACTHANDLER_H
#define AWSMOCK_SERVICE_ABSTRACTHANDLER_H

// C++ includes
#include <string>
#include <fstream>

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
#include <awsmock/core/LogStream.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/ServiceException.h>
#include <awsmock/core/ResourceNotFoundException.h>
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
       * Handles the HTTP method PUT.
       *
       * @param request HTTP request
       * @param response HTTP response
       * @param region AWS region
       * @param user current user
       */
      virtual void handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user);

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
       * Handles the HTTP method DELETE.
       *
       * @param request HTTP request
       * @param response HTTP response
       * @param region AWS region
       * @param user current user
       */
      virtual void handleDelete(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user);

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
       * @param reason status reason.
       */
      void handleHttpStatusCode(Poco::Net::HTTPServerResponse &response, int statusCode, const char* reason = nullptr);

      /**
       * CHecks whether a query parameter exists
       *
       * @param parameterKey parameter name.
       * @return true if parameter exists, otherwise false
       */
      bool QueryParameterExists(const std::string &parameterKey);

      /**
       * Returns a query parameter by name.
       *
       * @param name parameter name.
       * @param optional parameter is optional.
       * @return  arameter value.
       */
      std::string GetQueryParameter(const std::string &name, bool optional = true);

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
       * @return body string
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
       * Get a path parameter by index
       *
       * @param path HTTP request path
       * @param index path index
       * @return parameter value
       */
      std::string GetStringPathParameter(const std::string &path, int index);

      /**
       * Gets a string parameter at position index
       *
       * @param path HTTP request path
       * @param name name of the parameter
       * @param index index of tke key
       * @return parameter value at index
       */
      std::string GetStringParameter(const std::string &path, const std::string &name, int index);

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
      static std::string GetBodyAsString(Poco::Net::HTTPServerRequest &request);

        /**
         * Send a OK response (HTTP status code 200).
         *
         * @param response HTTP response object
         * @param payload HTTP body payload
         * @param extraHeader HTTP header map values, added to the default headers
         */
      void SendOkResponse(Poco::Net::HTTPServerResponse &response, const std::string &payload = {}, const HeaderMap &extraHeader = {});

      /**
       * Send a OK response (HTTP status code 200) with an output stream.
       *
       * @param response HTTP response object
       * @param fileName file to send
       * @param contentLength content length of the stream in bytes
       * @param extraHeader HTTP header map values, added to the default headers
       */
      void SendOkResponse(Poco::Net::HTTPServerResponse &response, const std::string &fileName, long contentLength, const HeaderMap &extraHeader = {});

      /**
       * Send a HEAD response (HTTP status code 200)
       *
       * @param response HTTP response object
       * @param headerMap HTTP header map values
       */
      void SendHeadResponse(Poco::Net::HTTPServerResponse &response, const HeaderMap &headerMap);

      /**
       * Send a DELETE response (HTTP status code 204) with an output stream.
       *
       * @param response HTTP response object
       * @param extraHeader HTTP header map values, added to the default headers
       */
      void SendDeleteResponse(Poco::Net::HTTPServerResponse &response, const HeaderMap &extraHeader = {});

      /**
       * Send an error response (HTTP status code 200).
       *
       * @param service service name
       * @param response HTTP response object
       * @param exc exception object
       */
      void SendErrorResponse(const std::string &service, Poco::Net::HTTPServerResponse &response, Poco::Exception &exc);

      /**
       * Send an error response (HTTP status code 200).
       *
       * @param service service name
       * @param response HTTP response object
       * @param exc service exception object
       */
      void SendErrorResponse(const std::string &service,Poco::Net::HTTPServerResponse &response, Core::ServiceException &exc);

      /**
       * Send a no content response, status: 204.
       *
       * @param response HTTP response
       * @param extraHeader extrac headers
       */
      void SendNoContentResponse(Poco::Net::HTTPServerResponse &response, const HeaderMap &extraHeader = {});

      /**
       * CHecks whether the request has a header with the given key
       *
       * @param request HTTP request
       * @param name header key
       * @return true if header with the specified key exists, otherwise false
       */
      bool HeaderExists(Poco::Net::HTTPServerRequest &request, const std::string &name);

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
      std::map<std::string,std::string> GetMetadata(Poco::Net::HTTPServerRequest &request);

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
       * @param request HTTP request
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
       */
      void SetHeaders(Poco::Net::HTTPServerResponse &response, unsigned long contentLength, HeaderMap headerMap = {});

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

    };

} // namespace AwsMock::Service

#endif // AWSMOCK_SERVICE_ABSTRACTHANDLER_H
