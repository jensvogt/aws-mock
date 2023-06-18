//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_CORE_AWSUTILS_H
#define AWSMOCK_CORE_AWSUTILS_H

// C++ standard includes
#include <string>

// Poco includes
#include <Poco/UUIDGenerator.h>

// AwsMock includes
#include "awsmock/core/StringUtils.h"

namespace AwsMock::Core {

    class AwsUtils {

    public:

      /**
       * Create Amazon Resource Names (ARNs)
       *
       * @param service AWS service name
       * @param region AWS region
       * @param accountId AWS accountId
       * @param resourceId AWS resourceId
       */
      static std::string CreateArn(const std::string &service, const std::string &region, const std::string &accountId, const std::string &resourceId);

      /**
       * Create S3 Amazon bucket/key ARN
       *
       * @param region AWS region
       * @param accountId AWS account ID
       * @param bucket bucket name
       * @param key object key
       */
      static std::string CreateS3Arn(const std::string &region, const std::string &accountId, const std::string &bucket, const std::string &key);

      /**
       * Create Lambda function ARN
       *
       * @param region AWS region
       * @param accountId AWS account ID
       * @param function lambda function name
       */
      static std::string CreateLambdaArn(const std::string &region, const std::string &accountId, const std::string &function);

      /**
       * Create SQS queue ARN
       *
       * @param region AWS region
       * @param accountId AWS account ID
       * @param queueName name of the queue
       */
      static std::string CreateSQSQueueArn(const std::string &region, const std::string &accountId, const std::string &queueName);

      /**
       * Create SNS topic ARN
       *
       * @param region AWS region
       * @param accountId AWS account ID
       * @param topicName name of the topic
       */
      static std::string CreateSNSTopicArn(const std::string &region, const std::string &accountId, const std::string &topicName);

      /**
       * Converts a queue ARN to a queue URL
       *
       * @param queueArn ARN of the queue
       * @return URL of the queue
       */
      static std::string ConvertSQSQueueArnToUrl(const std::string &queueArn);

      /**
       * Create SNS subscription ARN
       *
       * <p>Syntax arn:aws:sns:us-west-2:123456789012:MyTopic:6b0e71bd-7e97-4d97-80ce-4a0994e55286</p>
       *
       * @param region AWS region
       * @param accountId AWS account ID
       * @param topicName name of the topic
       */
      static std::string CreateSNSSubscriptionArn(const std::string &region, const std::string &accountId, const std::string &topicName);

      /**
       * Returns the default account ID, which will be used, in case no account ID is provided by the caller.
       *
       * @return default account ID
       */
      static std::string GetDefaultAccountId() {
          return "000000000000";
      }

      /**
       * Returns a request ID.
       *
       * @return request ID
       */
      static std::string GetRequestId() {
          return Poco::UUIDGenerator().createRandom().toString();
      }

    };
}
#endif //AWSMOCK_CORE_AWSUTILS_H
