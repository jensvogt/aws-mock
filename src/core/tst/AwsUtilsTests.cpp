//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_AWS_UTILS_TEST_H
#define AWMOCK_CORE_AWS_UTILS_TEST_H

// GTest includes
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Poco includes
#include "Poco/Path.h"

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/TestUtils.h>

namespace AwsMock::Core {

    class AwsUtilsTest : public ::testing::Test {

      public:

        void SetUp() override {
            _region = _configuration.getString("awsmock.region");
            _accountId = _configuration.getString("awsmock.account.userPoolId");
            _endpoint = SystemUtils::GetHostName() + ":" + _configuration.getString("awsmock.service.gateway.http.port");
        }

      protected:

        std::string _region, _accountId, _endpoint;
        Core::Configuration _configuration = Core::Configuration(TMP_PROPERTIES_FILE);
    };

    TEST_F(AwsUtilsTest, CreateS3ArnTest) {

        // arrange
        std::string bucket = "testBucket";
        std::string key = "testKey";
        std::string s3Arn = "arn:aws:s3:" + _region + ":" + _accountId + ":" + bucket + "/" + key;

        // act
        std::string result = AwsUtils::CreateS3Arn(_region, _accountId, bucket, key);

        // assert
        EXPECT_STREQ(result.c_str(), s3Arn.c_str());
    }

    TEST_F(AwsUtilsTest, CreateSqsArnTest) {

        // arrange
        std::string queueName = "testQueue";
        std::string sqsQueueArn = "arn:aws:sqs:" + _region + ":" + _accountId + ":" + queueName;

        // act
        std::string result = AwsUtils::CreateSQSQueueArn(_configuration, queueName);

        // assert
        EXPECT_STREQ(result.c_str(), sqsQueueArn.c_str());
    }

    TEST_F(AwsUtilsTest, CreateSNSTopicTest) {

        // arrange
        std::string topicName = "testTopic";
        std::string snsTopic3Arn = "arn:aws:sns:" + _region + ":" + _accountId + ":" + topicName;

        // act
        std::string result = AwsUtils::CreateSNSTopicArn(_region, _accountId, topicName);

        // assert
        EXPECT_STREQ(result.c_str(), snsTopic3Arn.c_str());
    }

    TEST_F(AwsUtilsTest, ConvertArnToUrlTest) {

        // arrange
        std::string queueName = "file-delivery1-queue";
        std::string sqsQueueUrl = "http://" + _endpoint + "/" + _accountId + "/" + queueName;
        std::string sqsQueueArn = "arn:aws:sqs:" + _region + ":" + _accountId + ":" + queueName;

        // act
        std::string result = AwsUtils::ConvertSQSQueueArnToUrl(_configuration, sqsQueueArn);

        // assert
        EXPECT_STREQ(result.c_str(), sqsQueueUrl.c_str());
    }

    /**
     * @brief Test the AWS4 signature
     *
     * From: @link https://docs.aws.amazon.com/AmazonS3/latest/API/sig-v4-header-based-auth.html @endlink
     *
     * GET /test.txt HTTP/1.1
     * Host: examplebucket.s3.amazonaws.com
     * Authorization: SignatureToBeCalculated
     * Range: bytes=0-9
     * x-amz-content-sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
     * x-amz-date: 20130524T000000Z
     */
    TEST_F(AwsUtilsTest, VerifySignaturePocoTest) {

        // arrange
        Poco::Net::HTTPRequest request;
        request.setMethod("GET");
        request.setURI("/test.txt");
        request.setHost("examplebucket.s3.amazonaws.com");
        request.set("Authorization", "AWS4-HMAC-SHA256 Credential=AKIAIOSFODNN7EXAMPLE/20130524/us-east-1/s3/aws4_request,SignedHeaders=host;range;x-amz-content-sha256;x-amz-date,Signature=f0e8bdb87c964420e857bd35b5d6ed310bd44f0170aba48dd91039c6036bdb41");
        request.set("Range", "bytes=0-9");
        request.set("x-amz-content-sha256", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
        request.set("x-amz-date", "20130524T000000Z");
        std::string secretAccessKey = "wJalrXUtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY";

        // act
        bool result = AwsUtils::VerifySignature(request, {}, secretAccessKey);

        // assert
        ASSERT_TRUE(result);
    }

}// namespace AwsMock::Core

#endif// AWMOCK_CORE_AWS_UTILS_TEST_H