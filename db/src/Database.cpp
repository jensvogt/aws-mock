//
// Created by vogje01 on 29/05/2023.
//

#include "awsmock/db/Database.h"

namespace AwsMock::Database {

    Database::Database(const Core::Configuration &configuration) : _logger(Poco::Logger::get("Database")), _configuration(configuration) {
        Core::Logger::SetDefaultConsoleLogger("Database");

        _name = _configuration.getString("awsmock.mongodb.name", "awsmock");
        _host = _configuration.getString("awsmock.mongodb.host", "localhost");
        _port = _configuration.getInt("awsmock.mongodb.port", 27017);
        _user = _configuration.getString("awsmock.mongodb.user", "admin");
        _password = _configuration.getString("awsmock.mongodb.password", "admin");

        // MongoDB URI
        _uri = mongocxx::uri("mongodb://" + _user + ":" + _password + "@" + _host + ":" + std::to_string(_port) + "/?maxPoolSize=32");
        _client = mongocxx::client{_uri};
        _logger.debug() << "MongoDB connection initialized" << std::endl;
    }

    mongocxx::database Database::GetConnection() {
        return _client[_name];
    }

} // namespace AwsMock::Database
