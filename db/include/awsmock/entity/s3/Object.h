//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DB_ENTITY_S3_OBJECT_H
#define AWSMOCK_DB_ENTITY_S3_OBJECT_H

// C++ includes
#include <string>
#include <chrono>

namespace AwsMock::Database::Entity::S3 {

    struct Object {

      /**
       * ID
       */
      std::string oid;

      /**
       * Aws region name
       */
      std::string region;

      /**
       * Bucket name
       */
      std::string bucket;

      /**
       * Object key
       */
      std::string key;

      /**
       * Object owner
       */
      std::string owner;

      /**
       * Object size
       */
      long size;

      /**
       * Object MD5Sum
       */
      std::string md5sum;

      /**
       * Object content type
       */
      std::string contentType;

      /**
       * Creation date
       */
      Poco::DateTime created;

      /**
       * Last modification date
       */
      Poco::DateTime modified;

      /**
       * Converts the entity to a MongoDB document
       *
       * @return entity as MongoDB document.
       */
      [[maybe_unused]] [[nodiscard]] view_or_value<view, value> ToDocument() const {

          view_or_value<view, value> bucketDoc = make_document(
              kvp("region", region),
              kvp("bucket", bucket),
              kvp("key", key),
              kvp("owner", owner),
              kvp("size", size),
              kvp("md5sum", md5sum),
              kvp("contentType", contentType),
              kvp("created", bsoncxx::types::b_date(std::chrono::milliseconds(created.timestamp().epochMicroseconds() / 1000))),
              kvp("modified", bsoncxx::types::b_date(std::chrono::milliseconds(modified.timestamp().epochMicroseconds() / 1000))));

          return bucketDoc;
      }

      /**
       * Converts the MongoDB document to an entity
       *
       * @return entity.
       */
      [[maybe_unused]] void FromDocument(mongocxx::stdx::optional<bsoncxx::document::value> mResult) {

          oid = mResult.value()["_id"].get_oid().value.to_string();
          region = mResult.value()["region"].get_string().value.to_string();
          bucket = mResult.value()["bucket"].get_string().value.to_string();
          key = mResult.value()["key"].get_string().value.to_string();
          owner = mResult.value()["owner"].get_string().value.to_string();
          size = mResult.value()["size"].get_int64().value;
          md5sum = mResult.value()["md5sum"].get_string().value.to_string();
          contentType = mResult.value()["contentType"].get_string().value.to_string();
          owner = mResult.value()["owner"].get_string().value.to_string();
          created = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["created"].get_date().value) / 1000));
          modified = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["modified"].get_date().value) / 1000));
      }

      /**
       * Converts the MongoDB document to an entity
       *
       * @return entity.
       */
      [[maybe_unused]] void FromDocument(mongocxx::stdx::optional<bsoncxx::document::view> mResult) {

          oid = mResult.value()["_id"].get_oid().value.to_string();
          region = mResult.value()["region"].get_string().value.to_string();
          bucket = mResult.value()["bucket"].get_string().value.to_string();
          key = mResult.value()["key"].get_string().value.to_string();
          owner = mResult.value()["owner"].get_string().value.to_string();
          size = mResult.value()["size"].get_int64().value;
          md5sum = mResult.value()["md5sum"].get_string().value.to_string();
          contentType = mResult.value()["contentType"].get_string().value.to_string();
          owner = mResult.value()["owner"].get_string().value.to_string();
          created = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["created"].get_date().value) / 1000));
          modified = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["modified"].get_date().value) / 1000));
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
      friend std::ostream &operator<<(std::ostream &os, const Object &o) {
          os << "Object={id='" + o.oid + "' bucket='" + o.bucket + "' key='" + o.key + "' owner='" + o.owner + "' size='" + std::to_string(o.size) +
              "' md5sum='" + o.md5sum + "' contentType='" + o.contentType + "'}";
          return os;
      }

    };

    typedef struct Object Object;
    typedef std::vector<Object> ObjectList;

}
// namespace AwsMock::Database::S3::Entity

#endif //AWSMOCK_DB_ENTITY_S3_OBJECT_H
