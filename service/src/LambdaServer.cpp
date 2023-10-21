//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/LambdaServer.h>

namespace AwsMock::Service {

  LambdaServer::LambdaServer(Core::Configuration &configuration, Core::MetricService &metricService, Poco::NotificationQueue &createQueue, Poco::NotificationQueue &invokeQueue)
      : AbstractWorker(configuration), _logger(Poco::Logger::get("LambdaWorker")), _configuration(configuration), _metricService(metricService),
        _createQueue(createQueue), _invokeQueue(invokeQueue), _running(false) {

    _dataDir = _configuration.getString("awsmock.data.dir") + Poco::Path::separator() + "lambda";
    _logger.debug() << "Lambda directory: " << _dataDir << std::endl;

    // Sleeping period
    _period = _configuration.getInt("awsmock.worker.lambda.period", 10000);
    log_debug_stream(_logger) << "Lambda worker period: " << _period << std::endl;

    // Create environment
    _region = _configuration.getString("awsmock.region");
    _lambdaDatabase = std::make_unique<Database::LambdaDatabase>(_configuration);

    // Lambda service connection
    _lambdaServiceHost = _configuration.getString("awsmock.service.lambda.host", "localhost");
    _lambdaServicePort = _configuration.getInt("awsmock.service.lambda.port", 9503);
    log_debug_stream(_logger) << "Lambda service endpoint: http://" << _lambdaServiceHost << ":" << _lambdaServicePort << std::endl;

    // Docker service
    _dockerService = std::make_unique<Service::DockerService>(_configuration);

    // Create lambda directory
    Core::DirUtils::EnsureDirectory(_dataDir);
    log_debug_stream(_logger) << "LambdaWorker initialized" << std::endl;
  }

  LambdaServer::~LambdaServer() {
    if (_httpServer) {
      _httpServer->stopAll(true);
      delete _httpServer;
      log_info_stream(_logger) << "Lambda rest service stopped" << std::endl;
    }
  }

  void LambdaServer::StartHttpServer() {

    // Set HTTP server parameter
    auto *httpServerParams = new Poco::Net::HTTPServerParams();
    httpServerParams->setMaxQueued(_maxQueueLength);
    httpServerParams->setMaxThreads(_maxThreads);
    log_debug_stream(_logger) << "HTTP server parameter set, maxQueue: " << _maxQueueLength << " maxThreads: " << _maxThreads << std::endl;

    _httpServer =
        new Poco::Net::HTTPServer(new LambdaRequestHandlerFactory(_configuration, _metricService, _createQueue, _invokeQueue),
                                  Poco::Net::ServerSocket(Poco::UInt16(_port)), httpServerParams);

    _httpServer->start();
    log_info_stream(_logger) << "Lambda rest service started, endpoint: http://" << _host << ":" << _port << std::endl;
  }

  void LambdaServer::CleanupContainers() {
    _dockerService->PruneContainers();
    log_debug_stream(_logger) << "Docker containers cleaned up" << std::endl;
  }

  void LambdaServer::StartLambdaFunctions() {

    log_debug_stream(_logger) << "Starting lambdas" << std::endl;
    std::vector<Database::Entity::Lambda::Lambda> lambdas = _lambdaDatabase->ListLambdas(_region);

    for (auto &lambda : lambdas) {

      Dto::Lambda::Code code = GetCode(lambda);

      // Create create function request
      Dto::Lambda::CreateFunctionRequest request = {
          .region=lambda.region,
          .functionName=lambda.function,
          .runtime=lambda.runtime,
          .code=code,
          .tags=lambda.tags
      };
      SendCreateFunctionRequest(request, "application/json");
      log_debug_stream(_logger) << "Lambda started, name:" << lambda.function << std::endl;
    }
  }

  Dto::Lambda::Code LambdaServer::GetCode(const Database::Entity::Lambda::Lambda &lambda) {

    Dto::Lambda::Code code;
    if (Core::FileUtils::FileExists(lambda.fileName)) {
      std::stringstream ss;
      std::ifstream ifs(lambda.fileName);
      ss << ifs.rdbuf();
      ifs.close();

      code = {
          .zipFile=ss.str()
      };
      log_debug_stream(_logger) << "Loaded lambda from file:" << lambda.fileName << " size: " << Core::FileUtils::FileSize(lambda.fileName) << std::endl;
    }
    return code;
  }

  void LambdaServer::run() {

    log_info_stream(_logger) << "Lambda worker started" << std::endl;

    // Check service active
    /*if (!_serviceDatabase->IsActive("Lambda")) {
        return;
    }*/

    // Cleanup
    CleanupContainers();

    // Start creator/executor
    Poco::ThreadPool::defaultPool().start(_lambdaCreator);
    Poco::ThreadPool::defaultPool().start(_lambdaExecutor);

    // Start all lambda functions
    StartLambdaFunctions();

    // Start monitoring thread
    _threadPool.StartThread(_configuration, _metricService);

    _running = true;
    while (_running) {
      log_debug_stream(_logger) << "LambdaWorker processing started" << std::endl;
      Poco::Thread::sleep(_period);
    }
  }

  void LambdaServer::SendCreateFunctionRequest(Dto::Lambda::CreateFunctionRequest &lambdaRequest, const std::string &contentType) {

    std::string url = "http://" + _lambdaServiceHost + ":" + std::to_string(_lambdaServicePort) + "/2015-03-31/functions";
    std::string body = lambdaRequest.ToJson();
    SendPostRequest(url, body, contentType);
    log_debug_stream(_logger) << "Lambda create function request send" << std::endl;
  }

} // namespace AwsMock::Worker
