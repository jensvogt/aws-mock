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
#include <awsmock/core/LogStream.h>
#include <awsmock/core/ThreadErrorHandler.h>
#include <awsmock/config/Configuration.h>
#include <awsmock/controller/Router.h>
#include <awsmock/controller/RestService.h>
#include <awsmock/db/Database.h>
#include <awsmock/service/LambdaServer.h>
#include <awsmock/service/S3Server.h>
#include <awsmock/service/SQSServer.h>
#include <awsmock/service/SNSServer.h>
#include <awsmock/service/TransferServer.h>
#include <awsmock/worker/SQSWorker.h>
#include <awsmock/worker/S3Worker.h>
#include <awsmock/worker/LambdaWorker.h>
#include <awsmock/worker/TransferWorker.h>

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

          InitializeMonitoring();
          InitializeErrorHandler();
          InitializeIndexes();
          log_info_stream(_logger) << "Starting " << Configuration::GetAppName() << " v" << Configuration::GetVersion() << " pid: " << getpid() << " loglevel: "
                                   << _configuration.GetLogLevel() << std::endl;
          Poco::Util::ServerApplication::initialize(self);
      }

      /**
       * Shutdown the application. Gets called when the application is about to stop.
       */
      [[maybe_unused]] void uninitialize() override {

          log_debug_stream(_logger) << "Starting system shutdown" << std::endl;

          // Shutdown monitoring
          _metricService.ShutdownServer();
          log_debug_stream(_logger) << "Metric server stopped" << std::endl;

          Poco::Util::Application::uninitialize();
          log_debug_stream(_logger) << "Bye, bye, and thanks for all the fish" << std::endl;
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
              helpFormatter.setHeader("AwsMock - AWS simulation written in C++ v" + Configuration::GetVersion());
              helpFormatter.format(std::cout);
              stopOptionsProcessing();
              exit(0);

          } else if (name == "config") {

              _configuration.SetFilename(value);

          } else if (name == "level") {

              _configuration.SetLogLevel(value);
              _logger.level(value);
              Core::Logger::SetLogLevel(value);
          }
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
          log_debug_stream(_logger) << "Error handler initialized" << std::endl;
      }

      /**
       * Initialize database indexes
       */
      void InitializeIndexes() {

          // Install error handler
          _database.CreateIndexes();
          log_debug_stream(_logger) << "Database indexes created" << std::endl;
      }

      void StartWorker() {

          // Start the S3 worker
          Poco::ThreadPool::defaultPool().start(_s3Worker);

          // Start the SQS worker
          Poco::ThreadPool::defaultPool().start(_sqsWorker);

          // Start the Lambda worker
          Poco::ThreadPool::defaultPool().start(_lambdaWorker);

          // Start the Transfer worker
          Poco::ThreadPool::defaultPool().start(_transferWorker);
      }

      void StartServices() {

          // Start the S3 server
          _s3Server.start();

          // Start the SQS server
          _sqsServer.start();

          // Start the SNS server
          _snsServer.start();

          // Start the lambda server
          _lambdaServer.start();

          // Start the transfer server
          _transferServer.start();
      }

      /**
       * Main routine.
       *
       * @param args command line arguments.
       * @return system exit code.
       */
      int main([[maybe_unused]]const ArgVec &args) override {

          log_debug_stream(_logger) << "Entering main routine" << std::endl;

          StartServices();

          StartWorker();

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
      Core::LogStream _logger = Core::LogStream(Poco::Logger::get("root"));

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
       * S3 worker
       */
      Worker::S3Worker _s3Worker = Worker::S3Worker(_configuration);

      /**
       * SQS worker
       */
      Worker::SQSWorker _sqsWorker = Worker::SQSWorker(_configuration);

      /**
       * Lambda worker
       */
      Worker::LambdaWorker _lambdaWorker = Worker::LambdaWorker(_configuration);

      /**
       * Transfer worker
       */
      Worker::TransferWorker _transferWorker = Worker::TransferWorker(_configuration);

      /**
       * S3 server
       */
      Service::S3Server _s3Server = Service::S3Server(_configuration, _metricService);

      /**
       * SQS server
       */
      Service::SQSServer _sqsServer = Service::SQSServer(_configuration, _metricService);

      /**
       * SNS server
       */
      Service::SNSServer _snsServer = Service::SNSServer(_configuration, _metricService);

      /**
       * Lambda server
       */
      Service::LambdaServer _lambdaServer = Service::LambdaServer(_configuration, _metricService);

      /**
       * Transfer server
       */
      Service::TransferServer _transferServer = Service::TransferServer(_configuration, _metricService);

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
