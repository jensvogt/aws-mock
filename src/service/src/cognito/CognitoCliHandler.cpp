
#include "awsmock/service/cognito/CognitoCliHandler.h"

namespace AwsMock::Service {

  CognitoCliHandler::CognitoCliHandler(Core::Configuration &configuration, Core::MetricService &metricService) : AbstractHandler(), _configuration(configuration), _metricService(metricService),_cognitoService(configuration) {
  }

  void CognitoCliHandler::handleGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
    log_debug << "Cognito GET request, URI: " + request.getURI() << " region: " << region << " user: " + user;
  }

  void CognitoCliHandler::handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
    log_debug << "Cognito POST request, URI: " << request.getURI() << " region: " << region << " user: " << user;
  }

  void CognitoCliHandler::handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
    log_debug << "Cognito POST request, URI: " << request.getURI() << " region: " << region << " user: " << user;

    try {

      Dto::Common::UserAgent userAgent;
      userAgent.FromRequest(request, "cognito-idp");

      if (userAgent.clientCommand == "create-user-pool") {

        std::string payload = Core::HttpUtils::GetBodyAsString(request);
        Dto::Cognito::CreateUserPoolRequest cognitoRequest{};
        cognitoRequest.FromJson(payload);
        cognitoRequest.region = region;

        log_debug << "Got list user pool request, json: " << cognitoRequest.ToString();

        Dto::Cognito::CreateUserPoolResponse serviceResponse = _cognitoService.CreateUserPool(cognitoRequest);
        SendOkResponse(response, serviceResponse.ToJson());

      } else if (userAgent.clientCommand == "list-user-pools") {

        std::string payload = Core::HttpUtils::GetBodyAsString(request);
        Dto::Cognito::ListUserPoolRequest cognitoRequest{};
        cognitoRequest.FromJson(payload);
        cognitoRequest.region = region;
        log_debug << "Got list user pool request, json: " << cognitoRequest.ToString();

        Dto::Cognito::ListUserPoolResponse serviceResponse = _cognitoService.ListUserPools(cognitoRequest);
        SendOkResponse(response, serviceResponse.ToJson());

      } else if (userAgent.clientCommand == "delete-user-pool") {

        std::string payload = Core::HttpUtils::GetBodyAsString(request);
        Dto::Cognito::DeleteUserPoolRequest cognitoRequest{};
        cognitoRequest.FromJson(payload);
        cognitoRequest.region = region;
        log_debug << "Got delete user pool request, json: " << cognitoRequest.ToString();

        _cognitoService.DeleteUserPool(cognitoRequest);
        SendOkResponse(response);

      } else if (userAgent.clientCommand == "admin-create-user") {

        std::string payload = Core::HttpUtils::GetBodyAsString(request);
        Dto::Cognito::AdminCreateUserRequest cognitoRequest{};
        cognitoRequest.FromJson(payload);
        cognitoRequest.region = region;
        log_debug << "Got admin create user request, json: " << cognitoRequest.ToString();

        Dto::Cognito::AdminCreateUserResponse cognitoResponse = _cognitoService.AdminCreateUser(cognitoRequest);
        SendOkResponse(response, cognitoResponse.ToJson());

      } else if (userAgent.clientCommand == "list-users") {

        std::string payload = Core::HttpUtils::GetBodyAsString(request);
        Dto::Cognito::ListUsersRequest cognitoRequest{};
        cognitoRequest.FromJson(payload);
        cognitoRequest.region = region;
        log_debug << "Got list users request, json: " << cognitoRequest.ToString();

        Dto::Cognito::ListUsersResponse cognitoResponse = _cognitoService.ListUsers(cognitoRequest);
        SendOkResponse(response, cognitoResponse.ToJson());

      } else if (userAgent.clientCommand == "admin-delete-user") {

        std::string payload = Core::HttpUtils::GetBodyAsString(request);
        Dto::Cognito::AdminDeleteUserRequest cognitoRequest{};
        cognitoRequest.FromJson(payload);
        cognitoRequest.region = region;
        log_debug << "Got admin delete user request, json: " << cognitoRequest.ToString();

        _cognitoService.AdminDeleteUser(cognitoRequest);
        SendOkResponse(response);
      }

    } catch (Poco::Exception &exc) {
      SendXmlErrorResponse("Cognito", response, exc);
    }
  }

  void CognitoCliHandler::handleDelete(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
    log_debug << "Cognito DELETE request, URI: " + request.getURI() << " region: " << region << " user: " << user;
  }

  void CognitoCliHandler::handleHead(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
    log_debug << "Cognito HEAD request, URI: " << request.getURI() << " region: " << region << " user: " << user;
  }

  void CognitoCliHandler::handleOptions(Poco::Net::HTTPServerResponse &response) {
    log_debug << "Cognito OPTIONS request";

    response.set("Allow", "GET, PUT, POST, DELETE, HEAD, OPTIONS");
    response.setContentType("text/plain; charset=utf-8");

    handleHttpStatusCode(response, 200);
    std::ostream &outputStream = response.send();
    outputStream.flush();
  }
}
