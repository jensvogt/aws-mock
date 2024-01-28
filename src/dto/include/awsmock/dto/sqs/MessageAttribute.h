//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_MESSAGE_ATTRIBUTE_H
#define AWSMOCK_DTO_SQS_MESSAGE_ATTRIBUTE_H

// C++ standard includes
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <algorithm>

// AwsMock includes
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/sqs/MessageAttributeDataType.h>

namespace AwsMock::Dto::SQS {

  struct MessageAttribute {

    /**
     * Message attribute name
     */
    std::string name;

    /**
     * Message attribute string value
     */
    std::string stringValue = {};

    /**
     * Message attribute number value
     */
    long numberValue = -1;

    /**
     * Message attribute binary value
     */
    unsigned char* binaryValue = nullptr;

    /**
     * Logical data type
     */
    MessageAttributeDataType type;

    /**
     * Transport data type
     */
    std::string transportType;

    /**
     * System attribute flag
     */
    bool systemAttribute = true;

    /**
     * Returns the MD5 sum of all userAttributes.
     *
     * @param attributes vector of userAttributes
     * @param systemAttribute system attribute flag, if true only system attributes are taken into account
     * @return MD5 sum of userAttributes string
     */
    static std::string GetMd5Attributes(const std::map<std::string, MessageAttribute> &attributes, bool systemAttribute);

    /**
     * Returns the MD5 sum of all userAttributes.
     *
     * @param attributes vector of userAttributes
     * @param systemAttribute system attribute flag, if true only system attributes are taken into account
     * @return MD5 sum of userAttributes string
     */
    static std::string GetMd5UserAttributes(const std::map<std::string, MessageAttribute> &attributes);

    static std::string GetMd5SystemAttributes(const std::map<std::string, MessageAttribute> &attributes);

    static void updateLengthAndBytes(EVP_MD_CTX *context, const std::string &str);

    /**
     * Returns a integer as byte array and fill it in the given byte array at position offset.
     *
     * @param n integer value
     * @param bytes output byte array
     * @param offset offset of the output byte array
     */
    static void GetIntAsByteArray(size_t n, unsigned char *bytes, int offset);

    /**
     * Convert from JSON string
     *
     * @param attributeObject attribute object
     */
    void FromJsonObject(const Poco::JSON::Object::Ptr &attributeObject);

    /**
     * Name comparator
     *
     * @param other
     * @return
     */
    bool operator<(const MessageAttribute &other) const;

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
    friend std::ostream &operator<<(std::ostream &os, const MessageAttribute &r);

  };

  typedef std::map<std::string, MessageAttribute> MessageAttributeList;

} // namespace AwsMock::Dto::SQS

#endif // AWSMOCK_DTO_SQS_MESSAGE_ATTRIBUTE_H
