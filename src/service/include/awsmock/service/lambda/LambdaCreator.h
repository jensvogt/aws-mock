//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_LAMBDA_CREATOR_H
#define AWSMOCK_SERVICE_LAMBDA_CREATOR_H

// C++ standard includes
#include <sstream>
#include <string>

// Poco includes
#include <Poco/Base64Decoder.h>
#include <Poco/NotificationQueue.h>
#include <Poco/StreamCopier.h>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/Configuration.h>
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/MetricService.h>
#include <awsmock/core/Task.h>
#include <awsmock/entity/lambda/Lambda.h>
#include <awsmock/repository/LambdaDatabase.h>
#include <awsmock/service/common/DockerService.h>

namespace AwsMock::Service {

    /**
     * Lambda async creator
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class LambdaCreator : public Core::Task {

      public:

        /**
         * @brief Constructor.
         *
         * Will start itself as background thread,
         *
         * @param functionCode zipped and BASE64 encoded function code
         * @param functionId lambda function OID
         */
        [[maybe_unused]] explicit LambdaCreator(std::string functionCode, std::string functionId);

        /**
         * Create new lambda function
         */
        void Run() override;

      private:

        /**
         * Save the ZIP file and unpack it in a temporary folder
         *
         * @param zipFile Base64 encoded ZIP file
         * @param lambdaEntity lambda entity
         * @param dockerTag docker tag to use
         */
        void CreateDockerImage(const std::string &zipFile, Database::Entity::Lambda::Lambda &lambdaEntity, const std::string &dockerTag);

        /**
         * Creates an new docker container, in case the container does not exists inside the docker daemon.
         *
         * @param lambdaEntity lambda entity.
         * @param dockerTag docker tag.
         */
        void CreateDockerContainer(Database::Entity::Lambda::Lambda &lambdaEntity, const std::string &dockerTag);

        /**
         * Converts the lambda environment to a vector of string, which is needed by the docker API
         *
         * @param lambdaEnvironment lambda environment
         * @return vector of strings containing the runtime environment
         */
        static std::vector<std::string> GetEnvironment(const Database::Entity::Lambda::Environment &lambdaEnvironment);

        /**
         * Unpack the provided ZIP file.
         *
         * <p>Needed only when the lambda function is provided as zipped request body.</p>
         *
         * @param zipFile Base64 encoded zip file.
         * @param runtime AWS lambda runtime name
         * @param fileName filename of the Base64 encoded and zipped code file
         * @return code directory
         */
        std::string UnpackZipFile(const std::string &zipFile, const std::string &runtime, const std::string &fileName);

        /**
         * Returns a random host port in the range 32768 - 65536 for the host port of the docker container which is running the lambda function.
         *
         * @return random port between 32768 and 65536
         */
        static int GetHostPort();

        /**
         * Returns the docker tag.
         *
         * <p>The method returns the docker tags in that order:
         * <ul>
         * <li>version: if the lambda entity has a version tag, the version tag is used.</li>
         * <li>dockerTag: if the lambda entity has a dockerTag, the dockerTag is used.</li>
         * <li>tag: if the lambda entity has a tag named tag, this tag is used.</li>
         * <li>latest: default return value.</li>
         * </ul>
         * </p>
         *
         * @param lambda lambda database entity
         * @return random port between 32768 and 65536
         */
        static std::string GetDockerTag(const Database::Entity::Lambda::Lambda &lambda);

        /**
         * Database connection
         */
        Database::LambdaDatabase &_lambdaDatabase;

        /**
         * Data directory
         */
        std::string _dataDir;

        /**
         * Temp directory
         */
        std::string _tempDir;

        /**
         * Docker module
         */
        Service::DockerService _dockerService;

        /**
         * Function code
         */
        std::string _functionCode;

        /**
         * Function ID
         */
        std::string _functionId;
    };

}//namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_LAMBDA_CREATOR_H
