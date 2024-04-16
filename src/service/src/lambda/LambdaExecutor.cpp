//
// Created by root on 10/9/23.
//

#include "awsmock/service/lambda/LambdaExecutor.h"

namespace AwsMock::Service {

  Poco::Mutex LambdaExecutor::_mutex;

  LambdaExecutor::LambdaExecutor(Core::MetricService &metricService) : _logger(Poco::Logger::get("LambdaExecutor")), _metricService(metricService) {}

  void LambdaExecutor::SendInvocationRequest(const std::string &url, const std::string &body) {
    Poco::ScopedLock lock(_mutex);

    Core::MetricServiceTimer measure(_metricService, "lambda_invocation_timer");
    _metricService.IncrementCounter("lambda_invocation_counter");
    log_debug_stream(_logger) << "Sending lambda invocation request, endpoint: " << url << std::endl;

    Core::CurlUtils _curlUtils;
    Core::CurlResponse response = _curlUtils.SendHttpRequest("POST", url, {}, body);
    if (response.statusCode != Poco::Net::HTTPResponse::HTTP_OK) {
      log_error_stream(_logger) << "HTTP error, status: " << response.statusCode << " reason: " + response.output << std::endl;
    }
    log_debug_stream(_logger) << "Lambda invocation finished send, status: " << response.statusCode << std::endl;
  }
}