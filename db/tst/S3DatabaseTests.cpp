//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_S3DATABASETEST_H
#define AWMOCK_CORE_S3DATABASETEST_H

// C++ standard includes
#include <iostream>
#include <vector>

// GTest includes
#include <gtest/gtest.h>

// Local includes
#include <awsmock/core/Configuration.h>
#include <awsmock/db/S3Database.h>

// MongoDB includes
#include <bsoncxx/builder/basic/document.hpp>
#include <mongocxx/client.hpp>

#define CONFIG_FILE "/tmp/aws-mock.properties"
#define BUCKET "test-bucket"
#define OBJECT "test-object"
#define OWNER "test-owner"

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;

    class S3DatabaseTest : public ::testing::Test {

    protected:

      void SetUp() override {
          _region = _configuration.getString("awsmock.region");
      }

      void TearDown() override {
          _s3database.DeleteAllBuckets();
          _s3database.DeleteAllObjects();
      }

      std::string _region;
      Core::Configuration _configuration = Core::Configuration(CONFIG_FILE);
      S3Database _s3database = S3Database(_configuration);
    };

    TEST_F(S3DatabaseTest, BucketCreateTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};

        // act
        Entity::S3::Bucket result = _s3database.CreateBucket(bucket);

        // assert
        EXPECT_TRUE(result.name == BUCKET);
        EXPECT_TRUE(result.region == _region);
    }

    TEST_F(S3DatabaseTest, BucketExistsTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        bucket = _s3database.CreateBucket(bucket);

        // act
        bool result = _s3database.BucketExists(bucket);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(S3DatabaseTest, BucketByRegionNameTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        bucket = _s3database.CreateBucket(bucket);

        // act
        Entity::S3::Bucket result = _s3database.GetBucketByRegionName(bucket.region, bucket.name);

        // assert
        EXPECT_TRUE(result.name == bucket.name);
        EXPECT_TRUE(result.region == _region);
    }

    TEST_F(S3DatabaseTest, BucketGetByIdTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        bucket = _s3database.CreateBucket(bucket);

        // act
        Entity::S3::Bucket result = _s3database.GetBucketById(bucket.oid);

        // assert
        EXPECT_EQ(result.oid, bucket.oid);
    }

    TEST_F(S3DatabaseTest, BucketListTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        _s3database.CreateBucket(bucket);

        // act
        Entity::S3::BucketList result = _s3database.ListBuckets();

        // assert
        EXPECT_EQ(result.size(), 1);
    }

    TEST_F(S3DatabaseTest, BucketListObjectTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        _s3database.CreateBucket(bucket);
        Entity::S3::Object object1 = {.bucket=bucket.name, .key=OBJECT, .owner=OWNER, .size=5};
        _s3database.CreateObject(object1);
        Entity::S3::Object object2 = {.bucket=bucket.name, .key="test1/" + std::string(OBJECT), .owner=OWNER, .size=5};
        _s3database.CreateObject(object2);

        // act
        Entity::S3::ObjectList result1 = _s3database.ListBucket(bucket.name);
        Entity::S3::ObjectList result2 = _s3database.ListBucket(bucket.name, "test1");

        // assert
        EXPECT_EQ(result1.size(), 2);
        EXPECT_TRUE(result1[0].key == OBJECT);
        EXPECT_EQ(result2.size(), 1);
        EXPECT_STREQ(result2[0].key.c_str(), (std::string("test1/") + std::string(OBJECT)).c_str());
    }

    TEST_F(S3DatabaseTest, BucketHasObjetsTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        _s3database.CreateBucket(bucket);
        Entity::S3::Object object1 = {.region=_region, .bucket=bucket.name, .key=OBJECT, .owner=OWNER, .size=5};
        _s3database.CreateObject(object1);
        Entity::S3::Object object2 = {.region=_region, .bucket=bucket.name, .key="test1/" + std::string(OBJECT), .owner=OWNER, .size=5};
        _s3database.CreateObject(object2);

        // act
        bool result = _s3database.HasObjects(bucket);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(S3DatabaseTest, BucketDeleteTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        bucket = _s3database.CreateBucket(bucket);

        // act
        EXPECT_NO_THROW({ _s3database.DeleteBucket(bucket); });
        bool result = _s3database.BucketExists({.region=bucket.region, .name=bucket.name});

        // assert
        EXPECT_FALSE(result);
    }

    TEST_F(S3DatabaseTest, BucketDeleteAllTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        bucket = _s3database.CreateBucket(bucket);

        // act
        EXPECT_NO_THROW({ _s3database.DeleteAllBuckets(); });
        bool result = _s3database.BucketExists({.region=bucket.region, .name=bucket.name});

        // assert
        EXPECT_FALSE(result);
    }

    TEST_F(S3DatabaseTest, ObjectExistsTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        bucket = _s3database.CreateBucket(bucket);
        Entity::S3::Object object = {.bucket=bucket.name, .key=OBJECT, .owner=OWNER, .size=5};
        object = _s3database.CreateObject(object);

        // act
        bool result = _s3database.ObjectExists(object);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(S3DatabaseTest, ObjectCreateTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        bucket = _s3database.CreateBucket(bucket);
        Entity::S3::Object object = {.region=_region, .bucket=bucket.name, .owner=OWNER, .size=5};
        object = _s3database.CreateObject(object);

        // act
        Entity::S3::Object result = _s3database.GetObject(_region, object.bucket, object.key);

        // assert
        EXPECT_STREQ(result.key.c_str(), object.key.c_str());
    }

    TEST_F(S3DatabaseTest, ObjectUpdateTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        bucket = _s3database.CreateBucket(bucket);
        Entity::S3::Object object = {.bucket=bucket.name, .owner=OWNER, .size=5};
        object = _s3database.CreateObject(object);
        Entity::S3::Object updateObject = {.bucket=bucket.name, .owner=OWNER, .size=object.size + 10};

        // act
        Entity::S3::Object result = _s3database.UpdateObject(updateObject);

        // assert
        EXPECT_EQ(15, result.size);
    }

    TEST_F(S3DatabaseTest, ObjectByIdTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        bucket = _s3database.CreateBucket(bucket);
        Entity::S3::Object object = {.bucket=bucket.name, .owner=OWNER, .size=5};
        object = _s3database.CreateObject(object);

        // act
        Entity::S3::Object result = _s3database.GetObjectById(object.oid);

        // assert
        EXPECT_TRUE(result.oid == object.oid);
    }

    TEST_F(S3DatabaseTest, ObjectDeleteTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        bucket = _s3database.CreateBucket(bucket);
        Entity::S3::Object object = {.bucket=bucket.name, .owner=OWNER, .size=5};
        object = _s3database.CreateObject(object);

        // act
        EXPECT_NO_THROW({ _s3database.DeleteObject(object); });
        bool result = _s3database.ObjectExists({.region=object.region, .bucket=object.bucket, .key=object.key});

        // assert
        EXPECT_FALSE(result);
    }

    TEST_F(S3DatabaseTest, ObjectCountTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        bucket = _s3database.CreateBucket(bucket);

        // Create objects
        for (int i = 0; i < 10; i++) {
            _s3database.CreateObject({.region=_region, .bucket=bucket.name, .key=std::string(OBJECT) + std::to_string(i), .owner=OWNER});
        }

        // act
        int result = _s3database.ObjectCount(bucket);

        // assert
        EXPECT_EQ(10, result);
    }

    TEST_F(S3DatabaseTest, ObjectDeleteManyTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        bucket = _s3database.CreateBucket(bucket);

        // Create objects
        std::vector<std::string> keys;
        for (int i = 0; i < 10; i++) {
            std::string key = std::string(OBJECT) + "-" + std::to_string(i);
            keys.push_back(key);
            Entity::S3::Object object = {.bucket=bucket.name, .key=key, .owner=OWNER};
            _s3database.CreateObject(object);
        }

        // act
        EXPECT_NO_THROW({ _s3database.DeleteObjects(bucket.name, keys); });
        bool result = _s3database.ObjectCount(bucket);

        // assert
        EXPECT_EQ(0, result);
    }

    TEST_F(S3DatabaseTest, ObjectDeleteAllTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        bucket = _s3database.CreateBucket(bucket);
        Entity::S3::Object object = {.bucket=bucket.name, .owner=OWNER, .size=5};
        object = _s3database.CreateObject(object);

        // act
        EXPECT_NO_THROW({ _s3database.DeleteAllObjects(); });
        bool result = _s3database.ObjectExists({.region=object.region, .bucket=object.bucket, .key=object.key});

        // assert
        EXPECT_FALSE(result);
    }

    TEST_F(S3DatabaseTest, CreateNotificationTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        bucket = _s3database.CreateBucket(bucket);
        Entity::S3::BucketNotification notification = {.event="s3:ObjectCreated:*", .lambdaArn="aws:arn:000000000:lambda:test"};

        // act
        Entity::S3::Bucket result = _s3database.CreateBucketNotification(bucket, notification);

        // assert
        EXPECT_EQ(4, result.notifications.size());
    }

    TEST_F(S3DatabaseTest, CreateNotificationPutTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        bucket = _s3database.CreateBucket(bucket);
        Entity::S3::BucketNotification notification = {.event="s3:ObjectCreated:Put", .lambdaArn="aws:arn:000000000:lambda:test"};

        // act
        Entity::S3::Bucket result = _s3database.CreateBucketNotification(bucket, notification);

        // assert
        EXPECT_EQ(1, result.notifications.size());
    }

    TEST_F(S3DatabaseTest, CreateNotificationTwiceTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        bucket = _s3database.CreateBucket(bucket);
        Entity::S3::BucketNotification notification = {.event="s3:ObjectCreated:Put", .lambdaArn="aws:arn:000000000:lambda:test"};
        bucket = _s3database.CreateBucketNotification(bucket, notification);

        // act
        Entity::S3::Bucket result = _s3database.CreateBucketNotification(bucket, notification);

        // assert
        EXPECT_EQ(1, result.notifications.size());
    }

    TEST_F(S3DatabaseTest, DeleteNotificationTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        bucket = _s3database.CreateBucket(bucket);
        Entity::S3::BucketNotification notification = {.event="s3:ObjectCreated:*", .lambdaArn="aws:arn:000000000:lambda:test"};
        bucket = _s3database.CreateBucketNotification(bucket, notification);

        // act
        Entity::S3::Bucket result = _s3database.DeleteBucketNotifications(bucket, notification);

        // assert
        EXPECT_TRUE(result.notifications.empty());
    }

    TEST_F(S3DatabaseTest, DeleteNotificationPutTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region=_region, .name=BUCKET, .owner=OWNER};
        bucket = _s3database.CreateBucket(bucket);
        Entity::S3::BucketNotification notification = {.event="s3:ObjectCreated:*", .lambdaArn="aws:arn:000000000:lambda:test"};
        bucket = _s3database.CreateBucketNotification(bucket, notification);
        Entity::S3::BucketNotification deleteNotification = {.event="s3:ObjectCreated:Put", .lambdaArn="aws:arn:000000000:lambda:test"};

        // act
        Entity::S3::Bucket result = _s3database.DeleteBucketNotifications(bucket, deleteNotification);

        // assert
        EXPECT_EQ(3, result.notifications.size());
    }

} // namespace AwsMock::Core

#endif // AWMOCK_CORE_S3DATABASETEST_H