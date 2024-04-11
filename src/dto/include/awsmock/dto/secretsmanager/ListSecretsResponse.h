//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SECRETMANAGER_LIST_SECRETS_RESPONSE_H
#define AWSMOCK_DTO_SECRETMANAGER_LIST_SECRETS_RESPONSE_H

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
#include "awsmock/core/JsonException.h"
#include <awsmock/dto/secretsmanager/ReplicationStatus.h>
#include <awsmock/dto/secretsmanager/VersionIdsToStages.h>
#include <awsmock/dto/secretsmanager/Secret.h>
#include <awsmock/dto/secretsmanager/SecretTags.h>

namespace AwsMock::Dto::SecretsManager {

  /**
   * List secrets response
   *
   * Example:
   * <pre>
   * {
   *    "NextToken": "string",
   *    "SecretList": [
   *    .....
   *   ]
   * }
   */
  struct ListSecretsResponse {

    /**
     * Region
     */
    std::string region;

    /**
     * Next token
     */
    std::string nextToken;

    /**
     * List of secrets
     */
    std::vector<Secret> secretList;

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
    friend std::ostream &operator<<(std::ostream &os, const ListSecretsResponse &r);

  };

} // namespace AwsMock::Dto

#endif // AWSMOCK_DTO_SECRETMANAGER_LIST_SECRETS_RESPONSE_H
