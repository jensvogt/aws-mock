//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_S3DATABASETEST_H
#define AWMOCK_CORE_S3DATABASETEST_H

// GTest includes
#include <gtest/gtest.h>

// Local includes
#include <awsmock/core/Configuration.h>
#include <awsmock/db/S3Database.h>

#define REGION "eu-central-1"
#define BUCKET "test-bucket"
#define OBJECT "test-object"
#define OWNER "test-owner"

namespace AwsMock::Database {

    using namespace Poco::Data::Keywords;

    class S3DatabaseTest : public ::testing::Test {

    protected:

      void SetUp() override {
      }

      void TearDown() override {
          Poco::Data::Statement stmt(_session);
          stmt << "DELETE FROM s3_notification;"
                  "DELETE FROM s3_object;"
                  "DELETE FROM s3_bucket;", now;
      }

      Core::Configuration _configuration = Core::Configuration("/tmp/aws-mock.properties");
      S3Database _database = S3Database(_configuration);
      Poco::Data::Session _session = _database.GetSession();
    };

    TEST_F(S3DatabaseTest, ConstructorTest) {

        // arrange
        int count = 0;

        // act
        Poco::Data::Statement stmt(_session);
        stmt << "SELECT COUNT(*) FROM s3_bucket", into(count), now;

        // assert
        EXPECT_EQ(count, 0);
    }

    TEST_F(S3DatabaseTest, BucketCreateTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=REGION, .name=BUCKET, .owner=OWNER};

        // act
        Entity::S3::Bucket result = _database.CreateBucket(bucket);

        // assert
        EXPECT_TRUE(result.name == BUCKET);
        EXPECT_TRUE(result.region == REGION);
    }

    TEST_F(S3DatabaseTest, BucketExistsTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=REGION, .name=BUCKET, .owner=OWNER};
        _database.CreateBucket(bucket);

        // act
        bool result = _database.BucketExists({.region=REGION, .name=BUCKET});

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(S3DatabaseTest, BucketGetByIdTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=REGION, .name=BUCKET, .owner=OWNER};
        bucket = _database.CreateBucket(bucket);

        // act
        Entity::S3::Bucket result = _database.GetBucketById(bucket.id);

        // assert
        EXPECT_EQ(result.id, bucket.id);
    }

    TEST_F(S3DatabaseTest, BucketGetByRegionNameTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=REGION, .name=BUCKET, .owner=OWNER};
        bucket = _database.CreateBucket(bucket);

        // act
        Entity::S3::Bucket result = _database.GetBucketByRegionName(REGION, BUCKET);

        // assert
        EXPECT_EQ(result.id, bucket.id);
    }

    TEST_F(S3DatabaseTest, BucketListTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=REGION, .name=BUCKET, .owner=OWNER};
        _database.CreateBucket(bucket);

        // act
        Entity::S3::BucketList result = _database.ListBuckets();

        // assert
        EXPECT_EQ(result.size() ,1);
    }

    TEST_F(S3DatabaseTest, BucketListObjectTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=REGION, .name=BUCKET, .owner=OWNER};
        _database.CreateBucket(bucket);
        Entity::S3::Object object = {.bucket=bucket.name, .key=OBJECT, .owner=OWNER, .size=5};
        _database.CreateObject(object);

        // act
        Entity::S3::ObjectList result = _database.ListBucket(bucket.name);

        // assert
        EXPECT_EQ(result.size() ,1);
    }

    TEST_F(S3DatabaseTest, BucketDeleteTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=REGION, .name=BUCKET, .owner=OWNER};
        bucket = _database.CreateBucket(bucket);

        // act
        EXPECT_NO_THROW({ _database.DeleteBucket(bucket); });
        bool result = _database.BucketExists({.region=bucket.region, .name=bucket.name});

        // assert
        EXPECT_FALSE(result);
    }

    TEST_F(S3DatabaseTest, ObjectExistsTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=REGION, .name=BUCKET, .owner=OWNER};
        bucket = _database.CreateBucket(bucket);
        Entity::S3::Object object = {.bucket=bucket.name, .key=OBJECT, .owner=OWNER, .size=5};
        object = _database.CreateObject(object);

        // act
        bool result = _database.ObjectExists(object);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(S3DatabaseTest, ObjectCreateTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=REGION, .name=BUCKET, .owner=OWNER};
        bucket = _database.CreateBucket(bucket);
        Entity::S3::Object object = {.bucket=bucket.name, .owner=OWNER, .size=5};
        object = _database.CreateObject(object);

        // act
        Entity::S3::Object result = _database.GetObject(object.bucket, object.key);

        // assert
        EXPECT_STREQ(result.key.c_str(), object.key.c_str());
    }

    TEST_F(S3DatabaseTest, ObjectByIdTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=REGION, .name=BUCKET, .owner=OWNER};
        bucket = _database.CreateBucket(bucket);
        Entity::S3::Object object = {.bucket=bucket.name, .owner=OWNER, .size=5};
        object = _database.CreateObject(object);

        // act
        Entity::S3::Object result = _database.GetObjectById(object.id);

        // assert
        EXPECT_EQ(result.id, object.id);
    }

    TEST_F(S3DatabaseTest, CreateNotificationTest) {

        // arrange
        Entity::S3::BucketNotification notification = {.region=REGION, .bucket=BUCKET, .function="aws:arn:000000000:lambda:test", .event="s3:ObjectCreated:*"};

        // act
        Entity::S3::BucketNotification result = _database.CreateBucketNotification(notification);

        // assert
        EXPECT_TRUE(result.id > 0);
    }

    TEST_F(S3DatabaseTest, HasNotificationTest) {

        // arrange
        Entity::S3::BucketNotification notification = {.region=REGION, .bucket=BUCKET, .function="aws:arn:000000000:lambda:test", .event="s3:ObjectCreated:*"};
        notification = _database.CreateBucketNotification(notification);

        // act
        bool result = _database.HasBucketNotification(notification);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(S3DatabaseTest, GetNotificationTest) {

        // arrange
        Entity::S3::BucketNotification notification = {.region=REGION, .bucket=BUCKET, .function="aws:arn:000000000:lambda:test", .event="s3:ObjectCreated:*"};
        notification = _database.CreateBucketNotification(notification);

        // act
        Entity::S3::BucketNotification result = _database.GetBucketNotification(notification);

        // assert
        EXPECT_TRUE(result.region == notification.region);
        EXPECT_TRUE(Core::StringUtils::Contains(result.event, "%"));
    }

    TEST_F(S3DatabaseTest, GetNotificationPutTest) {

        // arrange
        Entity::S3::BucketNotification notification = {.region=REGION, .bucket=BUCKET, .function="aws:arn:000000000:lambda:test", .event="s3:ObjectCreated:*"};
        notification = _database.CreateBucketNotification(notification);

        // act
        Entity::S3::BucketNotification search = {.region=REGION, .bucket=BUCKET, .function="aws:arn:000000000:lambda:test", .event="s3:ObjectCreated:Put"};
        Entity::S3::BucketNotification result = _database.GetBucketNotification(search);

        // assert
        EXPECT_TRUE(result.region == notification.region);
        EXPECT_TRUE(result.event == "s3:ObjectCreated:%");
    }

} // namespace AwsMock::Core

#endif // AWMOCK_CORE_S3DATABASETEST_H