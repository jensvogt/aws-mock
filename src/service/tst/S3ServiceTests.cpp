//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_S3SERVICETEST_H
#define AWMOCK_CORE_S3SERVICETEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/repository/S3Database.h>
#include <awsmock/service/S3Service.h>

// Test includes
#include <awsmock/core/TestUtils.h>

#define REGION "eu-central-1"
#define BUCKET "test-bucket"
#define KEY "testfile.json"
#define OWNER "test-owner"
#define LOCATION_CONSTRAINT "<CreateBucketConfiguration xmlns=\"http://s3.amazonaws.com/doc/2006-03-01/\"> \
                                <LocationConstraint>eu-central-1</LocationConstraint> \
                             </CreateBucketConfiguration>"

namespace AwsMock::Service {

  class S3ServiceTest : public ::testing::Test {

  protected:

    void SetUp() override {
      testFile = Core::FileUtils::CreateTempFile("/tmp", "json", 10);
    }

    void TearDown() override {
      _database.DeleteAllBuckets();
      Core::FileUtils::DeleteFile(testFile);
    }

    Core::Configuration _configuration = Core::Configuration(TMP_PROPERTIES_FILE);
    Database::S3Database _database = Database::S3Database(_configuration);
    S3Service _service = S3Service(_configuration);
    std::string testFile;
  };

  TEST_F(S3ServiceTest, BucketCreateTest) {

    // arrange
    Dto::S3::CreateBucketRequest request = Dto::S3::CreateBucketRequest(LOCATION_CONSTRAINT);

    // act
    Dto::S3::CreateBucketResponse response = _service.CreateBucket(BUCKET, OWNER, request);

    // assert
    EXPECT_TRUE(response.location == "eu-central-1");
  }

  TEST_F(S3ServiceTest, BucketDeleteTest) {

    // arrange
    Dto::S3::CreateBucketRequest request = Dto::S3::CreateBucketRequest(LOCATION_CONSTRAINT);
    Dto::S3::CreateBucketResponse response = _service.CreateBucket(BUCKET, OWNER, request);

    // act
    _service.DeleteBucket(REGION, BUCKET);
    Dto::S3::ListAllBucketResponse result = _service.ListAllBuckets();

    // assert
    EXPECT_EQ(0, result.bucketList.size());
  }

  TEST_F(S3ServiceTest, ObjectPutTest) {

    // arrange
    Dto::S3::CreateBucketRequest request = Dto::S3::CreateBucketRequest(LOCATION_CONSTRAINT);
    Dto::S3::CreateBucketResponse response = _service.CreateBucket(BUCKET, OWNER, request);
    std::ifstream ifs(testFile);

    // act
    Dto::S3::PutObjectRequest putRequest = {.region=REGION, .bucket=BUCKET, .key=KEY};
    Dto::S3::PutObjectResponse putResponse = _service.PutObject(putRequest, ifs);

    // assert
    EXPECT_TRUE(putResponse.bucket == BUCKET);
    EXPECT_TRUE(putResponse.key == KEY);
  }

  TEST_F(S3ServiceTest, ObjectDeleteTest) {

    // arrange
    Dto::S3::CreateBucketRequest request = Dto::S3::CreateBucketRequest(LOCATION_CONSTRAINT);
    Dto::S3::CreateBucketResponse response = _service.CreateBucket(BUCKET, OWNER, request);
    std::ifstream ifs(testFile);
    Dto::S3::PutObjectRequest putRequest = {.region=REGION, .bucket=BUCKET, .key=KEY};
    Dto::S3::PutObjectResponse putResponse = _service.PutObject(putRequest, ifs);

    // act
    Dto::S3::DeleteObjectRequest deleteRequest = {.region=REGION, .bucket=BUCKET, .key=KEY};
    EXPECT_NO_THROW({ _service.DeleteObject(deleteRequest); });

    // assert
  }

} // namespace AwsMock::Core

#endif // AWMOCK_CORE_S3SERVICETEST_H