//
// Created by vogje01 on 21/12/2022.
// Copyright 2022, 2023 Jens Vogt
//
// This file is part of aws-mock.
//
// aws-mock is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// aws-mock is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with aws-mock.  If not, see <http://www.gnu.org/licenses/>.

// C includes
#include <cstdlib>

// C++ standard includes
#include <iostream>

// Poco includes
#include <Poco/TaskManager.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/ServerApplication.h>

// AwsMock includes
#include "awsmock/service/kms/KMSServer.h"
#include <awsmock/controller/RestService.h>
#include <awsmock/controller/Router.h>
#include <awsmock/core/Configuration.h>
#include <awsmock/core/LogStream.h>

#define DEFAULT_CONFIG_FILE "/etc/awsmock.properties"
#define DEFAULT_LOG_LEVEL "debug"

namespace AwsMock {

    /**
     * @brief Main application class for the awsmock manager.
     * 
     * @author jens.vogt@opitz-consulting.com
     */
    class AwsMockServer : public Poco::Util::ServerApplication {

      public:

        AwsMockServer() = default;

      protected:

        /**
         * Initialization callback from Poco Application class.
         *
         * @param self application reference.
         */
        [[maybe_unused]] void initialize(Application &self) override {

            IntializeConfig();
            IntializeLogging();
            InitializeMonitoring();
            InitializeDatabase();
            InitializeCurl();
            log_info << "Starting " << Core::Configuration::GetAppName() << " " << Core::Configuration::GetVersion()
                     << " pid: " << getpid() << " loglevel: " << _configuration.getString("awsmock.log.level");
            log_info << "Configuration file: " << _configuration.GetFilename();
            Poco::Util::ServerApplication::initialize(self);
        }

        /**
         * Shutdown the application. Gets called when the application is about to stop.
         */
        [[maybe_unused]] void uninitialize() override {

            // Shutdown all services
            StopServices();
            log_debug << "Service stopped";

            // Stop HTTP manager
            _restService->StopServer();
            log_debug << "Gateway stopped";

            // Shutdown monitoring
            _metricService.Shutdown();
            log_debug << "Monitoring stopped";

            log_debug << "Bye, bye and thanks for all the fish";
        }

        /**
         * Define the command line options.
         *
         * @param options Poco options class.
         */
        [[maybe_unused]] void defineOptions(Poco::Util::OptionSet &options) override {

            Poco::Util::ServerApplication::defineOptions(options);
            options.addOption(Poco::Util::Option("config", "", "set the configuration file", false, "value", true)
                                      .repeatable(false)
                                      .callback(Poco::Util::OptionCallback<AwsMockServer>(this, &AwsMockServer::handleOption)));
            options.addOption(Poco::Util::Option("loglevel", "", "set the log level", false, "value", true)
                                      .repeatable(false)
                                      .callback(Poco::Util::OptionCallback<AwsMockServer>(this, &AwsMockServer::handleOption)));
            options.addOption(Poco::Util::Option("logfile", "", "set the log file", false, "value", true)
                                      .repeatable(false)
                                      .callback(Poco::Util::OptionCallback<AwsMockServer>(this, &AwsMockServer::handleOption)));
            options.addOption(Poco::Util::Option("version", "", "display version information", false, "value", true)
                                      .repeatable(false)
                                      .callback(Poco::Util::OptionCallback<AwsMockServer>(this, &AwsMockServer::handleOption)));
            options.addOption(Poco::Util::Option("help", "", "display help information", false)
                                      .repeatable(false)
                                      .callback(Poco::Util::OptionCallback<AwsMockServer>(this, &AwsMockServer::handleOption)));
        }

        /**
         * Handles the command line options. Uses Poco command line optios handling.
         *
         * @param name command line option name.
         * @param value command line option value.
         */
        void handleOption(const std::string &name, const std::string &value) override {

            if (name == "help") {

                Poco::Util::HelpFormatter helpFormatter(options());
                helpFormatter.setCommand(commandName());
                helpFormatter.setUsage("OPTIONS");
                helpFormatter.setHeader("AwsMock - AWS simulation written in C++ v" + Core::Configuration::GetVersion());
                helpFormatter.format(std::cout);
                stopOptionsProcessing();
                exit(0);

            } else if (name == "config") {

                Core::Configuration::instance().SetFilename(value);

            } else if (name == "version") {

                std::cout << Core::Configuration::GetAppName() << " v" << Core::Configuration::GetVersion() << std::endl;
                exit(0);

            } else if (name == "loglevel") {

                Core::Configuration::instance().setString("awsmock.log.level", value);
                Core::LogStream::SetLevel(value);
                _logLevelSet = true;

            } else if (name == "logfile") {

                // _logger.setFileChannel(value);
            }
        }

        /**
         * Initialize the configuration
         */
        static void IntializeConfig() {
            Core::Configuration::instance().SetFilename(DEFAULT_CONFIG_FILE);
        }

        /**
         * Initialize the logging
         */
        void IntializeLogging() const {
            Core::LogStream logStream;
            if (!_logLevelSet) {
                std::string logLevel = Core::Configuration::instance().getString("awsmock.log.level", DEFAULT_LOG_LEVEL);
                Core::LogStream::SetLevel(logLevel);
            }
#ifdef _WIN32
            Core::LogStream::SetLogfile("./awsmock.log");
#endif
        }

        /**
         * Initialize the Prometheus monitoring counters and StartServer the prometheus manager.
         */
        static void InitializeMonitoring() {
            Core::MetricService::instance().Start();
            log_info << "Metric service started";
        }

        /**
         * Initialize database
         */
        void InitializeDatabase() {

            // Create database indexes
            _moduleDatabase.CreateIndexes();
            log_debug << "Database indexes created";
        }

        /**
         * Initialize CURL library
         */
        static void InitializeCurl() {
            curl_global_init(CURL_GLOBAL_ALL);
            log_debug << "Curl library initialized";
        }

        void StartServices() {

            Core::Configuration &configuration = Core::Configuration::instance();
            Database::Entity::Module::ModuleList modules = _moduleDatabase.ListModules();
            for (const auto &module: modules) {
                if (configuration.has("awsmock.service." + module.name + ".active") && configuration.getBool("awsmock.service." + module.name + ".active")) {
                    _moduleDatabase.SetStatus(module.name, Database::Entity::Module::ModuleStatus::ACTIVE);
                }
            }
            modules = _moduleDatabase.ListModules();

            // Get last module configuration
            for (const auto &module: modules) {
                if (module.name == "s3" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                    _serverMap[module.name] = std::make_shared<Service::S3Server>(configuration);
                    _serverMap[module.name]->Start();
                } else if (module.name == "sqs" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                    _serverMap[module.name] = std::make_shared<Service::SQSServer>(configuration);
                    _serverMap[module.name]->Start();
                } else if (module.name == "sns" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                    _serverMap[module.name] = std::make_shared<Service::SNSServer>(configuration);
                    _serverMap[module.name]->Start();
                } else if (module.name == "lambda" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                    _serverMap[module.name] = std::make_shared<Service::LambdaServer>(configuration);
                    _serverMap[module.name]->Start();
                } else if (module.name == "transfer" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                    _serverMap[module.name] = std::make_shared<Service::TransferServer>(configuration);
                    _serverMap[module.name]->Start();
                } else if (module.name == "cognito" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                    _serverMap[module.name] = std::make_shared<Service::CognitoServer>(configuration);
                    _serverMap[module.name]->Start();
                } else if (module.name == "dynamodb" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                    _serverMap[module.name] = std::make_shared<Service::DynamoDbServer>(configuration);
                    _serverMap[module.name]->Start();
                } else if (module.name == "kms" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                    _serverMap[module.name] = std::make_shared<Service::KMSServer>(configuration);
                    _serverMap[module.name]->Start();
                } else if (module.name == "secretsmanager" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                    _serverMap[module.name] = std::make_shared<Service::SecretsManagerServer>(configuration);
                    _serverMap[module.name]->Start();
                } else if (module.name == "gateway" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                    _serverMap[module.name] = std::make_shared<Service::GatewayServer>(configuration);
                    _serverMap[module.name]->Start();
                }
                log_debug << "Module " << module.name << " started";
            }
        }

        void StopServices() {

            Database::Entity::Module::ModuleList modules = _moduleDatabase.ListModules();
            for (const auto &module: modules) {
                if (module.state == Database::Entity::Module::ModuleState::RUNNING) {
                    log_info << "Stopping module: " << module.name;
                    _moduleDatabase.SetState(module.name, Database::Entity::Module::ModuleState::STOPPED);
                    _serverMap[module.name]->StopServer();
                    log_debug << "Module " << module.name << " stopped";
                }
            }
        }

        /**
         * Main routine.
         *
         * @param args command line arguments.
         * @return system exit code.
         */
        int main([[maybe_unused]] const ArgVec &args) override {

            log_debug << "Entering main routine";

            // Start module and worker. Services needed to StartServer first, as the worker could possibly use the services.
            StartServices();

            // Start HTTP manager
            Controller::Router router = Controller::Router(_serverMap);
            _restService = std::make_unique<RestService>(router);
            _restService->StartServer();

            // Wait for termination
            this->waitForTerminationRequest();
            log_debug << "Starting termination";

            return Application::EXIT_OK;
        }

      private:

        /**
         * Logger
         */
        std::unique_ptr<Core::LogStream> _logger = nullptr;

        /**
         * Service database
         */
        Database::ModuleDatabase &_moduleDatabase = Database::ModuleDatabase::instance();

        /**
         * Gateway router
         */
        std::unique_ptr<Controller::Router> _router = nullptr;

        /**
         * Gateway controller
         */
        std::unique_ptr<RestService> _restService = nullptr;

        /**
         * Server map
         */
        Service::ServerMap _serverMap;

        /**
         * Log level set
         */
        bool _logLevelSet = false;
    };

}// namespace AwsMock

#ifndef TESTING

POCO_SERVER_MAIN(AwsMock::AwsMockServer)

#endif
