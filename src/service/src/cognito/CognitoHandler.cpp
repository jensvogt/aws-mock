
#include <awsmock/service/cognito/CognitoHandler.h>

namespace AwsMock::Service {

    http::response<http::dynamic_body> CognitoHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_debug << "Cognito POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::CognitoClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {

            std::string action = GetActionFromHeader(request);

            if (action == "CreateUserPool") {

                Dto::Cognito::CreateUserPoolRequest cognitoRequest{};
                cognitoRequest.FromJson(clientCommand.payload);
                cognitoRequest.region = clientCommand.region;
                cognitoRequest.requestId = clientCommand.requestId;
                cognitoRequest.user = clientCommand.user;

                log_debug << "Got create user pool request, json: " << cognitoRequest.ToString();

                Dto::Cognito::CreateUserPoolResponse serviceResponse = _cognitoService.CreateUserPool(cognitoRequest);
                return SendOkResponse(request, serviceResponse.ToJson());

            } else if (action == "CreateUserPoolDomain") {

                Dto::Cognito::CreateUserPoolDomainRequest cognitoRequest{};
                cognitoRequest.FromJson(clientCommand.payload);
                cognitoRequest.region = clientCommand.region;
                cognitoRequest.requestId = clientCommand.requestId;
                cognitoRequest.user = clientCommand.user;

                log_debug << "Got create user pool domain request, json: " << cognitoRequest.ToString();

                Dto::Cognito::CreateUserPoolDomainResponse serviceResponse = _cognitoService.CreateUserPoolDomain(cognitoRequest);
                return SendOkResponse(request, serviceResponse.ToJson());

            } else if (action == "ListUserPools") {

                Dto::Cognito::ListUserPoolRequest cognitoRequest{};
                cognitoRequest.FromJson(clientCommand.payload);
                cognitoRequest.region = clientCommand.region;
                cognitoRequest.requestId = clientCommand.requestId;
                cognitoRequest.user = clientCommand.user;

                log_debug << "Got list user pool request, json: " << cognitoRequest.ToString();

                Dto::Cognito::ListUserPoolResponse serviceResponse = _cognitoService.ListUserPools(cognitoRequest);
                return SendOkResponse(request, serviceResponse.ToJson());

            } else if (action == "DeleteUserPool") {

                Dto::Cognito::DeleteUserPoolRequest cognitoRequest{};
                cognitoRequest.FromJson(clientCommand.payload);
                cognitoRequest.region = clientCommand.region;
                cognitoRequest.requestId = clientCommand.requestId;
                cognitoRequest.user = clientCommand.user;
                log_debug << "Got delete user pool request, json: " << cognitoRequest.ToString();

                _cognitoService.DeleteUserPool(cognitoRequest);
                return SendOkResponse(request);

            } else if (action == "AdminCreateUser") {

                Dto::Cognito::AdminCreateUserRequest cognitoRequest{};
                cognitoRequest.FromJson(clientCommand.payload);
                cognitoRequest.region = clientCommand.region;
                cognitoRequest.requestId = clientCommand.requestId;
                cognitoRequest.user = clientCommand.user;
                log_debug << "Got admin create user request, json: " << cognitoRequest.ToString();

                Dto::Cognito::AdminCreateUserResponse cognitoResponse = _cognitoService.AdminCreateUser(cognitoRequest);
                return SendOkResponse(request, cognitoResponse.ToJson());

            } else if (action == "ListUsers") {

                Dto::Cognito::ListUsersRequest cognitoRequest{};
                cognitoRequest.FromJson(clientCommand.payload);
                cognitoRequest.region = clientCommand.region;
                cognitoRequest.requestId = clientCommand.requestId;
                cognitoRequest.user = clientCommand.user;
                log_debug << "Got list users request: " << cognitoRequest.ToString();

                Dto::Cognito::ListUsersResponse cognitoResponse = _cognitoService.ListUsers(cognitoRequest);
                return SendOkResponse(request, cognitoResponse.ToJson());

            } else if (action == "AdminDeleteUser") {

                Dto::Cognito::AdminDeleteUserRequest cognitoRequest{};
                cognitoRequest.FromJson(clientCommand.payload);
                cognitoRequest.region = clientCommand.region;
                cognitoRequest.requestId = clientCommand.requestId;
                cognitoRequest.user = clientCommand.user;
                log_debug << "Got admin delete user request, json: " << cognitoRequest.ToString();

                _cognitoService.AdminDeleteUser(cognitoRequest);
                return SendOkResponse(request);

            } else {

                return SendBadRequestError(request, "Unknown function");
            }

        } catch (Poco::Exception &exc) {
            return SendInternalServerError(request, exc.message());
        }
    }

    std::string CognitoHandler::GetActionFromHeader(const http::request<http::dynamic_body> &request) {

        if (!Core::HttpUtils::HasHeader(request, "X-Amz-Target")) {
            log_error << "Could not extract action";
            throw Core::BadRequestException("Could not extract action");
        }

        std::string headerValue = request["X-Amz-Target"];
        return Core::StringUtils::Split(headerValue, '.')[1];
    }

}// namespace AwsMock::Service
