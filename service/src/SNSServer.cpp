//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/SNSServer.h>

namespace AwsMock::Service {

  SNSServer::SNSServer(Core::Configuration &configuration, Core::MetricService &metricService, Poco::Condition &condition)
      : AbstractServer(configuration, condition), _logger(Poco::Logger::get("SNSServer")), _configuration(configuration), _metricService(metricService), _running(false) {

    // HTTP server configuration
    _port = _configuration.getInt("awsmock.service.sns.port", SNS_DEFAULT_PORT);
    _host = _configuration.getString("awsmock.service.sns.host", SNS_DEFAULT_HOST);
    _maxQueueLength = _configuration.getInt("awsmock.service.sns.max.queue", SNS_DEFAULT_QUEUE_LENGTH);
    _maxThreads = _configuration.getInt("awsmock.service.sns.max.threads", SNS_DEFAULT_THREADS);
    log_debug_stream(_logger) << "SNS rest service initialized, endpoint: " << _host << ":" << _port << std::endl;

    // Sleeping period
    _period = _configuration.getInt("awsmock.worker.sqs.period", 10000);

    // Create environment
    _region = _configuration.getString("awsmock.region");
    _sqsDatabase = std::make_unique<Database::SQSDatabase>(_configuration);
    _serviceDatabase = std::make_unique<Database::ServiceDatabase>(_configuration);

    log_debug_stream(_logger) << "SNSServer initialized" << std::endl;
  }

  SNSServer::~SNSServer() {
    StopServer();
    _condition.signal();
  }

  void SNSServer::MainLoop() {

    // Check service active
    if (!IsActive("sqs")) {
      log_info_stream(_logger) << "SNS service inactive" << std::endl;
      return;
    }

    log_info_stream(_logger) << "SNS service starting" << std::endl;

    // Start monitoring thread
    StartMonitoring();

    // Start REST service
    StartHttpServer();

    _running = true;
    while (_running) {
      log_debug_stream(_logger) << "SNSServer processing started" << std::endl;
      ResetMessages();

      // Wait for timeout or condition
      if (InterruptableSleep(_period)) {
        break;
      }
    }

    // Shutdown
    StopServer();
  }

  void SNSServer::StartMonitoring() {
    _threadPool.StartThread(_configuration, _metricService, _condition);
  }

  void SNSServer::StopServer() {
    _threadPool.stopAll();
    _running = false;
    StopHttpServer();
  }

  void SNSServer::StartHttpServer() {

    // Set HTTP server parameter
    auto *httpServerParams = new Poco::Net::HTTPServerParams();
    httpServerParams->setMaxQueued(_maxQueueLength);
    httpServerParams->setMaxThreads(_maxThreads);
    log_debug_stream(_logger) << "HTTP server parameter set, maxQueue: " << _maxQueueLength << " maxThreads: " << _maxThreads << std::endl;

    _httpServer = std::make_shared<Poco::Net::HTTPServer>(new SNSRequestHandlerFactory(_configuration, _metricService, _condition), Poco::Net::ServerSocket(Poco::UInt16(_port)), httpServerParams);
    _httpServer->start();

    log_info_stream(_logger) << "SNS rest service started, endpoint: http://" << _host << ":" << _port << std::endl;
  }

  void SNSServer::StopHttpServer() {
    if (_httpServer) {
      _httpServer->stopAll(true);
      _httpServer.reset();
      log_info_stream(_logger) << "SNS rest service stopped" << std::endl;
    }
  }

  void SNSServer::ResetMessages() {

    Database::Entity::SQS::QueueList queueList = _sqsDatabase->ListQueues(_region);
    log_trace_stream(_logger) << "Working on queue list, count" << queueList.size() << std::endl;

    for (auto &queue : queueList) {

      // Reset messages which have expired
      _sqsDatabase->ResetMessages(queue.queueUrl, queue.attributes.visibilityTimeout);

      // Set counter default attributes
      queue.attributes.approximateNumberOfMessages = _sqsDatabase->CountMessages(queue.region, queue.queueUrl);
      queue.attributes.approximateNumberOfMessagesDelayed = _sqsDatabase->CountMessagesByStatus(queue.region, queue.queueUrl, Database::Entity::SQS::DELAYED);
      queue.attributes.approximateNumberOfMessagesNotVisible = _sqsDatabase->CountMessagesByStatus(queue.region, queue.queueUrl, Database::Entity::SQS::SEND);

      // Check retries
      if (!queue.attributes.redrivePolicy.deadLetterTargetArn.empty()) {
        _sqsDatabase->RedriveMessages(queue.queueUrl, queue.attributes.redrivePolicy);
      }

      _sqsDatabase->UpdateQueue(queue);
      log_trace_stream(_logger) << "Queue updated, name" << queue.name << std::endl;
    }
  }
} // namespace AwsMock::Worker
