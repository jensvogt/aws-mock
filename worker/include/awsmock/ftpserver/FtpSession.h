#pragma once

// C++ includes
#include <deque>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <functional>
#include <fstream>
#include <sstream>

// Asio includes
#include "asio.hpp"

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/ftpserver/FtpMessage.h>
#include <awsmock/ftpserver/Filesystem.h>
#include <awsmock/ftpserver/UserDatabase.h>
#include <awsmock/ftpserver/FtpUser.h>

namespace AwsMock::FtpServer {
    class FtpSession : public std::enable_shared_from_this<FtpSession> {
    private:
      struct IoFile {
        IoFile(const std::string &filename, std::ios::openmode mode) : file_stream_(filename, mode), stream_buffer_(1024 * 1024) {
            file_stream_.rdbuf()->pubsetbuf(stream_buffer_.data(), static_cast<std::streamsize>(stream_buffer_.size()));
        }

        // Copy
        IoFile(const IoFile &) = delete;
        IoFile &operator=(const IoFile &) = delete;

        // Move disabled (as we are storing the shared_from_this() pointer in lambda captures)
        IoFile &operator=(IoFile &&) = delete;
        IoFile(IoFile &&) = delete;

        ~IoFile() {
            file_stream_.flush();
            file_stream_.close();
        }

        std::fstream file_stream_;
        std::vector<char> stream_buffer_;
      };

      ////////////////////////////////////////////////////////
      // Public API
      ////////////////////////////////////////////////////////
    public:
      FtpSession(asio::io_service &io_service, const UserDatabase &user_database, const std::function<void()> &completion_handler);

      // Copy (disabled, as we are inheriting from shared_from_this)
      FtpSession(const FtpSession &) = delete;
      FtpSession &operator=(const FtpSession &) = delete;

      // Move (disabled, as we are inheriting from shared_from_this)
      FtpSession &operator=(FtpSession &&) = delete;
      FtpSession(FtpSession &&) = delete;

      ~FtpSession();

      void start();

      asio::ip::tcp::socket &getSocket();

      ////////////////////////////////////////////////////////
      // FTP command-socket
      ////////////////////////////////////////////////////////
    private:
      void sendFtpMessage(const FtpMessage &message);
      void sendFtpMessage(FtpReplyCode code, const std::string &message);
      void sendRawFtpMessage(const std::string &raw_message);
      void startSendingMessages();
      void readFtpCommand();

      void handleFtpCommand(const std::string &command);

      ////////////////////////////////////////////////////////
      // FTP Commands
      ////////////////////////////////////////////////////////
    private:
      // Access control commands
      void handleFtpCommandUSER(const std::string &param);
      void handleFtpCommandPASS(const std::string &param);
      void handleFtpCommandACCT(const std::string &param);
      void handleFtpCommandCWD(const std::string &param);
      void handleFtpCommandCDUP(const std::string &param);
      void handleFtpCommandREIN(const std::string &param);
      void handleFtpCommandQUIT(const std::string &param);

      // Transfer parameter commands
      void handleFtpCommandPORT(const std::string &param);
      void handleFtpCommandPASV(const std::string &param);
      void handleFtpCommandTYPE(const std::string &param);
      void handleFtpCommandSTRU(const std::string &param);
      void handleFtpCommandMODE(const std::string &param);

      // Ftp service commands
      void handleFtpCommandRETR(const std::string &param);
      void handleFtpCommandSIZE(const std::string &param);
      void handleFtpCommandSTOR(const std::string &param);
      void handleFtpCommandSTOU(const std::string &param);
      void handleFtpCommandAPPE(const std::string &param);
      void handleFtpCommandALLO(const std::string &param);
      void handleFtpCommandREST(const std::string &param);
      void handleFtpCommandRNFR(const std::string &param);
      void handleFtpCommandRNTO(const std::string &param);
      void handleFtpCommandABOR(const std::string &param);
      void handleFtpCommandDELE(const std::string &param);
      void handleFtpCommandRMD(const std::string &param);
      void handleFtpCommandMKD(const std::string &param);
      void handleFtpCommandPWD(const std::string &param);
      void handleFtpCommandLIST(const std::string &param);
      void handleFtpCommandNLST(const std::string &param);
      void handleFtpCommandSITE(const std::string &param);
      void handleFtpCommandSYST(const std::string &param);
      void handleFtpCommandSTAT(const std::string &param);
      void handleFtpCommandHELP(const std::string &param);
      void handleFtpCommandNOOP(const std::string &param);

      // Modern FTP Commands
      void handleFtpCommandFEAT(const std::string &param);

      void handleFtpCommandOPTS(const std::string &param);

      ////////////////////////////////////////////////////////
      // FTP data-socket send
      ////////////////////////////////////////////////////////
    private:

      void sendDirectoryListing(const std::map<std::string, FileStatus> &directory_content);
      void sendNameList(const std::map<std::string, FileStatus> &directory_content);

      void sendFile(const std::shared_ptr<IoFile> &file);

      void readDataFromFileAndSend(const std::shared_ptr<IoFile> &file, const std::shared_ptr<asio::ip::tcp::socket> &data_socket);

      void addDataToBufferAndSend(const std::shared_ptr<std::vector<char>> &data,
                                  const std::shared_ptr<asio::ip::tcp::socket> &data_socket,
                                  const std::function<void(void)> &fetch_more = []() { return; });

      void writeDataToSocket(const std::shared_ptr<asio::ip::tcp::socket> &data_socket, const std::function<void(void)> &fetch_more);

      ////////////////////////////////////////////////////////
      // FTP data-socket receive
      ////////////////////////////////////////////////////////
    private:
      void receiveFile(const std::shared_ptr<IoFile> &file);

      void receiveDataFromSocketAndWriteToFile(const std::shared_ptr<IoFile> &file, const std::shared_ptr<asio::ip::tcp::socket> &data_socket);

      void writeDataToFile(const std::shared_ptr<std::vector<char>> &data,
                           const std::shared_ptr<IoFile> &file,
                           const std::function<void(void)> &fetch_more = []() { return; });

      void endDataReceiving(const std::shared_ptr<IoFile> &file);

      ////////////////////////////////////////////////////////
      // Helpers
      ////////////////////////////////////////////////////////
    private:
      std::string toAbsoluteFtpPath(const std::string &rel_or_abs_ftp_path) const;
      std::string toLocalPath(const std::string &ftp_path) const;
      static std::string createQuotedFtpPath(const std::string &unquoted_ftp_path);

      /** @brief Checks if a path is renamable
      *
      * Checks if the current user can rename the given path. A path is renameable
      * if it exists, a user is logged in and the user has sufficient permissions
      * (file / dir / both) to rename it.
      *
      * @param ftp_path: The source path
      *
      * @return (COMMAND_OK, "") if the path can be renamed or any other meaningfull error message if not.
      */
      FtpMessage checkIfPathIsRenamable(const std::string &ftp_path) const;

      FtpMessage executeCWD(const std::string &param);

      ////////////////////////////////////////////////////////
      // Member variables
      ////////////////////////////////////////////////////////
    private:

      /**
       * Logger
       */
      Core::LogStream _logger;

      // Completion handler
      const std::function<void()> completion_handler_;

      // User management
      const UserDatabase &user_database_;
      std::shared_ptr<FtpUser> logged_in_user_;

      // "Global" io service
      asio::io_service &io_service_;

      // Command Socket
      asio::ip::tcp::socket command_socket_;
      asio::io_service::strand command_write_strand_;
      asio::streambuf command_input_stream_;
      std::deque<std::string> command_output_queue_;

      std::string last_command_;
      std::string rename_from_path_;
      std::string username_for_login_;

      // Data Socket (=> passive mode)
      bool data_type_binary_;
      asio::ip::tcp::acceptor data_acceptor_;
      std::weak_ptr<asio::ip::tcp::socket> data_socket_weakptr_;
      std::deque<std::shared_ptr<std::vector<char>>> data_buffer_;
      asio::io_service::strand data_buffer_strand_;
      asio::io_service::strand file_rw_strand_;

      // Current state
      std::string ftp_working_directory_;
    };
}
