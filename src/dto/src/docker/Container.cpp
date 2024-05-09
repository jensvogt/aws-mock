//
// Created by vogje01 on 22/09/2023.
//

#include <awsmock/dto/docker/Container.h>

namespace AwsMock::Dto::Docker {

    int Container::GetLambdaPort() {
        auto it =
                find_if(ports.begin(), ports.end(), [](const Port &port) { return port.privatePort == LAMBDA_INTERNAL_PORT; });
        if (it != ports.end()) {
            return it->publicPort;
        }
        return -1;
    }

    void Container::FromJson(Poco::JSON::Object::Ptr object) {

        try {
            // Names array
            Poco::JSON::Array::Ptr namesArray = object->getArray("Names");
            if (!namesArray.isNull()) {
                for (const auto &nt: *namesArray) {
                    names.push_back(nt.convert<std::string>());
                }
            }

            // Ports array
            Poco::JSON::Array::Ptr portsArray = object->getArray("Ports");
            if (!portsArray.isNull()) {
                for (const auto &p: *portsArray) {
                    ports.emplace_back(p.extract<Poco::JSON::Object::Ptr>());
                }
            }

            Core::JsonUtils::GetJsonValueString("Id", object, id);
            Core::JsonUtils::GetJsonValueString("Image", object, image);
            Core::JsonUtils::GetJsonValueString("ImageID", object, imageId);
            Core::JsonUtils::GetJsonValueString("Command", object, command);
            Core::JsonUtils::GetJsonValueString("State", object, state);
            Core::JsonUtils::GetJsonValueString("Status", object, status);

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    Poco::JSON::Object Container::ToJsonObject() const {

        try {

            Poco::JSON::Object rootJson;
            rootJson.set("id", id);
            rootJson.set("image", image);
            rootJson.set("imageId", imageId);
            rootJson.set("command", command);
            rootJson.set("state", state);
            rootJson.set("names", Core::JsonUtils::GetJsonStringArray(names));

            return rootJson;

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    std::string Container::ToJson() const {
        return Core::JsonUtils::ToJsonString(ToJsonObject());
    }

    std::string Container::ToString() const {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Container &c) {
        os << "ListContainerResponse=" << c.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Docker
