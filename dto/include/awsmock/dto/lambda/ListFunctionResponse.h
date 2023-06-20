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
#include "CreateFunctionResponse.h"

namespace AwsMock::Dto::Lambda {

    struct DeadLetterConfig {

      /**
       * Target ARN
       */
      std::string targetArn;

      /**
       * Converts the DTO to a JSON representation.
       *
       * @return DTO as string for logging.
       */
      [[nodiscard]] Poco::JSON::Object ToJsonObject() const {
          try {
              Poco::JSON::Object rootJson;
              rootJson.set("TargetArn", targetArn);
              return rootJson;
          } catch (Poco::Exception &exc) {
              throw Core::ServiceException(exc.message(), 500);
          }
      }

    };

    struct Function {

      /**
       * Architectures
       */
      std::vector<std::string> architectures;

      /**
       * SHA256 checksum
       */
      std::string codeSha256;

      /**
       * Code size
       */
      long codeSize;

      /**
       * Dead letter config
       */
      DeadLetterConfig deadLetterConfig;

      /**
       * Description
       */
      std::string description;
      // TODO: Environment, EpheremalStorage, FileSystemConfig

      /**
       * Function ARN
       */
      std::string functionArn;

      /**
       * Function name
       */
      std::string functionName;

      /**
       * Function handler
       */
      std::string handler;

      // TODO: ImageConfigResponse

      /**
       * Last modification datetime
       */
      Poco::DateTime lastModified;

      /**
       * Last update status
       */
      std::string lastUpdateStatus;

      /**
       * Last update status reason
       */
      std::string lastUpdateStatusReason;

      /**
       * Last update status reason
       */
      std::string lastUpdateStatusReasonCode;

      // TODO: Layers

      /**
       * State
       */
      std::string state;

      /**
       * State reason
       */
      std::string stateReason;

      /**
       * State reason code
       */
      std::string stateReasonCode;

      /**
       * Timeout
       */
      int timeout;

      /**
       * Version
       */
      std::string version;

      /**
       * Converts the DTO to a JSON representation.
       *
       * @return DTO as string for logging.
       */
      [[nodiscard]] Poco::JSON::Object ToJsonObject() const {

          try {
              Poco::JSON::Object rootJson;
              Poco::JSON::Array architectureArray;
              for (const auto &archituecture : architectures) {
                  architectureArray.add(archituecture);
              }
              rootJson.set("Architectures", architectureArray);
              rootJson.set("CodeSha256", codeSha256);
              rootJson.set("CodeSize", codeSize);
              rootJson.set("DeadLetterConfig", deadLetterConfig.ToJsonObject());
              rootJson.set("Description", description);
              // TODO: Environment, EphemeralStorage, FileSystemConfig
              rootJson.set("FunctionArn", functionArn);
              rootJson.set("FunctionName", functionName);
              rootJson.set("Handler", handler);
              rootJson.set("FunctionName", functionName);
              // TODO: ImageConfig
              rootJson.set("LastModified", Poco::DateTimeFormatter().format(lastModified, Poco::DateTimeFormat::HTTP_FORMAT));
              rootJson.set("LastUpdateStatus", lastUpdateStatusReason);
              rootJson.set("LastUpdateStatusCode", lastUpdateStatusReasonCode);
              rootJson.set("State", state);
              rootJson.set("StateReson", stateReason);
              rootJson.set("StateReasonCode", stateReasonCode);
              return rootJson;

          } catch (Poco::Exception &exc) {
              throw Core::ServiceException(exc.message(), 500);
          }
      }

      /**
       * Converts the DTO to a string representation.
       *
       * @return DTO as string for logging.
       */
      [[nodiscard]] std::string ToString() const {
          std::stringstream ss;
          ss << (*this);
          return ss.str();
      }

      /**
       * Stream provider.
       *
       * @return output stream
       */
      friend std::ostream &operator<<(std::ostream &os, const Function &f) {
          os << "Function={functionName='" << f.functionName << "' handler='" << f.handler << "' functionName='" << f.functionName << "' version='" << f.version <<
             "' timeout='" << f.timeout << "' state='" << f.state << "' stateReason='" << f.stateReason << "' stateReasonCode='" << f.stateReasonCode << "'}";
          return os;
      }

    };

    typedef std::vector<Function> FunctionList;

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
      [[nodiscard]] std::string ToJson() const {

          try {
              Poco::JSON::Object rootJson;
              Poco::JSON::Array recordsJsonArray;
              for (const auto &function : functions) {
                  recordsJsonArray.add(function.ToJsonObject());
              }

              std::ostringstream os;
              rootJson.stringify(os);
              return os.str();

          } catch (Poco::Exception &exc) {
              throw Core::ServiceException(exc.message(), 500);
          }
      }

      /**
       * Converts the DTO to a string representation.
       *
       * @return DTO as string for logging.
       */
      [[nodiscard]] std::string ToString() const {
          std::stringstream ss;
          ss << (*this);
          return ss.str();
      }

      /**
       * Stream provider.
       *
       * @return output stream
       */
      friend std::ostream &operator<<(std::ostream &os, const ListFunctionResponse &r) {
          os << "ListFunctionResponse={[";
          for(const auto &f : r.functions) {
              os << f.ToString();
          }
          os << "]}";
          return os;
      }
    };

} // namespace AwsMock::Dto::Lambda

#endif // AWSMOCK_DTO_LAMBDA_LISTFUNCTIONREQUEST_H
