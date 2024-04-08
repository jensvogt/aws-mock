
#include <awsmock/service/secretsmanager/SecretsManagerCmdHandler.h>

namespace AwsMock::Service {

  SecretsManagerCmdHandler::SecretsManagerCmdHandler(Core::Configuration &configuration, Core::MetricService &metricService, Poco::Condition &condition)
    : AbstractHandler(), _logger(Poco::Logger::get("SecretsManagerCmdHandler")), _configuration(configuration), _metricService(metricService), _secretsManagerService(configuration, condition) {
  }

  void SecretsManagerCmdHandler::handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const Dto::Common::SecretsManagerClientCommand &secretsManagerClientCommand) {
    log_debug_stream(_logger) << "SQS POST request, URI: " << request.getURI() << " region: " << secretsManagerClientCommand.region << " user: " << secretsManagerClientCommand.user << " command: "
                              << Dto::Common::SecretsManagerCommandTypeToString(secretsManagerClientCommand.command) << std::endl;

    std::string requestId = GetHeaderValue(request, "RequestId", Poco::UUIDGenerator().createRandom().toString());

    switch (secretsManagerClientCommand.command) {

      case Dto::Common::SecretsManagerCommandType::CREATE_SECRET: {

        Dto::SecretsManager::CreateSecretRequest secretsManagerRequest = {.region=secretsManagerClientCommand.region, .requestId=requestId};
        secretsManagerRequest.FromJson(secretsManagerClientCommand.payload);

        Dto::SecretsManager::CreateSecretResponse secretsManagerResponse = _secretsManagerService.CreateSecret(secretsManagerRequest);
        SendOkResponse(response, secretsManagerResponse.ToJson());

        break;
      }

      case Dto::Common::SecretsManagerCommandType::DELETE_SECRET:{

        break;
      }

      case Dto::Common::SecretsManagerCommandType::UNKNOWN: {
        log_error_stream(_logger) << "Bad request, method: POST clientCommand: " << Dto::Common::SecretsManagerCommandTypeToString(secretsManagerClientCommand.command) << std::endl;
        throw Core::ServiceException("Bad request, method: POST clientCommand: " + Dto::Common::SecretsManagerCommandTypeToString(secretsManagerClientCommand.command));
      }
    }
  }
}
