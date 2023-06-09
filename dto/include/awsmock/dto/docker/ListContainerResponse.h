//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_LISTCONTAINERRESPONSE_H
#define AWSMOCK_DTO_DOCKER_LISTCONTAINERRESPONSE_H

// C++ includes
#include <string>
#include <sstream>
#include <vector>

// Poco includes
#include <Poco/DateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>

namespace AwsMock::Dto::Docker {

    struct Container {

      /**
       * Image ID
       */
      std::string id;

      /**
       * Container names
       */
      std::vector<std::string> names;

      /**
       * Image
       */
      std::string image;

      /**
       * Image ID
       */
      std::string imageId;

      /**
       * Command
       */
      std::string command;

      /**
       * State
       */
      std::string state;

      /**
       * Status
       */
      std::string status;

      /**
       * Convert to a JSON string
       *
       * @return JSON string
       */
      void FromJson(Poco::JSON::Object::Ptr object) {

          try {
              Poco::JSON::Array::Ptr namesArray = object->getArray("Names");
              if (namesArray != nullptr) {
                  for (Poco::JSON::Array::ConstIterator nt = namesArray->begin(); nt != namesArray->end(); ++nt) {
                      names.push_back(nt->convert<std::string>());
                  }
              }
              id = object->get("Id").convert<std::string>();
              image = object->get("Image").convert<std::string>();
              imageId = object->get("ImageID").convert<std::string>();
              command = object->get("Command").convert<std::string>();
              state = object->get("State").convert<std::string>();
              status = object->get("Status").convert<std::string>();
              object->clear();

          } catch (Poco::Exception &exc) {
              std::cerr << exc.message() << std::endl;
              throw Core::ServiceException(exc.message(), 500);
          }
      }

      /**
       * Converts the DTO to a string representation.
       *
       * @return DTO as string for logging.
       */
      [[nodiscard]] std::string
      ToString() const {
          std::stringstream ss;
          ss << (*this);
          return ss.str();
      }

      /**
       * Stream provider.
       *
       * @return output stream
       */
      friend std::ostream &operator<<(std::ostream &os, const Container &c) {
          os << "ListContainerResponse={id='" + c.id + "' image='" + c.image + "' imageId='" + c.imageId + "' command='" + c.command + "' state='" + c.state +
              "' status='" + c.status + "' names='";
          for (auto &it : c.names) {
              os << it + ",";
          }
          os << "'}";
          return os;
      }

    };

    struct ListContainerResponse {

      /**
       * Container list
       */
      std::vector<Container> containerList;

      /**
       * Convert to a JSON string
       *
       * @return JSON string
       */
      void FromJson(const std::string &body) {

          try {
              Poco::JSON::Parser parser;
              Poco::Dynamic::Var result = parser.parse(body);
              Poco::JSON::Array::Ptr rootArray = result.extract<Poco::JSON::Array::Ptr>();
              if (rootArray != nullptr) {
                  for (Poco::JSON::Array::ConstIterator it = rootArray->begin(); it != rootArray->end(); ++it) {
                      Container container;
                      container.FromJson(it->extract<Poco::JSON::Object::Ptr>());
                      containerList.push_back(container);
                  }
              }
              // Cleanup
              rootArray->clear();
              parser.reset();

          } catch (Poco::Exception &exc) {
              std::cerr << exc.message() << std::endl;
              throw Core::ServiceException(exc.message(), 500);
          }
      }

    };

} // namespace AwsMock::Dto::Docker

#endif //AWSMOCK_DTO_DOCKER_LISTCONTAINERRESPONSE_H
