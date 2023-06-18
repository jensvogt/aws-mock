    //
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_LISTALLBUCKETRESPONSE_H
#define AWSMOCK_DTO_S3_LISTALLBUCKETRESPONSE_H

// C++ standard includes
#include <string>
#include <sstream>

// Poco includes
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/DOMWriter.h"
#include "Poco/XML/XMLWriter.h"

// AwsMock includes
#include "awsmock/entity/s3/Bucket.h"

namespace AwsMock::Dto::S3 {

    struct ListAllBucketResponse {

      /**
       * List of buckets
       */
      Database::Entity::S3::BucketList bucketList;

      /**
       * Stream provider.
       *
       * @return output stream
       */
      friend std::ostream &operator<<(std::ostream &, const ListAllBucketResponse &);

      /**
       * Convert to XML representation
       *
       * @return XML string
       */
      [[nodiscard]] std::string ToXml() const {

          Poco::XML::AutoPtr<Poco::XML::Document> pDoc = new Poco::XML::Document;
          Poco::XML::AutoPtr<Poco::XML::Element> pRoot = pDoc->createElement("ListAllMyBucketsResult");
          pDoc->appendChild(pRoot);

          Poco::XML::AutoPtr<Poco::XML::Element> pBuckets;
          pBuckets = pDoc->createElement("Buckets");
          pRoot->appendChild(pBuckets);

          Poco::XML::AutoPtr<Poco::XML::Element> pBucket;
          Poco::XML::AutoPtr<Poco::XML::Element> pName;
          Poco::XML::AutoPtr<Poco::XML::Element> pCreated;
          Poco::XML::AutoPtr<Poco::XML::Text> pNameText;
          Poco::XML::AutoPtr<Poco::XML::Text> pCreatedText;
          for (auto &it : bucketList) {

              pBucket = pDoc->createElement("Bucket");
              pBuckets->appendChild(pBucket);

              pName = pDoc->createElement("Name");
              pBucket->appendChild(pName);
              pNameText = pDoc->createTextNode(it.name);
              pName->appendChild(pNameText);

              pCreated = pDoc->createElement("CreationDate");
              pBucket->appendChild(pCreated);
              pCreatedText = pDoc->createTextNode(Poco::DateTimeFormatter::format(it.created, Poco::DateTimeFormat::ISO8601_FORMAT));
              pCreated->appendChild(pCreatedText);
          }

          std::stringstream output;
          Poco::XML::DOMWriter writer;
          writer.setNewLine("\n");
          writer.setOptions(Poco::XML::XMLWriter::WRITE_XML_DECLARATION | Poco::XML::XMLWriter::PRETTY_PRINT);
          writer.writeNode(output, pDoc);

          std::string outStr = output.str();
          return outStr;
      }

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
      friend std::ostream &operator<<(std::ostream &os, const ListAllBucketResponse &r) {
          os << "ListAllBucketResponse={bucketList='";
          for (const auto &it : r.bucketList) {
              os << it.ToString() + ", ";
          }
          os << "'}";
          return os;
      }
    };

} // namespace AwsMock::Dto::S3

#endif // AWSMOCK_DTO_S3_LISTALLBUCKETRESPONSE_H
