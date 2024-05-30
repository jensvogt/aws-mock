//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_ABSTRACT_HANDLER_H
#define AWSMOCK_SERVICE_ABSTRACT_HANDLER_H

// C++ includes
#include <fstream>
#include <streambuf>
#include <string>

// Boost includes
#include <boost/beast.hpp>

// AwsMock includes
#include "awsmock/core/exception/ServiceException.h"
#include <awsmock/core/InvalidMethodCallException.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/MemoryMappedFile.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/exception/ForbiddenException.h>
#include <awsmock/dto/common/S3ClientCommand.h>
#include <awsmock/dto/common/UserAgent.h>
#include <awsmock/dto/s3/RestErrorResponse.h>
#include <awsmock/dto/sqs/RestErrorResponse.h>

namespace AwsMock::Service {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * @brief Abstract HTTP request handler
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class AbstractHandler {

      public:

        /**
         * @brief Default User-defined Constructor
         */
        AbstractHandler() = default;

        /**
         * @brief Handles the HTTP method GET.
         *
         * @param request HTTP request
         * @param region AWS region
         * @param user current user
         * @return HTTP response
         */
        virtual http::response<http::dynamic_body> HandleGetRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user);

        /**
         * @brief Handles the HTTP method PUT.
         *
         * @param request HTTP request
         * @param region AWS region
         * @param user current user
         * @return HTTP response
         * @return HTTP response
         */
        virtual http::response<http::dynamic_body> HandlePutRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user);

        /**
         * @brief Handles the HTTP method POST.
         *
         * @param request HTTP request
         * @param response HTTP response
         * @param s3Command S3 client command
         * @return HTTP response
         */
        virtual http::response<http::dynamic_body> HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user);

        /**
         * @brief Handles the HTTP method DELETE.
         *
         * @param request HTTP request
         * @param region AWS region
         * @param user current user
         * @return HTTP response
         */
        virtual http::response<http::dynamic_body> HandleDeleteRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user);

        /**
         * @brief Handles the HTTP method HEAD.
         *
         * @param request HTTP request
         * @param region AWS region
         * @param user current user
         * @return HTTP response
         */
        virtual http::response<http::dynamic_body> HandleHeadRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user);

        /**
         * @brief Send a OK response (HTTP state code 200).
         *
         * @param request HTTP request object
         * @param body HTTP body payload
         * @param headers HTTP header map values, added to the default headers
         * @return HTTP response
         */
        static http::response<http::dynamic_body> SendOkResponse(const http::request<http::dynamic_body> &request, const std::string &body = {}, const std::map<std::string, std::string> &headers = {});

        /**
         * @brief Send a OK response (HTTP state code 200) for a multipart download/upload
         *
         * @param request HTTP request object
         * @param fileName file to send
         * @param contentLength content length of the stream in bytes
         * @param extraHeader HTTP header map values, added to the default headers
         * @return HTTP response
         */
        static http::response<http::dynamic_body> SendOkResponse(const http::request<http::dynamic_body> &request, const std::string &fileName, long contentLength, const std::map<std::string, std::string> &headers = {});

        /**
         * @brief Send a bad request response (HTTP state code 400).
         *
         * @param request HTTP request object
         * @param headers HTTP header map values, added to the default headers
         * @return response HTTP response
         */
        static http::response<http::dynamic_body> SendNoContentResponse(const http::request<http::dynamic_body> &request, const std::map<std::string, std::string> &headers = {});

        /**
         * @brief Send a internal server error response (HTTP state code 500).
         *
         * @param request HTTP request
         * @param body HTTP body payload
         * @param headers HTTP header map values, added to the default headers
         * @param response HTTP response
         */
        static http::response<http::dynamic_body> SendInternalServerError(const http::request<http::dynamic_body> &request, const std::string &body = {}, const std::map<std::string, std::string> &headers = {});

        /**
         * @brief Send a bad request response (HTTP state code 400).
         *
         * @param request HTTP request
         * @param body HTTP body payload
         * @param headers HTTP header map values, added to the default headers
         * @return response HTTP response
         */
        static http::response<http::dynamic_body> SendBadRequestError(const http::request<http::dynamic_body> &request, const std::string &body = {}, const std::map<std::string, std::string> &headers = {});

        /**
         * Send a OK response (HTTP state code 200) with an part of an output.
         *
         * @param request HTTP request
         * @param fileName file to send
         * @param min minimum position
         * @param max minimum position
         * @param size total size of the file
         * @param headers HTTP header map values, added to the default headers
         * @return HTTP response
         */
        static http::response<http::dynamic_body> SendRangeResponse(const http::request<http::dynamic_body> &request, const std::string &fileName, long min, long max, long size, const std::map<std::string, std::string> &headers = {});

        /**
         * Send a HEAD response (HTTP state code 200)
         *
         * @param response HTTP response object
         * @param headerMap HTTP header map values
         */
        //static void SendHeadResponse(Poco::Net::HTTPServerResponse &response, const HeaderMap &headerMap);

        /**
         * Send a DELETE response (HTTP state code 204) with an output stream.
         *
         * @param response HTTP response object
         * @param extraHeader HTTP header map values, added to the default headers
         */
        //static void SendDeleteResponse(Poco::Net::HTTPServerResponse &response, const HeaderMap &extraHeader = {});

        /**
         * Send a no content response, state: 204.
         *
         * @param response HTTP response
         * @param extraHeader extra headers
         */
        //static void SendNoContentResponse(Poco::Net::HTTPServerResponse &response, const HeaderMap &extraHeader = {});
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_ABSTRACT_HANDLER_H
