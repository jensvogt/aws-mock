//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWSMOCK_CORE_CONFIGURATIONTEST_H
#define AWSMOCK_CORE_CONFIGURATIONTEST_H

// C++ standard includes
#include <fstream>

// GTest includes
#include <gtest/gtest.h>

// Local includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/CoreException.h>
#include <awsmock/core/FileUtils.h>

#define CONFIGURATION_FILE "/tmp/aws-mock.properties"

namespace AwsMock::Core {

    class ConfigurationTest : public ::testing::Test {

    protected:
      void SetUp() override {
          setenv("AWSMOCK_CORE_LOG_LEVEL", "error", true);
          std::ofstream ofs(CONFIGURATION_FILE);
          ofs << "awsmock.log.level=debug" << std::endl;
          ofs.close();
      }

      void TearDown() override {
      }

      Configuration _configuration = Configuration(CONFIGURATION_FILE);
    };

    TEST_F(ConfigurationTest, EmptyFilenameTest) {
        // arrange

        // act
        EXPECT_THROW({
                         try {
                             Configuration configuration = Configuration("");
                         }
                         catch (const CoreException &e) {
                             EXPECT_STREQ("Empty filename", e.message().c_str());
                             EXPECT_STREQ("CoreException: ", e.name());
                             throw;
                         }
                     }, CoreException);

        // assert
    }

    TEST_F(ConfigurationTest, ConstructorTest) {

        // arrange
        Configuration *configuration = nullptr;

        // act
        EXPECT_NO_THROW({ configuration = new Configuration(CONFIGURATION_FILE); });

        // assert
        EXPECT_STREQ(configuration->GetFilename().c_str(), CONFIGURATION_FILE);
        EXPECT_STREQ(configuration->GetLogLevel().c_str(), "debug");
    }

    TEST_F(ConfigurationTest, SetValueTest) {

        // arrange
        Configuration *configuration = nullptr;
        EXPECT_NO_THROW({ configuration = new Configuration(CONFIGURATION_FILE); });

        // act
        if (configuration != nullptr) {
            configuration->SetValue("awsmock.log.level", "error");
        }

        // assert
        EXPECT_STREQ(configuration->getString("awsmock.log.level").c_str(), "error");
    }

    TEST_F(ConfigurationTest, EnvironmentTest) {

        // arrange
        setenv("AWSMOCK_CORE_LOG_LEVEL", "error", true);

        // act

        // assert
        EXPECT_STREQ(_configuration.getString("awsmock.log.level").c_str(), "debug");
    }

} // namespace AwsMock::Core

#endif // AWSMOCK_CORE_CONFIGURATIONTEST_H