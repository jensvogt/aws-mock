//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_PUTQUEUEATTRIBUTEREQUEST_H
#define AWSMOCK_DTO_SQS_PUTQUEUEATTRIBUTEREQUEST_H

// C++ standard includes
#include <string>
#include <sstream>
#include <map>

// Poco includes
#include "Poco/UUID.h"
#include "Poco/UUIDGenerator.h"
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/ServiceException.h>

namespace AwsMock::Dto::SQS {

  struct SetQueueAttributesRequest {

    /**
     * AWS region
     */
    std::string region;

    /**
     * Queue URL
     */
    std::string queueUrl;

    /**
     * Attributes
     */
    std::map<std::string, std::string> attributes;

    /**
     * Resource
     */
    std::string resource = "SQS";

    /**
     * Resource
     */
    std::string requestId = Poco::UUIDGenerator().createRandom().toString();

    /**
     * Converts the JSON string to a DTO
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
    friend std::ostream &operator<<(std::ostream &os, const SetQueueAttributesRequest &r);

  };

} // namespace AwsMock::Dto::SQS

#endif // AWSMOCK_DTO_SQS_PUTQUEUEATTRIBUTEREQUEST_H
