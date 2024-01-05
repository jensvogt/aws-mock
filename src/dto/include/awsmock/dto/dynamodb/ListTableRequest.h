//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_LIST_TABLE_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_LIST_TABLE_REQUEST_H

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
#include "ProvisionedThroughput.h"

namespace AwsMock::Dto::DynamoDb {

  struct ListTableRequest {

    /**
     * AWS region
     */
    std::string region;

    /**
     * The first table name that this operation will evaluate. Use the value that was returned for LastEvaluatedTableName
     * in a previous operation, so that you can obtain the next page of results.
     */
    std::string exclusiveStartTableName;

    /**
     * Limit
     */
    int limit = 0;

    /**
     * Original HTTP request body
     */
    std::string body;

    /**
     * Original HTTP request headers
     */
    std::map<std::string, std::string> headers;

    /**
     * Creates a JSON string from the object.
     *
     * @return JSON string
     */
    [[nodiscard]] std::string ToJson();

    /**
     * Parse a JSON stream
     *
     * @param jsonBody JSON string
     */
    void FromJson(const std::string &jsonBody);

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
    friend std::ostream &operator<<(std::ostream &os, const ListTableRequest &r);

  };

} // namespace AwsMock::Dto::lambda

#endif // AWSMOCK_DTO_DYNAMODB_LIST_TABLE_REQUEST_H