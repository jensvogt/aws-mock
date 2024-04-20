//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_S3_CMD_HANDLER_H
#define AWSMOCK_SERVICE_S3_CMD_HANDLER_H

// Poco includes
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/MetricService.h>
#include <awsmock/core/MetricDefinition.h>
#include <awsmock/core/NumberUtils.h>
#include <awsmock/core/JsonException.h>
#include <awsmock/dto/common/UserAgent.h>
#include <awsmock/dto/common/S3ClientCommand.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/s3/S3Service.h>

namespace AwsMock::Service {

  /**
   * AWS S3 mock handler
   *
   * <p>
   * AWS S3 HTTP request handler. All S3 related REST call are ending here. Depending on the request header the S3 module will be selected in case the
   * authorization header contains the S3 module. As the different clients (Java, C++, Python, nodejs) are using different request structure, the request
   * are first send to the S3CmdHandler, which normalizes the commands.
   * </p>
   */
  class S3CmdHandler : public virtual AbstractHandler {

  public:

    /**
     * Constructor
     *
     * @param configuration application configuration
     * @param metricService monitoring module
     */
    S3CmdHandler(Core::Configuration &configuration, Core::MetricService &metricService) : AbstractHandler(), _configuration(configuration), _metricService(metricService), _s3Service(configuration) {}

  protected:

    /**
     * HTTP GET request.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @param s3Command S3 client command
     * @see AbstractResource::handleGet(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
     */
    void handleGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const Dto::Common::S3ClientCommand &s3Command) override;

    /**
     * HTTP PUT request.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @param s3Command S3 client command
     * @see AbstractResource::handlePut(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
     */
    void handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const Dto::Common::S3ClientCommand &s3Command) override;

    /**
     * HTTP POST request.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @param s3Command S3 client command
     * @see AbstractResource::handlePost(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
     */
    void handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const Dto::Common::S3ClientCommand &s3Command) override;

    /**
     * Delete DELETE request.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @param s3Command S3 client command
     * @see AbstractResource::handleDelete(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
     */
    void handleDelete(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const Dto::Common::S3ClientCommand &s3Command) override;

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
     * S3 handler configuration
     */
    Core::Configuration &_configuration;

    /**
     * Metric module
     */
    Core::MetricService &_metricService;

    /**
     * S3 module
     */
    Service::S3Service _s3Service;

  };

} // namespace AwsMock::Service

#endif // AWSMOCK_SERVICE_S3_CMD_HANDLER_H