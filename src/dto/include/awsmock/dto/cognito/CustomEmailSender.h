//
// Created by vogje01 on 4/20/24.
//

#ifndef AWSMOCK_DTO_COGNITO_CUSTOM_EMAIL_SENDER_H
#define AWSMOCK_DTO_COGNITO_CUSTOM_EMAIL_SENDER_H

// C++ standard includes
#include <string>
#include <sstream>

// AwsMock includes
#include <awsmock/core/JsonException.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/XmlUtils.h>

namespace AwsMock::Dto::Cognito {

  /**
   * Cognito lambda configuration
   *
   * Example:
   * <pre>
   * "CustomEmailSender": {
   *    "LambdaArn": "string",
   *    "LambdaVersion": "string"
   * },
   * </pre>
   */
  struct CustomEmailSender {

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

} // namespace AwsMock::Dto::Cognito

#endif // AWSMOCK_DTO_COGNITO_CUSTOM_EMAIL_SENDER_H