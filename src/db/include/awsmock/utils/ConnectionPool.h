//
// Created by vogje01 on 07/05/2024.
//

#ifndef AWSMOCK_CORE_CONNECTION_POOL_H
#define AWSMOCK_CORE_CONNECTION_POOL_H

// C++ includes
#include <cstdlib>
#include <memory>
#include <string>

// MongoDB includes
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/logger.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/Singleton.h>

namespace AwsMock::Database {

    /**
     * @brief MongoDB connection pool.
     *
     * Singleton connection pool.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ConnectionPool {

      public:

        /**
         * Singleton instance
         *
         * @return singleton
         */
        /*static ConnectionPool &instance() {
            static Poco::SingletonHolder<ConnectionPool> instance;
            return *instance.get();
        }*/

        /**
         * Configure the connection pool
         *
         * @param instance MongoDB instance
         * @param pool connection pool
         */
        void configure(std::unique_ptr<mongocxx::instance> instance, std::unique_ptr<mongocxx::pool> pool);

        /**
         * Return a connection from the pool
         *
         * @return connection from the pool
         */
        mongocxx::pool::entry GetConnection();

        /**
         * Try to get a connection
         *
         * @return optional connection from the pool
         */
        bsoncxx::stdx::optional<mongocxx::pool::entry> TryGetConnection();

        /**
         * Private constructor
         */
        ConnectionPool() = default;

      private:

        /**
         * MongoDB instance
         */
        std::unique_ptr<mongocxx::instance> _instance = nullptr;

        /**
         * Connection pool
         */
        std::unique_ptr<mongocxx::pool> _pool = nullptr;
    };

}// namespace AwsMock::Database

#endif//AWSMOCK_CORE_CONNECTION_POOL_H
