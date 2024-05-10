//
// Created by vogje01 on 4/21/24.
//

#ifndef AWSMOCK_SERVICE_LAMBDA_MONITORING_H
#define AWSMOCK_SERVICE_LAMBDA_MONITORING_H

// AwsMock includes
#include <awsmock/core/JTimer.h>
#include <awsmock/core/MetricDefinition.h>
#include <awsmock/core/MetricService.h>
#include <awsmock/repository/LambdaDatabase.h>

namespace AwsMock::Service {

    /**
     * Lambda monitoring thread
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class LambdaMonitoring : public Core::JTimer {

      public:

        /**
         * Constructor
         */
        explicit LambdaMonitoring(int timeout) : Core::JTimer("lambda-monitoring", timeout) {}

        /**
         * Initialization
         */
        void Initialize() override;

        /**
         * Main method
         */
        void Run() override;

        /**
         * Shutdown
         */
        void Shutdown() override;

      private:

        /**
         * Update counters
         */
        void UpdateCounter();

        /**
         * Metric service
         */
        Core::MetricService &_metricService = Core::MetricService::instance();

        /**
         * Database connection
         */
        Database::LambdaDatabase &_lambdaDatabase = Database::LambdaDatabase::instance();

        /**
         * Period
         */
        int _period{};
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_LAMBDA_MONITORING_H
