#include "awsmock/core/FileUtils.h"
#include "awsmock/core/ResourceNotFoundException.h"

namespace AwsMock::Core {

    std::string FileUtils::GetBasename(const std::string &fileName) {
        Poco::Path path(fileName);
        return path.getBaseName();
    }

    std::string FileUtils::GetExtension(const std::string &fileName) {
        Poco::Path path(fileName);
        return path.getExtension();
    }

    bool FileUtils::FileNameContainsString(const std::string &fileName, const std::string &pattern) {
        Poco::Path path(fileName);
        return path.getBaseName().find(pattern) != std::string::npos;
    }

    std::string FileUtils::GetTempFile(const std::string &extension) {
        return Poco::replace(Poco::toLower(Poco::Path::temp() + Poco::UUIDGenerator().createRandom().toString() + "." + extension), "-", "");
    }

    std::string FileUtils::GetTempFile(const std::string &dir, const std::string &extension) {
        return dir + Poco::Path::separator()
            + Poco::replace(Poco::toLower(Poco::Path::temp() + Poco::UUIDGenerator().createRandom().toString() + "." + extension), "-", "");
    }

    std::string FileUtils::GetParentPath(const std::string &fileName) {
        std::filesystem::path path(fileName);
        return path.parent_path();
    }

    long FileUtils::FileSize(const std::string &fileName) {
        Poco::File file(fileName);
        return (long)file.getSize();
    }

    void FileUtils::MoveTo(const std::string &sourceFileName, const std::string &targetFileName, bool createDir) {
        Poco::File sourceFile(sourceFileName);
        Poco::File targetFile(targetFileName);
        std::string parentPath = GetParentPath(targetFileName);
        if(createDir && !Core::DirUtils::DirectoryExists(parentPath)) {
            Poco::File parentFile(parentPath);
            parentFile.createDirectories();
        }
        sourceFile.renameTo(targetFileName);
    }

    void FileUtils::AppendBinaryFiles(const std::string &outFile, const std::string &inDir, const std::vector<std::string> &files) {
        std::ofstream ofs(outFile, std::ios::out |std::ios::trunc | std::ios::binary);
        for(auto &it : files) {
            std::string inFile = inDir;
            inFile.append(Poco::Path::separator() + it);
            std::ifstream ifs(inFile, std::ios::in | std::ios::binary);
            ofs << ifs.rdbuf();
            ifs.close();
        }
        ofs.close();
    }

    std::string FileUtils::CreateTempFile(const std::string &extension) {
        std::string tempFilename = GetTempFile(extension);
        std::ofstream tempFile;
        tempFile.open(tempFilename);
        tempFile << StringUtils::GenerateRandomString(100);
        tempFile.close();
        return tempFilename;
    }

    std::string FileUtils::CreateTempFile(const std::string &extension, int numBytes) {
        std::string tempFilename = GetTempFile(extension);
        std::ofstream tempFile;
        tempFile.open(tempFilename);
        tempFile << StringUtils::GenerateRandomString(numBytes);
        tempFile.close();
        return tempFilename;
    }

    std::string FileUtils::CreateTempFile(const std::string &dirName, const std::string &extension, int numBytes) {
        std::string tmpFileName = Poco::replace(Poco::UUIDGenerator().createRandom().toString(), "-", "") + "." + extension;
        std::ostringstream stringStream;
        stringStream << dirName << Poco::Path::separator() << tmpFileName;
        std::string tempFilename = stringStream.str();
        std::ofstream tempFile;
        tempFile.open(tempFilename);
        tempFile << StringUtils::GenerateRandomString(numBytes);
        tempFile.close();
        return tempFilename;
    }

    std::string FileUtils::CreateTempFile(const std::string &extension, const std::string &content) {
        std::string tempFilename = GetTempFile(extension);
        std::ofstream tempFile;
        tempFile.open(tempFilename);
        tempFile << content;
        tempFile.close();
        return tempFilename;
    }

    bool FileUtils::FileExists(const std::string &fileName) {
        Poco::File file(fileName);
        return file.exists();
    }

    std::string FileUtils::StripBasePath(const std::string &fileName) {
        return Poco::format("%s.%s", GetBasename(fileName), GetExtension(fileName));
    }

    std::string FileUtils::GetOwner(const std::string &fileName) {
        struct stat info{};
        stat(fileName.c_str(), &info);  // Error check omitted
        struct passwd *pw = getpwuid(info.st_uid);
        //struct group  *gr = getgrgid(info.st_gid);
        if(pw) {
            return pw->pw_name;
        }
        return {};
    }

    void FileUtils::DeleteFile(const std::string &fileName) {
        if(fileName.empty()) {
            return;
        }
        Poco::File file(fileName);
        if (file.exists()) {
            file.remove();
        }
    }

    void FileUtils::UnzipFiles(const std::string &zipFile, const std::string &dirName) {
        Poco::File tempDir = Poco::File(dirName);
        tempDir.createDirectories();
        poco_debug(Poco::Logger::get("FileUtils"), "Using output directory: " + dirName);

        std::ifstream inp(zipFile, std::ios::binary);
        poco_assert(inp);

        // Decompress to a temp dir
        Poco::Zip::Decompress dec(inp, dirName);

        // Decompress to directory
        dec.decompressAllFiles();
        poco_debug(Poco::Logger::get("FileUtils"), "File uncompressed, zipFile: " + zipFile + " directory:" + dirName);
    }

    void FileUtils::ZipFiles(const std::string &zipFile, const std::string &dirName) {
        poco_debug(Poco::Logger::get("FileUtils"), "Using directory: " + dirName);

        std::ofstream out(zipFile, std::ios::binary);
        Poco::Zip::Compress com(out, true);
        com.addRecursive(Poco::Path(dirName));
        com.close();
        poco_debug(Poco::Logger::get("FileUtils"), "Files compressed, zipFile: " + zipFile + " directory:" + dirName);
    }
}