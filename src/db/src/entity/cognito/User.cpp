//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/cognito/User.h>

namespace AwsMock::Database::Entity::Cognito {

  view_or_value<view, value> User::ToDocument() const {

    auto userAttributesDoc = bsoncxx::builder::basic::array{};
    for (const auto &attribute : userAttributes) {
      userAttributesDoc.append(make_document(kvp(attribute.name, attribute.value)));
    }

    view_or_value<view, value> userDocument = make_document(
        kvp("region", region),
        kvp("userName", userName),
        kvp("userPoolId", userPoolId),
        kvp("enabled", enabled),
        kvp("userStatus", Entity::Cognito::UserStatusToString(userStatus)),
        kvp("userAttributes", userAttributesDoc),
        kvp("created",
            bsoncxx::types::b_date(std::chrono::milliseconds(created.timestamp().epochMicroseconds() / 1000))),
        kvp("modified",
            bsoncxx::types::b_date(std::chrono::milliseconds(modified.timestamp().epochMicroseconds() / 1000))));
    return userDocument;

  }

  void User::FromDocument(mongocxx::stdx::optional<bsoncxx::document::view> mResult) {

    oid = mResult.value()["_id"].get_oid().value.to_string();
    region = bsoncxx::string::to_string(mResult.value()["region"].get_string().value);
    userName = bsoncxx::string::to_string(mResult.value()["userName"].get_string().value);
    userPoolId = bsoncxx::string::to_string(mResult.value()["userPoolId"].get_string().value);
    enabled = mResult.value()["enabled"].get_bool().value;
    userStatus =
        Entity::Cognito::UserStatusFromString(bsoncxx::string::to_string(mResult.value()["userStatus"].get_string().value));
    created = Poco::DateTime(Poco::Timestamp::fromEpochTime(
        bsoncxx::types::b_date(mResult.value()["created"].get_date().value) / 1000));
    modified = Poco::DateTime(Poco::Timestamp::fromEpochTime(
        bsoncxx::types::b_date(mResult.value()["modified"].get_date().value) / 1000));

    bsoncxx::array::view attributesView{mResult.value()["userAttributes"].get_array().value};
    for (bsoncxx::array::element attributeElement : attributesView) {
      UserAttribute attribute{
          .name=bsoncxx::string::to_string(attributeElement["name"].get_string().value),
          .value=bsoncxx::string::to_string(attributeElement["value"].get_string().value)
      };
      userAttributes.push_back(attribute);
    }
  }

  Poco::JSON::Object User::ToJsonObject() const {
    Poco::JSON::Object jsonObject;
    jsonObject.set("region", region);
    jsonObject.set("userName", userName);
    jsonObject.set("userPoolId", userPoolId);
    jsonObject.set("enabled", enabled);
    jsonObject.set("userStatus", Entity::Cognito::UserStatusToString(userStatus));

    // Attributes
    Poco::JSON::Array jsonAttributeArray;
    for (const auto &attribute : userAttributes) {
      Poco::JSON::Object jsonAttribute;
      jsonAttribute.set("name", attribute.name);
      jsonAttribute.set("value", attribute.value);
      jsonAttributeArray.add(jsonAttribute);
    }
    jsonObject.set("userAttributes", jsonAttributeArray);

    return jsonObject;
  }

  void User::FromJsonObject(const Poco::JSON::Object::Ptr &jsonObject) {

    Core::JsonUtils::GetJsonValueString("region", jsonObject, region);
    Core::JsonUtils::GetJsonValueString("userName", jsonObject, userName);
    Core::JsonUtils::GetJsonValueString("userPoolId", jsonObject, userPoolId);
    Core::JsonUtils::GetJsonValueBool("enabled", jsonObject, enabled);
    std::string userStatusStr;
    Core::JsonUtils::GetJsonValueString("userStatus", jsonObject, userStatusStr);
    userStatus = UserStatusFromString(userStatusStr);

    Poco::JSON::Array::Ptr jsonAttributeArray = jsonObject->getArray("userAttributes");
    for (int i = 0; i < jsonAttributeArray->size(); i++) {
      UserAttribute userAttribute;
      Poco::JSON::Object::Ptr jsonAttributeObject = jsonAttributeArray->getObject(i);
      Core::JsonUtils::GetJsonValueString("name", jsonAttributeObject, userAttribute.name);
      Core::JsonUtils::GetJsonValueString("value", jsonAttributeObject, userAttribute.value);
      userAttributes.emplace_back(userAttribute);
    }
  }

  std::string User::ToString() const {
    std::stringstream ss;
    ss << (*this);
    return ss.str();
  }

  std::ostream &operator<<(std::ostream &os, const User &u) {
    os << "User={oid='" << u.oid << "', region='" + u.region + "', userName='" << u.userName << "', enabled='"
       << u.enabled << "', userStatus='" << Entity::Cognito::UserStatusToString(u.userStatus) << "', userAttributes={";
    for (const auto &attribute : u.userAttributes) {
      os << attribute.name << "'" << attribute.value << "', ";
    }
    os << "\b\b" << "}";
    os << ", created='" << Poco::DateTimeFormatter().format(u.created, Poco::DateTimeFormat::HTTP_FORMAT)
       << "', modified='" << Poco::DateTimeFormatter().format(u.modified, Poco::DateTimeFormat::HTTP_FORMAT) << "'}";
    return os;
  }
}