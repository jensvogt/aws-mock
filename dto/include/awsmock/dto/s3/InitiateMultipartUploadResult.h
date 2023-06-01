//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_INITIATEMULTIPARTUPLOADRESULT_H
#define AWSMOCK_DTO_S3_INITIATEMULTIPARTUPLOADRESULT_H

// C++ standard includes
#include <string>
#include <sstream>

// Poco includes
#include "Poco/DOM/AutoPtr.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/DOMWriter.h"
#include "Poco/XML/XMLWriter.h"

namespace AwsMock::Dto::S3 {

    class InitiateMultipartUploadResult {

    public:

      /**
       * Constructor
       */
      InitiateMultipartUploadResult(const std::string &bucket, const std::string &key, const std::string &uploadId);

      /**
       * Convert to XML representation
       *
       * @return XML string
       */
      std::string ToXml();

      /**
       * Converts the DTO to a string representation.
       *
       * @return DTO as string for logging.
       */
      [[nodiscard]] std::string ToString() const;

    private:

      /**
       * Bucket name
       */
      std::string _bucket;

      /**
       * Key name
       */
      std::string _key;

      /**
       * Upload ID
       */
      std::string _uploadId;

      /**
       * Stream provider.
       *
       * @return output stream
       */
      friend std::ostream &operator<<(std::ostream &, const InitiateMultipartUploadResult &);
    };

 } // namespace AwsMock::Dto

#endif //AWSMOCK_DTO_S3_INITIATEMULTIPARTUPLOADRESULT_H