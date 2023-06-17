//
// Created by vogje01 on 22/08/2022.
//

#include "awsmock/core/Configuration.h"

namespace AwsMock::Core {

    Configuration::Configuration() : _logger(Poco::Logger::get("Configuration")) {
        Initialize();
    }

    Configuration::Configuration(const std::string &filename) : _logger(Poco::Logger::get("Configuration")) {
        Initialize();
        SetFilename(filename);
    }

    void Configuration::Initialize() {
        // Monitoring
        DefineProperty("awsmock.core.monitoring.port", "AWSMOCK_CORE_METRIC_PORT", "8081");
        DefineProperty("awsmock.core.monitoring.timeout", "AWSMOCK_CORE_METRIC_TIMEOUT", "60000");

        // Logging
        DefineProperty("awsmock.core.log.level", "AWSMOCK_CORE_LOG_LEVEL", "information");

        // Database
        DefineProperty("awsmock.db.file", "AWSMOCK_DB_FILE", "/tmp/data/db");
    }

    void Configuration::DefineProperty(const std::string &key, const std::string &envProperty, const std::string &defaultValue) {
        if (getenv(envProperty.c_str()) != nullptr) {
            setString(key, getenv(envProperty.c_str()));
        } else {
            setString(key, defaultValue);
        }
        poco_trace(_logger, "Defined property, key: " + key + " property: " + envProperty + " default: " + defaultValue);
    }

    std::string Configuration::GetFilename() const {
        if (_filename.empty()) {
            throw CoreException("Filename not set");
        }
        return _filename;
    }

    void Configuration::SetFilename(const std::string &filename) {
        if (filename.empty()) {
            throw CoreException("Empty filename");
        }
        _filename = filename;
        load(_filename);
    }

    int Configuration::GetMetricPort() const {
        if (!hasProperty("awsmock.monitoring.port")) {
            throw CoreException("Property not found, key: awsmock.prometheus.port");
        }
        return getInt("awsmock.monitoring.port");
    }

    long Configuration::GetMetricTimeout() const {
        if (!hasProperty("awsmock.monitoring.timeout")) {
            throw CoreException("Property not found, key: awsmock.monitoring.timeout");
        }
        return getInt("awsmock.monitoring.timeout");
    }

    std::string Configuration::GetLogLevel() const {
        if (!hasProperty("awsmock.log.level")) {
            throw CoreException("Property not found, key: awsmock.log.level");
        }
        return getString("awsmock.log.level");
    }

    void Configuration::SetLogLevel(const std::string &logLevel) {
        setString("awsmock.log.level", logLevel);
    }

    void Configuration::SetValue(const std::string &key, const std::string &value) {
        if (!hasProperty(key)) {
            throw CoreException("Property not found, key: " + key);
        }
        setString(key, value);
        poco_trace(_logger, "Value set, key: " + key);
    }

    std::string Configuration::GetAppName() {
        return PROJECT_NAME;
    }

    std::string Configuration::GetVersion() {
        return PROJECT_VER;
    }

    std::string Configuration::ToString() const {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Configuration &s) {
        os << "Configuration={";
        for (const auto &it : s) {
            os << it.first << "={" << it.second + "}, ";
        }
        return os;
    }

} // namespace AwsMock::Core
