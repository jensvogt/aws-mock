//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_CREATE_TABLE_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_CREATE_TABLE_RESPONSE_H

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
#include <awsmock/dto/dynamodb/TableStatus.h>

namespace AwsMock::Dto::DynamoDb {

  /**
   * Example:
   * <pre>
   * {
   *   "TableDescription":
   *     {
   *       "AttributeDefinitions": [
   *           {
   *             "AttributeName":"orgaNr",
   *             "AttributeType":"N"
   *           }
   *       ],
   *       "TableName":"test-table",
   *       "KeySchema":[
   *         {
   *           "AttributeName":"orgaNr",
   *           "KeyType":"HASH"
   *         }
   *       ],
   *       "TableStatus":"ACTIVE",
   *       "CreationDateTime":1703158038.811,
   *       "ProvisionedThroughput":
   *         {
   *           "LastIncreaseDateTime":0.000,
   *           "LastDecreaseDateTime":0.000,
   *           "NumberOfDecreasesToday":0,
   *           "ReadCapacityUnits":1,
   *           "WriteCapacityUnits":1
   *         },
   *       "TableSizeBytes":0,
   *       "ItemCount":0,
   *       "TableArn":"arn:aws:dynamodb:ddblocal:000000000000:table/test-table",
   *       "DeletionProtectionEnabled":false
   *    }
   * }
   * </pre>
   */
  struct CreateTableResponse {

    /**
     * Region
     */
    std::string region;

    /**
     * Table class
     */
    std::string tableClass;

    /**
     * Table name
     */
    std::string tableName;

    /**
     * Table ARN
     */
    std::string tableArn;

    /**
     * Table size
     */
    long tableSizeBytes;

    /**
     * Item count
     */
    long itemCount;

    /**
     * Delete protection enabled
     */
    bool deleteProtectionEnabled;

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
     * Table status
     */
    TableStatus tableStatus;

    /**
     * Original HTTP response body
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
    friend std::ostream &operator<<(std::ostream &os, const CreateTableResponse &r);

  };

} // namespace AwsMock::Dto::lambda

#endif // AWSMOCK_DTO_DYNAMODB_CREATE_TABLE_RESPONSE_H
