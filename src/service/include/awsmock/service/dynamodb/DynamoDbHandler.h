//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_DYNAMODB_HANDLER_H
#define AWSMOCK_SERVICE_DYNAMODB_HANDLER_H

// Boost includes
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/NotFoundException.h>
#include <awsmock/core/monitoring/MetricService.h>
#include <awsmock/dto/common/DynamoDbClientCommand.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/dynamodb/DynamoDbService.h>

namespace AwsMock::Service {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * @brief AWS DynamoDB mock handler
     *
     * AWS DynamoDB HTTP request handler. All DynamoDB related REST call are ending here.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class DynamoDbHandler : public AbstractHandler {

      public:

        /**
         * Constructor
         */
        explicit DynamoDbHandler();

        /**
         * HTTP POST request.
         *
         * @param request HTTP request
         * @param response HTTP response
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::HandlePostRequest
         */
        http::response<http::string_body> HandlePostRequest(const http::request<http::string_body> &request, const std::string &region, const std::string &user) override;

      private:

        /**
         * AWS secret access key
         */
        std::string _secretAccessKey;

        /**
         * DynamoDB service
         */
        Service::DynamoDbService _dynamoDbService;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_DYNAMODB_HANDLER_H
