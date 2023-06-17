//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_SQSHANDLER_H
#define AWSMOCK_SERVICE_SQSHANDLER_H

// Poco includes
#include "Poco/Logger.h"
#include "Poco/LogStream.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"

// AwsMock includes
#include "awsmock/core/Configuration.h"
#include "awsmock/core/MetricService.h"
#include "awsmock/core/MetricServiceTimer.h"
#include "awsmock/core/MetricDefinition.h"
#include "awsmock/service/AbstractHandler.h"
#include "awsmock/service/SQSService.h"

namespace AwsMock::Service {

    typedef std::map<std::string, std::string> AttributeList;

    /**
     * AWS S3 mock handler
     */
    class SQSHandler : public AbstractHandler {

    public:

      /**
       * Constructor
       *
       * @param configuration application configuration
       * @param metricService monitoring service
       */
      SQSHandler(Core::Configuration &configuration,Core::MetricService &metricService);

    protected:

      /**
       * HTTP GET request.
       *
       * @param request HTTP request
       * @param response HTTP response
       * @param region AWS region
       * @param user AWS user
       * @see AbstractResource::handleGet(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
       */
      void handleGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) override;

      /**
       * HTTP PUT request.
       *
       * @param request HTTP request
       * @param response HTTP response
       * @param region AWS region
       * @param user AWS user
       * @see AbstractResource::handlePut(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
       */
      void handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) override;

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

      /**
       * Delete DELETE request.
       *
       * @param request HTTP request
       * @param response HTTP response
       * @param region AWS region
       * @param user AWS user
       * @see AbstractResource::handleDelete(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
       */
      void handleDelete(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) override;

      /**
       * Options request.
       *
       * @param request HTTP request
       * @param response HTTP response
       * @see AbstractResource::handleOption(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
       */
      void handleOptions(Poco::Net::HTTPServerResponse &response) override;

      /**
       * Head request.
       *
       * @param request HTTP request
       * @param response HTTP response
       * @see AbstractResource::handleOption(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
       */
      void handleHead(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) override;

    private:

      /**
       * Get the action from the request body
       *
       * @param body HTTP request body (in)
       * @param action SQS action (out)
       * @param version SQS version (out)
       */
      void GetActionVersion(const std::string &body, std::string &action, std::string &version);

      /**
       * Get the action from the request body
       *
       * @param body HTTP request body
       * @param name parameter name
       * @return parameter value
       */
      std::string GetStringParameter(const std::string &body, const std::string &name);

      /**
       * Return an integer name.
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
       * @param body HTTP message body
       * @param parameter parameter name
       * @return number of sqs
       */
      int GetAttributeCount(const std::string &body, const std::string &parameter);

      /**
       * Get the endpoint from the request header
       *
       * @param request HTTP request
       * @return endpoint
       */
      static std::string GetEndpoint(Poco::Net::HTTPServerRequest &request);

      /**
       * Logger
       */
      Poco::LogStream _logger;

      /**
       * ImageHandler import configuration
       */
      Core::Configuration &_configuration;

      /**
       * Metric service
       */
      Core::MetricService &_metricService;

      /**
       * SQS service
       */
      Service::SQSService _sqsService;

      /**
       * Default account ID
       */
      std::string _accountId;
    };
} // namespace AwsMock::Service

#endif // AWSMOCK_SERVICE_SQSHANDLER_H
