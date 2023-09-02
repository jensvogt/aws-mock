//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/LambdaService.h>

namespace AwsMock::Service {

    LambdaService::LambdaService(const Core::Configuration &configuration) : _logger(Poco::Logger::get("LambdaService")),
                                                                             _configuration(configuration) {
        Initialize();
    }

    void LambdaService::Initialize() {

        // Initialize environment
        _accountId = _configuration.getString("awsmock.account.id", "000000000000");
        _dataDir = _configuration.getString("awsmock.data.dir", "/tmp/awsmock/data");
        _tempDir = _dataDir + Poco::Path::separator() + "tmp";
        _lambdaDatabase = std::make_shared<Database::LambdaDatabase>(_configuration);
        _s3Database = std::make_shared<Database::S3Database>(_configuration);
        _dockerService = std::make_shared<Service::DockerService>(_configuration);

        // Create temp directory
        if (!Core::DirUtils::DirectoryExists(_tempDir)) {
            Core::DirUtils::MakeDirectory(_tempDir);
        }
        log_debug_stream(_logger) << "Lambda service initialized" << std::endl;
    }

    Dto::Lambda::CreateFunctionResponse
    LambdaService::CreateFunctionConfiguration(Dto::Lambda::CreateFunctionRequest &request) {
        log_debug_stream(_logger) << "Create function configuration request, name: " + request.functionName << std::endl;

        Database::Entity::Lambda::Lambda lambdaEntity;
        std::string lambdaArn = Core::AwsUtils::CreateLambdaArn(request.region, _accountId, request.functionName);

        if (_lambdaDatabase->LambdaExists(request.region, request.functionName, request.runtime)) {

            lambdaEntity = _lambdaDatabase->GetLambdaByArn(lambdaArn);

        } else {

            Database::Entity::Lambda::Environment environment = {.variables=request.environmentVariables.variables};
            lambdaEntity =
                {.region=request.region, .user=request.user, .function=request.functionName, .runtime=request.runtime, .role=request.role,
                    .handler=request.handler, .tag=IMAGE_TAG, .arn=lambdaArn, .environment=environment};
        }

        // Build the docker image, if not existing
        if (!_dockerService->ImageExists(request.functionName, lambdaEntity.tag)) {

            CreateImage(request, lambdaEntity);
        }

        // Get the image struct
        Dto::Docker::Image image = _dockerService->GetImageByName(request.functionName, lambdaEntity.tag);
        lambdaEntity.size = image.size;
        lambdaEntity.imageId = image.id;

        // Create the container, if not existing
        if (!_dockerService->ContainerExists(request.functionName, lambdaEntity.tag)) {
            CreateContainer(request, lambdaEntity);
        }

        // Get the container
        Dto::Docker::Container container = _dockerService->GetContainerByName(request.functionName, lambdaEntity.tag);
        lambdaEntity.containerId = container.id;

        // Start container
        _dockerService->StartContainer(container.id);
        lambdaEntity.lastStarted = Poco::DateTime();
        lambdaEntity.state = Database::Entity::Lambda::LambdaState::ACTIVE;

        // Update database
        lambdaEntity = _lambdaDatabase->CreateOrUpdateLambda(lambdaEntity);

        // Create response
        Dto::Lambda::CreateFunctionResponse
            response
            {.functionArn=lambdaEntity.arn, .functionName=request.functionName, .runtime=request.runtime, .role=request.role, .handler=request.handler,
                .environment=request.environmentVariables, .memorySize=request.memorySize, .codeSize=lambdaEntity.size, .codeSha256=lambdaEntity.codeSha256,
                .dockerImageId=image.id, .dockerContainerId=container.id};

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
        log_debug_stream(_logger) << "Invocation event function eventNotification: " + eventNotification.ToString()
                                  << std::endl;

        for (const auto &record : eventNotification.records) {

            // Get the bucket eventNotification
            Database::Entity::S3::Bucket bucket = _s3Database->GetBucketByRegionName(record.region,
                                                                                     record.s3.bucket.name);
            if (bucket.HasNotification(record.eventName)) {

                Database::Entity::S3::BucketNotification notification = bucket.GetNotification(record.eventName);
                log_debug_stream(_logger) << "Got bucket eventNotification: " << notification.ToString() << std::endl;

                // Get the lambda entity
                Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->GetLambdaByArn(notification.lambdaArn);
                log_debug_stream(_logger) << "Got lambda entity eventNotification: " + lambda.ToString() << std::endl;

                SendInvocationRequest(lambda.hostPort, eventNotification.ToJson());
            }
        }
    }

    void LambdaService::InvokeSqsFunction(const std::string &functionName, const std::string &payload, const std::string &region, const std::string &user) {
        log_debug_stream(_logger) << "Invocation SQS function, functionName: " + functionName << std::endl;

        std::string lambdaArn = Core::AwsUtils::CreateLambdaArn(region, _accountId, functionName);

        // Get the lambda entity
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->GetLambdaByArn(lambdaArn);
        log_debug_stream(_logger) << "Got lambda entity, name: " + lambda.function << std::endl;

        SendInvocationRequest(lambda.hostPort, payload);
    }

    void LambdaService::DeleteFunction(Dto::Lambda::DeleteFunctionRequest &request) {
        log_debug_stream(_logger) << "Invocation event function notification: " + request.ToString() << std::endl;

        if (!_lambdaDatabase->LambdaExists(request.functionName)) {
            log_error_stream(_logger) << "Lambda function does not exist, function: " + request.functionName
                                      << std::endl;
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

    void LambdaService::SendInvocationRequest(int port, const std::string &body) {

        Poco::URI uri("http://localhost:" + std::to_string(port) + "/2015-03-31/functions/function/invocations");
        std::string path(uri.getPathAndQuery());

        // Create HTTP request and set headers
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, path, Poco::Net::HTTPMessage::HTTP_1_1);
        request.add("Content-Type", "application/json");
        request.setContentLength((long) body.length());
        log_trace_stream(_logger) << "Invocation request defined, body: " + body << std::endl;

        // Send request
        std::ostream &os = session.sendRequest(request);
        os << body;

        // Get the response status
        Poco::Net::HTTPResponse response;
        session.receiveResponse(response);
        if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK) {
            log_error_stream(_logger) << "HTTP error, status: " << response.getStatus()
                                      << " reason: " + response.getReason() << std::endl;
        }
        log_debug_stream(_logger) << "Invocation request send, status: " << response.getStatus() << std::endl;
    }

    void LambdaService::CreateImage(const Dto::Lambda::CreateFunctionRequest &request,
                                    Database::Entity::Lambda::Lambda &lambdaEntity) {
        if (!request.code.zipFile.empty()) {

            // Unzip provided zip-file into a temporary directory
            std::string codeDir = UnpackZipFile(request.code.zipFile, lambdaEntity.runtime);

            // Build the docker image using the docker service
            _dockerService->BuildImage(codeDir,
                                       request.functionName,
                                       lambdaEntity.tag,
                                       request.handler,
                                       lambdaEntity.runtime,
                                       lambdaEntity.size,
                                       lambdaEntity.codeSha256,
                                       lambdaEntity.environment.variables);

            // Cleanup
            Core::DirUtils::DeleteDirectory(codeDir);
            log_debug_stream(_logger) << "Docker image created, name: " << request.functionName << " size: " << lambdaEntity.size << std::endl;

        } else {
            log_error_stream(_logger) << "Empty lambda zip file" << std::endl;
        }
    }

    void LambdaService::CreateContainer(const Dto::Lambda::CreateFunctionRequest &request, Database::Entity::Lambda::Lambda &lambdaEntity) {

        std::vector<std::string> environment = GetEnvironment(lambdaEntity.environment);
        Dto::Docker::CreateContainerResponse containerCreateResponse = _dockerService->CreateContainer(request.functionName, lambdaEntity.tag, environment);

        lambdaEntity.hostPort = containerCreateResponse.hostPort;
        log_debug_stream(_logger) << "Lambda container created, hostPort: " << lambdaEntity.hostPort << std::endl;
    }

    std::string LambdaService::UnpackZipFile(const std::string &zipFile, const std::string &runtime) {

        std::string decodedZipFile = Core::Crypto::Base64Decode(zipFile);

        // Create directory
        std::string codeDir = _tempDir + Poco::Path::separator() + Poco::UUIDGenerator().createRandom().toString() + Poco::Path::separator();
        if (Core::StringUtils::ContainsIgnoreCase(runtime, "java")) {
            std::string classesDir = codeDir + "classes";
            if (!Core::DirUtils::DirectoryExists(classesDir)) {
                Core::DirUtils::MakeDirectory(classesDir);
            }

            // Decompress
            std::stringstream input(decodedZipFile);
            Poco::Zip::Decompress dec(input, Poco::Path(classesDir));
            dec.decompressAllFiles();
            input.clear();
            log_debug_stream(_logger) << "ZIP file unpacked, dir: " << codeDir << std::endl;

        } else {

            if (!Core::DirUtils::DirectoryExists(codeDir)) {
                Core::DirUtils::MakeDirectory(codeDir);
            }

            // Write to temp file
            std::ofstream ofs(_tempDir + "/zipfile.zip");
            ofs << decodedZipFile;
            ofs.close();

            // Decompress
            Core::ExecResult result = Core::SystemUtils::Exec("unzip -o -d " + codeDir + " " + _tempDir + "/zipfile.zip");
            log_debug_stream(_logger) << "ZIP file unpacked, dir: " << codeDir << " result: " << result.status << std::endl;
        }

        return codeDir;
    }

    std::vector<std::string> LambdaService::GetEnvironment(const Database::Entity::Lambda::Environment &lambdaEnvironment) {

        std::vector<std::string> environment;
        environment.reserve(lambdaEnvironment.variables.size());
        for (const std::pair<std::string, std::string> &variable : lambdaEnvironment.variables) {
            environment.emplace_back(variable.first + "=" + variable.second);
        }
        log_debug_stream(_logger) << "Lambda runtime environment converted, size: " << environment.size() << std::endl;
        return environment;
    }

} // namespace AwsMock::Service
