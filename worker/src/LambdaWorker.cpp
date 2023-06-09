//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/worker/LambdaWorker.h>

namespace AwsMock::Worker {

    [[maybe_unused]] LambdaWorker::LambdaWorker(const Core::Configuration &configuration)
        : _logger(Poco::Logger::get("LambdaWorker")), _configuration(configuration), _running(false) {

        // Set console logger
        Core::Logger::SetDefaultConsoleLogger("LambdaWorker");

        Initialize();
    }

    void LambdaWorker::Initialize() {

        _dataDir = _configuration.getString("awsmock.data.dir") + Poco::Path::separator() + "lambda";
        _logger.debug() << "Lambda directory: " << _dataDir << std::endl;

        // Sleeping period
        _period = _configuration.getInt("awsmock.worker.lambda.period", 10000);

        // Create lambda directory
        if (!Core::DirUtils::DirectoryExists(_dataDir)) {
            Core::DirUtils::MakeDirectory(_dataDir);
        }

        // Create environment
        _region = _configuration.getString("awsmock.region");
        _s3Service = std::make_unique<Service::S3Service>(_configuration);
        _lambdaDatabase = std::make_unique<Database::LambdaDatabase>(_configuration);

        _logger.debug() << "LambdaWorker initialized" << std::endl;
    }

    void LambdaWorker::run() {

        // Check service active
        if (!_serviceDatabase->IsActive("Lambda")) {
            return;
        }

        _running = true;
        while (_running) {
            Poco::Thread::sleep(_period);
        }
    }

} // namespace AwsMock::Worker
