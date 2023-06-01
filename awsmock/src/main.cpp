//
// Created by vogje01 on 21/01/2023.
//

// C++ standard includes
#include <iostream>
#include <unistd.h>

// Poco includes
#include <Poco/Logger.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/ServerApplication.h>

// Libri includes
#include <awsmock/core/Logger.h>
#include <awsmock/config/Configuration.h>
#include <awsmock/controller/Router.h>
#include <awsmock/controller/RestService.h>
#include <awsmock/db/Database.h>

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
          InitializeDatabase();
          poco_information(_logger,
                           "Starting aws-mock v" + Configuration::GetVersion() + " pid: " + std::to_string(getpid()) + " loglevel: " + _configuration.GetLogLevel());
          Poco::Util::ServerApplication::initialize(self);
      }

      /**
       * Shutdown the application. Gets called when the application is about to stop.
       */
      [[maybe_unused]] void uninitialize() override {
          poco_debug(_logger, "Starting system shutdown");

          Poco::ThreadPool::defaultPool().stopAll();
          poco_debug(_logger, "Default threadpool stopped");

          // Shutdown monitoring
          if (_metricService) {
              _metricService->ShutdownServer();
              delete _metricService;
              poco_debug(_logger, "Metric server stopped");
          }

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
          options.addOption(Poco::Util::Option("level", "l", "set the log level").required(false).repeatable(false).argument("value").callback(
              Poco::Util::OptionCallback<AwsMock>(this, &AwsMock::handleOption)));
          options.addOption(Poco::Util::Option("help", "h", "display argument help information").required(false).repeatable(false).callback(
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
              helpFormatter.setHeader("Libri ONIX splitting service written in C++.");
              helpFormatter.format(std::cout);
              stopOptionsProcessing();
              exit(0);
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
          _metricService = new Core::MetricService(_configuration);
          _metricService->Initialize();
          _metricService->StartServer();
      }

      /**
       * Initialize error handler
       */
      void InitializeErrorHandler() {
//          _processErrorHandler = new ProcessErrorHandler(*_metricService);
//          poco_debug(_logger, "Process error handler initialized");
      }

      /**
       * Initialize database
       */
      void InitializeDatabase() {
          std::make_shared<Database::Database>(_configuration);
          poco_debug(_logger, "Database initialized");
      }

      /**
       * Main routine.
       *
       * @param args command line arguments.
       * @return system exit code.
       */
      int main([[maybe_unused]]const ArgVec &args) override {

          poco_debug(_logger, "Entering main routine");

          // Install error handler
          //Poco::ErrorHandler::set(&_threadErrorHandler);
          //poco_debug(_logger, "Error handler initialized");

          // Start REST service
          _router = std::make_shared<Router>();
          _router->Initialize(&_configuration, _metricService);

          _restService = std::make_shared<RestService>(_configuration);
          _restService->setRouter(_router.get());
          _restService->start();
          poco_debug(_logger,
                     "Rest service initialized, endpoint: " + _configuration.getString("awsmock.rest.host")
                         + ":" + std::to_string(_configuration.getInt("awsmock.rest.port")));

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
      Core::MetricService *_metricService;

      /**
       * Thread error handler
       */
      //ThreadErrorHandler _threadErrorHandler;

      /**
       * Process error handler
       */
      //ProcessErrorHandler *_processErrorHandler;

      /**
       * Gateway router
       */
      std::shared_ptr<Router> _router;

      /**
       * Gateway controller
       */
      std::shared_ptr<RestService> _restService;

      /**
       * Database
       */
      std::shared_ptr<Database::Database> _database;
    };

} // namespace AwsMock

#ifndef TESTING

POCO_SERVER_MAIN (AwsMock::AwsMock)

#endif