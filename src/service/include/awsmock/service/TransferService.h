//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_TRANSFERSERVICE_H
#define AWSMOCK_SERVICE_TRANSFERSERVICE_H

// C++ standard includes
#include <string>
#include <sstream>

// Poco includes
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>
#include <Poco/RecursiveDirectoryIterator.h>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/ServiceException.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/SystemUtils.h>
#include <awsmock/core/TarUtils.h>
#include <awsmock/dto/transfer/CreateServerRequest.h>
#include <awsmock/dto/transfer/CreateServerResponse.h>
#include <awsmock/dto/transfer/CreateUserRequest.h>
#include <awsmock/dto/transfer/CreateUserResponse.h>
#include <awsmock/dto/transfer/DeleteServerRequest.h>
#include <awsmock/dto/transfer/ListServerRequest.h>
#include <awsmock/dto/transfer/ListServerResponse.h>
#include <awsmock/dto/transfer/StartServerRequest.h>
#include <awsmock/dto/transfer/StopServerRequest.h>
#include <awsmock/repository/TransferDatabase.h>

#define TRANSFER_DEFAULT_FTP_PORT 2121

namespace AwsMock::Service {

  class TransferService {

  public:

    /**
     * Constructor
     *
     * @param configuration module configuration
     */
    explicit TransferService(Core::Configuration &configuration);

    /**
     * Create transfer server request
     *
     * @param request create server request
     * @return CreateServerResponse
     */
    Dto::Transfer::CreateServerResponse CreateTransferServer(Dto::Transfer::CreateServerRequest &request);

    /**
     * Create a user for the transfer manager.
     *
     * @param request create user request
     * @return CreateUserResponse
     */
    Dto::Transfer::CreateUserResponse CreateUser(Dto::Transfer::CreateUserRequest &request);

    /**
     * Returns a list of available servers
     *
     * @param request list manager request
     * @return ListServerResponse
     */
    Dto::Transfer::ListServerResponse ListServers(const Dto::Transfer::ListServerRequest &request);

    /**
     * Starts an manager.
     *
     * @param request StartServer manager request
     */
    void StartServer(const Dto::Transfer::StartServerRequest &request);

    /**
     * Stops an manager.
     *
     * @param request stop manager request
     */
    void StopServer(const Dto::Transfer::StopServerRequest &request);

    /**
     * Deleted an manager.
     *
     * @param request delete manager request
     */
    void DeleteServer(const Dto::Transfer::DeleteServerRequest &request);

  private:

    /**
     * Logger
     */
    Core::LogStream _logger;

    /**
     * AWS region
     */
    std::string _region;

    /**
     * AWS account ID
     */
    std::string _accountId;

    /**
     * Configuration
     */
    Core::Configuration &_configuration;

    /**
     * Transfer database connection
     */
    Database::TransferDatabase& _transferDatabase;

    /**
     * FTP port
     */
    int _ftpPort;
  };

} //namespace AwsMock::Service

#endif //AWSMOCK_SERVICE_TRANSFERSERVICE_H
