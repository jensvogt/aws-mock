//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_RESOURCE_FACTORY_S3FACTORY_H
#define AWSMOCK_RESOURCE_FACTORY_S3FACTORY_H

// AwsMock includes
#include <awsmock/core/MetricService.h>
#include <awsmock/resource/factory/IFactory.h>
#include <awsmock/resource/handler/S3Handler.h>

namespace AwsMock::Resource::Factory {

    class S3Factory : public IFactory {

    public:

      /**
       * Create a new resource handler.
       *
       * @param configuration application configuration
       * @param metricService monitoring service
       * @return resource handler
       */
      Poco::Net::HTTPRequestHandler *createResource(Core::Configuration &configuration, Core::MetricService &metricService) override;
    };

}// namespace AwsMock::Resource::Factory

#endif // AWSMOCK_RESOURCE_FACTORY_S3FACTORY_H
