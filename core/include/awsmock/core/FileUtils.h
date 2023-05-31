//
// Created by vogje01 on 22/08/2022.
//

#ifndef LIBRI_COM_FILEUTILS_H
#define LIBRI_COM_FILEUTILS_H

// Standard C++ includes
#include <string>
#include <cstdio>
#include <fstream>
#include <sstream>

// Poco includes
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/String.h"
#include "Poco/Logger.h"
#include "Poco/BinaryReader.h"
#include "Poco/BinaryWriter.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/Zip/Decompress.h"
#include "Poco/Zip/Compress.h"
#include "Poco/DirectoryIterator.h"
#include "Poco/RegularExpression.h"

// Libri includes
#include "awsmock/core/StringUtils.h"

namespace AwsMock::Core {

    /**
     * File utilities.
     *
     * @author jens.vogt@opitz-consulting.com
     */
    class FileUtils {

    public:
      /**
       * Extracts the base name (without extension) from the given file name.
       */
      static std::string GetBasename(const std::string &fileName);

      /**
       * Returns the file extension.
       *
       * @param fileName name fo the file
       * @return file extension in lower case letters
       */
      static std::string GetExtension(const std::string &fileName);

      /**
       * Check whether the file has the given pattern
       *
       * @param fileName file name to check
       * @param pattern file name pattern
       * @return true in case the file matches the pattern
       */
      static bool FileNameContainsString(const std::string &fileName, const std::string &pattern);

      /**
       * Returns a thread safe temp file name.
       *
       * @param extension desired file extension
       * @return thread safe temp file name
       */
      static std::string GetTempFile(const std::string &extension);

      /**
       * Returns a thread safe temp file name.
       *
       * @param dir directory to create the file
       * @param extension desired file extension
       * @return thread safe temp file name
       */
      static std::string GetTempFile(const std::string &dir, const std::string &extension);

      /**
       * Creates an temporary file containing an random string
       *
       * @param extension file name extension
       * @return file name
       */
      static std::string CreateTempFile(const std::string &extension);

      /**
       * Creates an temporary file containing an specific content string
       *
       * @param extension file name extension
       * @param content content of the file
       * @return file name
       */
      static std::string CreateTempFile(const std::string &extension, const std::string &content);

      /**
       * Creates an temporary file containing a numBytes long random string
       *
       * @param extension file name extension
       * @param numBytes length of the file
       * @return file name
       */
      static std::string CreateTempFile(const std::string &extension, int numBytes);

      /**
       * Creates an temporary file containing a numBytes long random string in the given directory.
       *
       * @param dirName name of the directory
       * @param extension file name extension
       * @param numBytes length of the file
       * @return file name
       */
      static std::string CreateTempFile(const std::string &dirName, const std::string &extension, int numBytes);

      /**
       * Check whether a file exists.
       *
       * @param fileName name of the file.
       * @return true, if file exists otherwise false.
       */
      static bool FileExists(const std::string &fileName);

      /**
       * Returns the size of the given in bytes.
       *
       * @param fileName name of the file.
       * @return size of the in bytes.
       */
      static unsigned long FileSize(const std::string &fileName);

      /**
       * Append several binary files to a single output file.
       *
       * <p>The out file will be truncated, before its used </p>
       *
       * @param outName output file name
       * @param inDir input directory
       * @param files vector of binary file sto append to output file
       */
      static void AppendBinaryFiles(const std::string &outFile, const std::string &inDir, const std::vector<std::string> &files);

      /**
       * Strips the path from the filename
       *
       * @param fileName name of the file
       * @return file name without base path
       */
      static std::string StripBasePath(const std::string &fileName);

      /**
       * Delete the file with the given file name
       *
       * @param fileName name of the file to delete
       */
      static void DeleteFile(const std::string &fileName);

      /**
       * Unzip the given file to the given directory.
       *
       * @param zipFile zip file to decompress.
       * @param dirName output directory.
       */
      static void unzipFiles(const std::string &zipFile, const std::string &dirName);

      /**
       * Unzip the given file to the given directory.
       *
       * @param zipFile zip file to decompress.
       * @param dirName directory to zip.
       */
      static void zipFiles(const std::string &zipFile, const std::string &dirName);
    };
} // namespace Libri::COM
#endif // LIBRI_COM_FILEUTILS_H
