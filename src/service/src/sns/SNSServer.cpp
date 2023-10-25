//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/SNSServer.h>

namespace AwsMock::Service {

  SNSServer::SNSServer(Core::Configuration &configuration, Core::MetricService &metricService)
      : AbstractServer(configuration, "sns"), _logger(Poco::Logger::get("SNSServer")), _configuration(configuration), _metricService(metricService), _running(false) {

    // HTTP manager configuration
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
    _serviceDatabase = std::make_unique<Database::ModuleDatabase>(_configuration);

    log_debug_stream(_logger) << "SNSServer initialized" << std::endl;
  }

  SNSServer::~SNSServer() {
    StopServer();
  }

  void SNSServer::MainLoop() {

    // Check service active
    if (!IsActive("sqs")) {
      log_info_stream(_logger) << "SNS service inactive" << std::endl;
      return;
    }

    log_info_stream(_logger) << "SNS service starting" << std::endl;

    // Start monitoring thread
    StartMonitoringServer();

    // Start REST service
    StartHttpServer(_maxQueueLength, _maxThreads,_host,_port,new SNSRequestHandlerFactory(_configuration, _metricService, _condition));

    _running = true;
    while (_running) {
      log_debug_stream(_logger) << "SNSServer processing started" << std::endl;
      ResetMessages();

      // Wait for timeout or condition
      if (InterruptableSleep(_period)) {
        break;
      }
    }
  }

  void SNSServer::StartMonitoringServer() {
    _threadPool.StartThread(_configuration, _metricService, _condition);
  }

  void SNSServer::StopMonitoringServer() {
    _threadPool.stopAll();
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
