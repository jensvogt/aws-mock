//
// Created by vogje01 on 07/05/2024.
//

#include <awsmock/utils/ConnectionPool.h>

namespace AwsMock::Database {

    void ConnectionPool::configure(std::unique_ptr<mongocxx::instance> instance, std::unique_ptr<mongocxx::pool> pool) {
        _instance = std::move(instance);
        _pool = std::move(pool);
    }

    mongocxx::pool::entry ConnectionPool::GetConnection() {
        return _pool->acquire();
    }

    bsoncxx::stdx::optional<mongocxx::pool::entry> ConnectionPool::TryGetConnection() {
        return _pool->try_acquire();
    }

}// namespace AwsMock::Database
