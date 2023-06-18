//
// Created by vogje01 on 29/05/2023.
//

#include "awsmock/db/ServiceDatabase.h"

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;

    ServiceDatabase::ServiceDatabase(const Core::Configuration &configuration) : Database(configuration), _logger(Poco::Logger::get("ServiceDatabase")) {

        // Set default console logger
        Core::Logger::SetDefaultConsoleLogger("ServiceDatabase");


        // Get collections
        _serviceCollection = GetConnection()["service"];
    }

    bool ServiceDatabase::IsActive(const std::string &name) {

        int64_t count = _serviceCollection.count_documents(make_document(kvp("name", name)));
        _logger.trace() << "Service is active: " << (count > 0 ? "true" : "false") << std::endl;
        return count > 0;
    }
} // namespace AwsMock::Database
