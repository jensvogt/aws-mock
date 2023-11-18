//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_MODULEDATABASE_H
#define AWSMOCK_REPOSITORY_MODULEDATABASE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/Configuration.h>
#include <awsmock/entity/module/Module.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

  class ModuleDatabase : public Database {

  public:

    /**
     * Constructor
     *
     * @param configuration configuration properties
     */
    explicit ModuleDatabase(const Core::Configuration &configuration);

    /**
     * Checks the active flag.
     *
     * @param name module name
     * @return true if active
     */
    bool IsActive(const std::string &name);

    /**
     * Check existence of module
     *
     * @param module module module
     * @return created module
     */
    bool ModuleExists(const std::string &module);

    /**
     * Returns the module by id
     *
     * @param oid module oid
     * @return module, if existing
     * @throws DatabaseException
     */
    Entity::Module::Module GetModuleById(const bsoncxx::oid &oid);

    /**
     * Returns the module by id
     *
     * @param name module name
     * @return module, if existing
     * @throws DatabaseException
     */
    Entity::Module::Module GetModuleByName(const std::string &name);

    /**
     * Creates a module
     *
     * @param module module entity
     * @return created module
     */
    Entity::Module::Module CreateModule(const Entity::Module::Module &module);

    /**
     * Updates a module
     *
     * @param module module entity
     * @return updated module
     */
    Entity::Module::Module UpdateModule(const Entity::Module::Module &module);

    /**
     * Sets the status of module.
     *
     * @param name module name
     * @param status module status
     */
    void SetStatus(const std::string &name, const Entity::Module::ModuleStatus &status);

    /**
     * Sets the port of module.
     *
     * @param name module name
     * @param port module port
     */
    void SetPort(const std::string &name, int port);

    /**
     * Creates or updates a modules
     *
     * @param modules modules entity
     * @return updated modules
     */
    Entity::Module::Module CreateOrUpdateModule(const Entity::Module::Module &modules);

    /**
     * Counts the number of modules
     *
     * @return total number of modules
     */
    int ModuleCount();

    /**
     * @brief Returns a list of all modules
     *
     * @return list of all modules
     */
    Entity::Module::ModuleList ListModules();

    /**
     * Deletes module
     *
     * @param module module entity
     */
    void DeleteModule(const Entity::Module::Module &module);

    /**
     * Deletes all modules
     */
    void DeleteAllModules();

  private:

    /**
     * Logger
     */
    Core::LogStream _logger;

    /**
     * Service collection
     */
    mongocxx::collection _moduleCollection{};

    /**
     * Existing modules
     */
    static std::map<std::string, Entity::Module::Module> _existingModules;

  };

} // namespace AwsMock::Database

#endif // AWSMOCK_REPOSITORY_MODULEDATABASE_H
