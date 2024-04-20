//
// Created by vogje01 on 17/06/2023.
//

#ifndef AWSMOCK_SERVICE_COGNITOHANDLERFACTORY_H
#define AWSMOCK_SERVICE_COGNITOHANDLERFACTORY_H

// Poco includes
#include <Poco/Net/HTTPRequestHandlerFactory.h>

// AwsMock includes
#include "awsmock/core/Configuration.h"
#include "awsmock/core/LogStream.h"
#include "awsmock/core/MetricService.h"
#include "CognitoHandler.h"

namespace AwsMock::Service {

  /**
   * S3 request handler factory
   */
  class CognitoHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {

  public:

    /**
     * Constructor
     *
     * @param configuration application configuration
     * @param metricService  monitoring
     */
    CognitoHandlerFactory(Core::Configuration &configuration, Core::MetricService &metricService) : _configuration(configuration), _metricService(metricService) {}

    /**
     * Create a new request handler instance.
     *
     * @param request HTTP request
     * @return HTTP request handler
     */
    Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest &request) override {
      if(request.getURI().empty()) {
        return nullptr;
      }
      return new CognitoHandler(_configuration, _metricService);
    }

  private:

    /**
     * S3 handler configuration
     */
    Core::Configuration &_configuration;

    /**
     * Metric module
     */
    Core::MetricService &_metricService;

  };

} // namespace AwsMock::Service

#endif // AWSMOCK_SERVICE_COGNITOHANDLERFACTORY_H