//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWSMOCK_DTO_COMMON_SERVICES_H
#define AWSMOCK_DTO_COMMON_SERVICES_H

// C++ standard includes
#include <string>
#include <utility>

// Poco includes
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Array.h>
#include <Poco/JSON/Parser.h>
#include <Poco/UUID.h>
#include <Poco/UUIDGenerator.h>

// AwsMock includes
#include <awsmock/core/ServiceException.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/entity/cognito/User.h>
#include <awsmock/entity/cognito/UserPool.h>
#include <awsmock/entity/lambda/Lambda.h>
#include <awsmock/entity/transfer/Transfer.h>
#include <awsmock/entity/sns/Topic.h>
#include <awsmock/entity/sns/Message.h>
#include <awsmock/entity/sqs/Queue.h>
#include <awsmock/entity/sqs/Message.h>
#include <awsmock/entity/s3/Bucket.h>
#include <awsmock/entity/s3/Object.h>

namespace AwsMock::Dto::Common {

  struct Services {

    /**
     * Service names
     */
    std::vector<std::string>serviceNames;

    bool HasService(const std::string &service);

    /**
     * JSON representation
     *
     * @return Infrastructure as JSON string
     */
    std::string ToJson();

    /**
     * JSON representation
     *
     * @param payload HTTP request body
     */
    void FromJson(const std::string &payload);
  };

} // namespace AwsMock::Dto

#endif // AWSMOCK_DTO_COMMON_SERVICES_H