
#include "awsmock/core/LogStream.h"

namespace AwsMock::Core {

    //
    // LogStreamBuf
    //
    LogStreamBuf::LogStreamBuf(Poco::Logger &logger, Poco::Message::Priority priority, std::size_t bufferCapacity) :
        _logger(logger),
        _priority(priority) {
        _message.reserve(bufferCapacity);
    }

    LogStreamBuf::~LogStreamBuf() {
    }

    void LogStreamBuf::setPriority(Poco::Message::Priority priority) {
        _priority = priority;
    }

    void LogStreamBuf::setFile(const char* file) {
        _file = file;
    }

    void LogStreamBuf::setLine(int line) {
        _line = line;
    }

    void LogStreamBuf::reserve(std::size_t capacity) {
        _message.reserve(capacity);
    }

    int LogStreamBuf::writeToDevice(char c) {
        if (c == '\n' || c == '\r') {
            if (_message.find_first_not_of("\r\n") != std::string::npos) {
                Poco::Message msg(_logger.name(), _message, _priority, _file, _line);
                _message.clear();
                _logger.log(msg);
            }
        } else
            _message += c;
        return c;
    }

    //
    // LogIOS
    //
    LogIOS::LogIOS(Poco::Logger &logger, Poco::Message::Priority priority, std::size_t bufferCapacity) :
        _buf(logger, priority, bufferCapacity) {
        poco_ios_init(&_buf);
    }

    LogIOS::~LogIOS() {
    }

    LogStreamBuf *LogIOS::rdbuf() {
        return &_buf;
    }

    //
    // LogStream
    //
    LogStream::LogStream(Poco::Logger &logger, Poco::Message::Priority priority, std::size_t bufferCapacity) :
        LogIOS(logger, priority, bufferCapacity),
        std::ostream(&_buf) {
    }

    LogStream::LogStream(const std::string &loggerName, Poco::Message::Priority priority, std::size_t bufferCapacity) :
        LogIOS(Poco::Logger::get(loggerName), priority, bufferCapacity),
        std::ostream(&_buf) {
    }

    LogStream::~LogStream() {
    }

    LogStream &LogStream::fatal() {
        return priority(Poco::Message::PRIO_FATAL);
    }

    LogStream &LogStream::fatal(const char* file, int line) {
        _buf.setFile(file);
        _buf.setLine(line);
        return priority(Poco::Message::PRIO_FATAL);
    }

    LogStream &LogStream::fatal(const std::string &message) {
        _buf.logger().fatal(message);
        return priority(Poco::Message::PRIO_FATAL);
    }

    LogStream &LogStream::critical() {
        return priority(Poco::Message::PRIO_CRITICAL);
    }

    LogStream &LogStream::critical(const char* file, int line) {
        _buf.setFile(file);
        _buf.setLine(line);
        return priority(Poco::Message::PRIO_CRITICAL);
    }

    LogStream &LogStream::critical(const std::string &message) {
        _buf.logger().critical(message);
        return priority(Poco::Message::PRIO_CRITICAL);
    }

    LogStream &LogStream::error() {
        return priority(Poco::Message::PRIO_ERROR);
    }

    LogStream &LogStream::error(const char* file, int line) {
        _buf.setFile(file);
        _buf.setLine(line);
        return priority(Poco::Message::PRIO_ERROR);
    }

    LogStream &LogStream::error(const std::string &message) {
        _buf.logger().error(message);
        return priority(Poco::Message::PRIO_ERROR);
    }

    LogStream &LogStream::warning() {
        return priority(Poco::Message::PRIO_WARNING);
    }

    LogStream &LogStream::warning(const char* file, int line) {
        _buf.setFile(file);
        _buf.setLine(line);
        return priority(Poco::Message::PRIO_WARNING);
    }

    LogStream &LogStream::warning(const std::string &message) {
        _buf.logger().warning(message);
        return priority(Poco::Message::PRIO_WARNING);
    }

    LogStream &LogStream::notice() {
        return priority(Poco::Message::PRIO_NOTICE);
    }

    LogStream &LogStream::notice(const std::string &message) {
        _buf.logger().notice(message);
        return priority(Poco::Message::PRIO_NOTICE);
    }

    LogStream &LogStream::notice(const char* file, int line) {
        _buf.setFile(file);
        _buf.setLine(line);
        return priority(Poco::Message::PRIO_NOTICE);
    }

    LogStream &LogStream::information() {
        return priority(Poco::Message::PRIO_INFORMATION);
    }

    LogStream &LogStream::information(const char* file, int line) {
        _buf.setFile(file);
        _buf.setLine(line);
        return priority(Poco::Message::PRIO_INFORMATION);
    }

    LogStream &LogStream::information(const std::string &message) {
        _buf.logger().information(message);
        return priority(Poco::Message::PRIO_INFORMATION);
    }

    LogStream &LogStream::debug() {
        return priority(Poco::Message::PRIO_DEBUG);
    }

    LogStream &LogStream::debug(const char* file, int line) {
        _buf.setFile(file);
        _buf.setLine(line);
        return priority(Poco::Message::PRIO_DEBUG);
    }

    LogStream &LogStream::debug(const std::string &message) {
        _buf.logger().debug(message);
        return priority(Poco::Message::PRIO_DEBUG);
    }

    LogStream &LogStream::trace() {
        return priority(Poco::Message::PRIO_TRACE);
    }

    LogStream &LogStream::trace(const char* file, int line) {
        _buf.setFile(file);
        _buf.setLine(line);
        return priority(Poco::Message::PRIO_TRACE);
    }

    LogStream &LogStream::trace(const std::string &message) {
        _buf.logger().trace(message);
        return priority(Poco::Message::PRIO_TRACE);
    }

    LogStream &LogStream::priority(Poco::Message::Priority priority) {
        _buf.setPriority(priority);
        return *this;
    }

    LogStream &LogStream::level(const std::string &level) {
        _buf.logger().setLevel(level);
        return *this;
    }

} // namespace Poco
