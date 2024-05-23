//
// Created by vogje01 on 4/21/24.
//

#ifndef AWSMOCK_SERVICE_SQS_MONITORING_H
#define AWSMOCK_SERVICE_SQS_MONITORING_H

// AwsMock includes
#include <awsmock/core/Timer.h>
#include <awsmock/core/monitoring/MetricDefinition.h>
#include <awsmock/core/monitoring/MetricService.h>
#include <awsmock/repository/SQSDatabase.h>

namespace AwsMock::Service {

    /**
     * @brief SQS monitoring thread
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SQSMonitoring : public Core::Timer {

      public:

        /**
         * Constructor
         */
        explicit SQSMonitoring(int timeout);

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
        Database::SQSDatabase &_sqsDatabase = Database::SQSDatabase::instance();
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_SQS_MONITORING_H
