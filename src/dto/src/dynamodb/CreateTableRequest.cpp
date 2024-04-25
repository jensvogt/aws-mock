//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/dto/dynamodb/CreateTableRequest.h>

namespace AwsMock::Dto::DynamoDb {

    std::string CreateTableRequest::ToJson() const {

        try {
            Poco::JSON::Object rootJson;
            rootJson.set("Region", region);
            rootJson.set("TableClass", tableClass);
            rootJson.set("TableName", tableName);

            Poco::JSON::Array jsonTagsArray;
            for (const auto &tag: tags) {
                Poco::JSON::Object object;
                object.set("Key", tag.first);
                object.set("Value", tag.second);
                jsonTagsArray.add(object);
            }
            rootJson.set("Tags", jsonTagsArray);

            Poco::JSON::Array jsonAttributeArray;
            for (const auto &tag: attributes) {
                Poco::JSON::Object object;
                object.set("attributeName", tag.first);
                object.set("AttributeType", tag.second);
                jsonAttributeArray.add(object);
            }
            rootJson.set("Tags", jsonAttributeArray);

            Poco::JSON::Array jsonKeySchemasArray;
            for (const auto &keySchema: keySchemas) {
                Poco::JSON::Object object;
                object.set("attributeName", keySchema.first);
                object.set("KeyType", keySchema.second);
                jsonKeySchemasArray.add(object);
            }
            rootJson.set("KeySchema", jsonKeySchemasArray);

            return Core::JsonUtils::ToJsonString(rootJson);

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    void CreateTableRequest::FromJson(const std::string &jsonBody) {

        // Save original body
        body = jsonBody;

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(jsonBody);
        Poco::JSON::Object::Ptr rootObject = result.extract<Poco::JSON::Object::Ptr>();

        try {
            Core::JsonUtils::GetJsonValueString("Region", rootObject, region);
            Core::JsonUtils::GetJsonValueString("TableClass", rootObject, tableClass);
            Core::JsonUtils::GetJsonValueString("TableName", rootObject, tableName);

            // Tags
            Poco::JSON::Array::Ptr jsonTagsArray = rootObject->getArray("Tags");
            if (!jsonTagsArray.isNull()) {
                for (size_t i = 0; i < jsonTagsArray->size(); i++) {
                    std::string key, value;
                    Poco::JSON::Object::Ptr jsonTagsObject = jsonTagsArray->getObject(i);
                    Core::JsonUtils::GetJsonValueString("Key", jsonTagsObject, key);
                    Core::JsonUtils::GetJsonValueString("Value", jsonTagsObject, value);
                    tags[key] = value;
                }
            }
            //
            // "{\"AttributeDefinitions\": [{\"AttributeName\": \"orgaNr\", \"AttributeType\": \"N\"}], \"TableName\": \"pim_local_organr_zu_datenlieferant\", \"KeySchema\": [{\"AttributeName\": \"orgaNr\", \"KeyType\": \"HASH\"}], \"ProvisionedThroughput\": {\"ReadCapacityUnits\": 1, \"WriteCapacityUnits\": 1}}"

            // Attributes
            Poco::JSON::Array::Ptr jsonAttributeArray = rootObject->getArray("AttributeDefinitions");
            if (!jsonAttributeArray.isNull()) {
                for (size_t i = 0; i < jsonAttributeArray->size(); i++) {
                    std::string name, type;
                    Poco::JSON::Object::Ptr jsonAttributeObject = jsonAttributeArray->getObject(i);
                    Core::JsonUtils::GetJsonValueString("AttributeName", jsonAttributeObject, name);
                    Core::JsonUtils::GetJsonValueString("AttributeType", jsonAttributeObject, type);
                    attributes[name] = type;
                }
            }

            // Key schemas
            Poco::JSON::Array::Ptr jsonKeySchemasArray = rootObject->getArray("KeySchema");
            if (!jsonKeySchemasArray.isNull()) {
                for (size_t i = 0; i < jsonKeySchemasArray->size(); i++) {
                    std::string attributeName, keyType;
                    Poco::JSON::Object::Ptr jsonKeySchemaObject = jsonKeySchemasArray->getObject(i);
                    Core::JsonUtils::GetJsonValueString("AttributeName", jsonKeySchemaObject, attributeName);
                    Core::JsonUtils::GetJsonValueString("KeyType", jsonKeySchemaObject, keyType);
                    keySchemas[attributeName] = keyType;
                }
            }

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    std::string CreateTableRequest::ToString() const {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const CreateTableRequest &r) {
        os << "CreateTableRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::DynamoDb
