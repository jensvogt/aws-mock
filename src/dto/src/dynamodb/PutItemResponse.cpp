//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/dto/dynamodb/PutItemResponse.h>

namespace AwsMock::Dto::DynamoDb {

  std::string PutItemResponse::ToJson() {

    try {
      Poco::JSON::Object rootJson;
      rootJson.set("Region", region);
      rootJson.set("TableName", tableName);

      std::ostringstream os;
      rootJson.stringify(os);
      return os.str();

    } catch (Poco::Exception &exc) {
      throw Core::ServiceException(exc.message(), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
    }
  }

  void PutItemResponse::FromJson(const std::string &jsonString) {

    body = jsonString;

    Poco::JSON::Parser parser;
    Poco::Dynamic::Var result = parser.parse(jsonString);
    Poco::JSON::Object::Ptr rootObject = result.extract<Poco::JSON::Object::Ptr>();

    try {

      //Poco::JSON::Object::Ptr jsonTableDescription = rootObject->getObject("TableDescription");

      //Core::JsonUtils::GetJsonValueString("TableName", jsonTableDescription, tableName);

    } catch (Poco::Exception &exc) {
      std::cerr << exc.message()  <<std::endl;
      throw Core::ServiceException(exc.message(), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
    }
  }

  std::string PutItemResponse::ToString() const {
    std::stringstream ss;
    ss << (*this);
    return ss.str();
  }

  std::ostream &operator<<(std::ostream &os, const PutItemResponse &r) {
    os << "PutItemResponse={region='" << r.region << "', tableName='" << r.tableName << "}";
    return os;
  }

} // namespace AwsMock::Dto::DynamoDb
