//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/AdminCreateUserResponse.h>

namespace AwsMock::Dto::Cognito {

    void AdminCreateUserResponse::FromJson(const std::string &payload) {

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(payload);
        const auto &rootObject = result.extract<Poco::JSON::Object::Ptr>();

        try {

            Core::JsonUtils::GetJsonValueString("Region", rootObject, region);
            Core::JsonUtils::GetJsonValueString("UserName", rootObject, userName);

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

    std::string AdminCreateUserResponse::ToJson() const {

        try {
            Poco::JSON::Object rootJson;

            Poco::JSON::Object userJson;
            userJson.set("Username", userName);
            userJson.set("Enabled", enabled);
            rootJson.set("User", userJson);

            return Core::JsonUtils::ToJsonString(rootJson);

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    std::string AdminCreateUserResponse::ToString() const {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const AdminCreateUserResponse &r) {
        os << "AdminCreateUserResponse=" << r.ToJson();
        return os;
    }
}// namespace AwsMock::Dto::Cognito