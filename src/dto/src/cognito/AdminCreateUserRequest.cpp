//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/AdminCreateUserRequest.h>

namespace AwsMock::Dto::Cognito {

    std::string AdminCreateUserRequest::ToJson() const {
        try {
            Poco::JSON::Object rootJson;
            rootJson.set("Region", region);
            rootJson.set("UserPoolId", userPoolId);
            rootJson.set("Username", userName);
            rootJson.set("TemporaryPassword", temporaryPassword);

            return Core::JsonUtils::ToJsonString(rootJson);

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    void AdminCreateUserRequest::FromJson(const std::string &payload) {

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(payload);
        const auto &rootObject = result.extract<Poco::JSON::Object::Ptr>();

        try {

            Core::JsonUtils::GetJsonValueString("Region", rootObject, region);
            Core::JsonUtils::GetJsonValueString("UserPoolId", rootObject, userPoolId);
            Core::JsonUtils::GetJsonValueString("Username", rootObject, userName);
            Core::JsonUtils::GetJsonValueString("TemporaryPassword", rootObject, temporaryPassword);

            // Message action
            if (rootObject->has("MessageAction")) {
                messageAction = MessageActionFromString(rootObject->get("MessageAction").convert<std::string>());
            }

            Poco::JSON::Array::Ptr attributesArray = rootObject->getArray("UserAttributes");

            if (!attributesArray.isNull()) {
                for (const auto &it: *attributesArray) {
                    if (!it.isEmpty()) {
                        const auto &object = it.extract<Poco::JSON::Object::Ptr>();
                        UserAttribute userAttribute;
                        userAttribute.FromJson(object);
                        userAttributes.emplace_back(userAttribute);
                    }
                }
            }

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    std::string AdminCreateUserRequest::ToString() const {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const AdminCreateUserRequest &r) {
        os << "AdminCreateUserRequest=" << r.ToJson();
        return os;
    }
}// namespace AwsMock::Dto::Cognito