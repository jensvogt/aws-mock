//
// Created by vogje01 on 07/01/2023.
//

#ifndef AWSMOCK_CORE_METRIC_SERVICE_H
#define AWSMOCK_CORE_METRIC_SERVICE_H

// C++ Standard includes
#include <chrono>
#include <sstream>
#include <string>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/times.h>

// Poco includes
#include <Poco/Prometheus/Counter.h>
#include <Poco/Prometheus/Gauge.h>
#include <Poco/Prometheus/Histogram.h>
#include <Poco/Prometheus/MetricsServer.h>
#include <Poco/Util/ServerApplication.h>

// AwsMock utils
#include "MetricSystemCollector.h"
#include "awsmock/core/Configuration.h"
#include "awsmock/core/LogStream.h"
#include "awsmock/core/Timer.h"

#define TIME_DIFF_NAME(x) (std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - _timerStartMap[GetTimerStartKeyName(x)]).count())
#define TIME_DIFF_LABEL(x, y) (std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - _timerStartMap[GetTimerStartKeyLabel(x, y)]).count())

namespace AwsMock::Core {

    typedef std::map<std::string, Poco::Prometheus::Counter *> CounterMap;
    typedef std::map<std::string, Poco::Prometheus::Gauge *> GaugeMap;
    typedef std::map<std::string, Poco::Prometheus::Histogram *> HistogramMap;
    typedef std::map<std::string, Poco::Prometheus::Gauge *> TimerMap;
    typedef std::map<std::string, std::chrono::time_point<std::chrono::high_resolution_clock>> TimerStartMap;

    /**
     * Maintains a list of counter and gauges for monitoring via Prometheus. The data is made available via a HTTP
     * manager listening on port 9100. The port ist configurable.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class MetricService : public Core::Timer {

      public:

        /**
         * Default constructor
         */
        MetricService();

        /**
         * Singleton instance
         */
        static MetricService &instance() {
            static Poco::SingletonHolder<MetricService> sh;
            return *sh.get();
        }

        /**
         * Destructor
         */
        virtual ~MetricService() = default;

        /**
         * Initialization
         */
        void Initialize() override;

        /**
         * Run main loop
         */
        void Run() override;

        /**
         * Gracefully shutdown
         */
        void Shutdown() override;

        /**
         * Adds a counter to the map.
         *
         * @param name name of the counter
         */
        void AddCounter(const std::string &name);

        /**
         * Adds a counter to the map.
         *
         * @param name name of the counter
         * @param label label name of the counter
         */
        void AddCounter(const std::string &name, const std::string &label);

        /**
         * Check whether a counter exists
         *
         * @param name name of the counter.
         * @return true if counter exists.
         */
        bool CounterExists(const std::string &name);

        /**
         * Check whether a counter exists
         *
         * @param name name of the counter.
         * @param label label name of the counter
         * @return true if counter exists.
         */
        bool CounterExists(const std::string &name, const std::string &label);

        /**
         * Increments a counter.
         *
         * @param name of the counter
         * @param value value for the incrementation (default: 1), can be negative
         */
        void IncrementCounter(const std::string &name, int value = 1);

        /**
         * Increments a labeled counter.
         *
         * @param name of the counter
         * @param labelName name of the label
         * @param labelValue label value of the counter
         * @param value value for the incrementation (default: 1), can be negative
         */
        void IncrementCounter(const std::string &name, const std::string &labelName, const std::string &labelValue, int value = 1);

        /**
         * Decrements a counter.
         *
         * @param name of the counter
         * @param value value for the incrementation (default: 1), can be negative
         */
        void DecrementCounter(const std::string &name, int value = 1);

        /**
         * Decrements a labeled counter.
         *
         * @param name of the counter
         * @param labelName name of the label
         * @param labelValue label value of the counter
         * @param value value for the incrementation (default: 1), can be negative
         */
        void DecrementCounter(const std::string &name, const std::string &labelName, const std::string &labelValue, int value = 1);

        /**
         * Clears a counter.
         *
         * @param name of the counter
         */
        void ClearCounter(const std::string &name);

        /**
         * Adds a gauge to the map.
         *
         * @param name name of the gauge
         */
        void AddGauge(const std::string &name);

        /**
         * Adds a gauge to the map.
         *
         * @param name name of the gauge
         * @param label label of the gauge
         */
        void AddGauge(const std::string &name, const std::string &label);

        /**
         * Check whether a gauge exists
         *
         * @param name name of the gauge.
         * @return true if gauge exists.
         */
        bool GaugeExists(const std::string &name);

        /**
         * Check whether a gauge exists
         *
         * @param name name of the gauge.
         * @param label label of the gauge.
         * @return true if gauge exists.
         */
        bool GaugeExists(const std::string &name, const std::string &label);

        /**
         * Sets a integer gauge value in the map.
         *
         * @param name name of the gauge
         * @param value value of the gauge
         */
        [[maybe_unused]] void SetGauge(const std::string &name, int value);

        /**
         * Sets a integer gauge value in the map.
         *
         * @param name name of the gauge
         * @param labelName label name of the gauge
         * @param labelValue label value of the gauge
         * @param value value of the gauge
         */
        [[maybe_unused]] void SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, int value);

        /**
         * Sets a long integer gauge value in the map.
         *
         * @param name name of the gauge
         * @param value value of the gauge
         */
        [[maybe_unused]] void SetGauge(const std::string &name, long value);

        /**
         * Sets a long integer gauge value in the map.
         *
         * @param name name of the gauge
         * @param labelName label name of the gauge
         * @param labelValue label value of the gauge
         * @param value value of the gauge
         */
        [[maybe_unused]] void SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, long value);

        /**
         * Sets a unsigned long integer gauge value in the map.
         *
         * @param name name of the gauge
         * @param value value of the gauge
         */
        [[maybe_unused]] void SetGauge(const std::string &name, unsigned long value);

        /**
         * Sets a unsigned long integer gauge value in the map.
         *
         * @param name name of the gauge
         * @param labelName label name of the gauge
         * @param labelValue label value of the gauge
         * @param value value of the gauge
         */
        [[maybe_unused]] void SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, unsigned long value);

        /**
         * Sets a float gauge value in the map.
         *
         * @param name name of the gauge
         * @param value value of the gauge
         */
        [[maybe_unused]] void SetGauge(const std::string &name, float value);

        /**
         * Sets a float gauge value in the map.
         *
         * @param name name of the gauge
         * @param labelName label name of the gauge
         * @param labelValue label value of the gauge
         * @param value value of the gauge
         */
        [[maybe_unused]] void SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, float value);

        /**
         * Sets a double gauge value in the map.
         *
         * @param name name of the gauge
         * @param value value of the gauge
         */
        [[maybe_unused]] void SetGauge(const std::string &name, double value);

        /**
         * Sets a double gauge value in the map.
         *
         * @param name name of the gauge
         * @param labelName label name of the gauge
         * @param labelValue label value of the gauge
         * @param value value of the gauge
         */
        void SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, double value);

        /**
         * Increments a gauge.
         *
         * @param name of the gauge
         * @param value value for the incrementation (default: 1), can be negative
         */
        void IncrementGauge(const std::string &name, int value = 1);

        /**
         * Increments a gauge.
         *
         * @param name of the gauge
         * @param labelName label name of the gauge
         * @param labelValue label value of the gauge
         * @param value value for the incrementation (default: 1), can be negative
         */
        void IncrementGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, int value = 1);

        /**
         * Adds a histogram to the map.
         *
         * @param name name of the histogram
         */
        void AddHistogram(const std::string &name);

        /**
         * Adds a histogram to the map.
         *
         * @param name name of the histogram
         * @param label label of the histogram
         */
        void AddHistogram(const std::string &name, const std::string &label);

        /**
         * Check whether a histogram exists
         *
         * @param name name of the histogram.
         * @return true if histogram exists.
         */
        bool HistogramExists(const std::string &name);

        /**
         * Check whether a histogram exists
         *
         * @param name name of the histogram.
         * @param label label of the histogram.
         * @return true if histogram exists.
         */
        bool HistogramExists(const std::string &name, const std::string &label);

        /**
         * Add timer
         *
         * @param name name of the timer
         */
        void AddTimer(const std::string &name);

        /**
         * Add timer
         *
         * @param name name of the timer
         * @param label timer label
         */
        void AddTimer(const std::string &name, const std::string &label);

        /**
         * Starts a timer
         *
         * @param name name of the timer.
         */
        void StartTimer(const std::string &name);

        /**
         * Starts a timer
         *
         * @param name name of the timer.
         * @param label label name of the timer.
         */
        void StartTimer(const std::string &name, const std::string &label);

        /**
         * Stop and fill in the duration of a timer
         *
         * @param name name of the timer.
         */
        void StopTimer(const std::string &name);

        /**
         * Stop and fill in the duration of a timer
         *
         * @param name name of the timer.
         * @param label label name of the timer.
         */
        void StopTimer(const std::string &name, const std::string &label);

        /**
         * Resets a timers.
         *
         * @param name name of the timer.
         */
        [[maybe_unused]] void ResetTimer(const std::string &name);

        /**
         * Resets all timers.
         *
         * @param name name of the timer.
         */
        [[maybe_unused]] void ResetAllTimer();

        /**
         * Check whether a timer exists
         *
         * @param name name of the timer.
         * @return true if timer exists.
         */
        bool TimerExists(const std::string &name);

        /**
         * Check whether a timer exists
         *
         * @param name name of the timer.
         * @param label timer label
         * @return true if timer exists.
         */
        bool TimerExists(const std::string &name, const std::string &label);

      private:

        /**
         * Returns a timer key string.
         *
         * @param name name of the timer.
         * @param label timer label
         */
        static std::string GetTimerKey(const std::string &name, const std::string &label);

        /**
         * Returns a thread safe timer key string.
         *
         * @param name name of the timer.
         */
        static std::string GetTimerStartKeyName(const std::string &name);

        /**
         * Returns a thread safe timer key string.
         *
         * @param name name of the timer.
         * @param label timer label
         */
        static std::string GetTimerStartKeyLabel(const std::string &name, const std::string &label);

        /**
         * Metric manager for Prometheus
         */
        std::shared_ptr<Poco::Prometheus::MetricsServer> _server;

        /**
         * System monitoring thread
         */
        std::shared_ptr<MetricSystemCollector> _metricSystemCollector{};

        /**
         * Counter map
         */
        CounterMap _counterMap;

        /**
         * Counter map
         */
        GaugeMap _gaugeMap;

        /**
         * Counter map
         */
        HistogramMap _histogramMap;

        /**
         * Timer map
         */
        TimerMap _timerMap;

        /**
         * Timer StartServer map
         */
        TimerStartMap _timerStartMap;

        /**
         * Port for the monitoring manager
         */
        long _port;

        /**
         * Mutex
         */
        static Poco::Mutex _mutex;
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_METRIC_SERVICE_H
