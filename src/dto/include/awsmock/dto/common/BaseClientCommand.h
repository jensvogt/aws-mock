//
// Created by vogje01 on 5/12/24.
//

#ifndef AWSMOCK_DTO_BASE_CLIENT_COMMAND_H
#define AWSMOCK_DTO_BASE_CLIENT_COMMAND_H

// AwsMock includes
#include <awsmock/dto/common/HttpMethod.h>

namespace AwsMock::Dto::Common {

    class BaseClientCommand {

      public:

        /**
         * HTTP URL
         */
        std::string url;

        /**
         * HTTP request type
         */
        HttpMethod method;

        /**
         * HTTP headers
         */
        std::map<std::string, std::string> headers;

        /**
         * HTTP message body
         */
        std::string payload;

        /**
         * HTTP content type
         */
        std::string contentType;

        /**
         * HTTP content length
         */
        long contentLength;

        /**
         * Client region
         */
        std::string region;

        /**
         * Client user
         */
        std::string user;
    };

}// namespace AwsMock::Dto::Common

#endif//AWSMOCK_DTO_BASE_CLIENT_COMMAND_H
