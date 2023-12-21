//
// Created by vogje01 on 21/12/2023.
//

#include <awsmock/dto/dynamodb/DeleteTableRequest.h>

namespace AwsMock::Dto::DynamoDb {

  std::string DeleteTableRequest::ToJson() {

    try {
      Poco::JSON::Object rootJson;
      rootJson.set("Region", region);
      rootJson.set("TableName", tableName);

      std::ostringstream os;
      rootJson.stringify(os);
      return os.str();

    } catch (Poco::Exception &exc) {
      throw Core::ServiceException(exc.message(), 500);
    }
  }

  void DeleteTableRequest::FromJson(const std::string &jsonBody) {

    Poco::JSON::Parser parser;
    Poco::Dynamic::Var result = parser.parse(jsonBody);
    Poco::JSON::Object::Ptr rootObject = result.extract<Poco::JSON::Object::Ptr>();

    try {

      Core::JsonUtils::GetJsonValueString("Region", rootObject, region);
      Core::JsonUtils::GetJsonValueString("TableName", rootObject, tableName);

    } catch (Poco::Exception &exc) {
      throw Core::ServiceException(exc.message(), 500);
    }
  }

  std::string DeleteTableRequest::ToString() const {
    std::stringstream ss;
    ss << (*this);
    return ss.str();
  }

  std::ostream &operator<<(std::ostream &os, const DeleteTableRequest &r) {
    os << "DeleteTableRequest={region='" << r.region << "', tableName='" << r.tableName << "'}";
    return os;
  }

} // namespace AwsMock::Dto::lambda
