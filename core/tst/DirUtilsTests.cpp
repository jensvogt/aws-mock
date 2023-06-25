//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_DIRUTILSTEST_H
#define AWMOCK_CORE_DIRUTILSTEST_H

// GTest includes
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// Poco includes
#include "Poco/Path.h"

// Local includes
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/FileUtils.h>

#define FILE_SIZE (100*1024)

namespace AwsMock::Core {

    class DirUtilsTest : public ::testing::Test {

    protected:

      void SetUp() override {
          tempDir = Core::DirUtils::CreateTempDir();
      }

      void TearDown() override {
          Core::DirUtils::DeleteDirectory(tempDir);
      }

      std::string tempDir;
    };

    TEST_F(DirUtilsTest, DeleteDirTest) {

        // arrange
        std::string dirName = DirUtils::CreateTempDir(tempDir);

        // act
        EXPECT_NO_THROW({ DirUtils::DeleteDirectory(dirName); });

        // assert
        EXPECT_FALSE(DirUtils::DirectoryExists(dirName));
    }

    TEST_F(DirUtilsTest, IsDirectoryTest) {

        // arrange
        std::string dirName = DirUtils::CreateTempDir(tempDir);

        // act
        bool result = DirUtils::IsDirectory(dirName);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(DirUtilsTest, DeleteDirRecursiveTest) {

        // arrange
        std::string dirName = DirUtils::CreateTempDir(tempDir);
        for (int i = 0; i < 3; i++) {
            FileUtils::CreateTempFile(dirName, "json", 100);
        }

        // act
        EXPECT_NO_THROW({ DirUtils::DeleteDirectory(dirName, true); });

        // assert
        EXPECT_FALSE(DirUtils::DirectoryExists(dirName));
    }

    TEST_F(DirUtilsTest, DirectoryFileCountTest) {

        // arrange
        std::string dirName = DirUtils::CreateTempDir(tempDir);
        for (int i = 0; i < 3; i++) {
            FileUtils::CreateTempFile(dirName, "json", 100);
        }

        // act
        long result = 0;
        EXPECT_NO_THROW({ result = DirUtils::DirectoryCountFiles(dirName); });

        // assert
        EXPECT_EQ(result, 3);
    }

    TEST_F(DirUtilsTest, DirectoryEmptyTest) {

        // arrange
        std::string dirName = DirUtils::CreateTempDir(tempDir);

        // act
        bool result = false;
        EXPECT_NO_THROW({ result = DirUtils::DirectoryEmpty(dirName); });

        // assert
        EXPECT_EQ(result, true);
    }

    TEST_F(DirUtilsTest, ListFilesTest)
    {
        // arrange
        std::string dirName = DirUtils::CreateTempDir(tempDir);
        std::string fileName1 = FileUtils::CreateTempFile(dirName, "json", 100);
        std::string fileName2 = FileUtils::CreateTempFile(dirName, "txt", 100);
        std::string fileName3 = FileUtils::CreateTempFile(dirName, "xml", 100);

        // act
        std::vector<std::string> result;
        EXPECT_NO_THROW({ result = DirUtils::ListFiles(dirName); });

        // assert
        EXPECT_FALSE(result.empty());
        EXPECT_THAT(result, ::testing::Contains(::testing::Eq(::std::string(fileName1))));
        EXPECT_THAT(result, ::testing::Contains(::testing::Eq(::std::string(fileName2))));
        EXPECT_THAT(result, ::testing::Contains(::testing::Eq(::std::string(fileName3))));
    }

    TEST_F(DirUtilsTest, ListFilesPatternTest)
    {
        // arrange
        std::string dirName = DirUtils::CreateTempDir(tempDir);
        std::string fileName1 = FileUtils::CreateTempFile(dirName, "json", 100);
        std::string fileName2 = FileUtils::CreateTempFile(dirName, "txt", 100);
        std::string fileName3 = FileUtils::CreateTempFile(dirName, "xml", 100);

        // act
        std::vector<std::string> result;
        EXPECT_NO_THROW({ result = DirUtils::ListFilesByPattern(dirName, "^\\/tmp.*\\.xml$"); });

        // assert
        EXPECT_FALSE(result.empty());
        EXPECT_EQ(result.size(), 1);
        EXPECT_THAT(result, ::testing::Contains(::testing::Eq(::std::string(fileName3))));
    }

} // namespace AwsMock::Core

#endif // AWMOCK_CORE_DIRUTILSTEST_H