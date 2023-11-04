//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_SQSSERVICETEST_H
#define AWMOCK_CORE_SQSSERVICETEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/repository/SQSDatabase.h>
#include <awsmock/service/SQSService.h>

// AwsMOck includes
#include <awsmock/core/TestUtils.h>

#define REGION "eu-central-1"
#define QUEUE "test-queue"
#define QUEUE_URL "http://localhost:4566/000000000000/test-queue"
#define BODY "{\"TestObject\": \"TestValue\"}"
#define BODY_MD5 "bf54bf4281dc11635fcdd2d7d6c9e126"
#define OWNER "test-owner"

namespace AwsMock::Service {

    class SQSServiceTest : public ::testing::Test {

    protected:

      void SetUp() override {
        // Set log level
        Core::LogStream::SetGlobalLevel("error");
      }

      void TearDown() override {
        _database.DeleteAllQueues();
        _database.DeleteAllMessages();
      }

      Poco::Condition _condition;
      Core::Configuration _configuration = Core::Configuration(TMP_PROPERTIES_FILE);
      Database::SQSDatabase _database = Database::SQSDatabase(_configuration);
      SQSService _service = SQSService(_configuration, _condition);
    };

    TEST_F(SQSServiceTest, QueueCreateTest) {

        // arrange
        Dto::SQS::CreateQueueRequest request = {.region=REGION, .name=QUEUE, .queueUrl=QUEUE_URL, .owner=OWNER};

        // act
        Dto::SQS::CreateQueueResponse response = _service.CreateQueue(request);

        // assert
        EXPECT_TRUE(response.name == QUEUE);
        EXPECT_TRUE(response.region == REGION);
    }

    /*TEST_F(SQSServiceTest, QueueCreateAttributeTest) {

        // arrange
        Dto::SQS::CreateQueueRequest request = {.region=REGION, .name=QUEUE, .queueUrl=QUEUE_URL, .owner=OWNER};

        // act
        Dto::SQS::CreateQueueResponse response = _service.CreateQueue(request);
        Database::Entity::SQS::QueueAttribute attributes = _database.GetQueueAttributesByQueueUrl(response.queueUrl);

        // assert
        EXPECT_TRUE(response.name == QUEUE);
        EXPECT_TRUE(response.region == REGION);
        EXPECT_EQ(attributes.maxMessageSize, 262144);
        EXPECT_EQ(attributes.visibilityTimeout, 30);
    }*/

    TEST_F(SQSServiceTest, QueueDeleteTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.region=REGION, .name=QUEUE, .queueUrl=QUEUE_URL, .owner=OWNER};
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        Dto::SQS::DeleteQueueRequest deleteRequest = {.region=REGION, .queueUrl=QUEUE_URL};

        // act
        EXPECT_NO_THROW({ _service.DeleteQueue(deleteRequest);});

        // assert
        EXPECT_EQ(0, _database.ListQueues(REGION).size());
    }

    TEST_F(SQSServiceTest, MessageCreateTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.region=REGION, .name=QUEUE, .queueUrl=QUEUE_URL, .owner=OWNER};
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        Dto::SQS::SendMessageRequest request = {.region=REGION, .queueUrl=QUEUE_URL, .body=BODY};

        // act
        Dto::SQS::SendMessageResponse response = _service.SendMessage(request);

        // assert
        EXPECT_TRUE(response.messageId.length() > 0);
        EXPECT_TRUE(response.md5Body == BODY_MD5);
    }

    TEST_F(SQSServiceTest, MessageReceiveTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.region=REGION, .name=QUEUE, .queueUrl=QUEUE_URL, .owner=OWNER};
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        Dto::SQS::SendMessageRequest msgRequest = {.region=REGION, .queueUrl=QUEUE_URL, .body=BODY};
        Dto::SQS::SendMessageResponse msgResponse = _service.SendMessage(msgRequest);

        // act
        Dto::SQS::ReceiveMessageRequest receiveRequest = {.queueUrl=QUEUE_URL, .maxMessages=10, .waitTimeSeconds=1};
        Dto::SQS::ReceiveMessageResponse receiveResponse = _service.ReceiveMessages(receiveRequest);

        // assert
        EXPECT_EQ(receiveResponse.messageList.size(), 1);
    }

    TEST_F(SQSServiceTest, MessageDeleteTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.region=REGION, .name=QUEUE, .queueUrl=QUEUE_URL, .owner=OWNER};
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        Dto::SQS::SendMessageRequest msgRequest = {.region=REGION, .queueUrl=QUEUE_URL, .body=BODY};
        Dto::SQS::SendMessageResponse msgResponse = _service.SendMessage(msgRequest);

        // act
        Dto::SQS::DeleteMessageRequest delRequest = {.queueUrl=QUEUE, .receiptHandle=msgResponse.receiptHandle};
        Dto::SQS::DeleteMessageResponse delResponse;
        EXPECT_NO_FATAL_FAILURE({ _service.DeleteMessage(delRequest); });

        // assert
        EXPECT_EQ(0, _database.CountMessages(REGION, QUEUE_URL));
    }

    TEST_F(SQSServiceTest, GetMd5AttributesTest) {

        // arrange
        //
        // MessageAttribute.1.Name=contentType
        // MessageAttribute.1.Value.StringValue=application/json
        // MessageAttribute.1.Value.DataType=String
        //
        Dto::SQS::MessageAttribute messageAttribute = {.attributeName="contentType", .attributeValue="application/json", .type="String"};
        std::vector<Dto::SQS::MessageAttribute> messageAttributes;
        messageAttributes.push_back(messageAttribute);

        // act
        std::string md5sum = _service.GetMd5Attributes(messageAttributes);

        // assert
        EXPECT_TRUE("6ed5f16969b625c8d900cbd5da557e9e" == md5sum);
    }

    TEST_F(SQSServiceTest, GetMd5AttributeListTest) {

        // arrange
        //
        // MessageAttribute.1.Name=contentType
        // MessageAttribute.1.Value.StringValue=application/json
        // MessageAttribute.1.Value.DataType=String
        // MessageAttribute.2.Name=contentLength
        // MessageAttribute.2.Value.StringValue=42
        // MessageAttribute.2.Value.DataType=Number
        //
        Dto::SQS::MessageAttribute messageAttribute1 = {.attributeName="contentType", .attributeValue="application/json", .type="String"};
        Dto::SQS::MessageAttribute messageAttribute2 = {.attributeName="contentLength", .attributeValue="42", .type="Number"};
        std::vector<Dto::SQS::MessageAttribute> messageAttributes;
        messageAttributes.push_back(messageAttribute1);
        messageAttributes.push_back(messageAttribute2);

        // act
        std::string md5sum = _service.GetMd5Attributes(messageAttributes);

        // assert
        EXPECT_TRUE("ebade6c58059dfd4bbf8cee9da7465fe" == md5sum);
    }

} // namespace AwsMock::Core

#endif // AWMOCK_CORE_SQSSERVICETEST_H