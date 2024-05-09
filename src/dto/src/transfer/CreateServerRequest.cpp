//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/transfer/CreateServerRequest.h>

namespace AwsMock::Dto::Transfer {

    std::string CreateServerRequest::ToJson() const {

        try {

            Poco::JSON::Object rootJson;
            rootJson.set("Region", region);
            rootJson.set("Domain", domain);

            return Core::JsonUtils::ToJsonString(rootJson);

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    void CreateServerRequest::FromJson(const std::string &body) {

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(body);
        Poco::JSON::Object::Ptr rootObject = result.extract<Poco::JSON::Object::Ptr>();

        try {
            Core::JsonUtils::GetJsonValueString("Region", rootObject, region);
            Core::JsonUtils::GetJsonValueString("Domain", rootObject, domain);

            // Protocols
            if (rootObject->has("Protocols")) {
                Poco::JSON::Array::Ptr protocolsArray = rootObject->getArray("Protocols");
                if (!protocolsArray.isNull()) {
                    for (const auto &protocol: *protocolsArray) {
                        protocols.push_back(protocol.convert<std::string>());
                    }
                }
            }

            // Tags
            if (rootObject->has("Tags")) {
                Poco::JSON::Object::Ptr tagsObject = rootObject->getObject("Tags");
                for (const auto &tagName: tagsObject->getNames()) {
                    std::string value;
                    Core::JsonUtils::GetJsonValueString(tagName, tagsObject, value);
                    Tag tag = {.key = tagName, .value = value};
                    tags.emplace_back(tag);
                }
            }

            // Cleanup
            rootObject->clear();
            parser.reset();

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    std::string CreateServerRequest::ToString() const {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const CreateServerRequest &r) {
        os << "CreateServerRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Transfer
