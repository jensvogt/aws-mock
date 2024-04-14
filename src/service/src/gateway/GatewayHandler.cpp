
#include <awsmock/service/gateway/GatewayHandler.h>

#include <utility>

namespace AwsMock::Service {

  template<class F, class REQ, class RES, class REG, class USR>
  void CallAsyncGet(F &&fun, REQ &&request, RES &&response, REG &&region, USR &&user) {
    std::make_unique<std::future<void>>((std::async(std::launch::async, [&fun, &request, &response, &region, &user]() {
      fun.handleGet(request, response, region, user);
    }))).reset();
  }

  template<class F, class REQ, class RES, class REG, class USR>
  void CallAsyncPut(F &&fun, REQ &&request, RES &&response, REG &&region, USR &&user) {
    std::make_unique<std::future<void>>((std::async(std::launch::async, [&fun, &request, &response, &region, &user]() {
      fun.handlePut(request, response, region, user);
    }))).reset();
  }

  template<class F, class REQ, class RES, class REG, class USR>
  void CallAsyncPost(F &&fun, REQ &&request, RES &&response, REG &&region, USR &&user) {
    std::make_unique<std::future<void>>((std::async(std::launch::async, [&fun, &request, &response, &region, &user]() {
      fun.handlePost(request, response, region, user);
    }))).reset();
  }

  template<class F, class REQ, class RES, class REG, class USR>
  void CallAsyncDelete(F &&fun, REQ &&request, RES &&response, REG &&region, USR &&user) {
    std::make_unique<std::future<void>>((std::async(std::launch::async, [&fun, &request, &response, &region, &user]() {
      fun.handleDelete(request, response, region, user);
    }))).reset();
  }

  template<class F, class REQ, class RES, class REG, class USR>
  void CallAsyncHead(F &&fun, REQ &&request, RES &&response, REG &&region, USR &&user) {
    std::make_unique<std::future<void>>((std::async(std::launch::async, [&fun, &request, &response, &region, &user]() {
      fun.handleHead(request, response, region, user);
    }))).reset();
  }

  GatewayHandler::GatewayHandler(Core::Configuration &configuration, Core::MetricService &metricService, Service::GatewayRoute route)
    : AbstractHandler(), _logger(Poco::Logger::get("GatewayHandler")), _configuration(configuration), _metricService(metricService), _route(std::move(route)) {
  }

  void GatewayHandler::handleGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
    log_trace_stream(_logger) << "Gateway GET request, URI: " + request.getURI() << " region: " << region << " user: " + user << std::endl;

    Core::MetricServiceTimer measure(_metricService, GATEWAY_GET_TIMER);
    _metricService.IncrementCounter(GATEWAY_COUNTER, "method", "GET");
    log_debug_stream(_logger) << "Gateway GET request, URI: " + request.getURI() << " region: " << region << " user: " + user << std::endl;

    SetHeaders(request, region, user);
    switch (_route._handlerType) {

    case S3: {
      CallAsyncGet(S3Handler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case SQS: {
      CallAsyncGet(SQSHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case SNS: {
      CallAsyncGet(SNSHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case LAMBDA: {
      CallAsyncGet(LambdaHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case TRANSFER: {
      CallAsyncGet(TransferHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case COGNITO: {
      CallAsyncGet(CognitoHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case DYNAMODB: {
      CallAsyncGet(DynamoDbHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case SECRETS_MANAGER: {
      CallAsyncGet(SecretsManagerHandler(_configuration, _metricService), request, response, region, user);
      break;
    }
    }
  }

  void GatewayHandler::handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
    log_trace_stream(_logger) << "Gateway PUT request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

    Core::MetricServiceTimer measure(_metricService, GATEWAY_PUT_TIMER);
    _metricService.IncrementCounter(GATEWAY_COUNTER, "method", "PUT");
    log_debug_stream(_logger) << "Gateway PUT request, URI: " + request.getURI() << " region: " << region << " user: " + user << std::endl;

    SetHeaders(request, region, user);
    switch (_route._handlerType) {

    case S3: {
      CallAsyncPut(S3Handler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case SQS: {
      CallAsyncPut(SQSHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case SNS: {
      CallAsyncPut(SNSHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case LAMBDA: {
      CallAsyncPut(LambdaHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case TRANSFER: {
      CallAsyncPut(TransferHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case COGNITO: {
      CallAsyncPut(CognitoHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case DYNAMODB: {
      CallAsyncPut(DynamoDbHandler(_configuration, _metricService), request, response, region, user);
      break;
    }
    case SECRETS_MANAGER: {
      CallAsyncPut(SecretsManagerHandler(_configuration, _metricService), request, response, region, user);
      break;
    }
    }
  }

  void GatewayHandler::handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
    log_trace_stream(_logger) << "Gateway POST request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

    Core::MetricServiceTimer measure(_metricService, GATEWAY_POST_TIMER);
    _metricService.IncrementCounter(GATEWAY_COUNTER, "method", "POST");
    log_debug_stream(_logger) << "Gateway POST request, URI: " + request.getURI() << " region: " << region << " user: " + user << std::endl;

    SetHeaders(request, region, user);
    switch (_route._handlerType) {

    case S3: {
      CallAsyncPost(S3Handler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case SQS: {
      CallAsyncPost(SQSHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case SNS: {
      CallAsyncPost(SNSHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case LAMBDA: {
      CallAsyncPost(LambdaHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case TRANSFER: {
      CallAsyncPost(TransferHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case COGNITO: {
      CallAsyncPost(CognitoHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case DYNAMODB: {
      CallAsyncPost(DynamoDbHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case SECRETS_MANAGER: {
      CallAsyncPost(SecretsManagerHandler(_configuration, _metricService), request, response, region, user);
      break;
    }
    }
  }

  void GatewayHandler::handleDelete(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
    log_debug_stream(_logger) << "Gateway DELETE request, URI: " + request.getURI() << " region: " << region << " user: " << user << std::endl;

    Core::MetricServiceTimer measure(_metricService, GATEWAY_DELETE_TIMER);
    _metricService.IncrementCounter(GATEWAY_COUNTER, "method", "DELETE");
    log_debug_stream(_logger) << "Gateway DELETE request, URI: " + request.getURI() << " region: " << region << " user: " + user << std::endl;

    SetHeaders(request, region, user);
    switch (_route._handlerType) {

    case S3: {
      CallAsyncDelete(S3Handler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case SQS: {
      CallAsyncDelete(SQSHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case SNS: {
      CallAsyncDelete(SNSHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case LAMBDA: {
      CallAsyncDelete(LambdaHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case TRANSFER: {
      CallAsyncDelete(TransferHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case COGNITO: {
      CallAsyncDelete(CognitoHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case DYNAMODB: {
      CallAsyncDelete(DynamoDbHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case SECRETS_MANAGER: {
      CallAsyncDelete(SecretsManagerHandler(_configuration, _metricService), request, response, region, user);
      break;
    }
    }
  }

  void GatewayHandler::handleHead(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) {
    log_trace_stream(_logger) << "Gateway HEAD request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

    Core::MetricServiceTimer measure(_metricService, GATEWAY_HEAD_TIMER);
    _metricService.IncrementCounter(GATEWAY_COUNTER, "method", "HEAD");
    log_debug_stream(_logger) << "Gateway HEAD request, URI: " + request.getURI() << " region: " << region << " user: " + user << std::endl;

    SetHeaders(request, region, user);
    switch (_route._handlerType) {

    case S3: {
      CallAsyncHead(S3Handler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case SQS: {
      CallAsyncHead(SQSHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case SNS: {
      CallAsyncHead(SNSHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case LAMBDA: {
      CallAsyncHead(LambdaHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case TRANSFER: {
      CallAsyncHead(TransferHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case COGNITO: {
      CallAsyncHead(CognitoHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case DYNAMODB: {
      CallAsyncHead(DynamoDbHandler(_configuration, _metricService), request, response, region, user);
      break;
    }

    case SECRETS_MANAGER: {
      CallAsyncHead(SecretsManagerHandler(_configuration, _metricService), request, response, region, user);
      break;
    }
    }
  }

  void GatewayHandler::handleOptions(Poco::Net::HTTPServerResponse &response) {
    log_debug_stream(_logger) << "Gateway OPTIONS request" << std::endl;

    response.set("Allow", "GET, PUT, POST, DELETE, OPTIONS");
    response.setContentType("text/plain; charset=utf-8");

    handleHttpStatusCode(response, Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
    std::ostream &outputStream = response.send();
    outputStream.flush();
  }

  void GatewayHandler::SetHeaders(Poco::Net::HTTPServerRequest &request, const std::string &region, const std::string &user) {
    log_trace_stream(_logger) << "Setting request header values, region: " << region << " user: " << user << std::endl;

    // Default headers
    request.set("Region", region);
    request.set("User", user);
    request.set("RequestId", Core::AwsUtils::CreateRequestId());
  }
}
