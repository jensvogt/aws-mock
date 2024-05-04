//
// Created by vogje01 on 4/30/24.
//

#ifndef AWSMOCK_DTO_LAMBDA_TAGS_H
#define AWSMOCK_DTO_LAMBDA_TAGS_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonException.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/ServiceException.h>

namespace AwsMock::Dto::Lambda {

    struct Tags {

        /**
         * Tags
         */
        std::vector<std::pair<std::string, std::string>> tags;

        /**
         * Checks whether a tags with the given tags key exists.
         *
         * @param key key of the tags
         * @return true if tags with the given key exists.
         */
        bool HasTag(const std::string &key);

        /**
         * Returns a given tags value by key
         *
         * @param key key of the tags
         * @return tag value
         */
        std::string GetTagValue(const std::string &key);

        /**
         * Convert to a JSON string
         *
         * @param object JSON object
         */
        void FromJson(Poco::JSON::Object::Ptr object);

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
        friend std::ostream &operator<<(std::ostream &os, const Tags &r);
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_TAGS_H
