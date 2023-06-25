//
// Created by vogje01 on 29/05/2023.
//

#include "awsmock/core/DirUtils.h"

namespace AwsMock::Core {

    std::string DirUtils::RelativePath(const std::string &dir) {
        return dir.substr(1);
    }

    std::string DirUtils::GetTempDir() {
        return Poco::replace(Poco::toLower(Poco::Path::temp() + Poco::UUIDGenerator().createRandom().toString()), "-", "");
    }

    std::string DirUtils::CreateTempDir() {
        Poco::File tempDir = Poco::File(GetTempDir());
        tempDir.createDirectories();
        return tempDir.path();
    }

    std::string DirUtils::CreateTempDir(const std::string &parent) {
        Poco::File tempDir = Poco::File(parent + GetTempDir());
        tempDir.createDirectories();
        return tempDir.path();
    }

    bool DirUtils::DirectoryExists(const std::string &dirName) {
        Poco::File tempDir = Poco::File(dirName);
        return tempDir.exists();
    }

    long DirUtils::DirectoryCountFiles(const std::string &dirName) {
        Poco::DirectoryIterator it(dirName);
        Poco::DirectoryIterator end;
        int count = 0;
        while (it != end) {
            count++;
            ++it;
        }
        return count;
    }

    bool DirUtils::DirectoryEmpty(const std::string &dirName) {
        return DirectoryCountFiles(dirName) == 0;
    }

    bool DirUtils::IsDirectory(const std::string &dirName) {
        Poco::File file(dirName);
        return file.isDirectory();
    }

    void DirUtils::MakeDirectory(const std::string &dirName, bool recursive) {
        Poco::File file(dirName);
        if (recursive) {
            file.createDirectories();
        } else {
            file.createDirectory();
        }
    }

    std::vector<std::string> DirUtils::ListFiles(const std::string &dirName, bool recursive) {
        std::vector<std::string> fileNames;
        if( recursive) {
            Poco::RecursiveDirectoryIterator it(dirName);
            Poco::RecursiveDirectoryIterator end;
            while (it != end) {
                fileNames.push_back(it->path());
                ++it;
            }
        } else {
            Poco::DirectoryIterator it(dirName);
            Poco::DirectoryIterator end;
            while (it != end) {
                fileNames.push_back(it->path());
                ++it;
            }
        }
        return fileNames;
    }

    std::vector<std::string> DirUtils::ListFilesByPrefix(const std::string &dirName, const std::string &prefix) {

        Poco::DirectoryIterator it(dirName);
        Poco::DirectoryIterator end;
        std::vector<std::string> fileNames;
        while (it != end) {
            if(Core::StringUtils::StartsWith(it.name(), prefix)) {
                fileNames.push_back(it.name());
            }
            ++it;
        }
        std::sort(fileNames.begin(), fileNames.end());
        return fileNames;
    }

    std::vector<std::string> DirUtils::ListFilesByPattern(const std::string &dirName, const std::string &pattern, bool recursive) {

        Poco::RegularExpression re(pattern);
        std::vector<std::string> fileNames;
        if( recursive) {
            Poco::RecursiveDirectoryIterator it(dirName);
            Poco::RecursiveDirectoryIterator end;
            while (it != end) {
                if(re.match(it.path().toString())) {
                    fileNames.push_back(it->path());
                }
                ++it;
            }
        } else {
            Poco::DirectoryIterator it(dirName);
            Poco::DirectoryIterator end;
            while (it != end) {
                if(re.match(it.path().toString())) {
                    fileNames.push_back(it->path());
                }
                ++it;
            }
        }
        return fileNames;
    }

    void DirUtils::DeleteDirectory(const std::string &dirName, bool recursive) {

        if (DirectoryExists(dirName)) {
            Poco::File tempDir = Poco::File(dirName);
            tempDir.remove(recursive);
        }
    }

    void DirUtils::DeleteFilesInDirectory(const std::string &dirName) {

        if (DirectoryExists(dirName)) {
            std::vector<std::string> files = ListFiles(dirName);
            for(auto &it:files) {
                Poco::File tempFile = Poco::File(it);
                tempFile.remove();
            }
        }
    }
}