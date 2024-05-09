//
// Created by vogje01 on 30/09/2023.
//

#include <awsmock/dto/sqs/GetQueueAttributesRequest.h>

namespace AwsMock::Dto::SQS {

    void GetQueueAttributesRequest::FromJson(const std::string &jsonString) {

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(jsonString);
        const auto &rootObject = result.extract<Poco::JSON::Object::Ptr>();

        try {

            // Attributes
            Core::JsonUtils::GetJsonValueString("QueueUrl", rootObject, queueUrl);
            Poco::JSON::Array::Ptr attributesArray = rootObject->getArray("AttributeNames");

            if (!attributesArray.isNull()) {
                for (const auto &it: *attributesArray) {
                    if (!it.isEmpty()) {
                        attributeNames.emplace_back(it.extract<std::string>());
                    }
                }
            }

        } catch (Poco::Exception &exc) {
            throw Core::ServiceException(exc.message(), 500);
        }
    }

    std::string GetQueueAttributesRequest::ToString() const {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const GetQueueAttributesRequest &r) {
        os << "GetQueueAttributesRequest={region='" << r.region
           << "' queueUrl='" + r.queueUrl + "' resource='" + r.resource + "' requestId='" + r.requestId + "' attributeNames=[";
        for (const auto &it: r.attributeNames) {
            os << it;
        }
        os << "]}";
        return os;
    }

}// namespace AwsMock::Dto::SQS
