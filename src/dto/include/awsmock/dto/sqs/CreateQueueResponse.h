//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_CREATEQUEUERESPONSE_H
#define AWSMOCK_DTO_SQS_CREATEQUEUERESPONSE_H

// C++ standard includes
#include <string>
#include <sstream>
#include <iostream>

// Poco includes
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/UUID.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/DOMWriter.h"
#include "Poco/XML/XMLWriter.h"
#include <Poco/JSON/Object.h>

// AwsMock includes
#include <awsmock/core/ServiceException.h>

namespace AwsMock::Dto::SQS {

  struct CreateQueueResponse {

    /**
     * Region
     */
    std::string region;

    /**
     * Name
     */
    std::string name;

    /**
     * Owner
     */
    std::string owner;

    /**
     * Queue URL
     */
    std::string queueUrl;

    /**
     * Queue ARN
     */
    std::string queueArn;

    /**
     * Convert to a JSON string
     *
     * @return JSON string
     */
    [[nodiscard]] std::string ToJson() const;

    /**
     * Convert to XML representation
     *
     * @return XML string
     */
    [[nodiscard]] std::string ToXml() const;

    /**
     * Convert from XML representation
     *
     * @param xmlString  XML string
     */
    void FromXml(const std::string &xmlString);

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
    friend std::ostream &operator<<(std::ostream &os, const CreateQueueResponse &r);

  };

} // namespace AwsMock::Dto::SQS

#endif // AWSMOCK_DTO_SQS_CREATEQUEUERESPONSE_H
