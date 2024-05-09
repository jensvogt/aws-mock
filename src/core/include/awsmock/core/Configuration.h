//
// Created by vogje01 on 22/08/2022.
//

#ifndef AWSMOCK_CORE_CONFIGURATION_H
#define AWSMOCK_CORE_CONFIGURATION_H

// Standard C++ includes
#include <fstream>
#include <sstream>
#include <string>
#include <utility>

// Poco includes
#include <Poco/SingletonHolder.h>
#include <Poco/String.h>
#include <Poco/Util/LayeredConfiguration.h>
#include <Poco/Util/PropertyFileConfiguration.h>

// AwsMock includes
#include <awsmock/core/CoreException.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/Version.h>

namespace AwsMock::Core {

    /**
     * @brief Configuration handler.
     *
     * Configuration are read from the given configuration file and can be overruled by environment variables. Supported environment variables
     * are:
     * <ul>
     * <li>AWSMOCK_COM_BASENAME: the base name of the configuration file</li>
     * <li>AWSMOCK_COM_PROFILE: the profile of the configuration file</li>
     * <li>AWSMOCK_COM_METRIC_PORT: the port for the prometheus manager (default: 9100)</li>
     * <li>AWSMOCK_COM_METRIC_TIMEOUT: the timeout for the prometheus system monitoring (default: 60000)</li>
     * <li>AWSMOCK_COM_LOGLEVEL: the logging level (default: information, possible values: debug, information, warning, error, fatal)</li>
     * </ul>
     *
     * Properties in a configuration file are key-vale pairs. The following list shows all supported keys with their default values:
     * @code{.ini}
     * awsmock.monitoring.port=9100
     * awsmock.monitoring.timeout=60000
     * awsmock.logging.level=debug
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class Configuration : public Poco::Util::PropertyFileConfiguration {

      public:

        /**
         * Constructor
         */
        Configuration();

        /**
         * Singleton instance
         */
        static Configuration &instance() {
            static Poco::SingletonHolder<Configuration> sh;
            return *sh.get();
        }

        /**
         * Constructor
         *
         * @param basename basename of the configuration file.
         */
        explicit Configuration(const std::string &basename);

        /**
         * Define a new configuration property.
         *
         * <p>If the system environment has a value for the given configuration key, the environment value is set. If the configuration has already a value for the given
         * key, the key is preserved, otherwise the default value is taken. </p>
         *
         * @param key configuration key
         * @param envProperty environment variable name
         * @param defaultValue string default value
         */
        void DefineStringProperty(const std::string &key, const std::string &envProperty, const std::string &defaultValue);

        /**
         * Define a new configuration property.
         *
         * <p>If the system environment has a value for the given configuration key, the environment value is set. If the configuration has already a value for the given
         * key, the key is preserved, otherwise the default value is taken. </p>
         *
         * @param key configuration key
         * @param envProperty environment variable name
         * @param defaultValue boolean default value
         */
        void DefineBoolProperty(const std::string &key, const std::string &envProperty, bool defaultValue);

        /**
         * Define a new configuration property.
         *
         * <p>If the system environment has a value for the given configuration key, the environment value is set. If the configuration has already a value for the given
         * key, the key is preserved, otherwise the default value is taken. </p>
         *
         * @param key configuration key
         * @param envProperty environment variable name
         * @param defaultValue integer default value
         */
        void DefineIntProperty(const std::string &key, const std::string &envProperty, int defaultValue);

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
         * Sets a string configuration value
         *
         * @param key property key
         * @param value configuration value
         */
        void SetValue(const std::string &key, const std::string &value);

        /**
         * Sets a bool configuration value
         *
         * @param key property key
         * @param value configuration value
         */
        void SetValue(const std::string &key, bool value);

        /**
         * Sets an integer configuration value
         *
         * @param key property key
         * @param value configuration value
         */
        void SetValue(const std::string &key, int value);

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
         * Writes the current configuration the given file
         *
         * @param filename name of the configuration file
         */
        void WriteFile(const std::string &filename);

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

}// namespace AwsMock::Core

#endif//A WSMOCK_CORE_CONFIGURATION_H
