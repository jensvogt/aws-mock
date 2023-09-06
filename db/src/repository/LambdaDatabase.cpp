//
// Created by vogje01 on 29/05/2023.
//

#include "awsmock/repository/LambdaDatabase.h"

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;

    LambdaDatabase::LambdaDatabase(const Core::Configuration &configuration) : Database(configuration), _logger(Poco::Logger::get("LambdaDatabase")) {

        // Get collection
        _lambdaCollection = GetConnection()["lambda"];
    }

    bool LambdaDatabase::LambdaExists(const std::string &region, const std::string &function, const std::string &runtime) {

        int64_t count = _lambdaCollection.count_documents(make_document(kvp("region", region), kvp("function", function), kvp("runtime", runtime)));
        log_trace_stream(_logger) << "Lambda function exists: " << (count > 0 ? "true" : "false") << std::endl;
        return count > 0;
    }

    bool LambdaDatabase::LambdaExists(const Entity::Lambda::Lambda &lambda) {

        return LambdaExists(lambda.region, lambda.function, lambda.runtime);
    }

    bool LambdaDatabase::LambdaExists(const std::string &functionName) {

        int64_t count = _lambdaCollection.count_documents(make_document(kvp("function", functionName)));
        log_trace_stream(_logger) << "Lambda function exists: " << (count > 0 ? "true" : "false") << std::endl;
        return count > 0;
    }

    bool LambdaDatabase::LambdaExistsByArn(const std::string &arn) {

        int64_t count = _lambdaCollection.count_documents(make_document(kvp("arn", arn)));
        log_trace_stream(_logger) << "Lambda function exists: " << (count > 0 ? "true" : "false") << std::endl;
        return count > 0;
    }

    Entity::Lambda::Lambda LambdaDatabase::CreateLambda(const Entity::Lambda::Lambda &lambda) {

        auto result = _lambdaCollection.insert_one(lambda.ToDocument());
        log_trace_stream(_logger) << "Bucket created, oid: " << result->inserted_id().get_oid().value.to_string() << std::endl;

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

        log_trace_stream(_logger) << "Lambda updated: " << lambda.ToString() << std::endl;

        return GetLambdaByArn(lambda.arn);
    }

    Entity::Lambda::Lambda LambdaDatabase::GetLambdaByArn(const std::string &arn) {

        mongocxx::stdx::optional<bsoncxx::document::value> mResult = _lambdaCollection.find_one(make_document(kvp("arn", arn)));
        Entity::Lambda::Lambda result;
        result.FromDocument(mResult);

        return result;
    }

    std::vector<Entity::Lambda::Lambda> LambdaDatabase::ListLambdas(const std::string &region) {

        std::vector<Entity::Lambda::Lambda> lambdas;
        auto lamdaCursor = _lambdaCollection.find(make_document(kvp("region", region)));
        for (auto lambda : lamdaCursor) {
            Entity::Lambda::Lambda result;
            result.FromDocument(lambda);
            lambdas.push_back(result);
        }
        log_trace_stream(_logger) << "Got lamda list, size:" << lambdas.size() << std::endl;

        return lambdas;
    }

    void LambdaDatabase::DeleteLambda(const std::string &functionName) {

        auto result = _lambdaCollection.delete_many(make_document(kvp("function", functionName)));
        log_debug_stream(_logger) << "Lambda deleted, function: " << functionName << " count: " << result->deleted_count() << std::endl;
    }

    void LambdaDatabase::DeleteAllLambdas() {
        auto result = _lambdaCollection.delete_many({});
        log_debug_stream(_logger) << "All lambdas deleted, count: " << result->deleted_count() << std::endl;
    }

} // namespace AwsMock::Database
