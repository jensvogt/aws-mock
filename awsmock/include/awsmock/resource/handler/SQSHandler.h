//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_RESOURCE_SQSHANDLER_H
#define AWSMOCK_RESOURCE_SQSHANDLER_H

// Poco includes
#include "Poco/Logger.h"
#include "Poco/StreamCopier.h"
#include "Poco/Net/MediaType.h"
#include "Poco/JSON/Parser.h"

// Libri includes
#include "awsmock/core/Configuration.h"
#include "awsmock/core/MetricService.h"
#include "awsmock/core/MetricServiceTimer.h"
#include "awsmock/core/MetricDefinition.h"
#include "awsmock/resource/HandlerException.h"
#include "awsmock/resource/AbstractResource.h"
#include "awsmock/service/SQSService.h"
#include "awsmock/dto/sqs/CreateMessageRequest.h"
#include "awsmock/dto/sqs/CreateMessageResponse.h"
#include "awsmock/dto/sqs/CreateQueueRequest.h"
#include "awsmock/dto/sqs/CreateQueueResponse.h"

#define DEFAULT_USERID "000000000000"

namespace AwsMock {

    /**
     * AWS S3 mock handler
     */
    class SQSHandler : public AwsMock::Resource::AbstractResource {

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
       * @see AbstractResource::handleGet(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
       */
      void handleGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) override;

      /**
       * HTTP PUT request.
       *
       * @param request HTTP request
       * @param response HTTP response
       * @see AbstractResource::handlePut(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
       */
      void handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) override;

      /**
       * HTTP POST request.
       *
       * @param request HTTP request
       * @param response HTTP response
       * @see AbstractResource::handlePost(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
       */
      void handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) override;

      /**
       * Delete DELETE request.
       *
       * @param request HTTP request
       * @param response HTTP response
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
      void handleOptions(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) override;

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
      static void GetActionVersion(const std::string &body, std::string &action, std::string &version);

      /**
       * Get the action from the request body
       *
       * @param body HTTP request body (in)
       * @param name parameter name (out)
       * @param value parameter value (out)
       */
      static void GetParameter(const std::string &body, std::string &name, std::string &value);

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
      Poco::Logger &_logger;

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
    };
} // namespace AwsMock

#endif // AWSMOCK_RESOURCE_SQSHANDLER_H