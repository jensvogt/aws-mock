//
// Created by vogje01 on 29/05/2023.
//

#include "awsmock/db/LambdaDatabase.h"

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;

    LambdaDatabase::LambdaDatabase(const Core::Configuration &configuration) : Database(configuration), _logger(Poco::Logger::get("LambdaDatabase")) {

        // Set default console logger
        Core::Logger::SetDefaultConsoleLogger("LambdaDatabase");

        // Get collection
        _lambdaCollection = GetConnection()["lambda"];
    }

    bool LambdaDatabase::LambdaExists(const std::string &function, const std::string &runtime) {

        int64_t count = _lambdaCollection.count_documents(make_document(kvp("function", function), kvp("runtime", runtime)));
        _logger.trace() << "Lambda function exists: " << (count > 0 ? "true" : "false") << std::endl;
        return count > 0;
    }

    bool LambdaDatabase::LambdaExists(const Entity::Lambda::Lambda &lambda) {

        return LambdaExists(lambda.function, lambda.runtime);
    }

    bool LambdaDatabase::LambdaExists(const std::string &functionName) {

        int64_t count = _lambdaCollection.count_documents(make_document(kvp("function", functionName)));
        _logger.trace() << "Lambda function exists: " << (count > 0 ? "true" : "false") << std::endl;
        return count > 0;
    }

    Entity::Lambda::Lambda LambdaDatabase::CreateLambda(const Entity::Lambda::Lambda &lambda) {

        auto result = _lambdaCollection.insert_one(lambda.ToDocument());
        _logger.trace() << "Bucket created, oid: " << result->inserted_id().get_oid().value.to_string() << std::endl;

        return GetLambdaById(result->inserted_id().get_oid().value);
    }

    Entity::Lambda::Lambda LambdaDatabase::GetLambdaById(bsoncxx::oid oid) {

        mongocxx::stdx::optional<bsoncxx::document::value> mResult = _lambdaCollection.find_one(make_document(kvp("_id", oid)));
        Entity::Lambda::Lambda result;
        result.FromDocument(mResult);

        return result;
    }

    Entity::Lambda::Lambda LambdaDatabase::GetLambdaById(const std::string &oid) {
        return GetLambdaById(bsoncxx::oid(oid));
    }

    Entity::Lambda::Lambda LambdaDatabase::CreateOrUpdateLambda(const Entity::Lambda::Lambda &lambda) {

        if (LambdaExists(lambda)) {
            return UpdateLambda(lambda);
        } else {
            return CreateLambda(lambda);
        }
    }

    Entity::Lambda::Lambda LambdaDatabase::UpdateLambda(const Entity::Lambda::Lambda &lambda) {

        auto result = _lambdaCollection.replace_one(make_document(kvp("region", lambda.region), kvp("function", lambda.function), kvp("runtime", lambda.runtime)),
                                                    lambda.ToDocument());

        _logger.trace() << "Lambda updated: " << lambda.ToString() << std::endl;

        return GetLambdaByArn(lambda.arn);
    }

    Entity::Lambda::Lambda LambdaDatabase::GetLambdaByArn(const std::string &arn) {

        mongocxx::stdx::optional<bsoncxx::document::value> mResult = _lambdaCollection.find_one(make_document(kvp("arn", arn)));
        Entity::Lambda::Lambda result;
        result.FromDocument(mResult);

        return result;
    }

    void LambdaDatabase::DeleteLambda(const std::string &functionName) {

        auto result = _lambdaCollection.delete_many(make_document(kvp("function", functionName)));
        _logger.debug() << "Lambda deleted, function: " << functionName << " count: " << result->deleted_count() << std::endl;
    }

    void LambdaDatabase::DeleteAllLambdas() {
        auto result = _lambdaCollection.delete_many({});
        _logger.debug() << "All lambdas deleted, count: " << result->deleted_count() << std::endl;
    }

} // namespace AwsMock::Database
