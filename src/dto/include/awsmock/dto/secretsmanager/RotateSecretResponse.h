//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_ROTATE_SECRET_RESPONSE_H
#define AWSMOCK_CORE_DTO_ROTATE_SECRET_RESPONSE_H

// C++ standard includes
#include <string>
#include <sstream>

// Poco includes
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/Net/HTTPResponse.h>

// AwsMoc includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/ServiceException.h>
#include <awsmock/dto/secretsmanager/RotationRules.h>

namespace AwsMock::Dto::SecretsManager {

  /**
   * Rotate a secret value response.
   *
   * Example:
   * <pre>
   * {
   *   "ARN": "string",
   *   "Name": "string",
   *   "VersionId": "string"
   * }
   * </pre>
   */
  struct RotateSecretResponse {

    /**
     * Region
     */
    std::string region;

    /**
     * Secret ARN
     */
    std::string arn;

    /**
     * Version ID
     */
    std::string versionId;

    /**
     * AWS request ID
     */
    std::string requestId;

    /**
     * Converts the DTO to a JSON representation.
     *
     * @return DTO as string for logging.
     */
    [[nodiscard]] std::string ToJson() const;

    /**
     * Converts the JSON string to DTO.
     *
     * @param jsonString JSON string
     */
    void FromJson(const std::string &jsonString);

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
    friend std::ostream &operator<<(std::ostream &os, const RotateSecretResponse &r);

  };

} // namespace AwsMock::Dto::S3

#endif // AWSMOCK_CORE_DTO_ROTATE_SECRET_RESPONSE_H