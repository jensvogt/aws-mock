//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/Database.h>
#include <mongocxx/instance.hpp>

namespace AwsMock::Database {

    using bsoncxx::builder::basic::array;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    // TODO: Remove as connection pool take over
    DatabaseBase::DatabaseBase() : _configuration(Core::Configuration::instance()), _useDatabase(false) {

        _useDatabase = _configuration.getBool("awsmock.mongodb.active", false);
        _name = _configuration.getString("awsmock.mongodb.name", "awsmock");
        _host = _configuration.getString("awsmock.mongodb.host", "localhost");
        _port = _configuration.getInt("awsmock.mongodb.port", 27017);
        _user = _configuration.getString("awsmock.mongodb.user", "admin");
        _password = _configuration.getString("awsmock.mongodb.password", "admin");
        _poolSize = _configuration.getInt("awsmock.mongodb.pool.size", 256);

        /*if (_useDatabase) {

            // URL
            std::string url = "mongodb://" + _user + ":" + _password + "@" + _host + ":" + std::to_string(_port) + "/?maxPoolSize=" + std::to_string(_poolSize);
            mongocxx::uri _uri(url.c_str());

            // MongoDB connection pool
            _pool = std::make_unique<mongocxx::pool>(_uri);
            log_info << "MongoDB database initialized. url: " << url;
        }*/
    }

    mongocxx::database DatabaseBase::GetConnection() {
        mongocxx::pool::entry _client = _pool->acquire();
        return (*_client)[_name];
    }

    mongocxx::pool::entry DatabaseBase::GetClient() {
        if (!_pool) {
            log_fatal << "Pool is NULL";
        }
        return _pool->acquire();
    }

    bool DatabaseBase::HasDatabase() const {
        return _useDatabase;
    }

    std::string DatabaseBase::GetDatabaseName() const {
        return _name;
    }

    void DatabaseBase::StartDatabase() {

        _useDatabase = true;
        _configuration.SetValue("awsmock.mongodb.active", true);

        // Update module database
        mongocxx::pool::entry _client = _pool->acquire();
        (*_client)[_name]["module"].update_one(make_document(kvp("name", "database")), make_document(kvp("$set", make_document(kvp("state", "RUNNING")))));
        log_info << "Database module started, poolSize: " << _poolSize;
    }

    void DatabaseBase::StopDatabase() {

        // Update module database
        _configuration.SetValue("awsmock.mongodb.active", false);
        mongocxx::pool::entry _client = _pool->acquire();
        (*_client)[_name]["module"].update_one(make_document(kvp("name", "database")),
                                               make_document(kvp("$set", make_document(kvp("state", "STOPPED")))));

        _useDatabase = false;
        log_info << "Database module stopped";
    }

    void DatabaseBase::CreateIndexes() const {

        if (_useDatabase) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::database database = (*client)["awsmock"];

            database["sqs_message"].create_index(make_document(kvp("queueUrl", 1), kvp("status", 1), kvp("reset", 1)),
                                                 make_document(kvp("name", "sqs_queueurl_status_reset_idx1")));
            database["sqs_message"].create_index(make_document(kvp("queueUrl", 1), kvp("status", 1), kvp("retries", 1)),
                                                 make_document(kvp("name", "sqs_queueurl_status_retries_idx2")));
            database["sqs_queue"].create_index(make_document(kvp("region", 1), kvp("name", 1)),
                                               make_document(kvp("name", "sqs_region_name_idx1")));
            database["sqs_queue"].create_index(make_document(kvp("region", 1), kvp("url", 1)),
                                               make_document(kvp("name", "sqs_region_url_idx2")));
            database["sns_topic"].create_index(make_document(kvp("region", 1), kvp("arn", 1)),
                                               make_document(kvp("name", "sns_region_arn_idx1")));
            database["sns_message"].create_index(make_document(kvp("region", 1), kvp("topicArn", 1)),
                                                 make_document(kvp("name", "sns_region_topicarn_idx1")));
            database["s3_bucket"].create_index(make_document(kvp("region", 1), kvp("name", 1)),
                                               make_document(kvp("name", "s3_region_name_idx1")));
            database["s3_object"].create_index(make_document(kvp("bucket", 1)),
                                               make_document(kvp("name", "s3_object_bucket_idx1")));
            database["module"].create_index(make_document(kvp("name", 1), kvp("state", 1)),
                                            make_document(kvp("name", "module_name_status_idx1")));
            database["kms_key"].create_index(make_document(kvp("region", 1)),
                                             make_document(kvp("name", "kms_key_region_idx1")));
            database["kms_key"].create_index(make_document(kvp("keyId", 1)),
                                             make_document(kvp("name", "kms_key_keyid_idx1")));
            database["kms_key"].create_index(make_document(kvp("keyState", 1)),
                                             make_document(kvp("name", "kms_key_keystate_idx1")));
            log_debug << "SQS indexes created";
        }
    }

    void DatabaseBase::UpdateModuleStatus() {
        /*    auto session = GetSession();
    session.start_transaction();
    session.client()[_name]["module"].update_one(make_document(kvp("name", "database")), make_document(kvp("$set", make_document(kvp("state", "RUNNING")))));
    session.commit_transaction();*/
    }

}// namespace AwsMock::Database
