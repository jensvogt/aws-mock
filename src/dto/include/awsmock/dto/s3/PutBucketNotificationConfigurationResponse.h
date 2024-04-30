//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_DTO_S3_PUT_BUCKET_NOTIFICATION_CONFIGURATION_RESPONSE_H
#define AWSMOCK_DTO_S3_PUT_BUCKET_NOTIFICATION_CONFIGURATION_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/JsonException.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/dto/s3/model/LambdaConfiguration.h>
#include <awsmock/dto/s3/model/QueueConfiguration.h>
#include <awsmock/dto/s3/model/TopicConfiguration.h>

namespace AwsMock::Dto::S3 {

    /**
     * S3 Notification configuration
     *
     * Example:
     * @verbatim
       <NotificationConfiguration>
         <TopicConfiguration>
            <Topic>arn:aws:sns:us-east-1:356671443308:s3notificationtopic2</Topic>
            <Event>s3:ReducedRedundancyLostObject</Event>
         </TopicConfiguration>
         <QueueConfiguration>
            <Queue>arn:aws:sqs:us-east-1:356671443308:s3notificationqueue</Queue>
            <Event>s3:ObjectCreated:*</Event>
         </QueueConfiguration>
       </NotificationConfiguration>
     * @endverbatim
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct PutBucketNotificationConfigurationResponse {

        /**
         * Topic configurations
         */
        std::vector<TopicConfiguration> topicConfigurations;

        /**
         * SQS queue configurations
         */
        std::vector<QueueConfiguration> queueConfigurations;

        /**
         * Lambda function configurations
         */
        std::vector<LambdaConfiguration> lambdaConfigurations;

        /**
          * Convert from XML representation
          *
          * @param xmlString XML string
          */
        [[nodiscard]] std::string ToXml();

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const PutBucketNotificationConfigurationResponse &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_PUT_BUCKET_NOTIFICATION_CONFIGURATION_RESPONSE_H