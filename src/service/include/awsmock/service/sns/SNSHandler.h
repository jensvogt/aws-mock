//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVER_LAMBDA_SERVER_H
#define AWSMOCK_SERVER_LAMBDA_SERVER_H

// Poco includes
#include <Poco/DateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/MetricDefinition.h>
#include <awsmock/core/MetricService.h>
#include <awsmock/dto/common/SNSClientCommand.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/sns/SNSCmdHandler.h>
#include <awsmock/service/sns/SNSService.h>

namespace AwsMock::Service {

    typedef std::map<std::string, std::string> AttributeList;

    /**
     * AWS SNS mock handler
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SNSHandler : public SNSCmdHandler {

      public:

        /**
         * Constructor
         *
         * @param configuration application configuration
         */
        explicit SNSHandler(Core::Configuration &configuration) : SNSCmdHandler(configuration), _configuration(configuration), _snsService(configuration) {}

        /**
         * HTTP POST request.
         *
         * @param request HTTP request
         * @param response HTTP response
         * @param region AWS region
         * @param user AWS user
         * @see AbstractResource::handlePost(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
         */
        void handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) override;

      private:

        /**
         * ImageHandler import configuration
         */
        Core::Configuration &_configuration;

        /**
         * SNS module
         */
        Service::SNSService _snsService;

        /**
         * Default account ID
         */
        std::string _accountId;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVER_LAMBDA_SERVER_H
