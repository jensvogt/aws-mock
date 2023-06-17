//
// Created by vogje01 on 22/08/2022.
//

#ifndef AWS_MOCK_CORE_CONFIGURATION_H
#define AWS_MOCK_CORE_CONFIGURATION_H

// Standard C++ includes
#include <string>
#include <sstream>
#include <utility>

// Poco includes
#include "Poco/Logger.h"
#include "Poco/String.h"
#include "Poco/Util/LayeredConfiguration.h"
#include "Poco/Util/PropertyFileConfiguration.h"

// AwsMock includes
#include "CoreException.h"
#include "Version.h"

namespace AwsMock::Core {

    /**
     * Configuration handler. Configuration are read from the given configuration file and can be overruled by environment variables. Supported environment variables
     * are:
     * <ul>
     * <li>AWSMOCK_COM_BASENAME: the base name of the configuration file</li>
     * <li>AWSMOCK_COM_PROFILE: the profile of the configuration file</li>
     * <li>AWSMOCK_COM_METRIC_PORT: the port for the prometheus server (default: 8081)</li>
     * <li>AWSMOCK_COM_METRIC_TIMEOUT: the timeout for the prometheus system monitoring (default: 60000)</li>
     * <li>AWSMOCK_COM_LOGLEVEL: the logging level (default: information, possible values: debug, information, warning, error, fatal)</li>
     * </ul>
     *
     * Properties in a configuration file are key-vale pairs. The following list shows all supported keys with their default values:
     * <pre>
     * awsmock.monitoring.port=8081
     * awsmock.monitoring.timeout=60000
     * awsmock.logging.level=debug
     * </pre>
     */
    class Configuration : public Poco::Util::PropertyFileConfiguration {

    public:
      /**
       * Constructor
       */
      Configuration();

      /**
       * Constructor
       *
       * @param basename basename of the configuration file.
       */
      explicit Configuration(const std::string&  basename);

      /**
       * Define a new configuration property.
       *
       * @param key configuration key
       * @param envProperty  environment variable name
       * @param defaultValue  default value
       */
      void DefineProperty(const std::string &key, const std::string &envProperty, const std::string &defaultValue);

      /**
       * Returns the file name of the configuration file.
       *
       * @return file name of the configuration file.
       */
      std::string GetFilename() const;

      /**
       * Sets the file name of the configuration file.
       *
       * @param filename file name of the configuration file.
       */
     void SetFilename(const std::string &filename);

      /**
       * Returns the current port for the prometheus server.
       *
       * @return prometheus port
       */
      int GetMetricPort() const;

      /**
       * Returns the current timeout for the prometheus system monitoring.
       *
       * @return prometheus port
       */
      long GetMetricTimeout() const;

      /**
       * Return the current log level.
       *
       * @return current logging level
       */
      std::string GetLogLevel() const;

      /**
       * Sets the log level.
       *
       * @param level logging level to set
       */
      void SetLogLevel(const std::string &level);

      /**
       * Sets an configuration value
       *
       * @param key property key
       * @param value configuration value
       */
      void SetValue(const std::string &key, const std::string &value);

      /**
       * Returns the application name
       *
       * @return application name
       */
      static std::string GetAppName();

      /**
       * Returns the version of the library.
       *
       * @return library version
       */
      static std::string GetVersion();

      /**
       * Converts the DTO to a string representation.
       *
       * @return DTO as string for logging.
       */
      [[nodiscard]] std::string ToString() const;

    private:
      /**
       * Initialize the base properties
       */
      void Initialize();

      /**
       * Logger
       */
      Poco::Logger &_logger;

      /**
       * Name of the configuration file
       */
      std::string _filename;

      /**
       * Profile configuration file
       */
      std::string _profile;

      /**
       * Stream provider.
       *
       * @return output stream
       */
      friend std::ostream &operator<<(std::ostream &, const Configuration &);
    };

} // namespace AwsMock::Core

#endif //AWS_MOCK_CORE_CONFIGURATION_H
