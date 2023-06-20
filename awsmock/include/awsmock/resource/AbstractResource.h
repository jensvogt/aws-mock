//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_RESOURCE_ABSTRACTRESOURCE_H
#define AWSMOCK_RESOURCE_ABSTRACTRESOURCE_H

// C++ includes
#include <string>
#include <fstream>

// Poco includes
#include "Poco/Logger.h"
#include "Poco/LogStream.h"
#include "Poco/URI.h"
#include "Poco/StreamCopier.h"
#include <Poco/RegularExpression.h>
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include <Poco/Net/HTTPClientSession.h>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/Logger.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/ServiceException.h>
#include <awsmock/core/ResourceNotFoundException.h>
#include <awsmock/dto/s3/RestErrorResponse.h>
#include <awsmock/dto/sqs/RestErrorResponse.h>
#include <awsmock/resource/HandlerException.h>

namespace AwsMock::Resource {

    typedef std::vector<std::pair<std::string, std::string>> HeaderMap;

    /**
     * Abstract HTTP request handler
     */
    class AbstractResource : public Poco::Net::HTTPRequestHandler {

    public:
      /**
       * Default User-defined Constructor
       */
      AbstractResource();

      /**
       * Default Destructor
       */
      ~AbstractResource() override;

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
       */
      virtual void handleHead(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);

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
      static void handleHttpStatusCode(Poco::Net::HTTPServerResponse &response, int statusCode, const char* reason = nullptr);

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
       * Send a OK response (HTTP status code 200).
       *
       * @param response HTTP response object
       * @param payload HTTP body payload
       * @param extraHeader HTTP header map values, added to the default headers
       */
      void SendOkResponse(Poco::Net::HTTPServerResponse &response, const std::string &payload = {}, HeaderMap *extraHeader = nullptr);

      /**
       * Send a OK response (HTTP status code 200) with an output stream.
       *
       * @param response HTTP response object
       * @param fileName file to send
       * @param contentLength content length of the stream in bytes
       * @param extraHeader HTTP header map values, added to the default headers
       */
      void SendOkResponse(Poco::Net::HTTPServerResponse &response, const std::string &fileName, long contentLength, HeaderMap *extraHeader = nullptr);

      /**
       * Send an error response.
       *
       * @param response HTTP response object
       * @param payload payload of the error message
       */
      void SendErrorResponse(Poco::Net::HTTPServerResponse &response, const std::string &payload);

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
       * Dump the request to std::cerr
       *
       * @param request HTTP request
       */
      void DumpRequest(Poco::Net::HTTPServerRequest &request);

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

      /**
       * Forward request to service port
       *
       * @param request HTTP request
       * @param response HTTP response
       * @param host forward host
       * @param port forward port
       */
      void ForwardRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &host, int port);


      /**
       * Set default request header values
       *
       * @param request HTTP request object
       * @param region AWS request region
       * @param user request user
       */
      void SetHeaders(Poco::Net::HTTPServerRequest &request, const std::string &region, const std::string &user);

    private:

      /**
       * Set the header values
       *
       * @param response HTTP response object
       * @param contentLength payload content length
       * @param headerMap vector of header key/values pairs
       */
      void SetHeaders(Poco::Net::HTTPServerResponse &response, unsigned long contentLength, HeaderMap *headerMap = nullptr);

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

#define awsmock_debug(logger) logger.debug() << __FILE__ << ":" << __LINE__

} // namespace AwsMock::Resource

#endif // AWSMOCK_RESOURCE_ABSTRACTRESOURCE_H
