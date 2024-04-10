//
// Created by vogje01 on 22/09/2023.
//

#include <awsmock/dto/docker/Container.h>

namespace AwsMock::Dto::Docker {

  int Container::GetLambdaPort() {
    auto it = find_if(ports.begin(), ports.end(), [](const Port &port) { return port.privatePort == LAMBDA_INTERNAL_PORT; });
    if (it != ports.end()) {
      return it->publicPort;
    }
    return -1;
  }

  void Container::FromJson(Poco::JSON::Object::Ptr object) {

    try {
      // Names array
      Poco::JSON::Array::Ptr namesArray = object->getArray("Names");
      if (namesArray != nullptr) {
        for (const auto &nt : *namesArray) {
          names.push_back(nt.convert<std::string>());
        }
      }

      // Ports array
      Poco::JSON::Array::Ptr portsArray = object->getArray("Ports");
      if (portsArray != nullptr) {
        for (const auto &p : *portsArray) {
          ports.push_back(Port(p.extract<Poco::JSON::Object::Ptr>()));
        }
      }

      Core::JsonUtils::GetJsonValueString("Id", object, id);
      Core::JsonUtils::GetJsonValueString("Image", object, image);
      Core::JsonUtils::GetJsonValueString("ImageID", object, imageId);
      Core::JsonUtils::GetJsonValueString("Command", object, command);
      Core::JsonUtils::GetJsonValueString("State", object, state);
      Core::JsonUtils::GetJsonValueString("Status", object, status);

    } catch (Poco::Exception &exc) {
      throw Core::ServiceException(exc.message(), 500);
    }
  }

  std::string Container::ToString() const {
    std::stringstream ss;
    ss << (*this);
    return ss.str();
  }

  std::ostream &operator<<(std::ostream &os, const Container &c) {
    os << "ListContainerResponse={userPoolId='" << c.id << "' image='" << c.image << "' imageId='" << c.imageId << "' command='" << c.command << "' state='" << c.state <<
       "' state='" << c.status << "' names='";
    for (auto &it : c.names) {
      os << it << ",";
    }
    os << "'}";
    return os;
  }

} // namespace AwsMock::Dto::Docker
