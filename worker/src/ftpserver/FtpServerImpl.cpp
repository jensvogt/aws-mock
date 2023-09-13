#include "awsmock/ftpserver/FtpServerImpl.h"

#include "awsmock/ftpserver/FtpSession.h"

#include <memory>
#include <iostream>

namespace AwsMock::FtpServer {

    FtpServerImpl::FtpServerImpl(const std::string &address, uint16_t port)
        : port_(port), address_(address), acceptor_(io_service_), open_connection_count_(0), _logger(Poco::Logger::get("FtpServerImpl")) {}

    FtpServerImpl::~FtpServerImpl() {
        stop();
    }

    bool FtpServerImpl::addUser(const std::string &username, const std::string &password, const std::string &local_root_path, const Permission permissions) {
        return ftp_users_.addUser(username, password, local_root_path, permissions);
    }

    bool FtpServerImpl::addUserAnonymous(const std::string &local_root_path, const Permission permissions) {
        return ftp_users_.addUser("anonymous", "", local_root_path, permissions);
    }

    bool FtpServerImpl::start(size_t thread_count) {
        auto ftp_session = std::make_shared<FtpSession>(io_service_, ftp_users_, [this]() { open_connection_count_--; });

        // set up the acceptor to listen on the tcp port
        asio::error_code make_address_ec;
        const asio::ip::tcp::endpoint endpoint(asio::ip::make_address(address_, make_address_ec), port_);
        if (make_address_ec) {
            log_error_stream(_logger) << "Error creating address from string \"" << address_ << "\": " << make_address_ec.message() << std::endl;
            return false;
        }

        {
            asio::error_code ec;
            acceptor_.open(endpoint.protocol(), ec);
            if (ec) {
                log_error_stream(_logger) << "Error opening acceptor: " << ec.message() << std::endl;
                return false;
            }
        }

        {
            asio::error_code ec;
            acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true), ec);
            if (ec) {
                log_error_stream(_logger) << "Error setting reuse_address option: " << ec.message() << std::endl;
                return false;
            }
        }

        {
            asio::error_code ec;
            acceptor_.bind(endpoint, ec);
            if (ec) {
                log_error_stream(_logger) << "Error binding acceptor: " << ec.message() << std::endl;
                return false;
            }
        }

        {
            asio::error_code ec;
            acceptor_.listen(asio::socket_base::max_listen_connections, ec);
            if (ec) {
                log_error_stream(_logger) << "Error listening on acceptor: " << ec.message() << std::endl;
                return false;
            }
        }

        log_debug_stream(_logger) << "FTP Server created." << std::endl << "Listening at address " << acceptor_.local_endpoint().address() << " on port "
                                  << acceptor_.local_endpoint().port() << ":" << std::endl;

        acceptor_.async_accept(ftp_session->getSocket(), [this, ftp_session](auto ec) {
          open_connection_count_++;

          acceptFtpSession(ftp_session, ec);
        });

        for (size_t i = 0; i < thread_count; i++) {
            thread_pool_.emplace_back([this] { io_service_.run(); });
        }

        return true;
    }

    void FtpServerImpl::stop() {
        io_service_.stop();
        for (std::thread &thread : thread_pool_) {
            thread.join();
        }
        thread_pool_.clear();
    }

    void FtpServerImpl::acceptFtpSession(const std::shared_ptr<FtpSession> &ftp_session, asio::error_code const &error) {
        if (error) {
            log_error_stream(_logger) << "Error handling connection: " << error.message() << std::endl;
            return;
        }
        log_debug_stream(_logger) << "FTP Client connected: " << ftp_session->getSocket().remote_endpoint().address().to_string() << ":"
                                  << ftp_session->getSocket().remote_endpoint().port() << std::endl;

        ftp_session->start();

        auto new_session = std::make_shared<FtpSession>(io_service_, ftp_users_, [this]() { open_connection_count_--; });

        acceptor_.async_accept(new_session->getSocket(), [this, new_session](auto ec) {
          open_connection_count_++;
          acceptFtpSession(new_session, ec);
        });
    }

    int FtpServerImpl::getOpenConnectionCount() {
        return open_connection_count_;
    }

    uint16_t FtpServerImpl::getPort() {
        return acceptor_.local_endpoint().port();
    }

    std::string FtpServerImpl::getAddress() {
        return acceptor_.local_endpoint().address().to_string();
    }
}
