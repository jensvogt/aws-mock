//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/entity/dynamodb/AttributeValue.h>

namespace AwsMock::Database::Entity::DynamoDb {

  Poco::JSON::Object AttributeValue::ToJsonObject() const {

    try {
      Poco::JSON::Object rootJson;
      rootJson.set("S", stringValue);
      rootJson.set("N", numberValue);
      rootJson.set("BOOL", boolValue);
      rootJson.set("NULL", nullValue);

      // String array
      if (!stringSetValue.empty()) {
        Poco::JSON::Array jsonStringArray;
        for (const auto &sValue : stringSetValue) {
          jsonStringArray.add(sValue);
        }
        rootJson.set("SS", jsonStringArray);
      }

      // Number array
      if (!numberSetValue.empty()) {
        Poco::JSON::Array jsonNumberArray;
        for (const auto &nValue : numberSetValue) {
          jsonNumberArray.add(nValue);
        }
        rootJson.set("NS", jsonNumberArray);
      }

      return rootJson;

    } catch (Poco::Exception &exc) {
      throw Core::ServiceException(exc.message(), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
    }
  }

  void AttributeValue::FromJsonObject(const Poco::JSON::Object::Ptr &jsonObject) {

    try {

      for (size_t i = 0; i < jsonObject->getNames().size(); i++) {
        std::string valueType = jsonObject->getNames()[i];
        if (valueType == "S" && jsonObject->has(valueType)) {
          stringValue = jsonObject->getValue<std::string>(valueType);
        } else if (valueType == "SS") {
          Poco::JSON::Array::Ptr jsonNumberArray = jsonObject->getArray("SS");
          for (const auto &nValue : *jsonNumberArray) {
            stringSetValue.emplace_back(nValue.convert<std::string>());
          }
        } else if (valueType == "N") {
          numberValue = jsonObject->get(valueType).convert<std::string>();
        } else if (valueType == "NS") {
          Poco::JSON::Array::Ptr jsonNumberArray = jsonObject->getArray("NS");
          for (const auto &nValue : *jsonNumberArray) {
            numberSetValue.emplace_back(nValue.convert<std::string>());
          }
        } else if (valueType == "BOOL") {
          boolValue = jsonObject->get(valueType).convert<bool>();
        } else if (valueType == "NULL") {
          nullValue = jsonObject->get(valueType).convert<bool>();
        }
      }

    } catch (Poco::Exception &exc) {
      std::cerr << exc.message();
      throw Core::ServiceException(exc.message(), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
    }
  }

  view_or_value<view, value> AttributeValue::ToDocument() const {

    // Convert string set to document
    auto stringSetDoc = bsoncxx::builder::basic::array{};
    for (const auto &sValue : stringSetValue) {
      stringSetDoc.append(sValue);
    }

    // Convert string set to document
    auto numberSetDoc = bsoncxx::builder::basic::array{};
    for (const auto &nValue : numberSetValue) {
      numberSetDoc.append(nValue);
    }

    view_or_value<view, value> attributeDoc = make_document(
        kvp("S", stringValue),
        kvp("SS", stringSetDoc),
        kvp("N", numberValue),
        kvp("NS", numberSetDoc),
        kvp("BOOL", boolValue),
        kvp("NULL", nullValue),
        kvp("created",
            bsoncxx::types::b_date(std::chrono::milliseconds(created.timestamp().epochMicroseconds() / 1000))),
        kvp("modified",
            bsoncxx::types::b_date(std::chrono::milliseconds(modified.timestamp().epochMicroseconds() / 1000))));

    return attributeDoc;
  }

  std::string AttributeValue::ToString() const {
    std::stringstream ss;
    ss << (*this);
    return ss.str();
  }

  std::ostream &operator<<(std::ostream &os, const AttributeValue &a) {
    os << "AttributeValue=" << bsoncxx::to_json(a.ToDocument());
    return os;
  }

} // namespace AwsMock::Dto::lambda
