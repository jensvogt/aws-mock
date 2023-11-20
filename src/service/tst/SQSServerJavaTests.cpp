//
// Created by vogje01 on 21/10/2023.
//

#ifndef AWMOCK_SQS_JAVA_SERVERTEST_H
#define AWMOCK_SQS_JAVA_SERVERTEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/dto/sqs/CreateQueueResponse.h>
#include <awsmock/repository/S3Database.h>
#include <awsmock/service/SQSServer.h>

// Test includes
#include <awsmock/core/TestUtils.h>

#define REGION "eu-central-1"
#define OWNER "test-owner"
#define MESSAGE "{\"test-attribute\":\"test-value\"}"
#define VERSION std::string("2012-11-05")
#define CREATE_QUEUE_REQUEST "Action=CreateQueue&QueueName=TestQueue&Attribute.1.Name=VisibilityTimeout&Attribute.1.Value=40&Tag.Key=QueueType&Tag.Value=Production"
#define LIST_QUEUE_REQUEST "Action=ListQueues"
#define GET_QUEUE_URL_REQUEST "Action=GetQueueUrl&QueueName=TestQueue"
#define SEND_MESSAGE_REQUEST "Action=SendMessage&QueueUrl=TestQueue&MessageBody=%7B%22testattribute%22%3A%22testvalue%22%7D"

namespace AwsMock::Service {

  class SQSServerJavaTest : public ::testing::Test {

  protected:

    void SetUp() override {

      // Set log level
      Core::LogStream::SetGlobalLevel("error");

      // Create some test objects
      _extraHeaders["Authorization"] =
          "AWS4-HMAC-SHA256 Credential=none/20230618/eu-central-1/sqs/aws4_request, SignedHeaders=host;x-amz-date;x-amz-security-token, Signature=90d0e45560fa4ce03e6454b7a7f2a949e0c98b46c35bccb47f666272ec572840";
      _extraHeaders["User-Agent"] =
          "aws-sdk-cpp";

      // Define endpoint. This is the endpoint of the SQS server, not the gateway
      std::string _port = _configuration.getString("awsmock.module.sqs.port", std::to_string(SQS_DEFAULT_PORT));
      std::string _host = _configuration.getString("awsmock.module.sqs.host", SQS_DEFAULT_HOST);
      _endpoint = "http://" + _host + ":" + _port;

      // Start HTTP manager
      Poco::ThreadPool::defaultPool().start(_sqsServer);
      while (!_sqsServer.IsRunning()) {
        Poco::Thread::sleep(1000);
      }
    }

    void TearDown() override {
      _sqsServer.StopServer();
      _database.DeleteAllQueues();
      Core::FileUtils::DeleteFile(_testFile);
    }

    Core::CurlUtils _curlUtils;
    std::string _testFile, _endpoint;
    std::map<std::string, std::string> _extraHeaders;
    Core::Configuration _configuration = Core::Configuration(TMP_PROPERTIES_FILE);
    Core::MetricService _metricService = Core::MetricService(_configuration);
    Database::SQSDatabase _database = Database::SQSDatabase(_configuration);
    SQSServer _sqsServer = SQSServer(_configuration, _metricService);
  };

  TEST_F(SQSServerJavaTest, QueueCreateTest) {

    // arrange
    std::string body = "Action=CreateQueue&Version=" + VERSION + "&QueueName=TestQueue&Attribute.1.Name=VisibilityTimeout&Attribute.1.Value=40&Tag.Key=QueueType&Tag.Value=Production";

    // act
    Core::CurlResponse response = _curlUtils.SendHttpRequest("POST", _endpoint + "/", _extraHeaders, body);
    Database::Entity::SQS::QueueList queueList = _database.ListQueues();

    // assert
    EXPECT_TRUE(response.statusCode == Poco::Net::HTTPResponse::HTTP_OK);
    EXPECT_EQ(1, queueList.size());
  }

  TEST_F(SQSServerJavaTest, QueueListTest) {

    // arrange
    std::string body = "Action=CreateQueue&Version=" + VERSION + "&QueueName=TestQueue&Attribute.1.Name=VisibilityTimeout&Attribute.1.Value=40&Tag.Key=QueueType&Tag.Value=Production";
    Core::CurlResponse createResponse = _curlUtils.SendHttpRequest("POST", _endpoint + "/", _extraHeaders, body);
    EXPECT_TRUE(createResponse.statusCode == Poco::Net::HTTPResponse::HTTP_OK);

    // act
    Core::CurlResponse response = _curlUtils.SendHttpRequest("POST", _endpoint + "/", _extraHeaders, LIST_QUEUE_REQUEST);
    EXPECT_TRUE(createResponse.statusCode == Poco::Net::HTTPResponse::HTTP_OK);

    // assert
    EXPECT_TRUE(response.statusCode == Poco::Net::HTTPResponse::HTTP_OK);
    EXPECT_TRUE(Core::StringUtils::Contains(response.output, "TestQueue"));
  }

  TEST_F(SQSServerJavaTest, QueueDeleteTest) {

    // arrange
    std::string body = "Action=CreateQueue&Version=" + VERSION + "&QueueName=TestQueue&Attribute.1.Name=VisibilityTimeout&Attribute.1.Value=40&Tag.Key=QueueType&Tag.Value=Production";
    Core::CurlResponse curlResponse = _curlUtils.SendHttpRequest("POST", _endpoint + "/", _extraHeaders, body);
    EXPECT_TRUE(curlResponse.statusCode == Poco::Net::HTTPResponse::HTTP_OK);
    Dto::SQS::CreateQueueResponse createResponse;
    createResponse.FromXml(curlResponse.output);

    // act
    std::string deleteQueueRequest = "Action=DeleteQueue&QueueUrl=" + createResponse.queueUrl;
    curlResponse = _curlUtils.SendHttpRequest("POST", _endpoint, _extraHeaders, deleteQueueRequest);
    EXPECT_TRUE(curlResponse.statusCode == Poco::Net::HTTPResponse::HTTP_OK);
    Database::Entity::SQS::QueueList queueList = _database.ListQueues();

    // assert
    EXPECT_EQ(0, queueList.size());
  }

  TEST_F(SQSServerJavaTest, QueueGetUrlTest) {

    // arrange
    std::string body = "Action=CreateQueue&Version=" + VERSION + "&QueueName=TestQueue&Attribute.1.Name=VisibilityTimeout&Attribute.1.Value=40&Tag.Key=QueueType&Tag.Value=Production";
    Core::CurlResponse curlResponse = _curlUtils.SendHttpRequest("POST", _endpoint + "/", _extraHeaders, body);
    EXPECT_TRUE(curlResponse.statusCode == Poco::Net::HTTPResponse::HTTP_OK);
    Dto::SQS::CreateQueueResponse createResponse;
    createResponse.FromXml(curlResponse.output);

    // act
    curlResponse = _curlUtils.SendHttpRequest("POST", _endpoint + "/", _extraHeaders, GET_QUEUE_URL_REQUEST);
    EXPECT_TRUE(curlResponse.statusCode == Poco::Net::HTTPResponse::HTTP_OK);
    Dto::SQS::GetQueueUrlResponse getQueueUrlResponse;
    getQueueUrlResponse.FromXml(curlResponse.output);

    // assert
    EXPECT_TRUE(Core::StringUtils::Contains(getQueueUrlResponse.queueUrl, "TestQueue"));
  }

  TEST_F(SQSServerJavaTest, MessageSendTest) {

    // arrange
    std::string body = "Action=CreateQueue&Version=" + VERSION + "&QueueName=TestQueue&Attribute.1.Name=VisibilityTimeout&Attribute.1.Value=40&Tag.Key=QueueType&Tag.Value=Production";
    Core::CurlResponse curlResponse = _curlUtils.SendHttpRequest("POST", _endpoint + "/", _extraHeaders, body);
    EXPECT_TRUE(curlResponse.statusCode == Poco::Net::HTTPResponse::HTTP_OK);
    Dto::SQS::CreateQueueResponse createResponse;
    createResponse.FromXml(curlResponse.output);

    curlResponse = _curlUtils.SendHttpRequest("POST", _endpoint + "/", _extraHeaders, GET_QUEUE_URL_REQUEST);
    EXPECT_TRUE(curlResponse.statusCode == Poco::Net::HTTPResponse::HTTP_OK);
    Dto::SQS::GetQueueUrlResponse getQueueUrlResponse;
    getQueueUrlResponse.FromXml(curlResponse.output);
    std::string queueUrl = getQueueUrlResponse.queueUrl;

    // act
    body = "Action=SendMessage&Version=2003-11-05&QueueUrl=" + Core::StringUtils::UrlEncode(queueUrl) + "&MessageBody=" + Core::StringUtils::UrlEncode(MESSAGE);
    curlResponse = _curlUtils.SendHttpRequest("POST", _endpoint + "/", _extraHeaders, body);
    EXPECT_TRUE(curlResponse.statusCode == Poco::Net::HTTPResponse::HTTP_OK);
    Dto::SQS::SendMessageResponse sendMessageResponse;
    sendMessageResponse.FromXml(curlResponse.output);
    long messageCount = _database.CountMessages(REGION, queueUrl);

    // assert
    EXPECT_FALSE(sendMessageResponse.messageId.empty());
    EXPECT_EQ(1, messageCount);
  }

  TEST_F(SQSServerJavaTest, QueuePurgeTest) {

    // arrange
    std::string body = "Action=CreateQueue&Version=" + VERSION + "&QueueName=TestQueue&Attribute.1.Name=VisibilityTimeout&Attribute.1.Value=40&Tag.Key=QueueType&Tag.Value=Production";
    Core::CurlResponse curlResponse = _curlUtils.SendHttpRequest("POST", _endpoint + "/", _extraHeaders, body);
    EXPECT_TRUE(curlResponse.statusCode == Poco::Net::HTTPResponse::HTTP_OK);
    Dto::SQS::CreateQueueResponse createResponse;
    createResponse.FromXml(curlResponse.output);

    curlResponse = _curlUtils.SendHttpRequest("POST", _endpoint + "/", _extraHeaders, GET_QUEUE_URL_REQUEST);
    EXPECT_TRUE(curlResponse.statusCode == Poco::Net::HTTPResponse::HTTP_OK);
    Dto::SQS::GetQueueUrlResponse getQueueUrlResponse;
    getQueueUrlResponse.FromXml(curlResponse.output);
    std::string queueUrl = getQueueUrlResponse.queueUrl;

    curlResponse = _curlUtils.SendHttpRequest("POST", _endpoint + "/", _extraHeaders, SEND_MESSAGE_REQUEST);
    EXPECT_TRUE(curlResponse.statusCode == Poco::Net::HTTPResponse::HTTP_OK);
    Dto::SQS::SendMessageResponse sendMessageResponse;
    sendMessageResponse.FromXml(curlResponse.output);

    // act
    body = "Action=PurgeQueue&QueueUrl=" + queueUrl;
    _curlUtils.SendHttpRequest("POST", _endpoint + "/", _extraHeaders, body);
    long messageCount = _database.CountMessages(REGION, queueUrl);

    // assert
    EXPECT_EQ(0, messageCount);
  }

} // namespace AwsMock::Core

#endif // AWMOCK_SQS_JAVA_SERVERTEST_H