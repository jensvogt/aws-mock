//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_SNSDATABASETEST_H
#define AWMOCK_CORE_SNSDATABASETEST_H

// GTest includes
#include <gtest/gtest.h>

// Poco includes
#include "Poco/Path.h"

// Local includes
#include <awsmock/core/Configuration.h>
#include <awsmock/db/SNSDatabase.h>

#define CONFIG_FILE "/tmp/aws-mock.properties"
#define REGION "eu-central-1"
#define TOPIC "test-topic"
#define TOPIC_ARN "arn:aws:sns:eu-central-1:000000000000:test-topic"
#define TOPIC_URL "http://localhost:4567/000000000000/test-topic"
#define BODY "{\"TestObject\": \"TestValue\"}"
#define OWNER "test-owner"

namespace AwsMock::Database {

    class SNSDatabaseTest : public ::testing::Test {

    protected:

      void SetUp() override {
          _region = _configuration.getString("awsmock.region");
      }

      void TearDown() override {
          _snsDatabase.DeleteAllTopics();
          _snsDatabase.DeleteAllMessages();
      }

      std::string _region;
      Core::Configuration _configuration = Core::Configuration(CONFIG_FILE);
      SNSDatabase _snsDatabase = SNSDatabase(_configuration);
    };

    TEST_F(SNSDatabaseTest, TopicCreateTest) {

        // arrange
        Entity::SNS::Topic topic = {.region=_region, .topicName=TOPIC, .owner=OWNER};

        // act
        Entity::SNS::Topic result = _snsDatabase.CreateTopic(topic);

        // assert
        EXPECT_TRUE(result.topicName == TOPIC);
        EXPECT_TRUE(result.region == REGION);
        EXPECT_FALSE(result.oid.empty());
    }

    TEST_F(SNSDatabaseTest, TopicGetByIdTest) {

        // arrange
        Entity::SNS::Topic topic = _snsDatabase.CreateTopic({.region=_region, .topicName=TOPIC, .owner=OWNER});

        // act
        Entity::SNS::Topic result = _snsDatabase.GetTopicById(topic.oid);

        // assert
        EXPECT_TRUE(result.topicArn == topic.topicArn);
    }

    TEST_F(SNSDatabaseTest, TopicGetByArnTest) {

        // arrange
        Entity::SNS::Topic topic = _snsDatabase.CreateTopic({.region=_region, .topicName=TOPIC, .owner=OWNER});

        // act
        Entity::SNS::Topic result = _snsDatabase.GetTopicByArn(topic.topicArn);

        // assert
        EXPECT_TRUE(result.topicArn == topic.topicArn);
    }

    TEST_F(SNSDatabaseTest, TopicUpdateTest) {

        // arrange
        Entity::SNS::Topic topic = _snsDatabase.CreateTopic({.region=_region, .topicName=TOPIC, .owner=OWNER});

        // act
        std::string url = "http://localhost:4567/" + topic.topicName;
        topic.topicUrl = url;
        Entity::SNS::Topic result = _snsDatabase.UpdateTopic(topic);

        // assert
        EXPECT_TRUE(result.topicUrl == topic.topicUrl);
    }

    TEST_F(SNSDatabaseTest, TopicCountTest) {

        // arrange
        Entity::SNS::Topic topic = _snsDatabase.CreateTopic({.region=_region, .topicName=TOPIC, .owner=OWNER});

        // act
        long result = _snsDatabase.CountTopics(topic.region);

        // assert
        EXPECT_EQ(1, result);
    }

    TEST_F(SNSDatabaseTest, TopicListTest) {

        // arrange
        Entity::SNS::Topic topic = _snsDatabase.CreateTopic({.region=_region, .topicName=TOPIC, .owner=OWNER});

        // act
        Entity::SNS::TopicList result = _snsDatabase.ListTopics(topic.region);

        // assert
        EXPECT_EQ(result.size(), 1);
    }

    TEST_F(SNSDatabaseTest, TopicDeleteTest) {

        // arrange
        Entity::SNS::Topic topic = _snsDatabase.CreateTopic({.region=_region, .topicName=TOPIC, .owner=OWNER, .topicArn=TOPIC_ARN});

        // act
        _snsDatabase.DeleteTopic(topic);
        bool result = _snsDatabase.TopicExists(topic.region, topic.topicName);

        // assert
        EXPECT_FALSE(result);
    }

    TEST_F(SNSDatabaseTest, MessageCreateTest) {

        // arrange
        Entity::SNS::Topic topic = {.region=_region, .topicName=TOPIC, .owner=OWNER, .topicArn=TOPIC_ARN};
        topic = _snsDatabase.CreateTopic(topic);
        Entity::SNS::Message message = {.region=_region, .topicArn=topic.topicArn, .message=BODY};

        // act
        Entity::SNS::Message result = _snsDatabase.CreateMessage(message);

        // assert
        EXPECT_FALSE(result.oid.empty());
        EXPECT_TRUE(result.message == BODY);
    }

    TEST_F(SNSDatabaseTest, MessageCountTest) {

        // arrange
        Entity::SNS::Topic topic = _snsDatabase.CreateTopic({.region=_region, .topicName=TOPIC, .owner=OWNER, .topicArn=TOPIC_ARN});
        Entity::SNS::Message message = _snsDatabase.CreateMessage({.region=_region, .topicArn=topic.topicArn, .message=BODY});

        // act
        long result = _snsDatabase.CountMessages(_region, topic.topicArn);

        // assert
        EXPECT_EQ(1, result);
    }

    TEST_F(SNSDatabaseTest, MessageDeleteTest) {

        // arrange
        Entity::SNS::Topic topic = _snsDatabase.CreateTopic({.region=_region, .topicName=TOPIC, .owner=OWNER, .topicArn=TOPIC_ARN});
        Entity::SNS::Message message = _snsDatabase.CreateMessage({.region=_region, .topicArn=topic.topicArn, .message=BODY, .messageId="abcd"});

        // act
        _snsDatabase.DeleteMessage(message);
        long result = _snsDatabase.CountMessages(_region, topic.topicArn);

        // assert
        EXPECT_EQ(0, result);
    }

    TEST_F(SNSDatabaseTest, MessageDeleteAllTest) {

        // arrange
        Entity::SNS::Topic topic = _snsDatabase.CreateTopic({.region=_region, .topicName=TOPIC, .owner=OWNER, .topicArn=TOPIC_ARN});
        Entity::SNS::Message message = _snsDatabase.CreateMessage({.region=_region, .topicArn=topic.topicArn, .message=BODY, .messageId="abcd"});

        // act
        _snsDatabase.DeleteAllMessages();
        long result = _snsDatabase.CountMessages(_region, topic.topicArn);

        // assert
        EXPECT_EQ(0, result);
    }

} // namespace AwsMock::Core

#endif // AWMOCK_CORE_SNSDATABASETEST_H