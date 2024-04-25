//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_ADMIN_CREATE_USER_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_ADMIN_CREATE_USER_RESPONSE_H

// C++ standard includes
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonException.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/cognito/MessageAction.h>
#include <awsmock/dto/cognito/UserAttribute.h>

namespace AwsMock::Dto::Cognito {

    /**
     * Create user response
     *
     * @author jens.vogt@opitz-consulting.com
     */
    struct AdminCreateUserResponse {

        /**
         * AWS region
         */
        std::string region;

        /**
         * Name of the user
         */
        std::string userName;

        /**
         * Enabled flag
         */
        bool enabled = false;

        /**
         * User userAttributes list
         */
        UserAttributeList userAttributes;

        /**
         * Convert from a JSON object.
         *
         * @param payload json string object
         */
        void FromJson(const std::string &payload);

        /**
         * Convert from a JSON object.
         *
         * @return JSON representation of the object
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
        friend std::ostream &operator<<(std::ostream &os, const AdminCreateUserResponse &i);
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_ADMIN_CREATE_USER_RESPONSE_H
