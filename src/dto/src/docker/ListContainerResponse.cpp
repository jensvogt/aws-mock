//
// Created by vogje01 on 06/06/2023.
//

#include <awsmock/dto/docker/ListContainerResponse.h>

namespace AwsMock::Dto::Docker {

    ListContainerResponse::ListContainerResponse(const std::string &body) {
        FromJson(body);
    }

    void ListContainerResponse::FromJson(const std::string &body) {

        try {
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse(body);
            Poco::JSON::Array::Ptr rootArray = result.extract<Poco::JSON::Array::Ptr>();

            if (!rootArray.isNull()) {
                for (const auto &it: *rootArray) {
                    Container container;
                    container.FromJson(it.extract<Poco::JSON::Object::Ptr>());
                    containerList.push_back(container);
                }
            }

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    std::string ListContainerResponse::ToJson() const {

        try {

            Poco::JSON::Object rootJson;

            Poco::JSON::Array containerArray;
            for (const auto &container: containerList) {
                containerArray.add(container.ToJsonObject());
            }

            return Core::JsonUtils::ToJsonString(rootJson);

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    std::string ListContainerResponse::ToString() const {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListContainerResponse &r) {
        os << "ListContainerResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Docker
