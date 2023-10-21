//
// Created by vogje01 on 17/06/2023.
//

#ifndef AWSMOCK_SERVICE_SQSHANDLERFACTORY_H
#define AWSMOCK_SERVICE_SQSHANDLERFACTORY_H

// Poco includes
#include "Poco/Condition.h"
#include "Poco/Logger.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/MetricService.h>
#include <awsmock/service/SQSHandler.h>

namespace AwsMock::Service {

  /**
   * S3 request handler factory
   */
  class SQSRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {

    public:

      /**
       * Constructor
       *
       * @param configuration application configuration
       * @param metricService  monitoring
       * @param condition stop condition
       */
      SQSRequestHandlerFactory(Core::Configuration &configuration, Core::MetricService &metricService, Poco::Condition &condition) : _configuration(configuration), _metricService(metricService), _condition(condition) {}

      /**
       * Create a new request handler
       *
       * @return new request handler
       */
      Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest &) override {
        return new SQSHandler(_configuration, _metricService, _condition);
      }

    private:

      /**
       * S3 handler configuration
       */
      Core::Configuration &_configuration;

      /**
       * Metric service
       */
      Core::MetricService &_metricService;

      /**
       * Shutdown condition
       */
      Poco::Condition &_condition;
  };

} // namespace AwsMock::Service

#endif //AWSMOCK_SERVICE_SQSHANDLERFACTORY_H
