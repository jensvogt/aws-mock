//
// Created by vogje01 on 06/06/2023.
//

#include <awsmock/dto/docker/ListImageResponse.h>

namespace AwsMock::Dto::Docker {

    ListImageResponse::ListImageResponse(const std::string &body) {
        FromJson(body);
    }

    void ListImageResponse::FromJson(const std::string &jsonString) {

        try {
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse(jsonString);
            Poco::JSON::Array::Ptr rootArray = result.extract<Poco::JSON::Array::Ptr>();
            if (!rootArray.isNull()) {
                for (const auto &it: *rootArray) {
                    Image image;
                    image.FromJson(it.extract<Poco::JSON::Object::Ptr>());
                    imageList.push_back(image);
                }
            }

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    std::string ListImageResponse::ToJson() const {

        try {

            Poco::JSON::Object rootJson;

            Poco::JSON::Array imageArray;
            for (const auto &image: imageList) {
                imageArray.add(image.ToJsonObject());
            }

            return Core::JsonUtils::ToJsonString(rootJson);

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    std::string ListImageResponse::ToString() const {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListImageResponse &r) {
        os << "ImageListResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Docker
