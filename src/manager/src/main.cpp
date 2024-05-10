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

// C++ standard includes
#include <cstdlib>
#include <iostream>

// MongoDB includes
#include <mongocxx/instance.hpp>

// Poco includes
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/ServerApplication.h>

// AwsMock includes
#include <awsmock/controller/RestService.h>
#include <awsmock/service/kms/KMSServer.h>

#define DEFAULT_CONFIG_FILE "/etc/awsmock.properties"
#define DEFAULT_LOG_LEVEL "debug"

namespace AwsMock::Manager {

    /**
     * @brief Main application class for the awsmock manager.
     *
     * The manager is controlling the different services. Services are activated ib the configuration file or via environment variables. Only the activated
     * services are started. The general flow is:
     *   - Process command line parameters
     *   - Read the configuration file and process environment variables
     *   - Start the database (either MongoDB or in-memory database)
     *   - General initializations
     *   - Start the activated services are background threads
     *   - Start the API gateway on port 4566 by default (can be changed in the configuration file)
     *   - Wait for a termination signal
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

            InitializeConfig();
            InitializeLogging();
            InitializeMonitoring();
            InitializeDatabase();
            InitializeCurl();
            log_info << "Starting " << Core::Configuration::GetAppName() << " " << Core::Configuration::GetVersion()
                     << " pid: " << getpid() << " loglevel: " << Core::Configuration::instance().getString("awsmock.log.level");
            log_info << "Configuration file: " << Core::Configuration::instance().GetFilename();
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
            Core::MetricService::instance().Shutdown();
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
            options.addOption(Poco::Util::Option("config", "", "set the configuration file").required(false).repeatable(false).argument("value").callback(Poco::Util::OptionCallback<AwsMockServer>(this, &AwsMockServer::handleOption)));
            options.addOption(
                    Poco::Util::Option("loglevel", "", "set the log level").required(false).repeatable(false).argument("value").callback(Poco::Util::OptionCallback<AwsMockServer>(this, &AwsMockServer::handleOption)));
            options.addOption(
                    Poco::Util::Option("logfile", "", "set the log file").required(false).repeatable(false).argument("value").callback(Poco::Util::OptionCallback<AwsMockServer>(this, &AwsMockServer::handleOption)));
            options.addOption(
                    Poco::Util::Option("version", "", "display version information").required(false).repeatable(false).callback(Poco::Util::OptionCallback<AwsMockServer>(this, &AwsMockServer::handleOption)));
            options.addOption(Poco::Util::Option("help", "", "display help information").required(false).repeatable(false).callback(Poco::Util::OptionCallback<AwsMockServer>(this, &AwsMockServer::handleOption)));
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

                std::cout << Core::Configuration::GetAppName() << " v" << Core::Configuration::GetVersion()
                          << std::endl;
                exit(0);

            } else if (name == "loglevel") {

                _logLevelSet = true;
                Core::Configuration::instance().setString("awsmock.log.level", value);
                Core::LogStream::SetSeverity(value);

            } else if (name == "logfile") {

                Core::Configuration::instance().setString("awsmock.log.file", value);
                Core::LogStream::SetFilename(value);
            }
        }

        /**
         * Initialize the configuration
         */
        static void InitializeConfig() {
            Core::Configuration::instance().SetFilename(DEFAULT_CONFIG_FILE);
        }

        /**
         * Initialize the logging
         */
        void InitializeLogging() const {
            if (!_logLevelSet) {
                std::string logLevel = Core::Configuration::instance().getString("awsmock.log.level", DEFAULT_LOG_LEVEL);
                Core::LogStream::SetSeverity(logLevel);
            }
        }

        /**
         * Initialize the Prometheus monitoring counters and StartServer the prometheus manager.
         */
        static void InitializeMonitoring() {
            Core::MetricService::instance().Start();
        }

        /**
         * Initialize database
         */
        static void InitializeDatabase() {

            // Get database variables
            Core::Configuration &configuration = Core::Configuration::instance();
            if (configuration.getBool("awsmock.mongodb.active")) {
                std::string name = configuration.getString("awsmock.mongodb.name", "awsmock");
                std::string host = configuration.getString("awsmock.mongodb.host", "localhost");
                std::string user = configuration.getString("awsmock.mongodb.user", "admin");
                std::string password = configuration.getString("awsmock.mongodb.password", "admin");
                int _port = configuration.getInt("awsmock.mongodb.port", 27017);
                int poolSize = configuration.getInt("awsmock.mongodb.pool.size", 256);

                // MongoDB URL
                std::string url = "mongodb://" + user + ":" + password + "@" + host + ":" + std::to_string(_port) + "/?maxPoolSize=" + std::to_string(poolSize);
                mongocxx::uri _uri(url.c_str());

                auto instance = bsoncxx::stdx::make_unique<mongocxx::instance>();
                Database::ConnectionPool &pool = Database::ConnectionPool::instance();

                // Options
                mongocxx::options::client client_options;
                auto api = mongocxx::options::server_api{mongocxx::options::server_api::version::k_version_1};
                client_options.server_api_opts(api);
                pool.configure(std::move(instance), bsoncxx::stdx::make_unique<mongocxx::pool>(std::move(_uri), std::move(client_options)));

                // Create database indexes
                Database::ModuleDatabase::instance().CreateIndexes();
                log_debug << "Database indexes created";
            }
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
            Database::ModuleDatabase &moduleDatabase = Database::ModuleDatabase::instance();
            std::map<std::string, Database::Entity::Module::Module> existingModules = Database::ModuleDatabase::instance().GetExisting();
            for (const auto &module: existingModules) {
                if (configuration.has("awsmock.service." + module.first + ".active") && configuration.getBool("awsmock.service." + module.first + ".active")) {
                    if (!moduleDatabase.ModuleExists(module.first)) {
                        moduleDatabase.CreateModule({.name = module.first, .state = Database::Entity::Module::ModuleState::STOPPED, .status = Database::Entity::Module::ModuleStatus::ACTIVE});
                    } else {
                        moduleDatabase.SetStatus(module.first, Database::Entity::Module::ModuleStatus::ACTIVE);
                    }
                }
            }
            Database::Entity::Module::ModuleList modules = moduleDatabase.ListModules();

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
                    _serverMap[module.name] = std::make_shared<Service::GatewayServer>(configuration, Core::MetricService::instance());
                    _serverMap[module.name]->Start();
                }
                log_debug << "Module " << module.name << " started";
            }
        }

        void StopServices() {

            Database::ModuleDatabase &moduleDatabase = Database::ModuleDatabase::instance();
            Database::Entity::Module::ModuleList modules = moduleDatabase.ListModules();
            for (const auto &module: modules) {
                if (module.state == Database::Entity::Module::ModuleState::RUNNING) {
                    log_info << "Stopping module: " << module.name;
                    moduleDatabase.SetState(module.name, Database::Entity::Module::ModuleState::STOPPED);
                    _serverMap[module.name]->Stop();
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
            _router = std::make_unique<Manager::Router>(_serverMap);
            _restService = std::make_unique<RestService>();
            _restService->setRouter(std::move(_router));
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
        Core::LogStream _logger = Core::LogStream("info");

        /**
         * Gateway router
         */
        std::unique_ptr<Manager::Router> _router = nullptr;

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

}// namespace AwsMock::Manager

#ifndef TESTING

POCO_SERVER_MAIN(AwsMock::Manager::AwsMockServer)

#endif
