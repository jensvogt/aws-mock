//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_SERVICEDATABASETEST_H
#define AWMOCK_CORE_SERVICEDATABASETEST_H

// C++ standard includes
#include <iostream>
#include <vector>

// GTest includes
#include <gtest/gtest.h>

// Local includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/ModuleDatabase.h>

// MongoDB includes
#include <bsoncxx/builder/basic/document.hpp>
#include <mongocxx/client.hpp>

#define SERVICE "test-module"

namespace AwsMock::Database {

  using bsoncxx::builder::basic::kvp;
  using bsoncxx::builder::basic::make_array;
  using bsoncxx::builder::basic::make_document;

  class ModuleDatabaseTest : public ::testing::Test {

  protected:

    void SetUp() override {
      _region = _configuration.getString("awsmock.region");
    }

    void TearDown() override {
      _moduleDatabase.DeleteAllModules();
    }

    std::string _region;
    Core::Configuration _configuration = Core::Configuration(TMP_PROPERTIES_FILE);
    ModuleDatabase _moduleDatabase = ModuleDatabase(_configuration);
  };

  TEST_F(ModuleDatabaseTest, ModuleCreateTest) {

    // arrange
    Entity::Module::Module module = {.name=SERVICE, .status=Entity::Module::ModuleStatus::RUNNING};

    // act
    Entity::Module::Module result = _moduleDatabase.CreateModule(module);

    // assert
    EXPECT_TRUE(result.name == SERVICE);
    EXPECT_TRUE(result.status == Entity::Module::ModuleStatus::RUNNING);
  }

  TEST_F(ModuleDatabaseTest, ModuleExistsTest) {

    // arrange
    Entity::Module::Module module = {.name=SERVICE, .status=Entity::Module::ModuleStatus::RUNNING};
    module = _moduleDatabase.CreateModule(module);

    // act
    bool result1 = _moduleDatabase.ModuleExists(module.name);
    bool result2 = _moduleDatabase.ModuleExists("blabla");

    // assert
    EXPECT_TRUE(result1);
    EXPECT_FALSE(result2);
  }

  TEST_F(ModuleDatabaseTest, ModuleActiveTest) {

    // arrange
    Entity::Module::Module module = {.name=SERVICE, .status=Entity::Module::ModuleStatus::RUNNING};
    module = _moduleDatabase.CreateModule(module);

    // act
    bool result = _moduleDatabase.IsActive(module.name);

    // assert
    EXPECT_TRUE(result);
  }

  TEST_F(ModuleDatabaseTest, ModuleGetByNameTest) {

    // arrange
    Entity::Module::Module module = {.name=SERVICE, .status=Entity::Module::ModuleStatus::RUNNING};
    module = _moduleDatabase.CreateModule(module);

    // act
    Entity::Module::Module result = _moduleDatabase.GetModuleByName(module.name);

    // assert
    EXPECT_TRUE(result.name == SERVICE);
  }

  TEST_F(ModuleDatabaseTest, ModuleGetByIdTest) {

    // arrange
    Entity::Module::Module module = {.name=SERVICE, .status=Entity::Module::ModuleStatus::RUNNING};
    module = _moduleDatabase.CreateModule(module);
    bsoncxx::oid oid(module.oid);

    // act
    Entity::Module::Module result = _moduleDatabase.GetModuleById(oid);

    // assert
    EXPECT_TRUE(result.name == SERVICE);
  }

  TEST_F(ModuleDatabaseTest, ModuleUpdateTest) {

    // arrange
    Entity::Module::Module module = {.name=SERVICE, .status=Entity::Module::ModuleStatus::RUNNING};
    module = _moduleDatabase.CreateModule(module);
    module.status = Entity::Module::ModuleStatus::STOPPED;

    // act
    Entity::Module::Module result = _moduleDatabase.UpdateModule(module);

    // assert
    EXPECT_TRUE(result.status == Entity::Module::ModuleStatus::STOPPED);
  }

  TEST_F(ModuleDatabaseTest, ModuleListTest) {

    // arrange
    Entity::Module::Module module = {.name=SERVICE, .status=Entity::Module::ModuleStatus::RUNNING};
    _moduleDatabase.CreateModule(module);

    // act
    Entity::Module::ModuleList result = _moduleDatabase.ListModules();

    // assert
    EXPECT_EQ(8, result.size());
  }

  TEST_F(ModuleDatabaseTest, ModuleDeleteTest) {

    // arrange
    Entity::Module::Module module = {.name=SERVICE, .status=Entity::Module::ModuleStatus::RUNNING};
    module = _moduleDatabase.CreateModule(module);

    // act
    _moduleDatabase.DeleteModule(module);
    int count = _moduleDatabase.ModuleCount();

    // assert
    EXPECT_EQ(7, count);
  }

  TEST_F(ModuleDatabaseTest, ModuleSetStatusTest) {

    // arrange
    Entity::Module::Module module = {.name=SERVICE, .status=Entity::Module::ModuleStatus::RUNNING};
    module = _moduleDatabase.CreateModule(module);

    // act
    _moduleDatabase.SetStatus(SERVICE, Entity::Module::ModuleStatus::STOPPED);
    Entity::Module::Module updatedModule = _moduleDatabase.GetModuleByName(SERVICE);

    // assert
    EXPECT_EQ(updatedModule.status, Entity::Module::ModuleStatus::STOPPED);
  }

  TEST_F(ModuleDatabaseTest, ModuleSetPortTest) {

    // arrange
    Entity::Module::Module module = {.name=SERVICE, .status=Entity::Module::ModuleStatus::RUNNING};
    module = _moduleDatabase.CreateModule(module);

    // act
    _moduleDatabase.SetPort(SERVICE, 9999);
    Entity::Module::Module updatedModule = _moduleDatabase.GetModuleByName(SERVICE);

    // assert
    EXPECT_EQ(updatedModule.port, 9999);
  }

} // namespace AwsMock::Database

#endif // AWMOCK_CORE_SERVICEDATABASETEST_H