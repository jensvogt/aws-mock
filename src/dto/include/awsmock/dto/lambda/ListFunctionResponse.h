//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_LISTFUNCTIONREQUEST_H
#define AWSMOCK_DTO_LAMBDA_LISTFUNCTIONREQUEST_H

// C++ standard includes
#include <string>
#include <sstream>
#include <vector>

// Poco includes
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

// AwsMock includes
#include <awsmock/dto/lambda/Environment.h>
#include <awsmock/dto/lambda/Function.h>
#include <awsmock/entity/lambda/Lambda.h>

namespace AwsMock::Dto::Lambda {

  struct ListFunctionResponse {

    /**
     * List of queues
     */
    Database::Entity::Lambda::LambdaList lambdaList;

    /**
     * Name of the function
     */
    FunctionList functions;

    /**
     * Convert to a JSON string
     *
     * @return JSON string
     */
    [[nodiscard]] std::string ToJson();

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
    friend std::ostream &operator<<(std::ostream &os, const ListFunctionResponse &r);

  };

} // namespace AwsMock::Dto::lambda

#endif // AWSMOCK_DTO_LAMBDA_LISTFUNCTIONREQUEST_H
