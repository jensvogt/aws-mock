//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_S3CREATEBUCKETREQUEST_H
#define AWSMOCK_CORE_DTO_S3CREATEBUCKETREQUEST_H

// C++ standard includes
#include <string>
#include <sstream>

// Poco includes
#include "Poco/DOM/AutoPtr.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/Element.h"

namespace AwsMock::Dto::S3 {

    struct CreateBucketRequest {

      /**
       * Bucket location
       */
      std::string _locationConstraint;

      /**
       * Constructor
       *
       * @param xmlString XML string
       */
      explicit CreateBucketRequest(const std::string &xmlString);

      /**
       * Convert from XML representation
       *
       * @param xmlString XML string
       */
      void FromXml(const std::string &xmlString);

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
      friend std::ostream &operator<<(std::ostream &os, const CreateBucketRequest &r);

    };

} // namespace AwsMock::Dto::s3

#endif // AWSMOCK_CORE_DTO_S3CREATEBUCKETREQUEST_H
