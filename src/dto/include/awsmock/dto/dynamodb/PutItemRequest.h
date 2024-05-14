//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_PUT_ITEM_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_PUT_ITEM_REQUEST_H

// C++ standard includes
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// AwsMock includes
#include "awsmock/core/exception/JsonException.h"
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/dynamodb/AttributeValue.h>
#include <awsmock/dto/dynamodb/GetItemKey.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * DynamoDB put item request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct PutItemRequest {

        /**
         * Region
         */
        std::string region;

        /**
         * The name of the table to contain the item.
         *
         * <p>
         * Pattern: [a-zA-Z0-9_.-]+
         * </p>
         */
        std::string tableName;

        /**
         * Item
         *
         * <p>
         * A map of attribute name/value pairs, one for each attribute. Only the primary key attributes are required; you
         * can optionally provide other attribute name-value pairs for the item.
         * </p>
         * <p>
         * You must provide all of the attributes for the primary key. For example, with a simple primary key, you only
         * need to provide a value for the partition key. For a composite primary key, you must provide both values for
         * both the partition key and the sort key.
         */
        std::map<std::string, AttributeValue> item;

        /**
         * Conditional expression
         *
         * <p>
         * A condition that must be satisfied in order for a conditional PutItem operation to succeed.
         * </p>
         */
        std::string conditionalExpression;

        /**
         * Return consumed capacity.
         *
         * <p>
         * Determines the level of detail about either provisioned or on-demand throughput consumption that is returned in
         * the response: INDEXES | TOTAL | NONE
         * </p>
         */
        bool returnConsumedCapacity;

        /**
         * Return values
         *
         * <p>
         * Use ReturnValues if you want to get the item attributes as they appeared before they were updated with the
         * PutItem request. For PutItem, the valid values are: NONE | ALL_OLD.
         */
        std::string returnValues;

        /**
         * Original HTTP request body.
         *
         * <p>
         * Needed for the request to the docker image.
         * </p>
         */
        std::string body;

        /**
         * Original HTTP request headers.
         *
         * <p>
         * Needed for the request to the docker image.
         * </p>
         */
        std::map<std::string, std::string> headers;

        /**
         * Creates a JSON string from the object.
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

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
        friend std::ostream &operator<<(std::ostream &os, const PutItemRequest &r);
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_PUT_ITEM_REQUEST_H
