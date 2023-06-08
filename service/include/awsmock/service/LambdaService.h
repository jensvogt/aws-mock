//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_LAMBDASERVICE_H
#define AWSMOCK_SERVICE_LAMBDASERVICE_H

// C++ standard includes
#include <string>

// Poco includes
#include <Poco/Logger.h>
#include <Poco/UUID.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/RecursiveDirectoryIterator.h>

// Easyzip
#include <easyzip/easyzip.h>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/ServiceException.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/SystemUtils.h>
#include <awsmock/core/TarUtils.h>
#include <awsmock/db/LambdaDatabase.h>
#include <awsmock/dto/lambda/CreateFunctionRequest.h>
#include <awsmock/dto/lambda/CreateFunctionResponse.h>
#include <awsmock/service/DockerService.h>

namespace AwsMock::Service {

    class DockerRunner : public Poco::Runnable {

    public:

      /**
       * Thread main method
       */
      void run() {

          _running = true;
          while (_running) {
              Poco::Thread::sleep(1000);
          }
      }

      void stop() {
          _running = false;
      }

    private:

      /**
       * Start the docker image
       */
      void StartDockerImage();

      /**
       * Running flag
       */
      bool _running = false;
    };

    class LambdaService {

    public:

      /**
       * Constructor
       *
       * @param configuration service configuration
       */
      explicit LambdaService(const Core::Configuration &configuration);

      /**
       * Create lambda function
       *
       * @param request create lambda request
       * @return CreateFunctionResponse
       */
      Dto::Lambda::CreateFunctionResponse CreateFunction(Dto::Lambda::CreateFunctionRequest &request);

    private:

      /**
       * Initialize the service
       */
      void Initialize();

      /**
       * Unpack the provided ZIP file.
       *
       * <p>Needed only when the Lambda function is provided as zipped request body.</p>
       *
       * @param zipFile Base64 encoded zip file.
       * @return code directory
       */
      std::string UnpackZipFile(const std::string &zipFile);

      /**
       * Create the docker image.
       *
       * @param codeDir code directory
       * @param functionName function name
       * @param handler function handler
       * @return location of docker file
       */
      std::string BuildDockerImage(const std::string &codeDir, const std::string &functionName, const std::string &handler);

      /**
       * Logger
       */
      Poco::Logger &_logger;

      /**
       * Data directory
       */
      std::string _dataDir;

      /**
       * Temp directory
       */
      std::string _tempDir;

      /**
       * AWS region
       */
      std::string _region;

      /**
       * AWS account ID
       */
      std::string _accountId;

      /**
       * Configuration
       */
      const Core::Configuration &_configuration;

      /**
       * Database connection
       */
      std::unique_ptr<Database::LambdaDatabase> _database;

      /**
       * Database connection
       */
      std::unique_ptr<Service::DockerService> _dockerService;
    };

} //namespace AwsMock::Service

#endif //AWSMOCK_SERVICE_LAMBDASERVICE_H
