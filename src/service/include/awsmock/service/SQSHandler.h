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

#define DEFAULT_SQS_ENDPOINT "localhost:4566"
#define DEFAULT_SQS_ACCOUNT_ID "000000000000"

namespace AwsMock::Service {

  enum class UserAgentType {
    AWS_SDK_JAVA,
    AWS_SDK_CPP,
    AWS_CLI,
    AWS_SDK_UNKNOWN
  };

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
  class SQSHandler : public AbstractHandler {

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
     * Get the queue attributes.
     *
     * @param payload HTTP body
     * @return list of queue attributes
     */
    std::vector<Dto::SQS::QueueAttribute> GetQueueAttributes(const std::string &payload);

    /**
     * Get the queue tags.
     *
     * @param payload HTTP body
     * @return list of queue tags
     */
    std::map<std::string,std::string> GetQueueTags(const std::string &payload);

    /**
     * Get the queue attribute names.
     *
     * @param payload HTTP body
     * @return list of queue attribute names
     */
    std::vector<std::string> GetQueueAttributeNames(const std::string &payload);

    /**
     * Get the message attributes.
     *
     * @param payload HTTP body
     * @return list of message attributes
     */
    static std::vector<Dto::SQS::MessageAttribute> GetMessageAttributes(const std::string &payload);

    /**
     * User agent dependent extraction of the queue name.
     *
     * <p>Depending on the user-agent (aws-sdk-java, aws-sdk-cpp, aws-cli, etc.) the request looks totally different. AWS has no common policy how the HTTP request should look like. Therefore the request parameter must be extracted differently fpr the
     * different user-agents.</p>.
     *
     * @param request HTTP request
     * @param payload HTTP message body
     * @return queue name
     */
    static std::string GetQueueName(const Poco::Net::HTTPServerRequest &request, const std::string &payload);

    /**
     * User agent dependent extraction of the queue URL
     *
     * <p>Depending on the user-agent (aws-sdk-java, aws-sdk-cpp, aws-cli, etc.) the request looks totally different. AWS has no common policy how the HTTP request should look like. Therefore the request parameter must be extracted differently fpr the
     * different user-agents.</p>.
     *
     * @param request HTTP request
     * @param payload HTTP message body
     * @return queue URL
     */
    static std::string GetQueueUrl(const Poco::Net::HTTPServerRequest &request, const std::string &payload);

    /**
     * Extracts the user agent from the HTTP header.
     *
     * <p>Depending on the user-agent (aws-sdk-java, aws-sdk-cpp, aws-cli, etc.) the request looks totally different. AWS has no common policy how the HTTP request should look like. Therefore the request parameter must be extracted differently fpr the
     * different user-agents.</p>.
     *
     * @param request HTTP request
     * @param payload HTTP message body
     * @return user agent (@see UserAgentType)
     */
    static UserAgentType GetUserAgent(const Poco::Net::HTTPServerRequest &request);

    /**
     * Logger
     */
    Core::LogStream _logger;

    /**
     * ImageHandler import configuration
     */
    Core::Configuration &_configuration;

    /**
     * Metric module
     */
    Core::MetricService &_metricService;

    /**
     * SQS module
     */
    Service::SQSService _sqsService;

    /**
     * Default account ID
     */
    std::string _accountId;

    /**
     * Default endpoint
     */
    std::string _endpoint;
  };

} // namespace AwsMock::Service

#endif // AWSMOCK_SERVICE_SQSHANDLER_H
