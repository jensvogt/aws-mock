//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_CREATE_FUNCTION_RESPONSE_H
#define AWSMOCK_DTO_LAMBDA_CREATE_FUNCTION_RESPONSE_H

// C++ standard includes
#include <sstream>
#include <string>
#include <vector>

// Poco includes
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/dto/lambda/model/Environment.h>
#include <awsmock/dto/lambda/model/EphemeralStorage.h>

namespace AwsMock::Dto::Lambda {

    /**
     * Create function response
     *
     * Example:
     * @code{.json}
     * {
     *   "FunctionName": "ftp-file-copy",
     *   "FunctionArn": "arn:aws:lambda:eu-central-1:000000000000:function:ftp-file-copy",
     *   "Runtime": "java11",
     *   "Role": "arn:aws:iam::000000000000:role/ignoreme",
     *   "Handler": "org.springframework.cloud.function.adapter.aws.FunctionInvoker",
     *   "CodeSize": 23155331,
     *   "Description": "",
     *   "Timeout": 3,
     *   "MemorySize": 128,
     *   "LastModified": "2023-06-06T12:52:34.721631+0000",
     *   "CodeSha256": "kkUavVRsVEFvEIXyUwhl+n8/oaz50GYVleY9F3MiuYU=",
     *   "Version": "$LATEST",
     *     "Environment": {
     *       "Variables": {
     *         "JAVA_TOOL_OPTIONS": "-Duser.timezone=Europe/Berlin -Dspring.profiles.active=localstack"
     *       }
     *     },
     *   "TracingConfig": {
     *      "Mode": "PassThrough"
     *    },
     *   "RevisionId": "24e41752-636b-4f0f-840d-92bcc4e572ff",
     *   "State": "Pending",
     *   "StateReason": "The function is being created.",
     *   "StateReasonCode": "Creating",
     *   "PackageType": "Zip",
     *   "Architectures": [
     *     "x86_64"
     *   ],
     *   "EphemeralStorage": {
     *     "Size": 512
     *   },
     *   "SnapStart": {
     *     "ApplyOn": "None",
     *     "OptimizationStatus": "Off"
     *    },
     *    "RuntimeVersionConfig": {
     *    "RuntimeVersionArn": "arn:aws:lambda:eu-central-1::runtime:8eeff65f6809a3ce81507fe733fe09b835899b99481ba22fd75b5a7338290ec1"
     *   }
     * }
     * @endcode
     */
    struct CreateFunctionResponse {

        /**
         * Name of the function
         */
        std::string functionArn;

        /**
         * Name of the function
         */
        std::string functionName;

        /**
         * Runtime environment
         */
        std::string runtime;

        /**
         * Role
         */
        std::string role;

        /**
         * Role
         */
        std::string handler;

        /**
         * Environment
         */
        EnvironmentVariables environment;

        /**
         * Memory size
         */
        long memorySize = 128;

        /**
         * Code size
         */
        long codeSize;

        /**
         * Description
         */
        std::string description;

        /**
         * Timeout
         */
        int timeout = 3;

        /**
         * Code SHA256
         */
        std::string codeSha256;

        /**
         * Temporary disk space in MB
         */
        EphemeralStorage ephemeralStorage;

        /**
         * Modified
         */
        std::string modified = Core::DateTimeUtils::AwsDatetimeNow();

        /**
         * Docker image ID
         */
        std::string dockerImageId;

        /**
         * Docker container ID
         */
        std::string dockerContainerId;

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
        friend std::ostream &operator<<(std::ostream &os, const CreateFunctionResponse &r);
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_CREATE_FUNCTION_RESPONSE_H
