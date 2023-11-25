//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_MODULEMEMORYDB_H
#define AWSMOCK_REPOSITORY_MODULEMEMORYDB_H

// C++ standard includes
#include <string>

// Poco includes
#include <Poco/Mutex.h>
#include <Poco/ScopedLock.h>
#include <Poco/UUIDGenerator.h>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/Configuration.h>
#include <awsmock/core/DatabaseException.h>
#include <awsmock/entity/module/Module.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

  class ModuleMemoryDb {

  public:

    /**
     * Constructor
     */
    explicit ModuleMemoryDb();

    /**
     * Singleton instance
     */
    static ModuleMemoryDb &instance() {
      static Poco::SingletonHolder<ModuleMemoryDb> sh;
      return *sh.get();
    }

    /**
     * Checks the active flag.
     *
     * @param name module name
     * @return true if active
     */
    bool IsActive(const std::string &name);

    /**
     * Check existence of name
     *
     * @param name name name
     * @return created name
     */
    bool ModuleExists(const std::string &name);

    /**
     * Returns the module by id
     *
     * @param oid module oid
     * @return module, if existing
     * @throws DatabaseException
     */
    Entity::Module::Module GetModuleById(const std::string &oid);

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
     * Sets the state of module.
     *
     * <p>State can be one of STARTING, RUNNING, STOPPED, UNKNOWN.</p>
     *
     * @param name module name
     * @param state module state
     * @see AwsMock::Database::Entity::Module::ModuleState()
     */
    void SetState(const std::string &name, const Entity::Module::ModuleState &state);

    /**
     * Sets the status of module.
     *
     * <p>Status can be one of UNKNOWN, ACTIVE, INACTIVE.</p>
     *
     * @param name module name
     * @param status module status
     * @see AwsMock::Database::Entity::Module::ModuleStatus()
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
     * Modules map
     */
    std::map<std::string, Entity::Module::Module> _modules;

    /**
     * Existing modules
     */
    static std::map<std::string, Entity::Module::Module> _existingModules;

    /**
     * Object lock
     */
     Poco::Mutex _moduleMutex;
  };

} // namespace AwsMock::Database

#endif // AWSMOCK_REPOSITORY_MODULEMEMORYDB_H