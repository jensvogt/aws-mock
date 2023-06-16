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
#include <iostream>
#include <unistd.h>

// Poco includes
#include <Poco/Logger.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/ServerApplication.h>

// AwsMock includes
#include <awsmock/core/Logger.h>
#include <awsmock/core/ThreadErrorHandler.h>
#include <awsmock/config/Configuration.h>
#include <awsmock/controller/Router.h>
#include <awsmock/controller/RestService.h>
#include <awsmock/db/Database.h>
#include <awsmock/worker/S3Worker.h>
#include <awsmock/worker/SQSWorker.h>

namespace AwsMock {

    /**
     * Main application class.
     */
    class AwsMock : public Poco::Util::ServerApplication {

    protected:

      /**
       * Initialization callback from Poco Application class.
       *
       * @param self application reference.
       */
      [[maybe_unused]] void initialize(Application &self) override {

          InitializeLogging();
          InitializeMonitoring();
          InitializeErrorHandler();
          poco_information(_logger,
                           "Starting aws-mock v" + Configuration::GetVersion() + " pid: " + std::to_string(getpid()) + " loglevel: " + _configuration.GetLogLevel());
          Poco::Util::ServerApplication::initialize(self);
      }

      /**
       * Shutdown the application. Gets called when the application is about to stop.
       */
      [[maybe_unused]] void uninitialize() override {

          poco_debug(_logger, "Starting system shutdown");

          // Shutdown monitoring
          _metricService.ShutdownServer();
          poco_debug(_logger, "Metric server stopped");

          Poco::Util::Application::uninitialize();
          poco_debug(_logger, "Bye, bye, and thanks for all the fish");
      }

      /**
       * Define the command line options.
       *
       * @param options Poco options class.
       */
      [[maybe_unused]] void defineOptions(Poco::Util::OptionSet &options) override {

          Poco::Util::ServerApplication::defineOptions(options);
          options.addOption(Poco::Util::Option("config", "", "set the configuration file").required(false).repeatable(false).argument("value").callback(
              Poco::Util::OptionCallback<AwsMock>(this, &AwsMock::handleOption)));
          options.addOption(Poco::Util::Option("level", "", "set the log level").required(false).repeatable(false).argument("value").callback(
              Poco::Util::OptionCallback<AwsMock>(this, &AwsMock::handleOption)));
          options.addOption(Poco::Util::Option("help", "", "display help information").required(false).repeatable(false).callback(
              Poco::Util::OptionCallback<AwsMock>(this, &AwsMock::handleOption)));
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
              helpFormatter.setHeader("AwsMock - AWS simulation written in C++ v" + _configuration.GetVersion());
              helpFormatter.format(std::cout);
              stopOptionsProcessing();
              exit(0);
          } else if (name == "config") {
              _configuration.SetFilename(value);
          } else if (name == "level") {
              _configuration.SetLogLevel(value);
              _logger.setLevel(value);
              Core::Logger::SetLogLevel(value);
          }
      }

      /**
       * Initialize the logging system
       */
      static void InitializeLogging() {

          Core::Logger::SetDefaultConsoleLogger("root");
      }

      /**
       * Initialize the Prometheus monitoring counters and start the prometheus server.
       */
      void InitializeMonitoring() {

          _metricService.Initialize();
          _metricService.StartServer();
      }

      /**
       * Initialize error handler
       */
      void InitializeErrorHandler() {

          // Install error handler
          Poco::ErrorHandler::set(&_threadErrorHandler);
          poco_debug(_logger, "Error handler initialized");
      }

      /**
       * Main routine.
       *
       * @param args command line arguments.
       * @return system exit code.
       */
      int main([[maybe_unused]]const ArgVec &args) override {

          poco_debug(_logger, "Entering main routine");

          // Start the S3 worker
          Worker::S3Worker _s3Worker = Worker::S3Worker(_configuration);
          Poco::ThreadPool::defaultPool().start(_s3Worker);

          // Start the SQS worker
          Worker::SQSWorker _sqsWorker = Worker::SQSWorker(_configuration);
          Poco::ThreadPool::defaultPool().start(_sqsWorker);

          // Start HTTP server
          _restService.setRouter(&_router);
          _restService.start();

          // Wait for termination
          this->waitForTerminationRequest();
          return Application::EXIT_OK;
      }

    private:
      /**
       * Logger
       */
      Poco::Logger &_logger = Poco::Logger::get("root");

      /**
       * Application configuration
       */
      Configuration _configuration = Configuration(CONFIGURATION_BASE_PATH);

      /**
       * Monitoring service
       */
      Core::MetricService _metricService = Core::MetricService(_configuration);

      /**
       * Gateway router
       */
      Controller::Router _router = Controller::Router(_configuration, _metricService);

      /**
       * Gateway controller
       */
      RestService _restService = RestService(_configuration);

      /**
       * Thread error handler
       */
      Core::ThreadErrorHandler _threadErrorHandler;

      /**
       * Database
       */
      Database::Database _database = Database::Database(_configuration);
    };

} // namespace AwsMock

#ifndef TESTING

POCO_SERVER_MAIN (AwsMock::AwsMock)

#endif
