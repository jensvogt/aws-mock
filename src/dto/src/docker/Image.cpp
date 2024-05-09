//
// Created by JVO on 22.04.2024.
//

#include <awsmock/dto/docker/Image.h>

namespace AwsMock::Dto::Docker {

    void Image::FromJson(const std::string &jsonString) {

        try {
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse(jsonString);

            this->FromJson(result.extract<Poco::JSON::Object::Ptr>());

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    void Image::FromJson(Poco::JSON::Object::Ptr jsonObject) {

        try {
            Core::JsonUtils::GetJsonValueString("Id", jsonObject, id);
            Core::JsonUtils::GetJsonValueString("ParentId", jsonObject, parentId);
            Core::JsonUtils::GetJsonValueLong("Size", jsonObject, size);
            Core::JsonUtils::GetJsonValueLong("SharedSize", jsonObject, sharedSize);
            Core::JsonUtils::GetJsonValueLong("VirtualSize", jsonObject, virtualSize);
            Core::JsonUtils::GetJsonValueInt("Containers", jsonObject, containers);

            Poco::JSON::Array::Ptr reproTagsArray = jsonObject->getArray("RepoTags");
            if (!reproTagsArray.isNull()) {
                for (const auto &nt: *reproTagsArray) {
                    repoTags.push_back(nt.convert<std::string>());
                }
            }

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    Poco::JSON::Object Image::ToJsonObject() const {

        try {

            Poco::JSON::Object rootJson;
            rootJson.set("Id", id);
            rootJson.set("ParentId", parentId);
            rootJson.set("Size", size);
            rootJson.set("SharedSize", sharedSize);
            rootJson.set("VirtualSize", virtualSize);
            rootJson.set("Containers", containers);
            rootJson.set("RepoTags", Core::JsonUtils::GetJsonStringArray(repoTags));

            return rootJson;

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    std::string Image::ToJson() const {
        return Core::JsonUtils::ToJsonString(ToJsonObject());
    }

    std::string Image::ToString() const {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Image &i) {
        os << "Image=" << i.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Docker
