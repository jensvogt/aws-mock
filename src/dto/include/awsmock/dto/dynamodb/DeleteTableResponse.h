//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_DELETE_TABLE_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_DELETE_TABLE_RESPONSE_H

// C++ standard includes
#include <string>
#include <sstream>
#include <vector>
#include <utility>

// Poco includes
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/ServiceException.h>
#include <awsmock/dto/dynamodb/ProvisionedThroughput.h>
#include <awsmock/dto/dynamodb/TableStatus.h>

namespace AwsMock::Dto::DynamoDb {

  struct DeleteTableResponse {

    /**
     * Region
     */
    std::string region;

    /**
     * Table name
     */
    std::string tableName;

    /**
     * Table ID
     */
    std::string tableId;

    /**
     * Table ARN
     */
    std::string tableArn;

    /**
     * Key schema
     */
    std::map<std::string, std::string> keySchemas;

    /**
     * Tags
     */
    std::map<std::string, std::string> tags;

    /**
     * Attribute definitions
     */
    std::map<std::string, std::string> attributes;

    /**
     * Provisioned throughput
     */
    ProvisionedThroughput provisionedThroughput;

    /**
     * Table status
     */
    TableStatus tableStatus;

    /**
     * HTTP response body
     */
    std::string body;

    /**
     * Creates a JSON string from the object.
     *
     * @return JSON string
     */
    [[nodiscard]] std::string ToJson();

    /**
     * Parse a JSON stream
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
    friend std::ostream &operator<<(std::ostream &os, const DeleteTableResponse &r);

  };

} // namespace AwsMock::Dto::lambda

#endif // AWSMOCK_DTO_DYNAMODB_DELETE_TABLE_RESPONSE_H