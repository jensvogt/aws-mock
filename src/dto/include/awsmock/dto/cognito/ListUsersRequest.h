//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USERS_REQUEST_H
#define AWSMOCK_DTO_COGNITO_LIST_USERS_REQUEST_H

// C++ standard includes
#include <string>
#include <sstream>

// AwsMock includes
#include <awsmock/core/JsonException.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>

namespace AwsMock::Dto::Cognito {

  struct ListUsersRequest {

    /**
     * AWS region
     */
    std::string region;

    /**
     * User pool ID
     */
    std::string userPoolId;

    /**
     * Convert from a JSON object.
     *
     * @param payload json string object
     */
    void FromJson(const std::string &payload);

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
    friend std::ostream &operator<<(std::ostream &os, const ListUsersRequest &i);

  };

} // namespace AwsMock::Dto::Cognito

#endif // AWSMOCK_DTO_COGNITO_LIST_USERS_REQUEST_H
