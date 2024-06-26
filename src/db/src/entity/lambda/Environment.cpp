//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/lambda/Environment.h>

namespace AwsMock::Database::Entity::Lambda {

    Poco::JSON::Object Environment::ToJsonObject() const {

        Poco::JSON::Object jsonObject;
        if (!variables.empty()) {
            Poco::JSON::Array jsonArray;
            for (const auto &variable: variables) {
                Poco::JSON::Object object;
                object.set("name", variable.first);
                object.set("value", variable.second);
                jsonArray.add(object);
            }
            jsonObject.set("variables", jsonArray);
        }
        return jsonObject;
    }

    void Environment::FromDocument(mongocxx::stdx::optional<bsoncxx::document::view> mResult) {

        if (mResult.value().find("variables") != mResult.value().end()) {
            auto varDoc = mResult.value()["variables"].get_array();
            for (auto &v: varDoc.value) {
                for (auto &it: v.get_document().value) {
                    variables[std::string{it.key()}] = bsoncxx::string::to_string(it.get_string().value);
                }
            }
        }
    }
}// namespace AwsMock::Database::Entity::Lambda