//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_MODULE_SERVICE_H
#define AWSMOCK_SERVICE_MODULE_SERVICE_H

// C++ standard includes
#include <string>

// Poco includes
#include <Poco/Logger.h>
#include <Poco/Net/HTTPResponse.h>

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/ServiceException.h>
#include <awsmock/dto/common/Infrastructure.h>
#include <awsmock/dto/common/Services.h>
#include <awsmock/entity/module/Module.h>
#include <awsmock/repository/ModuleDatabase.h>
#include <awsmock/repository/SecretsManagerDatabase.h>
#include <awsmock/service/gateway/GatewayServer.h>
#include <awsmock/service/s3/S3Server.h>
#include <awsmock/service/sqs/SQSServer.h>
#include <awsmock/service/sns/SNSServer.h>
#include <awsmock/service/lambda/LambdaServer.h>
#include <awsmock/service/cognito/CognitoServer.h>
#include <awsmock/service/dynamodb/DynamoDbServer.h>
#include <awsmock/service/transfer/TransferServer.h>

namespace AwsMock::Service {

  /**
   * The ModuleService controls the different services
   */
  class ModuleService {

  public:

    /**
     * Constructor
     *
     * @param configuration module configuration
     * @param serverMap module map
     */
    explicit ModuleService(Core::Configuration &configuration, Service::ServerMap &serverMap);

    /**
     * Return all list of all modules
     *
     * @param list of all modules
     */
    Database::Entity::Module::ModuleList ListModules();

    /**
     * Returns the running state
     *
     * @param module module name
     * @return module state
     */
    bool IsRunning(const std::string &module);

    /**
     * Starts a module
     *
     * @param name module name
     */
    Database::Entity::Module::Module StartService(const std::string &name);

    /**
     * Starts all services
     */
    void StartAllServices();

    /**
     * Restarts a module
     *
     * @param name module name
     */
    Database::Entity::Module::Module RestartService(const std::string &name);

    /**
     * Restarts all services
     */
    void RestartAllServices();

    /**
     * Stops a module
     *
     * @param name module name
     */
    Database::Entity::Module::Module StopService(const std::string &name);

    /**
     * Stops all services
     */
    void StopAllServices();

    /**
     * Exports the current infrastructure
     *
     * @param services service name list
     * @param prettyPrint JSON pretty print, if true JSON indent = 4
     * @return JSON string
     */
    std::string ExportInfrastructure(const Dto::Common::Services &services, bool prettyPrint = false);

    /**
     * Import the infrastructure
     *
     * @param jsonString infrastructure JSON string
     */
    void ImportInfrastructure(const std::string &jsonString);

    /**
     * Cleans the current infrastructure.
     *
     * <p>All SQS queues, SNS topics, S3 buckets etc. will be deleted, as well as all objects.</p>
     *
     * @param services service name list
     */
    void CleanInfrastructure(const Dto::Common::Services &services);

    /**
     * Cleans the objects from the infrastructure.
     *
     * <p>Cleans all objects from the infrastructure. This means all SQS messages, SNS messages, S3 object keys, etc. will be deleted.</p>
     *
     * @param services service name list
     */
    void CleanObjects(const Dto::Common::Services &services);

  private:

    /**
     * Configuration
     */
    Core::Configuration &_configuration;

    /**
     * Server map
     */
    Service::ServerMap &_serverMap;

    /**
     * Module database
     */
    Database::ModuleDatabase &_moduleDatabase;

    /**
     * JSON pretty print
     */
    bool _prettyPrint;
  };

} // namespace AwsMock::Service

#endif // AWSMOCK_SERVICE_MODULE_SERVICE_H