//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/worker/SQSWorker.h>

namespace AwsMock::Worker {

    SQSWorker::SQSWorker(const Core::Configuration &configuration) : _logger(Poco::Logger::get("SQSWorker")), _configuration(configuration) {
        Core::Logger::SetDefaultConsoleLogger("SQSWorker");

        Initialize();

        poco_debug(_logger, "SQSWorker initialized");
    }

    SQSWorker::~SQSWorker() {
    }

    void SQSWorker::Initialize() {

        // Create environment
        _region = _configuration.getString("awsmock.region");
        _sqsDatabase = std::make_unique<Database::SQSDatabase>(_configuration);
    }

    [[noreturn]] void SQSWorker::run() {

        while (true) {
            ResetMessages();
            Poco::Thread::sleep(1000);
        }
    }

    void SQSWorker::ResetMessages() {
        Database::Entity::SQS::QueueList queueList = _sqsDatabase->ListQueues(_region);
        for(const auto &q:queueList) {
            _sqsDatabase->ResetMessages(q.url, q.visibilityTimeout);
        }
    }
} // namespace AwsMock::Worker