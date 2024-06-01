//
// Created by vogje01 on 5/28/24.
//

#include <awsmock/core/HttpSocket.h>

namespace AwsMock::Core {

    HttpSocketResponse HttpSocket::SendJson(http::verb method, const std::string &host, int port, const std::string &path, const std::string &body, const std::map<std::string, std::string> &headers) {

        boost::system::error_code ec;
        boost::asio::io_context ctx;

        boost::asio::ip::tcp::resolver resolver(ctx);
        boost::beast::tcp_stream stream(ctx);

        // Resolve host/port
        auto const results = resolver.resolve(host, std::to_string(port));

        // Connect
        stream.connect(results, ec);
        if (ec) {
            log_error << "Connect to " << host << ":" << port << " failed, error: " << ec.message();
            return {.statusCode = http::status::internal_server_error, .body = ec.message()};
        }

        // Prepare message
        http::request<http::string_body> request = PrepareJsonMessage(method, host, path, body, headers);

        // Write to TCP socket
        http::write(stream, request, ec);
        if (ec) {
            log_error << "Send to " << host << ":" << port << " failed, error: " << ec.message();
            return {.statusCode = http::status::internal_server_error, .body = ec.message()};
        }

        boost::beast::flat_buffer buffer;
        http::response<http::string_body> response;

        http::read(stream, buffer, response);
        if (ec) {
            log_error << "Read from " << host << ":" << port << " failed, error: " << ec.message();
            return {.statusCode = http::status::internal_server_error, .body = ec.message()};
        }

        // Cleanup
        ec = stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        if (ec) {
            log_error << "Shutdown socket failed, error: " << ec.message();
            return {.statusCode = http::status::internal_server_error, .body = ec.message()};
        }
        return PrepareResult(response);
    }

    HttpSocketResponse HttpSocket::SendBinary(http::verb method, const std::string &host, int port, const std::string &path, const std::string &filename, const std::map<std::string, std::string> &headers) {

        boost::system::error_code ec;
        boost::asio::io_context ctx;

        boost::asio::ip::tcp::resolver resolver(ctx);
        boost::beast::tcp_stream stream(ctx);

        // Prepare message
        http::request<http::file_body> request = PrepareBinaryMessage(method, path, filename, headers);

        // Write to socket
        boost::beast::http::write(stream, request);
        if (ec) {
            log_error << "Send to " << host << ":" << port << " failed, error: " << ec.message();
            return {.statusCode = http::status::internal_server_error, .body = ec.message()};
        }

        boost::beast::flat_buffer buffer;
        http::response<http::dynamic_body> response;
        read(stream, buffer, response, ec);
        if (ec) {
            log_error << "Read from " << host << ":" << port << " failed, error: " << ec.message();
            return {.statusCode = http::status::internal_server_error, .body = ec.message()};
        }

        ec = stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        if (ec) {
            log_error << "Shutdown socket failed, error: " << ec.message();
            return {.statusCode = http::status::internal_server_error, .body = ec.message()};
        }
        return PrepareResult(response);
    }

    http::request<http::string_body> HttpSocket::PrepareJsonMessage(http::verb method, const std::string &host, const std::string &path, const std::string &body, const std::map<std::string, std::string> &headers) {

        http::request<http::string_body> request;

        request.method(method);
        request.set(http::field::host, host);
        request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        request.target(path);
        request.body() = body;
        request.prepare_payload();
        request.base().set("Host", "localhost");

        if (!headers.empty()) {
            for (const auto &header: headers) {
                request.base().set(header.first, header.second);
            }
        }
        return request;
    }

    http::request<http::file_body> HttpSocket::PrepareBinaryMessage(http::verb method, const std::string &path, const std::string &filename, const std::map<std::string, std::string> &headers) {

        boost::system::error_code ec;
        http::request<http::file_body> request;
        request.method(method);
        request.target(path);
        request.base().set("Host", "localhost");
        request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        request.body().open(filename.c_str(), boost::beast::file_mode::scan, ec);
        request.prepare_payload();

        if (!headers.empty()) {
            for (const auto &header: headers) {
                request.base().set(header.first, header.second);
            }
        }
        return request;
    }

    HttpSocketResponse HttpSocket::PrepareResult(http::response<http::string_body> response) {
        return {.statusCode = response.result(), .body = response.body()};
    }

    HttpSocketResponse HttpSocket::PrepareResult(http::response<http::dynamic_body> response) {
        boost::beast::net::streambuf sb;
        sb.commit(boost::beast::net::buffer_copy(sb.prepare(response.body().size()), response.body().cdata()));

        return {.statusCode = response.result(), .body = boost::beast::buffers_to_string(sb.data())};
    }

}// namespace AwsMock::Core
