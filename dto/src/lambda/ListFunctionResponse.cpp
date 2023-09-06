//
// Created by vogje01 on 06/09/2023.
//

#include <awsmock/dto/lambda/ListFunctionResponse.h>

namespace AwsMock::Dto::Lambda {

    std::string ListFunctionResponse::ToJson() {

        for (auto &lambda : lambdaList) {
            functions.push_back({.codeSize=lambda.memorySize,
                                    .functionArn=lambda.arn,
                                    .functionName=lambda.function,
                                    .handler=lambda.handler,
                                    .lastModified=lambda.modified});
        }

        try {
            Poco::JSON::Object rootJson;
            Poco::JSON::Array recordsJsonArray;
            for (const auto &function : functions) {
                recordsJsonArray.add(function.ToJsonObject());
            }
            rootJson.set("Functions", recordsJsonArray);

            std::ostringstream os;
            rootJson.stringify(os);
            return os.str();

        } catch (Poco::Exception &exc) {
            throw Core::ServiceException(exc.message(), 500);
        }
    }

    std::string ListFunctionResponse::ToString() const {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListFunctionResponse &r) {
        os << "ListFunctionResponse={[";
        for (const auto &f : r.functions) {
            os << f.ToString();
        }
        os << "]}";
        return os;
    }

} // namespace AwsMock::Dto::Lambda
