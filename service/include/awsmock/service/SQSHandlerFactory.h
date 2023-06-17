//
// Created by vogje01 on 17/06/2023.
//

#ifndef AWSMOCK_SERVICE_SQSHANDLERFACTORY_H
#define AWSMOCK_SERVICE_SQSHANDLERFACTORY_H

// Poco includes
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
      SQSRequestHandlerFactory(Core::Configuration &configuration, Core::MetricService &metricService) : _configuration(configuration), _metricService(metricService) {}

      virtual Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest &) {
          return new SQSHandler(_configuration, _metricService);
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

    };

} // namespace AwsMock::Service

#endif //AWSMOCK_SERVICE_SQSHANDLERFACTORY_H
