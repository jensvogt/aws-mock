//
// Created by vogje01 on 06/06/2023.
//

#include <awsmock/service/ModuleService.h>
#include "awsmock/repository/CognitoDatabase.h"

namespace AwsMock::Service {

  ModuleService::ModuleService(Core::Configuration &configuration, Service::ServerMap &serverMap) : _logger(Poco::Logger::get("ModuleService")), _configuration(configuration), _serverMap(serverMap), _prettyPrint(false) {

    _prettyPrint = _configuration.getBool("awsmock.pretty");
    _moduleDatabase = std::make_shared<Database::ModuleDatabase>(_configuration);
  }

  Database::Entity::Module::ModuleList ModuleService::ListModules() {

    Database::Entity::Module::ModuleList modules = _moduleDatabase->ListModules();
    log_debug_stream(_logger) << "Module list, count: " << modules.size() << std::endl;
    return modules;
  }

  bool ModuleService::IsRunning(const std::string &moduleName) {

    Database::Entity::Module::Module module = _moduleDatabase->GetModuleByName(moduleName);
    log_debug_stream(_logger) << "Module state, state: " << Database::Entity::Module::ModuleStateToString(module.state) << std::endl;
    return module.state == Database::Entity::Module::ModuleState::RUNNING;
  }

  Database::Entity::Module::Module ModuleService::StartService(const std::string &name) {

    // Set state
    Database::Entity::Module::Module module = _moduleDatabase->GetModuleByName(name);
    if (module.state == Database::Entity::Module::ModuleState::RUNNING) {

      log_info_stream(_logger) << "Module " + name + " already running" << std::endl;
      throw Core::ServiceException("Module " + name + " already running", Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);

    } else {

      // Set state
      _moduleDatabase->SetState(name, Database::Entity::Module::ModuleState::RUNNING);

      if (module.name == "database") {
        _moduleDatabase->StartDatabase();
      } else if (module.name == "s3") {
        auto *s3server = (Service::S3Server *) _serverMap[module.name];
        Poco::ThreadPool::defaultPool().start(*s3server);
      } else if (module.name == "sqs") {
        auto *sqsServer = (Service::SQSServer *) _serverMap[module.name];
        Poco::ThreadPool::defaultPool().start(*sqsServer);
      } else if (module.name == "sns") {
        auto *snsServer = (Service::SNSServer *) _serverMap[module.name];
        Poco::ThreadPool::defaultPool().start(*snsServer);
      } else if (module.name == "lambda") {
        auto *lambdaServer = (Service::LambdaServer *) _serverMap[module.name];
        Poco::ThreadPool::defaultPool().start(*lambdaServer);
      } else if (module.name == "transfer") {
        auto *transferServer = (Service::TransferServer *) _serverMap[module.name];
        Poco::ThreadPool::defaultPool().start(*transferServer);
      } else if (module.name == "cognito") {
        auto *cognitoServer = (Service::CognitoServer *) _serverMap[module.name];
        Poco::ThreadPool::defaultPool().start(*cognitoServer);
      } else if (module.name == "dynamodb") {
        auto *dynamoDbServer = (Service::DynamoDbServer *) _serverMap[module.name];
        Poco::ThreadPool::defaultPool().start(*dynamoDbServer);
      } else if (module.name == "gateway") {
        auto *gatewayServer = (Service::GatewayServer *) _serverMap[module.name];
        Poco::ThreadPool::defaultPool().start(*gatewayServer);
      }
      log_info_stream(_logger) << "Module " + name + " started" << std::endl;
    }
    return module;
  }

  void ModuleService::StartAllServices() {

    Database::Entity::Module::ModuleList modules = _moduleDatabase->ListModules();
    for (const auto &module : modules) {
      StartService(module.name);
    }
  }

  Database::Entity::Module::Module ModuleService::RestartService(const std::string &name) {

    // Set state
    Database::Entity::Module::Module module = _moduleDatabase->GetModuleByName(name);
    if (module.state == Database::Entity::Module::ModuleState::RUNNING) {

      StopService(name);
      log_info_stream(_logger) << "Module " + name + " already running" << std::endl;
      throw Core::ServiceException("Module " + name + " already running", Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);

    }

    log_info_stream(_logger) << "Module " + name + " restarted" << std::endl;
    return StartService(name);
  }

  void ModuleService::RestartAllServices() {

    Database::Entity::Module::ModuleList modules = _moduleDatabase->ListModules();
    for (const auto &module : modules) {
      RestartService(module.name);
    }
  }

  Database::Entity::Module::Module ModuleService::StopService(const std::string &name) {

    // Set state
    Database::Entity::Module::Module module = _moduleDatabase->GetModuleByName(name);
    if (module.state != Database::Entity::Module::ModuleState::RUNNING) {

      throw Core::ServiceException("Module " + name + " not running", Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);

    } else {

      // Set state
      _moduleDatabase->SetState(name, Database::Entity::Module::ModuleState::STOPPED);

      // Stop module
      if (name == "database") {
        _moduleDatabase->StopDatabase();
      } else {
        for (const auto &server : _serverMap) {
          if (name == server.first) {
            server.second->StopServer();
            log_info_stream(_logger) << "Module " << name << " stopped" << std::endl;
          }
        }
      }
    }
    return module;
  }

  void ModuleService::StopAllServices() {

    Database::Entity::Module::ModuleList modules = _moduleDatabase->ListModules();
    for (const auto &module : modules) {
      StopService(module.name);
    }
  }

  std::string ModuleService::ExportInfrastructure(const Dto::Common::Services &services, bool prettyPrint) {

    Dto::Common::Infrastructure infrastructure;

    if (services.HasService("all") || services.HasService("s3")) {
      std::shared_ptr<Database::S3Database> _s3Database = std::make_shared<Database::S3Database>(_configuration);
      infrastructure.s3Buckets = _s3Database->ListBuckets();
      infrastructure.s3Objects = _s3Database->ListObjects();
    }
    if (services.HasService("all") || services.HasService("sqs")) {
      std::shared_ptr<Database::SQSDatabase> _sqsDatabase = std::make_shared<Database::SQSDatabase>(_configuration);
      infrastructure.sqsQueues = _sqsDatabase->ListQueues();
      infrastructure.sqsMessages = _sqsDatabase->ListMessages();
    }
    if (services.HasService("all") || services.HasService("sns")) {
      std::shared_ptr<Database::SNSDatabase> _snsDatabase = std::make_shared<Database::SNSDatabase>(_configuration);
      infrastructure.snsTopics = _snsDatabase->ListTopics();
      infrastructure.snsMessages = _snsDatabase->ListMessages();
    }
    if (services.HasService("all") || services.HasService("lambda")) {
      std::shared_ptr<Database::LambdaDatabase> _lambdaDatabase = std::make_shared<Database::LambdaDatabase>(_configuration);
      infrastructure.lambdas = _lambdaDatabase->ListLambdas();
    }
    if (services.HasService("all") || services.HasService("cognito")) {
      std::shared_ptr<Database::CognitoDatabase> _cognitoDatabase = std::make_shared<Database::CognitoDatabase>(_configuration);
      infrastructure.cognitoUserPools = _cognitoDatabase->ListUserPools();
      infrastructure.cognitoUsers = _cognitoDatabase->ListUsers();
    }
    if (services.HasService("all") || services.HasService("dynamodb")) {
      std::shared_ptr<Database::DynamoDbDatabase> _dynamoDbDatabase = std::make_shared<Database::DynamoDbDatabase>(_configuration);
      infrastructure.dynamoDbTables = _dynamoDbDatabase->ListTables();
      infrastructure.dynamoDbItems = _dynamoDbDatabase->ListItems();
    }
    if (services.HasService("all") || services.HasService("transfer")) {
      std::shared_ptr<Database::TransferDatabase> _transferDatabase = std::make_shared<Database::TransferDatabase>(_configuration);
      infrastructure.transferServers = _transferDatabase->ListServers();
    }
    return infrastructure.ToJson(prettyPrint);
  }

  void ModuleService::ImportInfrastructure(const std::string &jsonString) {
    log_info_stream(_logger) << "Importing services, length: " << jsonString.length() << std::endl;

    Dto::Common::Infrastructure infrastructure;
    infrastructure.FromJson(jsonString);

    if (!infrastructure.s3Buckets.empty() || !infrastructure.s3Objects.empty()) {
      std::shared_ptr<Database::S3Database> _s3Database = std::make_shared<Database::S3Database>(_configuration);
      if (!infrastructure.s3Buckets.empty()) {
        for (const auto &bucket : infrastructure.s3Buckets) {
          _s3Database->CreateOrUpdateBucket(bucket);
        }
        log_info_stream(_logger) << "S3 buckets imported, count: " << infrastructure.s3Buckets.size() << std::endl;
      }
      if (!infrastructure.s3Objects.empty()) {
        for (const auto &object : infrastructure.s3Objects) {
          _s3Database->CreateOrUpdateObject(object);
        }
        log_info_stream(_logger) << "S3 objects imported, count: " << infrastructure.s3Objects.size() << std::endl;
      }
    }
    if (!infrastructure.sqsQueues.empty() || !infrastructure.sqsMessages.empty()) {
      std::shared_ptr<Database::SQSDatabase> _sqsDatabase = std::make_shared<Database::SQSDatabase>(_configuration);
      if (!infrastructure.sqsQueues.empty()) {
        for (auto &queue : infrastructure.sqsQueues) {
          _sqsDatabase->CreateOrUpdateQueue(queue);
        }
        log_info_stream(_logger) << "SQS queues imported, count: " << infrastructure.sqsQueues.size() << std::endl;
      }
      if (!infrastructure.sqsMessages.empty()) {
        for (auto &message : infrastructure.sqsMessages) {
          _sqsDatabase->CreateOrUpdateMessage(message);
        }
        log_info_stream(_logger) << "SQS messages imported, count: " << infrastructure.sqsMessages.size() << std::endl;
      }
    }
    if (!infrastructure.snsTopics.empty() || !infrastructure.snsMessages.empty()) {
      std::shared_ptr<Database::SNSDatabase> _snsDatabase = std::make_shared<Database::SNSDatabase>(_configuration);
      if (!infrastructure.snsTopics.empty()) {
        for (auto &topic : infrastructure.snsTopics) {
          _snsDatabase->CreateOrUpdateTopic(topic);
        }
        log_info_stream(_logger) << "SNS topics imported, count: " << infrastructure.snsTopics.size() << std::endl;
      }
      if (!infrastructure.snsMessages.empty()) {
        for (auto &message : infrastructure.snsMessages) {
          _snsDatabase->CreateOrUpdateMessage(message);
        }
        log_info_stream(_logger) << "SNS messages imported, count: " << infrastructure.snsMessages.size() << std::endl;
      }
    }
    if (!infrastructure.lambdas.empty()) {
      std::shared_ptr<Database::LambdaDatabase> _lambdaDatabase = std::make_shared<Database::LambdaDatabase>(_configuration);
      for (auto &lambda : infrastructure.lambdas) {
        _lambdaDatabase->CreateOrUpdateLambda(lambda);
      }
      log_info_stream(_logger) << "Lambda functions imported, count: " << infrastructure.lambdas.size() << std::endl;
    }
    if (!infrastructure.transferServers.empty()) {
      std::shared_ptr<Database::TransferDatabase> _transferDatabase = std::make_shared<Database::TransferDatabase>(_configuration);
      for (auto &transfer : infrastructure.transferServers) {
        _transferDatabase->CreateOrUpdateTransfer(transfer);
      }
      log_info_stream(_logger) << "Transfer servers imported, count: " << infrastructure.transferServers.size() << std::endl;
    }

    // Cognito
    if (!infrastructure.cognitoUserPools.empty() || !infrastructure.cognitoUsers.empty()) {
      std::shared_ptr<Database::CognitoDatabase> _cognitoDatabase = std::make_shared<Database::CognitoDatabase>(_configuration);
      if (!infrastructure.cognitoUserPools.empty()) {
        for (auto &userPool : infrastructure.cognitoUserPools) {
          _cognitoDatabase->CreateOrUpdateUserPool(userPool);
        }
        log_info_stream(_logger) << "Cognito user pools imported, count: " << infrastructure.cognitoUserPools.size() << std::endl;
      }
      if (!infrastructure.cognitoUsers.empty()) {
        for (auto &user : infrastructure.cognitoUsers) {
          _cognitoDatabase->CreateOrUpdateUser(user);
        }
        log_info_stream(_logger) << "Cognito users imported, count: " << infrastructure.cognitoUsers.size() << std::endl;
      }
    }

    // DynamoDB
    if (!infrastructure.dynamoDbTables.empty() || !infrastructure.dynamoDbItems.empty()) {
      std::shared_ptr<Database::DynamoDbDatabase> _dynamoDatabase = std::make_shared<Database::DynamoDbDatabase>(_configuration);
      if (!infrastructure.dynamoDbTables.empty()) {
        for (auto &table : infrastructure.dynamoDbTables) {
          _dynamoDatabase->CreateOrUpdateTable(table);
        }
        log_info_stream(_logger) << "DynamoDb tables imported, count: " << infrastructure.dynamoDbTables.size() << std::endl;
      }
      if (!infrastructure.dynamoDbItems.empty()) {
        for (auto &item : infrastructure.dynamoDbItems) {
          //_dynamoDatabase->CreateOrUpdateUser(user);
        }
        log_info_stream(_logger) << "DynamoDb items imported, count: " << infrastructure.dynamoDbItems.size() << std::endl;
      }
    }
  }

  void ModuleService::CleanInfrastructure(const Dto::Common::Services &services) {
    log_info_stream(_logger) << "Cleaning services, length: " << services.serviceNames.size() << std::endl;

    if (services.HasService("all") || services.HasService("s3")) {
      std::shared_ptr<Database::S3Database> _s3Database = std::make_shared<Database::S3Database>(_configuration);
      _s3Database->DeleteAllObjects();
      _s3Database->DeleteAllBuckets();
    }
    if (services.HasService("all") || services.HasService("sqs")) {
      std::shared_ptr<Database::SQSDatabase> _sqsDatabase = std::make_shared<Database::SQSDatabase>(_configuration);
      _sqsDatabase->DeleteAllMessages();
      _sqsDatabase->DeleteAllQueues();
    }
    if (services.HasService("all") || services.HasService("sns")) {
      std::shared_ptr<Database::SNSDatabase> _snsDatabase = std::make_shared<Database::SNSDatabase>(_configuration);
      _snsDatabase->DeleteAllMessages();
      _snsDatabase->DeleteAllTopics();
    }
    if (services.HasService("all") || services.HasService("lambda")) {
      std::shared_ptr<Database::LambdaDatabase> _lambdaDatabase = std::make_shared<Database::LambdaDatabase>(_configuration);
      _lambdaDatabase->DeleteAllLambdas();
    }
    if (services.HasService("all") || services.HasService("cognito")) {
      std::shared_ptr<Database::CognitoDatabase> _cognitoDatabase = std::make_shared<Database::CognitoDatabase>(_configuration);
      _cognitoDatabase->DeleteAllUsers();
      _cognitoDatabase->DeleteAllUserPools();
    }
    if (services.HasService("all") || services.HasService("dynamodb")) {
      std::shared_ptr<Database::DynamoDbDatabase> _dynamoDbDatabase = std::make_shared<Database::DynamoDbDatabase>(_configuration);
      _dynamoDbDatabase->DeleteAllItems();
      _dynamoDbDatabase->DeleteAllTables();
    }
    if (services.HasService("all") || services.HasService("transfer")) {
      std::shared_ptr<Database::TransferDatabase> _transferDatabase = std::make_shared<Database::TransferDatabase>(_configuration);
      _transferDatabase->DeleteAllTransfers();
    }
  }
}
