//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_MODULESERVICE_H
#define AWSMOCK_SERVICE_MODULESERVICE_H

// C++ standard includes
#include <string>

// Poco includes
#include <Poco/Logger.h>
#include <Poco/Net/HTTPResponse.h>

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/ServiceException.h>
#include <awsmock/entity/module/Module.h>
#include <awsmock/repository/ModuleDatabase.h>
#include <awsmock/service/GatewayServer.h>
#include <awsmock/service/S3Server.h>
#include <awsmock/service/SQSServer.h>
#include <awsmock/service/SNSServer.h>
#include <awsmock/service/LambdaServer.h>
#include <awsmock/service/TransferServer.h>

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

  private:

    /**
     * Logger
     */
    Core::LogStream _logger;

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
    std::shared_ptr<Database::ModuleDatabase> _moduleDatabase;
  };

} // namespace AwsMock::Service

#endif // AWSMOCK_SERVICE_MODULESERVICE_H
