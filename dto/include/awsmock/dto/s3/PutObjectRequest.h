//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_PUTOBJECTREQUEST_H
#define AWSMOCK_CORE_DTO_PUTOBJECTREQUEST_H

// C++ standard includes
#include <string>
#include <sstream>

namespace AwsMock::Dto::S3 {

    struct PutObjectRequest {

      /**
       * Region
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
       * MD5 sum
       */
      std::string md5Sum;

      /**
       * Content type
       */
      std::string contentType;

      /**
       * Size
       */
      long size;

      /**
       * Owner
       */
      std::string owner;

      /**
       * Write file
       *
       * <p>Indicates that the file is part of the body.</p>
       */
      bool writeFile = true;

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
      friend std::ostream &operator<<(std::ostream &os, const PutObjectRequest &r) {
          os << "PutObjectRequest={bucket='" + r.bucket + "' key='" + r.key + "' md5sum='" + r.md5Sum + "' contentType='" + r.contentType + "' size='" +
              std::to_string(r.size) + "' owner='" + r.owner + "' region=' " + r.region + "'}";
          return os;
      }

    };

} // namespace AwsMock::Dto::s3

#endif //AWSMOCK_CORE_DTO_PUTOBJECTREQUEST_H
