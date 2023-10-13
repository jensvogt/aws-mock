
#include "awsmock/service/TransferHandler.h"

namespace AwsMock::Service {

  TransferHandler::TransferHandler(Core::Configuration &configuration, Core::MetricService &metricService)
      : AbstractHandler(), _logger(Poco::Logger::get("TransferServiceHandler")), _configuration(configuration), _metricService(metricService), _transferService(configuration) {
  }

  void TransferHandler::handleGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, [[maybe_unused]]const std::string &user) {
    log_trace_stream(_logger) << "Lambda GET request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

    try {

      // TODO: Remove or fix
/*            std::string version, action;
            GetVersionActionFromUri(request.getURI(), version, action);

            if(action == "functions/") {

                Dto::Lambda::ListFunctionResponse lambdaResponse = _lambdaService.ListFunctions(region);
                SendOkResponse(response, lambdaResponse.ToJson());
            }*/

    } catch (Core::ServiceException &exc) {
      SendErrorResponse("S3", response, exc);
    } catch (Core::ResourceNotFoundException &exc) {
      SendErrorResponse("Lambda", response, exc);
    }
  }

  void TransferHandler::handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, [[maybe_unused]]const std::string &region, [[maybe_unused]]const std::string &user) {
    log_trace_stream(_logger) << "Lambda PUT request, URI: " << request.getURI() << " region: " << region << " user: " + user << std::endl;

    try {
      std::string version, action;
      GetVersionActionFromUri(request.getURI(), version, action);

    } catch (Poco::Exception &exc) {
      SendErrorResponse("Lambda", response, exc);
    }
  }

  void TransferHandler::handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
    log_trace_stream(_logger) << "Lambda POST request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

    try {
      std::string body = GetBodyAsString(request);

      //DumpRequest(request);

      std::string target = GetTarget(request);

      if (target == "TransferService.CreateServer") {

        Dto::Transfer::CreateTransferRequest transferRequest = {.region=region};
        transferRequest.FromJson(body);
        Dto::Transfer::CreateTransferResponse transferResponse = _transferService.CreateTransferServer(transferRequest);
        SendOkResponse(response, transferResponse.ToJson());

      } else if (target == "TransferService.CreateUser") {

        Dto::Transfer::CreateUserRequest transferRequest = {.region=region};
        transferRequest.FromJson(body);

        Dto::Transfer::CreateUserResponse transferResponse = _transferService.CreateUser(transferRequest);
        SendOkResponse(response, transferResponse.ToJson());

      } else if (target == "TransferService.ListServers") {

        Dto::Transfer::ListServerRequest transferRequest = {.region=region};
        transferRequest.FromJson(body);
        Dto::Transfer::ListServerResponse transferResponse = _transferService.ListServers(transferRequest);
        std::string tmp = transferResponse.ToJson();
        SendOkResponse(response, transferResponse.ToJson());

      } else if (target == "TransferService.StartServer") {

        Dto::Transfer::StartServerRequest transferRequest = {.region=region};
        transferRequest.FromJson(body);
        _transferService.StartServer(transferRequest);
        SendOkResponse(response);

      } else if (target == "TransferService.StopServer") {

        Dto::Transfer::StopServerRequest transferRequest = {.region=region};
        transferRequest.FromJson(body);
        _transferService.StopServer(transferRequest);
        SendOkResponse(response);

      } else if (target == "TransferService.DeleteServer") {

        Dto::Transfer::DeleteServerRequest transferRequest = {.region=region};
        transferRequest.FromJson(body);
        _transferService.DeleteServer(transferRequest);
        SendOkResponse(response);

      }

    } catch (Poco::Exception &exc) {
      SendErrorResponse("Transfer", response, exc);
    }
  }

  void TransferHandler::handleDelete(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
    log_trace_stream(_logger) << "Lambda DELETE request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

    try {
      std::string version, action;
      GetVersionActionFromUri(request.getURI(), version, action);

    } catch (Core::ServiceException &exc) {
      SendErrorResponse("Lambda", response, exc);
    }
  }

  void TransferHandler::handleOptions(Poco::Net::HTTPServerResponse &response) {
    log_trace_stream(_logger) << "Lambda OPTIONS request" << std::endl;

    response.set("Allow", "GET, PUT, POST, DELETE, OPTIONS");
    response.setContentType("text/plain; charset=utf-8");

    handleHttpStatusCode(response, 200);
    std::ostream &outputStream = response.send();
    outputStream.flush();
  }

  void TransferHandler::handleHead(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
    log_trace_stream(_logger) << "Lambda HEAD request, address: " << request.clientAddress().toString() << std::endl;

    try {

      std::string version, action;
      GetVersionActionFromUri(request.getURI(), version, action);

      HeaderMap headerMap;
      headerMap["Connection"] = "closed";
      headerMap["Server"] = "AmazonS3";

      SendOkResponse(response, {}, headerMap);

    } catch (Poco::Exception &exc) {
      SendErrorResponse("Lambda", response, exc);
    }
  }

  std::string TransferHandler::GetTarget(const Poco::Net::HTTPServerRequest &request) {
    return request.get("X-Amz-Target");
  }

  std::string TransferHandler::GetRequestType(const std::string &body) {
    if (Core::StringUtils::IsNullOrEmpty(&body)) {
      return "CreateServer";
    }
    if (Core::StringUtils::Contains(body, "Protocols")) {
      return "CreateServer";
    } else if (Core::StringUtils::Contains(body, "Role") && Core::StringUtils::Contains(body, "UserName") && Core::StringUtils::Contains(body, "ServerId")) {
      return "CreateUser";
    }
    log_warning_stream(_logger) << "Could not determine request type, body: " << body << std::endl;
    return "Unknown";
  }
}
