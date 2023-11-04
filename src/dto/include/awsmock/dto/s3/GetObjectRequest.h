//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_GETOBJECTREQUEST_H
#define AWSMOCK_DTO_S3_GETOBJECTREQUEST_H

// C++ standard includes
#include <string>
#include <sstream>

namespace AwsMock::Dto::S3 {

    struct GetObjectRequest {

      /**
       * AWS region name
       */
      std::string region;

      /**
       * Bucket
       */
      std::string bucket;

      /**
       * Key
       */
      std::string key;

      /**
       * Version ID
       */
      std::string versionId;

      /**
       * Range minimum
       */
      long min;

      /**
       * Range maximum
       */
      long max;

      /**
       * Converts the DTO to a string representation.
       *
       * @return DTO as string for logging.
       */
      [[nodiscard]] std::string ToString() const;

      /**
       * Stream provider.
       *
       * @return output stream
       */
      friend std::ostream &operator<<(std::ostream &os, const GetObjectRequest &r);

    };

} // namespace AwsMock::Dto::S3

#endif // AWSMOCK_DTO_S3_GETOBJECTREQUEST_H
