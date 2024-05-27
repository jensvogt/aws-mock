//
// Created by vogje01 on 26/08/2022.
//

#ifndef AWSMOCK_CORE_METRIC_DEFINITION_H
#define AWSMOCK_CORE_METRIC_DEFINITION_H

// HTTP timer
#define GATEWAY_GET_TIMER "gateway_get_time"
#define GATEWAY_PUT_TIMER "gateway_put_time"
#define GATEWAY_POST_TIMER "gateway_post_time"
#define GATEWAY_DELETE_TIMER "gateway_delete_time"
#define GATEWAY_OPTIONS_TIMER "gateway_options_time"
#define GATEWAY_HEAD_TIMER "gateway_head_time"
#define GATEWAY_COUNTER "gateway_count"

#define MODULE_GET_TIMER "manager_get_time"
#define MODULE_PUT_TIMER "manager_put_time"
#define MODULE_POST_TIMER "manager_post_time"
#define MODULE_DELETE_TIMER "manager_delete_time"
#define MODULE_OPTIONS_TIMER "manager_options_time"
#define MODULE_HEAD_TIMER "manager_head_time"
#define MODULE_COUNTER "manager_count"

// SNS counter, timer
#define SNS_TOPIC_COUNT "sns_topic_count_total"
#define SNS_MESSAGE_COUNT "sns_topic_count_total"
#define SNS_MESSAGE_BY_TOPIC_COUNT "sns_message_by_topic_count"
#define SNS_SERVICE_TIMER "sns_service_timer"

// SQS counter, timer
#define SQS_QUEUE_COUNT "sqs_queue_count_total"
#define SQS_MESSAGE_COUNT "sqs_message_count_total"
#define SQS_MESSAGE_BY_QUEUE_COUNT "sqs_message_by_queue_count"
#define SQS_SERVICE_TIMER "sqs_service_timer"

// S3 counter, timer
#define S3_BUCKET_COUNT "s3_bucket_count_total"
#define S3_OBJECT_COUNT "s3_object_count_total"
#define S3_OBJECT_BY_BUCKET_COUNT "s3_object_by_bucket_count"
#define S3_SERVICE_TIMER "s3_service_time"

// Lambda counter, timer
#define LAMBDA_FUNCTION_COUNT "lambda_function_count_total"
#define LAMBDA_INVOCATION_TIMER "lambda_invocation_time"
#define LAMBDA_INVOCATION_COUNT "lambda_invocation_count"
#define LAMBDA_SERVICE_TIMER "lambda_service_time"

#define DYNAMODB_TABLE_COUNT "dynamodb_table_count_total"
#define DYNAMODB_ITEM_COUNT "dynamodb_item_count_total"
#define DYNAMODB_SERVICE_TIMER "dynamodb_service_time"

#define SECRETSMANAGER_SECRETS_COUNT "secretsmanager_secret_count_total"

#define COGNITO_USER_COUNT "cognito_user_count_total"
#define COGNITO_USERPOOL_COUNT "cognito_userpool_count_total"
#define COGNITO_USER_BY_USERPOOL_COUNT "cognito_user_by_userpool_count"
#define COGNITO_SERVICE_TIMER "cognito_service_timer"

#define TRANSFER_SERVER_COUNT "transfer_server_count_total"
#define TRANSFER_SERVER_UPLOAD_COUNT "transfer_upload_count_total"
#define TRANSFER_SERVER_DOWNLOAD_COUNT "transfer_download_count_total"

// KMS counter, timer
#define KMS_KEY_COUNT "kms_key_count_total"
#define KMS_KEYACCESS_COUNT "kms_key_access_count_total"
#define KMS_ENCRYPTION_COUNT "kms_encryption_count_totel"
#define KMS_DECRYPTION_COUNT "kms_decryption_count_totel"
#define KMS_SERVICE_TIMER "kms_service_timer"

#endif// AWSMOCK_CORE_METRIC_DEFINITION_H