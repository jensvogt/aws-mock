//
// Created by vogje01 on 11/26/23.
//

#ifndef AWSMOCK_DTO_COMMON_DYNAMODB_CLIENT_COMMAND_H
#define AWSMOCK_DTO_COMMON_DYNAMODB_CLIENT_COMMAND_H

// C++ includes
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/core/exception/UnauthorizedException.h>
#include <awsmock/dto/common/BaseClientCommand.h>
#include <awsmock/dto/common/HttpMethod.h>

namespace AwsMock::Dto::Common {

    enum class DynamoDbCommandType {
        CREATE_TABLE,
        LIST_TABLES,
        DESCRIBE_TABLE,
        DELETE_TABLE,
        GET_ITEM,
        PUT_ITEM,
        QUERY,
        SCAN,
        DELETE_ITEM,
        UNKNOWN
    };

    static std::map<DynamoDbCommandType, std::string> DynamoDbCommandTypeNames{
            {DynamoDbCommandType::CREATE_TABLE, "CreateTable"},
            {DynamoDbCommandType::LIST_TABLES, "ListTables"},
            {DynamoDbCommandType::DESCRIBE_TABLE, "DescribeTable"},
            {DynamoDbCommandType::DELETE_TABLE, "DeleteTable"},
            {DynamoDbCommandType::GET_ITEM, "GetItem"},
            {DynamoDbCommandType::PUT_ITEM, "PutItem"},
            {DynamoDbCommandType::QUERY, "Query"},
            {DynamoDbCommandType::SCAN, "Scan"},
            {DynamoDbCommandType::DELETE_ITEM, "DeleteItem"},
            {DynamoDbCommandType::UNKNOWN, "Unknown"},
    };

    [[maybe_unused]] static std::string DynamoDbCommandTypeToString(DynamoDbCommandType commandType) {
        return DynamoDbCommandTypeNames[commandType];
    }

    [[maybe_unused]] static DynamoDbCommandType DynamoDbCommandTypeFromString(const std::string &commandType) {
        for (auto &it: DynamoDbCommandTypeNames) {
            if (Core::StringUtils::StartsWith(commandType, it.second)) {
                return it.first;
            }
        }
        return DynamoDbCommandType::UNKNOWN;
    }

    /**
     * @brief The DynamoDB client command is used as a standardized way of interpreting the different ways the clients are calling the REST services.
     *
     * Each client type is using a different way of calling the AWS REST services.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DynamoDbClientCommand : public BaseClientCommand {

        /**
         * Client command
         */
        DynamoDbCommandType command{};

        /**
         * @brief Gets the client command from the HTTP target header.
         *
         * @param request HTTP request
         * @return client command from AWS target header
         */
        static DynamoDbCommandType GetClientCommandFromHeader(const Poco::Net::HTTPServerRequest &request);

        /**
         * Getś the value from the user-agent string
         *
         * @param method HTTP method
         * @param request HTTP server request
         * @param region AWS region
         * @param user AWS user
         */
        void FromRequest(const HttpMethod &method, Poco::Net::HTTPServerRequest &request, const std::string &region, const std::string &user);

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
        friend std::ostream &operator<<(std::ostream &os, const DynamoDbClientCommand &i);

        /**
         * AWS secret access key
         *
         * @return output stream
         */
        std::string _secretAccessKey = Core::Configuration::instance().getString("awsmock.secret.access.key", "none");
    };

}// namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_DYNAMODB_CLIENT_COMMAND_H
