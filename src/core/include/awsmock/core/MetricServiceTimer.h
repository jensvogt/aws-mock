//
// Created by vogje01 on 18/05/2023.
//

#ifndef AWSMOCK_CORE_METRIC_SERVICE_TIMER_H
#define AWSMOCK_CORE_METRIC_SERVICE_TIMER_H

#include <awsmock/core/MetricService.h>

#include <utility>

namespace AwsMock::Core {

    /**
     * @brief Measure a methods execution time.
     *
     * The timer is stopped by the destructor, which is called when the method is left.
     *
     * @author jens.vogt\@opitz-consulting.com
     *
     * @tparam M
     */
    template<class M>
    class MetricServiceTimer {

      public:

        /**
         * @brief Constructor
         *
         * @param metricService metric module
         * @param name name of the underlying timer
         */
        explicit MetricServiceTimer(M &metricService, std::string name) : _metricService(metricService), _name(std::move(name)) {
            if (!_metricService.TimerExists(_name)) {
                _metricService.AddTimer(_name);
            }
            _metricService.StartTimer(_name);
        }

        /**
         * @brief Destructor
         */
        ~MetricServiceTimer() {
            _metricService.StopTimer(_name);
        }

        /**
         * @brief Default constructor
         */
        MetricServiceTimer() = delete;

        /**
         * @brief Copy constructor
         */
        MetricServiceTimer(const MetricServiceTimer &) = delete;

        /**
         * @brief Equals operator
         */
        MetricServiceTimer &operator=(const MetricServiceTimer &) = delete;

      private:

        /**
         * Metric module
         */
        M &_metricService;

        /**
         * Name of the timer
         */
        const std::string _name;
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_METRIC_SERVICE_TIMER_H
