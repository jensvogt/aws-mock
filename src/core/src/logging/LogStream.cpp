
#include "awsmock/core/LogStream.h"
#include <iostream>
#include <utility>

namespace AwsMock::Core {

    LogStream::LogStream() {
        plog::init<plog::TxtFormatter>(plog::verbose, plog::streamStdOut);
    }

    LogStream::LogStream(const std::string &severity) {
        plog::init<plog::TxtFormatter>(plog::severityFromString(severity.c_str()), plog::streamStdOut);
    }

    void LogStream::SetLevel(const std::string &severity) {
        plog::get()->setMaxSeverity(plog::severityFromString(severity.c_str()));
    }

    void LogStream::SetLogfile(const std::string &filename) {
        static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(filename.c_str(), 10 * 1024 * 1024, 5);
        plog::get()->addAppender(&fileAppender);
    }

}// namespace AwsMock::Core
