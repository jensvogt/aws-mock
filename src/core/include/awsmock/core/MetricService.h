//
// Created by vogje01 on 07/01/2023.
//

#ifndef AWSMOCK_CORE_METRIC_SERVICE_H
#define AWSMOCK_CORE_METRIC_SERVICE_H

// C++ Standard includes
#include <chrono>
#include <sstream>
#include <string>
#ifndef _WIN32
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/times.h>
#endif
#include <thread>

// Poco includes
#include <Poco/Prometheus/Counter.h>
#include <Poco/Prometheus/Gauge.h>
#include <Poco/Prometheus/Histogram.h>
#include <Poco/Prometheus/MetricsServer.h>
#include <Poco/Util/ServerApplication.h>

// AwsMock utils
#include <awsmock/core/Configuration.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/MetricSystemCollector.h>
#include <awsmock/core/Timer.h>

#define TIME_DIFF(x) ((double) std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - _timerStartMap[GetTimerKey(x)]).count())

namespace AwsMock::Core {

    typedef std::map<std::string, Poco::Prometheus::Counter *> CounterMap;
    typedef std::map<std::string, Poco::Prometheus::Gauge *> GaugeMap;
    typedef std::map<std::string, Poco::Prometheus::Histogram *> HistogramMap;
    typedef std::map<std::string, Poco::Prometheus::Gauge *> TimerMap;
    typedef std::map<std::string, std::chrono::time_point<std::chrono::high_resolution_clock>> TimerStartMap;

    /**
     * @brief Maintains a list of counter and gauges for monitoring via Prometheus.
     *
     * The data is made available via a HTTP manager listening on port 9100. The port ist configurable in the
     * awsmock properties file.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class MetricService : public Core::Timer {

      public:

        /**
         * @brief Default constructor
         */
        MetricService();

        /**
         * @brief Singleton instance
         */
        static MetricService &instance() {
            static Poco::SingletonHolder<MetricService> sh;
            return *sh.get();
        }

        /**
         * @brief Destructor
         */
        virtual ~MetricService() = default;

        /**
         * @brief Initialization
         */
        void Initialize() override;

        /**
         * @brief Run main loop
         */
        void Run() override;

        /**
         * @brief Gracefully shutdown
         */
        void Shutdown() override;

        /**
         * @brief Adds a counter to the map.
         *
         * @param name name of the counter
         */
        void AddCounter(const std::string &name);

        /**
         * @brief Adds a counter to the map.
         *
         * @param name name of the counter
         * @param label label name of the counter
         */
        void AddCounter(const std::string &name, const std::string &label);

        /**
         * @brief Check whether a counter exists
         *
         * @param name name of the counter.
         * @return true if counter exists.
         */
        bool CounterExists(const std::string &name);

        /**
         * @brief Check whether a counter exists
         *
         * @param name name of the counter.
         * @param label label name of the counter
         * @return true if counter exists.
         */
        bool CounterExists(const std::string &name, const std::string &label);

        /**
         * @brief Increments a counter.
         *
         * @param name of the counter
         * @param value value for the incrementation (default: 1), can be negative
         */
        void IncrementCounter(const std::string &name, int value = 1);

        /**
         * @brief Increments a labeled counter.
         *
         * @param name of the counter
         * @param labelName name of the label
         * @param labelValue label value of the counter
         * @param value value for the incrementation (default: 1), can be negative
         */
        void IncrementCounter(const std::string &name, const std::string &labelName, const std::string &labelValue, int value = 1);

        /**
         * @brief Decrements a counter.
         *
         * @param name of the counter
         * @param value value for the incrementation (default: 1), can be negative
         */
        void DecrementCounter(const std::string &name, int value = 1);

        /**
         * @brief Decrements a labeled counter.
         *
         * @param name of the counter
         * @param labelName name of the label
         * @param labelValue label value of the counter
         * @param value value for the incrementation (default: 1), can be negative
         */
        void DecrementCounter(const std::string &name, const std::string &labelName, const std::string &labelValue, int value = 1);

        /**
         * @brief Clears a counter.
         *
         * @param name of the counter
         */
        void ClearCounter(const std::string &name);

        /**
         * @brief Adds a gauge to the map.
         *
         * @param name name of the gauge
         */
        void AddGauge(const std::string &name);

        /**
         * @brief Adds a gauge to the map.
         *
         * @param name name of the gauge
         * @param label label of the gauge
         */
        void AddGauge(const std::string &name, const std::string &label);

        /**
         * @brief Check whether a gauge exists
         *
         * @param name name of the gauge.
         * @return true if gauge exists.
         */
        bool GaugeExists(const std::string &name);

        /**
         * @brief Check whether a gauge exists
         *
         * @param name name of the gauge.
         * @param label label of the gauge.
         * @return true if gauge exists.
         */
        bool GaugeExists(const std::string &name, const std::string &label);

        /**
         * @brief Sets a integer gauge value in the map.
         *
         * @param name name of the gauge
         * @param value value of the gauge
         */
        [[maybe_unused]] void SetGauge(const std::string &name, int value);

        /**
         * @brief Sets a integer gauge value in the map.
         *
         * @param name name of the gauge
         * @param labelName label name of the gauge
         * @param labelValue label value of the gauge
         * @param value value of the gauge
         */
        [[maybe_unused]] void SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, int value);

        /**
         * @brief Sets a long integer gauge value in the map.
         *
         * @param name name of the gauge
         * @param value value of the gauge
         */
        [[maybe_unused]] void SetGauge(const std::string &name, long value);

        /**
         * @brief Sets a long integer gauge value in the map.
         *
         * @param name name of the gauge
         * @param labelName label name of the gauge
         * @param labelValue label value of the gauge
         * @param value value of the gauge
         */
        [[maybe_unused]] void SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, long value);

        /**
         * @brief Sets a unsigned long integer gauge value in the map.
         *
         * @param name name of the gauge
         * @param value value of the gauge
         */
        [[maybe_unused]] void SetGauge(const std::string &name, unsigned long value);

        /**
         * @brief Sets a unsigned long integer gauge value in the map.
         *
         * @param name name of the gauge
         * @param labelName label name of the gauge
         * @param labelValue label value of the gauge
         * @param value value of the gauge
         */
        [[maybe_unused]] void SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, unsigned long value);

        /**
         * @brief Sets a float gauge value in the map.
         *
         * @param name name of the gauge
         * @param value value of the gauge
         */
        [[maybe_unused]] void SetGauge(const std::string &name, float value);

        /**
         * @brief Sets a float gauge value in the map.
         *
         * @param name name of the gauge
         * @param labelName label name of the gauge
         * @param labelValue label value of the gauge
         * @param value value of the gauge
         */
        [[maybe_unused]] void SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, float value);

        /**
         * @brief Sets a double gauge value in the map.
         *
         * @param name name of the gauge
         * @param value value of the gauge
         */
        [[maybe_unused]] void SetGauge(const std::string &name, double value);

        /**
         * @brief Sets a double gauge value in the map.
         *
         * @param name name of the gauge
         * @param labelName label name of the gauge
         * @param labelValue label value of the gauge
         * @param value value of the gauge
         */
        void SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, double value);

        /**
         * @brief Increments a gauge.
         *
         * @param name of the gauge
         * @param value value for the incrementation (default: 1), can be negative
         */
        void IncrementGauge(const std::string &name, int value = 1);

        /**
         * @brief Increments a gauge.
         *
         * @param name of the gauge
         * @param labelName label name of the gauge
         * @param labelValue label value of the gauge
         * @param value value for the incrementation (default: 1), can be negative
         */
        void IncrementGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, int value = 1);

        /**
         * @brief Adds a histogram to the map.
         *
         * @param name name of the histogram
         */
        void AddHistogram(const std::string &name);

        /**
         *@brief  Adds a histogram to the map.
         *
         * @param name name of the histogram
         * @param label label of the histogram
         */
        void AddHistogram(const std::string &name, const std::string &label);

        /**
         * @brief Check whether a histogram exists
         *
         * @param name name of the histogram.
         * @return true if histogram exists.
         */
        bool HistogramExists(const std::string &name);

        /**
         * @brief Check whether a histogram exists
         *
         * @param name name of the histogram.
         * @param label label of the histogram.
         * @return true if histogram exists.
         */
        bool HistogramExists(const std::string &name, const std::string &label);

        /**
         * @brief Add timer
         */
        void AddTimer(const std::string &name);

        /**
         * @brief Starts a timer
         *
         * @param name name of the timer.
         */
        void StartTimer(const std::string &name);

        /**
         * @brief Stop and fill in the duration of a timer
         *
         * @param name name of the timer.
         */
        void StopTimer(const std::string &name);

        /**
         * @brief Resets a timers.
         *
         * @param name name of the timer.
         */
        [[maybe_unused]] void ResetTimer(const std::string &name);

        /**
         * @brief Resets all timers.
         *
         * @param name name of the timer.
         */
        [[maybe_unused]] void ResetAllTimer();

        /**
         * @brief Check whether a timer exists
         *
         * @param name name of the timer.
         * @return true if timer exists.
         */
        bool TimerExists(const std::string &name);

      private:

        /**
         * @brief Returns a thread safe timer key string.
         *
         * @param name name of the timer.
         */
        static std::string GetTimerKey(const std::string &name);

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
