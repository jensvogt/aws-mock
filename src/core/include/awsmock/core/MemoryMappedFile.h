//
// Created by vogje01 on 11/2/23.
//

#ifndef AWS_MOCK_CORE_MEMORY_MAPPED_FILE_H
#define AWS_MOCK_CORE_MEMORY_MAPPED_FILE_H

// C includes
#include <fcntl.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <cerrno>
#include <sys/mman.h>
#include <unistd.h>
#endif

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/LogStream.h>

namespace AwsMock::Core {

    /**
     * Memory mapped file utility.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class MemoryMappedFile {

      public:

        /**
         * Constructor
         */
        MemoryMappedFile() : _start(nullptr), _membuffer(nullptr), _mapped(false) {}

        /**
         * Singleton
         *
         * @return singleton instance
         */
        static MemoryMappedFile &instance() {
            static MemoryMappedFile memoryMappedFile;
            return memoryMappedFile;
        }

        /**
         * Opens a file and prepares the memory map
         *
         * @param filename name of the file.
         * @return true on success
         */
        bool OpenFile(const std::string &filename);

        /**
         * Release all resources
         */
        void CloseFile();

        /**
         * Copy a chunk of data from the memory mapped file the provided output buffer
         *
         * @param start start index
         * @param end end index
         * @param buffer char buffer
         * @return number of bytes actually read
         */
        long ReadChunk(long start, long end, char *buffer);

        /**
         * Returns true in case the file is mapped already.
         *
         * @return true, in case file is already mapped.
         */
        [[nodiscard]] bool IsMapped() const { return _mapped; }
#ifdef _WIN32
        /// tweak performance
        enum CacheHint {
            Normal,        ///< good overall performance
            SequentialScan,///< read file only once with few seeks
            RandomAccess   ///< jump around
        };
#endif

      private:

#ifdef _WIN32
        typedef void *FileHandle;
        /// Windows handle to memory mapping of _file
        void *_mappedFile;
#else
        typedef int FileHandle;
#endif

        /**
         * Start pointer
         */
        void *_start;

        /**
         * Char buffer
         */
        char *_membuffer;

        /**
         * Mapped flag
         */
        bool _mapped;

        /**
         * Start pointer
        */
        std::string _filename;

        /**
         * File handle
         */
        FileHandle _file{};

        /**
         * File size
         */
        long _fileSize = 0;
    };

}// namespace AwsMock::Core

#endif// AWS_MOCK_CORE_MEMORY_MAPPED_FILE_H
