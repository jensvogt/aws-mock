//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_DELETEOBJECTREQUEST_H
#define AWSMOCK_CORE_DTO_DELETEOBJECTREQUEST_H

// C++ standard includes
#include <string>
#include <sstream>

namespace AwsMock::Dto::S3 {

    struct DeleteObjectRequest {

      /**
       * Region
       */
      std::string region;

      /**
       * Region
       */
      std::string user;

      /**
       * Bucket
       */
      std::string bucket;

      /**
       * Key
       */
      std::string key;

      /**
       * Converts the DTO to a string representation.
       *
       * @return DTO as string for logging.
       */
      [[nodiscard]] std::string ToString() const {
          std::stringstream ss;
          ss << (*this);
          return ss.str();
      }

      /**
       * Stream provider.
       *
       * @return output stream
       */
      friend std::ostream &operator<<(std::ostream &os, const DeleteObjectRequest &r) {
          os << "DeleteObjectRequest={region='" + r.region + "' user='" + r.user + "' bucket='" + r.bucket + "' key='" + r.key + "'}";
          return os;
      }

    };

} // namespace AwsMock::Dto::S3

#endif //AWSMOCK_CORE_DTO_DELETEOBJECTSREQUEST_H
