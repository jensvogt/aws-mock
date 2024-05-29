//
// Created by vogje01 on 5/27/24.
//

#include <awsmock/service/gateway/GatewaySession.h>

namespace AwsMock::Service {

    GatewaySession::RoutingTable GatewaySession::_routingTable = {
            {"s3", std::make_shared<S3Handler>()},
            {"s3api", std::make_shared<S3Handler>()},
            {"sqs", std::make_shared<SQSHandler>()},
            {"sns", std::make_shared<SNSHandler>()},
            {"lambda", std::make_shared<LambdaHandler>()},
            {"transfer", std::make_shared<TransferHandler>()},
            {"cognito-idp", std::make_shared<CognitoHandler>()},
            {"cognito-identity", std::make_shared<CognitoHandler>()},
            {"secretsmanager", std::make_shared<SecretsManagerHandler>()},
            {"kms", std::make_shared<KMSHandler>()},
            {"dynamodb", std::make_shared<DynamoDbHandler>()}};

    /*    _routingTable["sqs"] = {._name = "sqs", ._handlerType = HandlerType::SQS, ._handler = new SQSHandler(configuration)};
    _routingTable["sns"] = {._name = "sns", ._handlerType = HandlerType::SNS, ._handler = new SNSHandler(configuration)};
    _routingTable["lambda"] = {._name = "lambda", ._handlerType = HandlerType::LAMBDA, ._handler = new LambdaHandler(configuration)};
    _routingTable["transfer"] = {._name = "transfer", ._handlerType = HandlerType::TRANSFER, ._handler = new TransferHandler(configuration)};
    _routingTable["secretsmanager"] = {._name = "secretsmanager", ._handlerType = HandlerType::SECRETS_MANAGER, ._handler = new SecretsManagerHandler(configuration)};
    _routingTable["kms"] = {._name = "kms", ._handlerType = HandlerType::KMS, ._handler = new KMSHandler(configuration)};
*/
    GatewaySession::GatewaySession(ip::tcp::socket &&socket) : stream_(std::move(socket)) {
        static_assert(queue_limit > 0, "queue limit must be positive");
    };

    void GatewaySession::Run() {
        boost::asio::dispatch(stream_.get_executor(), boost::beast::bind_front_handler(&GatewaySession::DoRead, this->shared_from_this()));
    }

    void GatewaySession::DoRead() {
        // Construct a new parser for each message
        parser_.emplace();

        // Apply a reasonable limit to the allowed size
        // of the body in bytes to prevent abuse.
        parser_->body_limit(10000);

        // Set the timeout.
        stream_.expires_after(std::chrono::seconds(30));

        // Read a request using the parser-oriented interface
        http::async_read(stream_, buffer_, *parser_, boost::beast::bind_front_handler(&GatewaySession::OnRead, this->shared_from_this()));
    }

    void GatewaySession::OnRead(boost::beast::error_code ec, std::size_t bytes_transferred) {

        boost::ignore_unused(bytes_transferred);

        // This means they closed the connection
        if (ec == http::error::end_of_stream)
            return DoClose();

        if (ec) {
            log_error << ec.message();
            return;
        }

        // Send the response
        QueueWrite(HandleRequest(parser_->release()));

        // If we aren't at the queue limit, try to pipeline another request
        if (response_queue_.size() < queue_limit)
            DoRead();
    }

    void GatewaySession::QueueWrite(http::message_generator response) {

        // Allocate and store the work
        response_queue_.push(std::move(response));

        // If there was no previous work, start the write loop
        if (response_queue_.size() == 1)
            DoWrite();
    }

    // Return a response for the given request.
    //
    // The concrete type of the response message (which depends on the
    // request), is type-erased in message_generator.
    template<class Body, class Allocator>
    http::message_generator GatewaySession::HandleRequest(http::request<Body, http::basic_fields<Allocator>> &&req) {

        // Returns a bad request response
        auto const bad_request =
                [&req](boost::beast::string_view why) {
                    http::response<http::string_body> res{http::status::bad_request, req.version()};
                    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                    res.set(http::field::content_type, "text/html");
                    res.keep_alive(req.keep_alive());
                    res.body() = std::string(why);
                    res.prepare_payload();
                    return res;
                };

        // Returns a bad request response
        auto const unauthorized =
                [&req](boost::beast::string_view why) {
                    http::response<http::string_body> res{http::status::unauthorized, req.version()};
                    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                    res.set(http::field::content_type, "text/html");
                    res.keep_alive(req.keep_alive());
                    res.body() = std::string(why);
                    res.prepare_payload();
                    return res;
                };
        // Returns a bad request response
        auto const notimplemented =
                [&req](boost::beast::string_view why) {
                    http::response<http::string_body> res{http::status::not_implemented, req.version()};
                    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                    res.set(http::field::content_type, "text/html");
                    res.keep_alive(req.keep_alive());
                    res.body() = std::string(why);
                    res.prepare_payload();
                    return res;
                };

        // Make sure we can handle the method
        if (req.method() != http::verb::get && req.method() != http::verb::put &&
            req.method() != http::verb::post && req.method() != http::verb::delete_ &&
            req.method() != http::verb::head) {
            return bad_request("Unknown HTTP-method");
        }

        // Request path must be absolute and not contain "..".
        if (req.target().empty() || req.target()[0] != '/' || req.target().find("..") != boost::beast::string_view::npos) {
            log_error << "Illegal request-target";
            return bad_request("Illegal request-target");
        }

        /*if (!Core::AwsUtils::VerifySignature(request, req.body(), "none")) {
            log_error << "AWS signature could not be verified";
            throw Core::UnauthorizedException("AWS signature could not be verified");
        }*/

        Core::AuthorizationHeaderKeys authKey = GetAuthorizationKeys(req["Authorization"], {});

        std::shared_ptr<AbstractHandler> handler = _routingTable[authKey.module];
        if (handler) {

            switch (req.method()) {
                case http::verb::get:
                    log_debug << "Handle GET request";
                    return handler->HandleGetRequest(req, authKey.region, "none");
                    break;
                case http::verb::put:
                    log_debug << "Handle PUT request";
                    return handler->HandlePutRequest(req, authKey.region, "none");
                    break;
                case http::verb::post:
                    log_debug << "Handle POST request";
                    return handler->HandlePostRequest(req, authKey.region, "none");
                    break;
            }
        }
        return notimplemented("Not yet implemented");
    }

    // Called to start/continue the write-loop. Should not be called when
    // write_loop is already active.
    void GatewaySession::DoWrite() {
        if (!response_queue_.empty()) {
            bool keep_alive = response_queue_.front().keep_alive();
            boost::beast::async_write(stream_, std::move(response_queue_.front()), boost::beast::bind_front_handler(&GatewaySession::OnWrite, shared_from_this(), keep_alive));
        }
    }

    void GatewaySession::OnWrite(bool keep_alive, boost::beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if (ec) {
            log_error << ec.message();
            return;
        }

        if (!keep_alive) {
            // This means we should close the connection, usually because the response indicated the "Connection: close" semantic.
            return DoClose();
        }

        // Resume the read if it has been paused
        if (response_queue_.size() == queue_limit)
            DoRead();

        response_queue_.pop();

        DoWrite();
    }

    void GatewaySession::DoClose() {

        // Send a TCP shutdown
        boost::beast::error_code ec;
        stream_.socket().shutdown(ip::tcp::socket::shutdown_send, ec);

        // At this point the connection is closed gracefully
    }

    Core::AuthorizationHeaderKeys GatewaySession::GetAuthorizationKeys(const std::string &authorizationHeader, const std::string &secretAccessKey) {

        // Get signing version
        Core::AuthorizationHeaderKeys authKeys;
        authKeys.signingVersion = Core::StringUtils::Split(authorizationHeader, ' ')[0];

        try {
            Poco::RegularExpression::MatchVec posVec;
            Poco::RegularExpression pattern(R"(Credential=([a-zA-Z0-9]+)\/([0-9]{8})\/([a-zA-Z0-9\-]+)\/([a-zA-Z0-9\-]+)\/(aws4_request),\ ?SignedHeaders=(.*),\ ?Signature=(.*)$)");
            if (!pattern.match(authorizationHeader, 0, posVec)) {
                log_error << "Could not extract authorization, authorization: " << authorizationHeader;
                throw Core::UnauthorizedException("Could not extract authorization, authorization: " + authorizationHeader);
            }
            authKeys.secretAccessKey = secretAccessKey.empty() ? "none" : secretAccessKey;
            authKeys.dateTime = authorizationHeader.substr(posVec[2].offset, posVec[2].length);
            authKeys.region = authorizationHeader.substr(posVec[3].offset, posVec[3].length);
            authKeys.module = authorizationHeader.substr(posVec[4].offset, posVec[4].length);
            authKeys.requestVersion = authorizationHeader.substr(posVec[5].offset, posVec[5].length);
            authKeys.signedHeaders = authorizationHeader.substr(posVec[6].offset, posVec[6].length);
            authKeys.signature = authorizationHeader.substr(posVec[7].offset, posVec[7].length);
            authKeys.scope = authKeys.dateTime + "/" + authKeys.region + "/" + authKeys.module + "/" + authKeys.requestVersion;
            //authKeys.isoDateTime = request.has("x-amz-date") ? request.get("x-amz-date") : GetISODateString();
            return authKeys;

        } catch (Poco::Exception &e) {
            log_error << e.message();
        }
        return {};
    }
}// namespace AwsMock::Service