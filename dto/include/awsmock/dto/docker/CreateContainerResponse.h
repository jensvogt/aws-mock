//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_CREATECONTAINERRESPONSE_H
#define AWSMOCK_DTO_DOCKER_CREATECONTAINERRESPONSE_H

// C++ includes
#include <string>
#include <sstream>
#include <vector>

// Poco includes
#include <Poco/DateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

// AwsMock includes
#include <awsmock/core/ServiceException.h>

namespace AwsMock::Dto::Docker {

    struct CreateContainerResponse {

      /**
       * ID
       */
      std::string id;

      /**
       * Warnings
       */
      std::vector<std::string> warnings;

      /**
       * Assigned random port
       */
      int hostPort;

      /**
       * Convert to a JSON string
       *
       * @param body JSON string
       */
      void FromJson(const std::string &body) {

          Poco::JSON::Parser parser;
          Poco::Dynamic::Var result = parser.parse(body);
          Poco::JSON::Object::Ptr rootObject = result.extract<Poco::JSON::Object::Ptr>();

          try {
              id = rootObject->get("Id").convert<std::string>();

              // Cleanup
              rootObject->clear();
              parser.reset();

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
      friend std::ostream &operator<<(std::ostream &os, const CreateContainerResponse &r) {
          os << "CreateContainerResponse={id='" + r.id + "' hostPort='" + std::to_string(r.hostPort) + "'}";
          return os;
      }

    };

} // namespace AwsMock::Dto::Docker

#endif //AWSMOCK_DTO_DOCKER_CREATECONTAINERRESPONSE_H
