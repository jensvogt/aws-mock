//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_SQSHANDLER_H
#define AWSMOCK_SERVICE_SQSHANDLER_H

// Poco includes
#include <Poco/Condition.h>
#include <Poco/DateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/MetricService.h>
#include <awsmock/core/MetricServiceTimer.h>
#include <awsmock/core/MetricDefinition.h>
#include <awsmock/dto/sqs/GetQueueUrlRequest.h>
#include <awsmock/dto/sqs/GetQueueUrlResponse.h>
#include <awsmock/dto/sqs/DeleteMessageBatchEntry.h>
#include <awsmock/dto/sqs/DeleteMessageBatchRequest.h>
#include <awsmock/service/AbstractHandler.h>
#include <awsmock/service/SQSService.h>
#include <awsmock/service/SQSCliHandler.h>
#include <awsmock/service/SQSCppHandler.h>
#include <awsmock/service/SQSJava2Handler.h>

#define DEFAULT_SQS_ENDPOINT "localhost:4566"
#define DEFAULT_SQS_ACCOUNT_ID "000000000000"

namespace AwsMock::Service {

  /**
   * Attribute  list
   */
  typedef std::map<std::string, std::string> AttributeList;

  /**
   * AWS S3 mock handler.
   *
   * <p>The SQS request are coming in two different flavours. Using the AWS CLI the queue URL is part of the HTTP parameters in the body of the message. Both are
   * using POST request, whereas the Java SDK is providing the queue-url as part of the HTTP URL in the header of the request.</p>
   *
   * @author jens.vogt@opitz-consulting.com
   */
  class SQSHandler : public SQSCliHandler, public SQSCppHandler, public SQSJava2Handler {

  public:

    /**
     * Constructor
     *
     * @param configuration application configuration
     * @param metricService monitoring module
     * @param condition stop condition
     */
    SQSHandler(Core::Configuration &configuration, Core::MetricService &metricService, Poco::Condition &condition);

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
     * @param response HTTP response
     * @see AbstractResource::handleOption(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
     */
    void handleOptions(Poco::Net::HTTPServerResponse &response) override;

    /**
     * Head request.
     *
     * @param request HTTP request
     * @param response HTTP response
     * @param region AWS region
     * @param user AWS user
     * @see AbstractResource::handleOption(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
     */
    void handleHead(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) override;

  private:

    /**
     * Logger
     */
    Core::LogStream _logger;
  };

} // namespace AwsMock::Service

#endif // AWSMOCK_SERVICE_SQSHANDLER_H
