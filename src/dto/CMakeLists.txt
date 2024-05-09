
set(LIBRARY awsmockdto)
set(LIBRARY_STATIC awsmockdto_static)

set(COMMON_SOURCES src/common/UserAgent.cpp src/common/S3ClientCommand.cpp src/common/SQSClientCommand.cpp
        src/common/SNSClientCommand.cpp src/common/DynamoDbClientCommand.cpp src/common/Infrastructure.cpp
        src/common/Services.cpp src/common/SecretsManagerClientCommand.cpp src/common/KMSClientCommand.cpp)
set(SQS_SOURCES src/sqs/CreateQueueRequest.cpp src/sqs/CreateQueueResponse.cpp src/sqs/MessageAttribute.cpp
        src/sqs/SendMessageRequest.cpp src/sqs/SendMessageResponse.cpp src/sqs/ListQueueResponse.cpp
        src/sqs/GetQueueAttributesRequest.cpp src/sqs/GetQueueAttributesResponse.cpp src/sqs/TagQueueRequest.cpp
        src/sqs/SetQueueAttributesRequest.cpp src/sqs/SetQueueAttributesResponse.cpp src/sqs/GetQueueUrlRequest.cpp
        src/sqs/RestErrorResponse.cpp src/sqs/GetQueueUrlResponse.cpp src/sqs/ReceiveMessageRequest.cpp
        src/sqs/ReceiveMessageResponse.cpp src/sqs/PurgeQueueRequest.cpp src/sqs/DeleteQueueRequest.cpp
        src/sqs/ChangeMessageVisibilityRequest.cpp src/sqs/DeleteMessageRequest.cpp src/sqs/DeleteMessageBatchEntry.cpp
        src/sqs/DeleteMessageBatchRequest.cpp src/sqs/SqsCommonRequest.cpp src/sqs/DeleteQueueResponse.cpp
        src/sqs/DeleteMessageResponse.cpp)
set(SNS_SOURCES src/sns/SqsNotificationRequest.cpp src/sns/SubscribeRequest.cpp src/sns/SubscribeResponse.cpp
        src/sns/UnsubscribeRequest.cpp src/sns/UnsubscribeResponse.cpp src/sns/CreateTopicRequest.cpp
        src/sns/CreateTopicResponse.cpp src/sns/DeleteTopicResponse.cpp src/sns/ListTopicsResponse.cpp
        src/sns/PublishRequest.cpp src/sns/PublishResponse.cpp src/sns/TagResourceRequest.cpp
        src/sns/TagResourceResponse.cpp src/sns/GetTopicAttributesRequest.cpp src/sns/GetTopicAttributesResponse.cpp
        src/sns/ListSubscriptionsByTopicRequest.cpp src/sns/ListSubscriptionsByTopicResponse.cpp src/sns/Subscription.cpp)
set(S3_SOURCES src/s3/CreateBucketRequest.cpp src/s3/CreateBucketResponse.cpp src/s3/PutBucketVersioningRequest.cpp
        src/s3/PutObjectRequest.cpp src/s3/GetObjectRequest.cpp src/s3/GetObjectResponse.cpp
        src/s3/DeleteObjectRequest.cpp src/s3/DeleteObjectsRequest.cpp src/s3/DeleteObjectsResponse.cpp
        src/s3/CompleteMultipartUploadResult.cpp src/s3/CopyObjectRequest.cpp src/s3/CopyObjectResponse.cpp
        src/s3/MoveObjectRequest.cpp src/s3/MoveObjectResponse.cpp src/s3/CreateBucketConstraint.cpp
        src/s3/model/UserIdentity.cpp src/s3/GetMetadataRequest.cpp src/s3/GetMetadataResponse.cpp
        src/s3/ListAllBucketResponse.cpp src/s3/ListBucketRequest.cpp src/s3/ListBucketResponse.cpp
        src/s3/ListAllBucketResponse.cpp src/s3/DeleteBucketRequest.cpp src/s3/CreateMultipartUploadRequest.cpp
        src/s3/model/Owner.cpp src/s3/model/Content.cpp src/s3/CreateMultipartUploadResult.cpp src/s3/ListObjectVersionsRequest.cpp
        src/s3/ListObjectVersionsResponse.cpp src/s3/PutBucketNotificationRequest.cpp src/s3/model/FilterRule.cpp
        src/s3/PutBucketNotificationConfigurationRequest.cpp src/s3/PutBucketNotificationConfigurationResponse.cpp
        src/s3/model/QueueConfiguration.cpp src/s3/model/TopicConfiguration.cpp src/s3/model/LambdaConfiguration.cpp
        src/s3/PutBucketEncryptionRequest.cpp)
set(DOCKER_SOURCES src/docker/Port.cpp src/docker/Container.cpp src/docker/Image.cpp src/docker/ListContainerResponse.cpp
        src/docker/ListImageResponse.cpp src/docker/CreateContainerRequest.cpp src/docker/Filters.cpp
        src/docker/CreateContainerResponse.cpp)
set(LAMBDA_SOURCES src/lambda/ListTagsResponse.cpp src/lambda/ListFunctionResponse.cpp src/lambda/DeadLetterConfig.cpp
        src/lambda/Function.cpp src/lambda/Code.cpp src/lambda/CreateTagRequest.cpp src/lambda/CreateFunctionRequest.cpp
        src/lambda/GetFunctionResponse.cpp src/lambda/Tags.cpp src/lambda/Error.cpp src/lambda/Environment.cpp)
set(COGNITO_SOURCES src/cognito/ListUserPoolRequest.cpp src/cognito/ListUserPoolResponse.cpp
        src/cognito/CreateUserPoolRequest.cpp src/cognito/CreateUserPoolResponse.cpp src/cognito/UserAttribute.cpp
        src/cognito/DeleteUserPoolRequest.cpp src/cognito/AdminCreateUserRequest.cpp
        src/cognito/AdminCreateUserResponse.cpp src/cognito/AdminDeleteUserRequest.cpp src/cognito/ListUsersRequest.cpp
        src/cognito/ListUsersResponse.cpp src/cognito/LambdaConfig.cpp src/cognito/CustomEmailSender.cpp
        src/cognito/CustomSmsSender.cpp src/cognito/PreTokenGenerationConfig.cpp)
set(TRANSFER_SOURCES src/transfer/Tag.cpp src/transfer/CreateServerRequest.cpp src/transfer/CreateServerResponse.cpp
        src/transfer/CreateUserRequest.cpp src/transfer/CreateUserResponse.cpp src/transfer/DeleteServerRequest.cpp
        src/transfer/IdentityProviderDetails.cpp src/transfer/ListServerRequest.cpp src/transfer/ListServerResponse.cpp
        src/transfer/Server.cpp src/transfer/StartServerRequest.cpp src/transfer/StopServerRequest.cpp)
set(DYNAMODB_SOURCES src/dynamodb/CreateTableRequest.cpp src/dynamodb/CreateTableResponse.cpp
        src/dynamodb/DeleteTableRequest.cpp src/dynamodb/DeleteTableResponse.cpp src/dynamodb/ProvisionedThroughput.cpp
        src/dynamodb/ListTableRequest.cpp src/dynamodb/ListTableRequest.cpp src/dynamodb/ListTableResponse.cpp
        src/dynamodb/GetItemRequest.cpp src/dynamodb/GetItemResponse.cpp src/dynamodb/PutItemRequest.cpp
        src/dynamodb/PutItemResponse.cpp src/dynamodb/QueryRequest.cpp src/dynamodb/QueryResponse.cpp
        src/dynamodb/DeleteItemRequest.cpp src/dynamodb/DeleteItemResponse.cpp src/dynamodb/AttributeValue.cpp
        src/dynamodb/DescribeTableRequest.cpp src/dynamodb/DescribeTableResponse.cpp src/dynamodb/ScanRequest.cpp
        src/dynamodb/ScanResponse.cpp)
set(SECRETSMANAGER_SOURCES src/secretsmanager/CreateSecretRequest.cpp src/secretsmanager/CreateSecretResponse.cpp
        src/secretsmanager/DeleteSecretRequest.cpp src/secretsmanager/DeleteSecretResponse.cpp
        src/secretsmanager/DescribeSecretRequest.cpp src/secretsmanager/DescribeSecretResponse.cpp
        src/secretsmanager/GetSecretValueRequest.cpp src/secretsmanager/GetSecretValueResponse.cpp
        src/secretsmanager/ReplicationStatus.cpp src/secretsmanager/VersionIdsToStages.cpp
        src/secretsmanager/UpdateSecretRequest.cpp src/secretsmanager/UpdateSecretResponse.cpp
        src/secretsmanager/SecretTags.cpp src/secretsmanager/RotateSecretRequest.cpp src/secretsmanager/Filter.cpp
        src/secretsmanager/RotateSecretResponse.cpp src/secretsmanager/RotationRules.cpp src/secretsmanager/Secret.cpp
        src/secretsmanager/ListSecretsRequest.cpp src/secretsmanager/ListSecretsResponse.cpp)
set(KMS_SOURCES src/kms/CreateKeyRequest.cpp src/kms/CreateKeyResponse.cpp src/kms/Key.cpp src/kms/ListKey.cpp
        src/kms/ScheduleKeyDeletionRequest.cpp src/kms/ScheduleKeyDeletionResponse.cpp src/kms/ListKeysRequest.cpp
        src/kms/ListKeysResponse.cpp src/kms/DescribeKeyRequest.cpp src/kms/DescribeKeyResponse.cpp
        src/kms/EncryptRequest.cpp src/kms/EncryptResponse.cpp src/kms/DecryptRequest.cpp src/kms/DecryptResponse.cpp
        src/kms/DescribeKeyRequest.cpp src/kms/DescribeKeyResponse.cpp)
set(MODULE_SOURCES src/module/Module.cpp src/module/GatewayConfig.cpp)

set(SOURCES ${COMMON_SOURCES} ${SQS_SOURCES} ${SNS_SOURCES} ${S3_SOURCES} ${DOCKER_SOURCES} ${LAMBDA_SOURCES}
        ${COGNITO_SOURCES} ${TRANSFER_SOURCES} ${DYNAMODB_SOURCES} ${SECRETSMANAGER_SOURCES} ${KMS_SOURCES}
        ${MODULE_SOURCES})

# Test directory
if (NOT WIN32)
add_subdirectory(tst)
endif()

# Includes
if(WIN32)
    include_directories(./include ../core/include ../db/include ${AWSMOCK_WIN32_DIR}/include)
    link_directories(${AWSMOCK_WIN32_DIR}/lib ../core/${CMAKE_BUILD_TYPE} ../db/${CMAKE_BUILD_TYPE})
else()
    include_directories(include ../core/include ../db/include /usr/include /usr/include/mongo /usr/include/bsoncxx/v_noabi /usr/include/mongocxx/v_noabi)
endif ()

add_library(${LIBRARY} SHARED ${SOURCES})
add_library(${LIBRARY_STATIC} STATIC ${SOURCES})
set_target_properties(${LIBRARY_STATIC} PROPERTIES OUTPUT_NAME ${LIBRARY})

if (CYGWIN)
    target_link_libraries(${LIBRARY} awsmockcore.lib awsmockdb.lib libssl.dll.a libcrypto.dll.a libcurl.dll.a
            libarchive.dll.a bson-1.0.lib bsoncxx-v_noabi-rhb-x64-v143-md.lib mongocxx-v_noabi-rhb-x64-v143-md.lib
            libPocoPrometheus.dll.a libPocoJSON.dll.a libPocoXML.dll.a libPocoNet.dll.a libPocoZip.dll.a
            libPocoUtil.dll.a libPocoFoundation.dll.a)
endif ()

install(TARGETS ${LIBRARY} DESTINATION lib)
