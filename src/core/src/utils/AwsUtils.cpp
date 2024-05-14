//
// Created by vogje01 on 06/06/2023.
//

#include <awsmock/core/AwsUtils.h>

namespace AwsMock::Core {

    std::string AwsUtils::CreateArn(const std::string &service, const std::string &region, const std::string &accountId, const std::string &resourceId) {
        return "arn:aws:" + service + ":" + region + ":" + accountId + ":" + resourceId;
    }

    std::string AwsUtils::CreateS3Arn(const std::string &region, const std::string &accountId, const std::string &bucket, const std::string &key) {
        return CreateArn("s3", region, accountId, bucket + "/" + key);
    }

    std::string AwsUtils::CreateLambdaArn(const std::string &region, const std::string &accountId, const std::string &function) {
        return CreateArn("lambda", region, accountId, "function:" + function);
    }

    std::string AwsUtils::CreateSqsQueueUrl(const Configuration &configuration, const std::string &queueName) {
        std::string endpoint = GetEndpoint(configuration);
        std::string accountId = configuration.getString("awsmock.account.userPoolId", SQS_DEFAULT_ACCOUNT_ID);
        log_trace << "Endpoint: " << endpoint << " accountId: " << accountId;
        return endpoint + "/" + accountId + "/" + queueName;
    }

    std::string AwsUtils::CreateSqsQueueArn(const Configuration &configuration, const std::string &queueName) {
        std::string region = configuration.getString("awsmock.region", GATEWAY_DEFAULT_REGION);
        std::string accountId = configuration.getString("awsmock.account.userPoolId", SQS_DEFAULT_ACCOUNT_ID);
        log_trace << "Region: " << region << " accountId: " << accountId;
        return CreateArn("sqs", region, accountId, queueName);
    }

    std::string AwsUtils::ConvertSQSQueueArnToUrl(const Configuration &configuration, const std::string &queueArn) {

        std::string endpoint = GetEndpoint(configuration);
        std::vector<std::string> parts = StringUtils::Split(queueArn, ':');
        if (parts.size() < 6) {
            return {};
        }

        std::string accountId = parts[4];
        std::string queueName = parts[5];
        parts.clear();

        return endpoint + "/" + accountId + "/" + queueName;
    }

    std::string AwsUtils::CreateSNSTopicArn(const std::string &region, const std::string &accountId, const std::string &topicName) {
        return CreateArn("sns", region, accountId, topicName);
    }

    std::string AwsUtils::CreateSNSSubscriptionArn(const std::string &region, const std::string &accountId, const std::string &topicName) {
        return CreateArn("sns", region, accountId, topicName + ":" + Poco::UUIDGenerator().createRandom().toString());
    }

    std::string AwsUtils::CreateTransferArn(const std::string &region, const std::string &accountId, const std::string &serverId) {
        return CreateArn("transfer", region, accountId, "manager/" + serverId);
    }

    std::string AwsUtils::CreateSecretArn(const std::string &region, const std::string &accountId, const std::string &secretId) {
        return CreateArn("secretsmanager", region, accountId, "secret:" + secretId);
    }

    std::string AwsUtils::CreateKMSKeyArn(const std::string &region, const std::string &accountId, const std::string &keyId) {
        return CreateArn("kms", region, accountId, "key/" + keyId);
    }

    std::string AwsUtils::GetAuthorizationHeader(const Configuration &configuration, const std::string &module) {
        std::string accountId = configuration.getString("awsmock.account.userPoolId");
        std::string clientId = configuration.getString("awsmock.client.userPoolId");
        std::string region = configuration.getString("awsmock.region");
        return "AWS4-HMAC-SHA256 Credential=" + accountId + "/" + clientId + "/" + region + "/" + module + "/aws4_request, SignedHeaders=host;x-amz-date;x-amz-security-token, Signature=90d0e45560fa4ce03e6454b7a7f2a949e0c98b46c35bccb47f666272ec572840";
    }

    std::string AwsUtils::CreateRequestId() {
        return Poco::UUIDGenerator().createRandom().toString();
    }

    std::string AwsUtils::CreateMessageId() {
        return Poco::UUIDGenerator().createRandom().toString();
    }

    std::string AwsUtils::SanitzeSQSUrl(const std::string &queue) {
        if (Core::StringUtils::ContainsIgnoreCase(queue, "http")) {
            return queue;
        }

        if (Core::StringUtils::ContainsIgnoreCase(queue, "arn")) {
            return Core::AwsUtils::ConvertSQSQueueArnToUrl(Core::Configuration::instance(), queue);
        }
        return Core::AwsUtils::CreateSqsQueueUrl(Core::Configuration::instance(), queue);
    }
}// namespace AwsMock::Core
