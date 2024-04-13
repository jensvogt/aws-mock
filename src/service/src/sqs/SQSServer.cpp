//
// Created by vogje01 on 03/06/2023.
//

#include "awsmock/service/sqs/SQSServer.h"

namespace AwsMock::Service {

  SQSServer::SQSServer(Core::Configuration &configuration, Core::MetricService &metricService)
    : AbstractServer(configuration, "sqs"), _logger(Poco::Logger::get("SQSServer")), _configuration(configuration), _metricService(metricService), _serviceDatabase(Database::ModuleDatabase::instance()), _sqsDatabase(Database::SQSDatabase::instance()) {

    // HTTP manager configuration
    _port = _configuration.getInt("awsmock.service.sqs.port", SQS_DEFAULT_PORT);
    _host = _configuration.getString("awsmock.service.sqs.host", SQS_DEFAULT_HOST);
    _maxQueueLength = _configuration.getInt("awsmock.service.sqs.max.queue", SQS_DEFAULT_QUEUE_LENGTH);
    _maxThreads = _configuration.getInt("awsmock.service.sqs.max.threads", SQS_DEFAULT_THREADS);
    _requestTimeout = _configuration.getInt("awsmock.service.sqs.timeout", SQS_DEFAULT_TIMEOUT);
    log_debug_stream(_logger) << "SQS rest module initialized, endpoint: " << _host << ":" << _port << std::endl;

    // Sleeping period
    _period = _configuration.getInt("awsmock.worker.sqs.period", 10000);

    // Create environment
    _region = _configuration.getString("awsmock.region");
    log_debug_stream(_logger) << "SQSServer initialized" << std::endl;
  }

  void SQSServer::Initialize() {

    // Check module active
    if (!IsActive("sqs")) {
      log_info_stream(_logger) << "SQS module inactive" << std::endl;
      return;
    }
    log_info_stream(_logger) << "SQS server starting, port: " << _port << std::endl;

    // Start REST module
    StartHttpServer(_maxQueueLength, _maxThreads, _requestTimeout, _host, _port, new SQSRequestHandlerFactory(_configuration, _metricService));
  }

  void SQSServer::Run() {
    log_trace_stream(_logger) << "SQSServer processing started" << std::endl;

    // Reset messages
    ResetMessages();

    // Update counter
    UpdateCounters();
  }

  void SQSServer::Shutdown() {
      StopHttpServer();
  }

  void SQSServer::ResetMessages() {

    Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues(_region);
    log_trace_stream(_logger) << "Working on queue list, count" << queueList.size() << std::endl;

    for (auto &queue : queueList) {

      // Check retention period
      if (queue.attributes.messageRetentionPeriod > 0) {
        _sqsDatabase.MessageRetention(queue.queueUrl, queue.attributes.messageRetentionPeriod);
      }

      // Reset messages which have expired
      _sqsDatabase.ResetMessages(queue.queueUrl, queue.attributes.visibilityTimeout);

      // Set counter default userAttributes
      queue.attributes.approximateNumberOfMessages = _sqsDatabase.CountMessages(queue.region, queue.queueUrl);
      queue.attributes.approximateNumberOfMessagesDelayed = _sqsDatabase.CountMessagesByStatus(queue.region, queue.queueUrl, Database::Entity::SQS::MessageStatus::DELAYED);
      queue.attributes.approximateNumberOfMessagesNotVisible = _sqsDatabase.CountMessagesByStatus(queue.region, queue.queueUrl, Database::Entity::SQS::MessageStatus::INVISIBLE);

      // Check retries
      if (!queue.attributes.redrivePolicy.deadLetterTargetArn.empty()) {
        _sqsDatabase.RedriveMessages(queue.queueUrl, queue.attributes.redrivePolicy);
      }

      // Check delays
      if (queue.attributes.delaySeconds > 0) {
        _sqsDatabase.ResetDelayedMessages(queue.queueUrl, queue.attributes.delaySeconds);
      }

      _sqsDatabase.UpdateQueue(queue);
      log_trace_stream(_logger) << "Queue updated, queueName" << queue.name << std::endl;
    }
  }

  void SQSServer::UpdateCounters() {

    // Get total counts
    long queues = _sqsDatabase.CountQueues();
    long messages = _sqsDatabase.CountMessages();
    _metricService.SetGauge("sqs_queue_count_total", queues);
    _metricService.SetGauge("sqs_message_count_total", messages);

    // Count messages per queue
    for (const auto &queue : _sqsDatabase.ListQueues()) {
      std::string labelValue = Poco::replace(queue.name, "-", "_");
      long messagesPerQueue = _sqsDatabase.CountMessages(queue.region, queue.queueUrl);
      _metricService.SetGauge("sqs_message_count", "queue", labelValue, messagesPerQueue);
    }
    log_trace_stream(_logger) << "SQS update counter finished" << std::endl;
  }

} // namespace AwsMock::Worker
