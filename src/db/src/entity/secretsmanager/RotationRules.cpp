//
// Created by vogje01 on 24/09/2023.
//

#include <awsmock/entity/secretsmanager/RotationRules.h>

namespace AwsMock::Database::Entity::SecretsManager {

  view_or_value<view, value> RotationRules::ToDocument() const {

    view_or_value<view, value> rotationRulesDoc = make_document(
      kvp("automaticallyAfterDays", automaticallyAfterDays),
      kvp("duration", duration),
      kvp("scheduleExpression", scheduleExpression));

    return rotationRulesDoc;
  }

  void RotationRules::FromDocument(mongocxx::stdx::optional<bsoncxx::document::view> mResult) {

    automaticallyAfterDays = mResult.value()["automaticallyAfterDays"].get_int64().value;
    duration = mResult.value()["duration"].get_string().value;
    scheduleExpression = mResult.value()["scheduleExpression"].get_string().value;
  }

  Poco::JSON::Object RotationRules::ToJsonObject() const {

    Poco::JSON::Object jsonObject;
    jsonObject.set("automaticallyAfterDays", automaticallyAfterDays);
    jsonObject.set("duration", duration);
    jsonObject.set("scheduleExpression", scheduleExpression);
    return jsonObject;

  }

  std::string RotationRules::ToString() const {
    std::stringstream ss;
    ss << (*this);
    return ss.str();
  }

  std::ostream &operator<<(std::ostream &os, const RotationRules &s) {
    os << "RotationRules=" << bsoncxx::to_json(s.ToDocument());
    return os;
  }

} // namespace AwsMock::Database::Entity::S3