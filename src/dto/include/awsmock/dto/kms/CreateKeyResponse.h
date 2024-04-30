//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_CREATE_KEY_RESPONSE_H
#define AWSMOCK_DTO_KMS_CREATE_KEY_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

// Poco includes
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>

// AwsMock includes
#include <awsmock/core/JsonException.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/kms/Key.h>
#include <awsmock/dto/kms/KeySpec.h>

namespace AwsMock::Dto::KMS {

    /**
     * Create KMS key response
     *
     * Example:
     * @verbatim
     * {
     *   "Key": {
     *      "Arn": "string",
     *      "AWSAccountId": "string",
     *      "CloudHsmClusterId": "string",
     *      "CreationDate": number,
     *      "KeySpec": "string",
     *      "CustomKeyStoreId": "string",
     *      "DeletionDate": number,
     *      "Description": "string",
     *      "Enabled": boolean,
     *      "EncryptionAlgorithms": [ "string" ],
     *      "ExpirationModel": "string",
     *      "KeyId": "string",
     *      "KeyManager": "string",
     *      "KeySpec": "string",
     *      "KeyState": "string",
     *      "KeyUsage": "string",
     *      "MacAlgorithms": [ "string" ],
     *      "MultiRegion": boolean,
     *      "MultiRegionConfiguration": {
     *         "MultiRegionKeyType": "string",
     *         "PrimaryKey": {
     *            "Arn": "string",
     *            "Region": "string"
     *         },
     *         "ReplicaKeys": [
     *           {
     *             "Arn": "string",
     *             "Region": "string"
     *           }
     *         ]
     *      },
     *      "Origin": "string",
     *      "PendingDeletionWindowInDays": number,
     *      "SigningAlgorithms": [ "string" ],
     *      "ValidTo": number,
     *      "XksKeyConfiguration": {
     *         "Id": "string"
     *      }
     *    }
     * }
     * @endverbatim
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateKeyResponse {

        /**
         * Key metadata
         */
        Key key;

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as JSON string.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const CreateKeyResponse &r);
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_CREATE_KEY_RESPONSE_H
