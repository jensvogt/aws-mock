//
// Created by vogje01 on 02/09/2022.
//

#ifndef AWSMOCK_CONTROLLER_RESOURCENOTFOUND_H
#define AWSMOCK_CONTROLLER_RESOURCENOTFOUND_H

// Poco includes
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPRequestHandler.h"

// AwsMock includes
#include "awsmock/dto/common/ResourceNotFounResponse.h"

namespace AwsMock {

    /**
     * AwsMock resource not found response.
     *
     * @author jens.vogt@opitz-consulting.com
     */
    class [[maybe_unused]] ResourceNotFound : public Poco::Net::HTTPRequestHandler {
    public:
        /**
         * Constructor
         *
         * @param request HTTP restfull request
         * @param response HTTP restfull response
         */
        void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) override;
    };
} // namespace AwsMock

#endif // AWSMOCK_CONTROLLER_RESOURCENOTFOUND_H
