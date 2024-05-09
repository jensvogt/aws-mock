//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_RESOURCE_IFACTORY_H
#define AWSMOCK_RESOURCE_IFACTORY_H

// Poco includes
#include "Poco/Net/HTTPRequestHandler.h"

// AwsMock includes
#include "awsmock/service/common/AbstractServer.h"
#include <awsmock/core/Configuration.h>
#include <awsmock/core/MetricService.h>

namespace AwsMock::Resource::Factory {

    /**
     * Factory interface
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class IFactory {

      public:

        /**
         * Create a new resource.
         *
         * @param serverMap module map
         * @return HTTP request handler
         */
        virtual Poco::Net::HTTPRequestHandler *createResource(Service::ServerMap &serverMap) = 0;
    };

}// namespace AwsMock::Resource::Factory

#endif// AWSMOCK_RESOURCE_IFACTORY_H
