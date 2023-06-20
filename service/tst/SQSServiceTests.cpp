//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_SQSSERVICETEST_H
#define AWMOCK_CORE_SQSSERVICETEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/db/SQSDatabase.h>
#include <awsmock/service/SQSService.h>

#define REGION "eu-central-1"
#define QUEUE "test-queue"
#define QUEUE_URL "http://localhost:4567/000000000000/test-queue"
#define BODY "{\"TestObject\": \"TestValue\"}"
#define BODY_MD5 "bf54bf4281dc11635fcdd2d7d6c9e126"
#define OWNER "test-owner"

namespace AwsMock::Service {

    class SQSServiceTest : public ::testing::Test {

    protected:

      void SetUp() override {
      }

      void TearDown() override {

      }

      Core::Configuration _configuration = Core::Configuration("/tmp/aws-mock.properties");
      Database::SQSDatabase _database = Database::SQSDatabase(_configuration);
      SQSService _service = SQSService(_configuration);
      //Poco::Data::Session _session = _snsDatabase.GetSession();
    };

    /*TEST_F(SQSServiceTest, QueueCreateTest) {

        // arrange
        Dto::SQS::CreateQueueRequest request = {.region=REGION, .name=QUEUE, .queueUrl=QUEUE_URL, .owner=OWNER};

        // act
        Dto::SQS::CreateQueueResponse response = _service.CreateQueue(request);

        // assert
        EXPECT_TRUE(response.name == QUEUE);
        EXPECT_TRUE(response.region == REGION);
    }

    TEST_F(SQSServiceTest, QueueCreateAttributeTest) {

        // arrange
        Dto::SQS::CreateQueueRequest request = {.region=REGION, .name=QUEUE, .queueUrl=QUEUE_URL, .owner=OWNER};

        // act
        Dto::SQS::CreateQueueResponse response = _service.CreateQueue(request);
        Database::Entity::SQS::QueueAttribute attributes = _snsDatabase.GetQueueAttributesByQueueUrl(response.queueUrl);

        // assert
        EXPECT_TRUE(response.name == QUEUE);
        EXPECT_TRUE(response.region == REGION);
        EXPECT_EQ(attributes.maxMessageSize, 262144);
        EXPECT_EQ(attributes.visibilityTimeout, 30);
    }

    TEST_F(SQSServiceTest, QueueDeleteTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.region=REGION, .name=QUEUE, .queueUrl=QUEUE_URL, .owner=OWNER};
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        Dto::SQS::DeleteQueueRequest deleteRequest = {.queueUrl=QUEUE_URL};

        // act
        EXPECT_NO_THROW({ _service.DeleteQueue(deleteRequest);});

        // assert
    }

    TEST_F(SQSServiceTest, MessageCreateTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.region=REGION, .name=QUEUE, .queueUrl=QUEUE_URL, .owner=OWNER};
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        Dto::SQS::CreateMessageRequest request = {.url=QUEUE_URL, .body=BODY};

        // act
        Dto::SQS::CreateMessageResponse response = _service.CreateMessage(request);

        // assert
        EXPECT_TRUE(response.id > 0);
        EXPECT_TRUE(response.messageId.length() > 0);
        EXPECT_TRUE(response.md5Body == BODY_MD5);
    }

    TEST_F(SQSServiceTest, MessageReceiveTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.region=REGION, .name=QUEUE, .queueUrl=QUEUE_URL, .owner=OWNER};
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        Dto::SQS::CreateMessageRequest msgRequest = {.url=QUEUE_URL, .body=BODY};
        Dto::SQS::CreateMessageResponse msgResponse = _service.CreateMessage(msgRequest);

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
        Dto::SQS::CreateMessageRequest msgRequest = {.url=QUEUE_URL, .body=BODY};
        Dto::SQS::CreateMessageResponse msgResponse = _service.CreateMessage(msgRequest);

        // act
        Dto::SQS::DeleteMessageRequest delRequest = {.queueUrl=QUEUE, .receiptHandle=msgResponse.receiptHandle};
        Dto::SQS::DeleteMessageResponse delResponse;
        EXPECT_NO_FATAL_FAILURE({ _service.DeleteMessage(delRequest); });

        // assert
    }*/

} // namespace AwsMock::Core

#endif // AWMOCK_CORE_SQSSERVICETEST_H