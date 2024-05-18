//
// Created by vogje01 on 4/20/24.
//

#ifndef AWSMOCK_DTO_COGNITO_CUSTOM_EMAIL_SENDER_H
#define AWSMOCK_DTO_COGNITO_CUSTOM_EMAIL_SENDER_H

// C++ standard includes
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/cognito/BaseRequest.h>

namespace AwsMock::Dto::Cognito {

    /**
     * Cognito custom email sender
     *
     * Example:
     * @code{.json}
     * "CustomEmailSender": {
     *    "LambdaArn": "string",
     *    "LambdaVersion": "string"
     * },
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CustomEmailSender : public BaseRequest {

        /**
         * Lambda ARN
         */
        std::string lambdaArn;

        /**
         * Lambda version
         */
        std::string lambdaVersion;

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Convert to a JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] Poco::JSON::Object ToJsonObject() const;

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
        friend std::ostream &operator<<(std::ostream &os, const CustomEmailSender &r);
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_CUSTOM_EMAIL_SENDER_H
