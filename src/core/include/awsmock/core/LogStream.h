//
// Created by vogje01 on 20/06/2023.
//

#ifndef AWSMOCK_CORE_LOGSTREAM_H
#define AWSMOCK_CORE_LOGSTREAM_H

// C++ standard includes
#include <istream>
#include <iostream>

// Poco includes
#include <Poco/Foundation.h>
#include <Poco/Logger.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/FormattingChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/UnbufferedStreamBuf.h>
#include <Poco/DateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/FileChannel.h>

#define LOG_PATTERN "%d-%m-%Y %H:%M:%S.%i [%q] %I %s:%u - %t"

namespace AwsMock::Core {

  class LogStreamBuf : public Poco::UnbufferedStreamBuf
    /// This class implements a streambuf interface
    /// to a Logger.
    ///
    /// The streambuf appends all characters written to it
    /// to a string. As soon as a CR or LF (std::endl) is written,
    /// the string is sent to the Logger, with the set
    /// priority.
  {
  public:
    LogStreamBuf(Poco::Logger &logger, Poco::Message::Priority priority, std::size_t bufferCapacity = 0);
    /// Creates the LogStream.

    ~LogStreamBuf() override;
    /// Destroys the LogStream.

    void setPriority(Poco::Message::Priority priority);
    /// Sets the priority for log messages.

    [[nodiscard]] Poco::Message::Priority getPriority() const;
    /// Returns the priority for log messages.

    void setChannel(const Poco::Channel::Ptr &channel);
    /// Sets the log channel

    void setFile(const char *file);
    /// Sets the file for log messages.

    void setLine(int line);
    /// Sets the line for log messages.

    [[nodiscard]] Poco::Logger &logger() const;
    /// Returns a reference to the Logger.

    [[nodiscard]] std::size_t capacity() const;
    /// Returns the internal message buffer capacity.

    void reserve(std::size_t capacity);
    /// Sets the capacity of the internal message buffer to the given size.

  private:

    /**
     * Write to underlying device
     *
     * @param c character to write
     * @return character
     */
    int writeToDevice(char c) override;

    Poco::Mutex _mutex;
    Poco::Logger &_logger;
    Poco::Message::Priority _priority;
    std::string _message;
    const char *_file;
    int _line = 0;
  };

  class Foundation_API LogIOS : public virtual std::ios
    /// The base class for LogStream.
    ///
    /// This class is needed to ensure the correct initialization
    /// order of the stream buffer and base classes.
  {
  public:
    LogIOS(Poco::Logger &logger, Poco::Message::Priority priority, std::size_t bufferCapacity = 0);
    ~LogIOS() override;
    LogStreamBuf *rdbuf();

  protected:
    LogStreamBuf _buf;
  };

  class Foundation_API LogStream : public LogIOS, public std::ostream
    /// This class implements an ostream interface
    /// to a Logger.
    ///
    /// The stream's buffer appends all characters written to it
    /// to a string. As soon as a CR or LF (std::endl) is written,
    /// the string is sent to the Logger, with the current
    /// priority.
    ///
    /// Usage example:
    ///     LogStream ls(someLogger);
    ///     ls << "Some informational message" << std::endl;
    ///     ls.error() << "Some error message" << std::endl;
  {
  public:
    static const std::size_t DEFAULT_BUFFER_CAPACITY = 9092;

    explicit LogStream(Poco::Logger &logger, Poco::Message::Priority priority = Poco::Message::PRIO_INFORMATION, std::size_t bufferCapacity = DEFAULT_BUFFER_CAPACITY);
    /// Creates the LogStream, using the given logger and priority.

    ~LogStream() override;
    /// Destroys the LogStream.

    /**
     * Priority strings
     */
    static const std::string priorities[];

    void setChannel(const Poco::Channel::Ptr& channel);
    /// Sets the log channel

    void setFileChannel(const std::string &filename);
    /// Sets the file channel for the root logger

    void setConsoleChannel();
    /// Sets the standard console log channel for root logger

    LogStream &level(const std::string &level);
    /// Sets the logging level

    LogStream &fatal();
    /// Sets the priority for log messages to Message::PRIO_FATAL.

    LogStream &fatal(const char *file, int line);
    /// Sets the priority for log messages to Message::PRIO_FATAL and file and line attributes.

    LogStream &fatal(const std::string &message);
    /// Sets the priority for log messages to Message::PRIO_FATAL
    /// and writes the given message.

    LogStream &critical();
    /// Sets the priority for log messages to Message::PRIO_CRITICAL.

    LogStream &critical(const char *file, int line);
    /// Sets the priority for log messages to Message::PRIO_CRITICAL and file and line attributes.

    LogStream &critical(const std::string &message);
    /// Sets the priority for log messages to Message::PRIO_CRITICAL
    /// and writes the given message.

    LogStream &error();
    /// Sets the priority for log messages to Message::PRIO_ERROR.

    LogStream &error(const char *file, int line);
    /// Sets the priority for log messages to Message::PRIO_ERROR and file and line userAttributes.

    LogStream &error(const std::string &message);
    /// Sets the priority for log messages to Message::PRIO_ERROR
    /// and writes the given message.

    LogStream &warning();
    /// Sets the priority for log messages to Message::PRIO_WARNING.

    LogStream &warning(const char *file, int line);
    /// Sets the priority for log messages to Message::PRIO_WARNING and file and line attributes.

    LogStream &warning(const std::string &message);
    /// Sets the priority for log messages to Message::PRIO_WARNING
    /// and writes the given message.

    LogStream &notice();
    /// Sets the priority for log messages to Message::PRIO_NOTICE.

    LogStream &notice(const char *file, int line);
    /// Sets the priority for log messages to Message::PRIO_NOTICE and file and line attributes.

    LogStream &notice(const std::string &message);
    /// Sets the priority for log messages to Message::PRIO_NOTICE
    /// and writes the given message.

    LogStream &information();
    /// Sets the priority for log messages to Message::PRIO_INFORMATION.

    LogStream &information(const char *file, int line);
    /// Sets the priority for log messages to Message::PRIO_INFORMATION and file and line attributes.

    LogStream &information(const std::string &message);
    /// Sets the priority for log messages to Message::PRIO_INFORMATION
    /// and writes the given message.

    LogStream &debug();
    /// Sets the priority for log messages to Message::PRIO_DEBUG.

    LogStream &debug(const char *file, int line);
    /// Sets the priority for log messages to Message::PRIO_DEBUG and file and line attributes.

    LogStream &debug(const std::string &message);
    /// Sets the priority for log messages to Message::PRIO_DEBUG
    /// and writes the given message.

    LogStream &trace();
    /// Sets the priority for log messages to Message::PRIO_TRACE.

    LogStream &trace(const char *file, int line);
    /// Sets the priority for log messages to Message::PRIO_TRACE and file and line attributes.

    LogStream &trace(const std::string &message);
    /// Sets the priority for log messages to Message::PRIO_TRACE
    /// and writes the given message.

    LogStream &priority(Poco::Message::Priority priority);
    /// Sets the priority for log messages.

    static void SetGlobalLevel(const std::string &level);

    Poco::AutoPtr<Poco::ConsoleChannel> _pConsoleChannel;
    Poco::AutoPtr<Poco::FileChannel> _pFileChannel;
    Poco::AutoPtr<Poco::PatternFormatter> _pFormatter;
    Poco::AutoPtr<Poco::FormattingChannel> _pFormattingChannel;
  };

  //
  // inlines
  //
  inline std::size_t LogStreamBuf::capacity() const {
    return _message.capacity();
  }

  inline Poco::Message::Priority LogStreamBuf::getPriority() const {
    return _priority;
  }

  inline Poco::Logger &LogStreamBuf::logger() const {
    return _logger;
  }

} // namespace Poco

#define log_fatal_stream(logger) \
    if ((logger).fatal()) (logger).fatal( __FILE__, __LINE__)
#define log_critical_stream(logger) \
    if ((logger).critical()) (logger).critical( __FILE__, __LINE__)
#define log_error_stream(logger) \
    if ((logger).error()) (logger).error( __FILE__, __LINE__)
#define log_warning_stream(logger) \
    if ((logger).warning()) (logger).warning( __FILE__, __LINE__)
#define log_notice_stream(logger) \
    if ((logger).notice()) (logger).notice( __FILE__, __LINE__)
#define log_info_stream(logger) \
    if ((logger).information()) (logger).information( __FILE__, __LINE__)
#define log_debug_stream(logger) \
    if ((logger).debug()) (logger).debug( __FILE__, __LINE__)
#define log_trace_stream(logger) \
    if ((logger).trace()) (logger).trace( __FILE__, __LINE__)

#endif // AWSMOCK_CORE_LOGSTREAM_H
