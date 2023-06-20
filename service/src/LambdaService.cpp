//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/LambdaService.h>

namespace AwsMock::Service {

    LambdaService::LambdaService(const Core::Configuration &configuration) : _logger(Poco::Logger::get("LambdaService")), _configuration(configuration) {
        Initialize();
    }

    void LambdaService::Initialize() {

        // Set console logger
        Core::Logger::SetDefaultConsoleLogger("LambdaService");

        // Initialize environment
        _region = _configuration.getString("awsmock.region", "eu-central-1");
        _accountId = _configuration.getString("awsmock.account.id", "000000000000");
        _dataDir = _configuration.getString("awsmock.data.dir", "/tmp/awsmock/data");
        _tempDir = _dataDir + Poco::Path::separator() + "tmp";
        _lambdaDatabase = std::make_unique<Database::LambdaDatabase>(_configuration);
        _s3Database = std::make_unique<Database::S3Database>(_configuration);
        _dockerService = std::make_unique<Service::DockerService>(_configuration);

        // Create temp directory
        if (!Core::DirUtils::DirectoryExists(_tempDir)) {
            Core::DirUtils::MakeDirectory(_tempDir);
        }
        log_debug_stream(_logger) << "Lambda service initialized" << std::endl;
    }

    Dto::Lambda::CreateFunctionResponse LambdaService::CreateFunctionConfiguration(Dto::Lambda::CreateFunctionRequest &request) {
        log_debug_stream(_logger) << "Create function configuration request: " + request.ToString() << std::endl;

        Database::Entity::Lambda::Lambda lambdaEntity = {.function=request.functionName, .runtime=request.runtime, .role=request.role, .handler=request.handler,
            .tag=IMAGE_TAG};

        // Build the docker image, if not existing
        if (!_dockerService->ImageExists(request.functionName, "latest")) {

            // Unzip provided zip-file
            std::string codeDir;
            if (!request.code.zipFile.empty()) {
                codeDir = UnpackZipFile(request.code.zipFile);
            }

            // Build the docker image
            _dockerService->BuildImage(codeDir, request.functionName, "latest", request.handler);

            // Cleanup
            Core::DirUtils::DeleteDirectory(codeDir);
        }

        // Get the image struct
        Dto::Docker::Image image = _dockerService->GetImageByName(request.functionName, "latest");
        lambdaEntity.size = image.size;
        lambdaEntity.imageId = image.id;

        // Create the container, if not existing
        if (!_dockerService->ContainerExists(request.functionName, "latest")) {
            Dto::Docker::CreateContainerResponse containerCreateResponse = _dockerService->CreateContainer(request.functionName, "latest");
            lambdaEntity.hostPort = containerCreateResponse.hostPort;
        }

        // Get the container
        Dto::Docker::Container container = _dockerService->GetContainerByName(request.functionName, "latest");
        lambdaEntity.containerId = container.id;

        // Start container
        _dockerService->StartContainer(container.id);
        lambdaEntity.lastStarted = Poco::DateTime();

        // Update database
        lambdaEntity.arn = Core::AwsUtils::CreateLambdaArn(_region, _accountId, request.functionName);
        lambdaEntity = _lambdaDatabase->CreateOrUpdateLambda(lambdaEntity);

        // Create response
        Dto::Lambda::CreateFunctionResponse
            response{.functionArn=lambdaEntity.arn, .functionName=request.functionName, .runtime=request.runtime, .role=request.role, .handler=request.handler,
            .environment=request.environment, .memorySize=request.memorySize, .dockerImageId=image.id, .dockerContainerId=container.id};

        return response;
    }

    Dto::Lambda::ListFunctionResponse LambdaService::ListFunctionConfiguration(const std::string &region) {

        try {
            std::vector<Database::Entity::Lambda::Lambda> lambdas = _lambdaDatabase->ListLambdas(region);

            auto response = Dto::Lambda::ListFunctionResponse(lambdas);
            log_trace_stream(_logger) << "Lambda list outcome: " + response.ToJson() << std::endl;
            return response;

        } catch (Poco::Exception &ex) {
            log_error_stream(_logger) << "Lambda list request failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500);
        }
    }

    void LambdaService::InvokeEventFunction(const Dto::S3::EventNotification &eventNotification) {
        log_debug_stream(_logger) << "Invocation event function eventNotification: " + eventNotification.ToString() << std::endl;

        for (const auto &record : eventNotification.records) {

            // Get the bucket eventNotification
            Database::Entity::S3::Bucket bucket = _s3Database->GetBucketByRegionName(record.region, record.s3.bucket.name);
            auto notification =
                find_if(bucket.notifications.begin(), bucket.notifications.end(), [&record](const Database::Entity::S3::BucketNotification eventNotification) {
                  return record.eventName == eventNotification.event;
                });
            if (notification != bucket.notifications.end()) {

                log_debug_stream(_logger) << "Got bucket eventNotification: " << notification->ToString() << std::endl;

                // Get the lambda entity
                Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->GetLambdaByArn(notification->lambdaArn);
                log_debug_stream(_logger) << "Got lambda entity eventNotification: " + lambda.ToString() << std::endl;

                SendInvocationRequest(lambda.hostPort, eventNotification.ToJson());
            }
        }
    }

    void LambdaService::DeleteFunction(Dto::Lambda::DeleteFunctionRequest &request) {
        log_debug_stream(_logger) << "Invocation event function notification: " + request.ToString() << std::endl;

        if (!_lambdaDatabase->LambdaExists(request.functionName)) {
            log_error_stream(_logger) << "Lambda function does not exist, function: " + request.functionName << std::endl;
            throw Core::ServiceException("Lambda function does not exist", 500);
        }

        // Delete the container, if existing
        if (_dockerService->ContainerExists(request.functionName, "latest")) {
            Dto::Docker::Container container = _dockerService->GetContainerByName(request.functionName, "latest");
            _dockerService->DeleteContainer(container);
            log_debug_stream(_logger) << "Docker container deleted, function: " + request.functionName << std::endl;
        }

        // Delete the image, if existing
        if (_dockerService->ImageExists(request.functionName, "latest")) {
            _dockerService->DeleteImage(request.functionName, "latest");
            log_debug_stream(_logger) << "Docker image deleted, function: " + request.functionName << std::endl;
        }

        _lambdaDatabase->DeleteLambda(request.functionName);
        _logger.information() << "Lambda function deleted, function: " + request.functionName << std::endl;
    }

    std::string LambdaService::UnpackZipFile(const std::string &zipFile) {

        std::string decodedZipFile = Core::Crypto::Base64Decode(zipFile);

        // Create directory
        std::string codeDir = _tempDir + Poco::Path::separator() + Poco::UUIDGenerator().createRandom().toString() + Poco::Path::separator();
        std::string classesDir = codeDir + "classes";
        if (Core::DirUtils::DirectoryExists(classesDir)) {
            Core::DirUtils::MakeDirectory(classesDir);
        }

        // Decompress
        std::stringstream input(decodedZipFile);
        Poco::Zip::Decompress dec(input, Poco::Path(classesDir));
        dec.decompressAllFiles();
        input.clear();

        return codeDir;
    }

    void LambdaService::SendInvocationRequest(int port, const std::string &body) {

        Poco::URI uri("http://localhost:" + std::to_string(port) + "/2015-03-31/functions/function/invocations");
        std::string path(uri.getPathAndQuery());

        // Create HTTP request and set headers
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, path, Poco::Net::HTTPMessage::HTTP_1_1);
        request.add("Content-Type", "application/json");
        request.setContentLength((long) body.length());
        log_debug_stream(_logger) << "Invocation request defined, body: " + body << std::endl;

        // Send request
        std::ostream &os = session.sendRequest(request);
        os << body;

        // Get the response status
        Poco::Net::HTTPResponse response;
        if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK) {
            log_error_stream(_logger) << "HTTP error, status: " + std::to_string(response.getStatus()) + " reason: " + response.getReason() << std::endl;
        }
        log_debug_stream(_logger) << "Invocation request send, status: " << response.getStatus() << std::endl;
    }
} // namespace AwsMock::Service
