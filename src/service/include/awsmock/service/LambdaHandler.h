//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_LAMBDAHANDLER_H
#define AWSMOCK_SERVICE_LAMBDAHANDLER_H

// Poco includes
#include "Poco/Logger.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include <Poco/NotificationQueue.h>

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/MetricService.h>
#include <awsmock/core/MetricServiceTimer.h>
#include <awsmock/core/MetricDefinition.h>
#include <awsmock/service/AbstractHandler.h>
#include <awsmock/service/LambdaService.h>

namespace AwsMock::Service {

  /**
   * AWS lambda mock handler
   *
   * <p>AWS lambda HTTP request handler. All lambda related REST call are ending here.<p>
   */
  class LambdaHandler : public AbstractHandler {

    public:

    /**
     * Constructor
     *
     * @param configuration application configuration
     * @param metricService monitoring service
     * @param createQueue create notification queue
     * @param invokeQueue invoke notification queue
     */
    LambdaHandler(Core::Configuration &configuration, Core::MetricService &metricService, Poco::NotificationQueue &createQueue, Poco::NotificationQueue &invokeQueue);

    protected:

    /**
     * HTTP GET request.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @param region AWS region name
     * @param user AWS user
     * @see AbstractResource::handleGet(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
     */
    void handleGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) override;

    /**
     * HTTP PUT request.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @param region AWS region name
     * @param user AWS user
     * @see AbstractResource::handlePut(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
     */
    void handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) override;

    /**
     * HTTP POST request.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @param region AWS region name
     * @param user AWS user
     * @see AbstractResource::handlePost(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
     */
    void handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) override;

    /**
     * Delete DELETE request.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @param region AWS region name
     * @param user AWS user
     * @see AbstractResource::handleDelete(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
     */
    void handleDelete(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) override;

    /**
     * Options request.
     *
     * @param response HTTP response
     * @see AbstractResource::handleOption(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
     */
    void handleOptions(Poco::Net::HTTPServerResponse &response) override;

    /**
     * Head request.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @param region AWS region name
     * @param user AWS user
     * @see AbstractResource::handleHead(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
     */
    void handleHead(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) override;

    private:

    /**
     * Logger
     */
    Core::LogStream _logger;

    /**
     * S3 handler configuration
     */
    Core::Configuration &_configuration;

    /**
     * Metric service
     */
    Core::MetricService &_metricService;

    /**
     * lambda service
     */
    Service::LambdaService _lambdaService;
  };

} // namespace AwsMock

#endif // AWSMOCK_SERVICE_LAMBDAHANDLER_H
