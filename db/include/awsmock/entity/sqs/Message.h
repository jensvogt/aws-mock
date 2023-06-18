//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_SQS_MESSAGE_H
#define AWSMOCK_DB_ENTITY_SQS_MESSAGE_H

// C++ includes
#include <string>
#include <vector>

// Poco includes
#include <Poco/DateTime.h>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <mongocxx/stdx.hpp>

namespace AwsMock::Database::Entity::SQS {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::view_or_value;
    using bsoncxx::document::view;
    using bsoncxx::document::value;

    enum STATUS { INITIAL, SEND, RESEND, DELAYED };

    struct MessageAttribute {

      /**
       * MessageAttribute name
       */
      std::string attributeName;

      /**
       * MessageAttribute value
       */
      std::string attributeValue;

      /**
       * Converts the entity to a MongoDB document
       *
       * @return entity as MongoDB document.
       */
      [[maybe_unused]] [[nodiscard]] view_or_value<view, value> ToDocument() const {

          view_or_value<view, value> messageAttributeDoc = make_document(
              kvp("name", attributeName),
              kvp("value", attributeValue));

          return messageAttributeDoc;
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
      friend std::ostream &operator<<(std::ostream &os, const MessageAttribute &m) {
          os << "MessageAttribute={name='" + m.attributeValue + "'value='" + m.attributeValue + "'}";
          return os;
      }

    };

    typedef struct MessageAttribute MessageAttribute;
    typedef std::vector<MessageAttribute> MessageAttributeList;

    struct Message {

      /**
       * ID
       */
      std::string oid;

      /**
       * Aws region name
       */
      std::string region;

      /**
       * Queue URL
       */
      std::string queueUrl;

      /**
       * Message body
       */
      std::string body;

      /**
       * Status
       */
      int status = INITIAL;

      /**
       * Last send datetime
       */
      Poco::DateTime reset;

      /**
       * Send retries
       */
      int retries = 0;

      /**
       * Message ID
       */
      std::string messageId;

      /**
       * Receipt handle
       */
      std::string receiptHandle;

      /**
       * MD5 sum body
       */
      std::string md5Body;

      /**
       * MD5 sum sqs
       */
      std::string md5Attr;

      /**
       * List of sqs
       */
      MessageAttributeList attributes;

      /**
       * Creation date
       */
      Poco::DateTime created = Poco::DateTime();

      /**
       * Last modification date
       */
      Poco::DateTime modified = Poco::DateTime();

      /**
       * Converts the entity to a MongoDB document
       *
       * @return entity as MongoDB document.
       */
      [[nodiscard]] view_or_value<view, value> ToDocument() const {

          auto messageAttributesDoc = bsoncxx::builder::basic::array{};
          for (const auto &attribute : attributes) {
              messageAttributesDoc.append(attribute.ToDocument());
          }

          view_or_value<view, value> messageDoc = make_document(
              kvp("region", region),
              kvp("queueUrl", queueUrl),
              kvp("body", body),
              kvp("status", status),
              kvp("retries", retries),
              kvp("messageId", messageId),
              kvp("receiptHandle", receiptHandle),
              kvp("md5Body", md5Body),
              kvp("md5Attr", md5Attr),
              kvp("attributes", messageAttributesDoc),
              kvp("reset", bsoncxx::types::b_date(std::chrono::milliseconds(reset.timestamp().epochMicroseconds() / 1000))),
              kvp("created", bsoncxx::types::b_date(std::chrono::milliseconds(created.timestamp().epochMicroseconds() / 1000))),
              kvp("modified", bsoncxx::types::b_date(std::chrono::milliseconds(modified.timestamp().epochMicroseconds() / 1000))));

          return messageDoc;
      }

      /**
       * Converts the MongoDB document to an entity
       *
       * @return entity.
       */
      void FromDocument(mongocxx::stdx::optional<bsoncxx::document::value> mResult) {

          oid = mResult.value()["_id"].get_oid().value.to_string();
          region = mResult.value()["region"].get_string().value.to_string();
          queueUrl = mResult.value()["queueUrl"].get_string().value.to_string();
          body = mResult.value()["body"].get_string().value.to_string();
          status = mResult.value()["status"].get_int32().value;
          retries = mResult.value()["retries"].get_int32().value;
          messageId = mResult.value()["messageId"].get_string().value.to_string();
          receiptHandle = mResult.value()["receiptHandle"].get_string().value.to_string();
          md5Body = mResult.value()["md5Body"].get_string().value.to_string();
          md5Attr = mResult.value()["md5Attr"].get_string().value.to_string();
          reset = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["reset"].get_date().value) / 1000));
          created = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["created"].get_date().value) / 1000));
          modified = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["modified"].get_date().value) / 1000));

          bsoncxx::array::view attributesView{mResult.value()["attributes"].get_array().value};
          for (bsoncxx::array::element attributeElement : attributesView) {
              MessageAttribute attribute{
                  .attributeName=attributeElement["attributeName"].get_string().value.to_string(),
                  .attributeValue=attributeElement["attributeValue"].get_string().value.to_string()
              };
              attributes.push_back(attribute);
          }
      }

      /**
       * Converts the MongoDB document to an entity
       *
       * @return entity.
       */
      void FromDocument(mongocxx::stdx::optional<bsoncxx::document::view> mResult) {

          oid = mResult.value()["_id"].get_oid().value.to_string();
          region = mResult.value()["region"].get_string().value.to_string();
          queueUrl = mResult.value()["queueUrl"].get_string().value.to_string();
          body = mResult.value()["body"].get_string().value.to_string();
          status = mResult.value()["status"].get_int32().value;
          retries = mResult.value()["retries"].get_int32().value;
          messageId = mResult.value()["messageId"].get_string().value.to_string();
          receiptHandle = mResult.value()["receiptHandle"].get_string().value.to_string();
          md5Body = mResult.value()["md5Body"].get_string().value.to_string();
          md5Attr = mResult.value()["md5Attr"].get_string().value.to_string();
          reset = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["reset"].get_date().value) / 1000));
          created = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["created"].get_date().value) / 1000));
          modified = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["modified"].get_date().value) / 1000));

          bsoncxx::array::view attributesView{mResult.value()["attributes"].get_array().value};
          for (bsoncxx::array::element attributeElement : attributesView) {
              MessageAttribute attribute{
                  .attributeName=attributeElement["attributeName"].get_string().value.to_string(),
                  .attributeValue=attributeElement["attributeValue"].get_string().value.to_string()
              };
              attributes.push_back(attribute);
          }
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
      friend std::ostream &operator<<(std::ostream &os, const Message &m) {
          os << "Message={oid='" + m.oid + "' queueUrl='" + m.queueUrl + "'body='" + m.body + "' status='" + std::to_string(m.status) + "' reset='" +
              Poco::DateTimeFormatter().format(m.reset, Poco::DateTimeFormat::HTTP_FORMAT) + "' retries='" + std::to_string(m.retries) +
              "' messageId='" + m.messageId + "' receiptHandle='" + m.receiptHandle + "' md5body='" + m.md5Body + "' md5Attr='" + m.md5Attr + "'}";
          return os;
      }

    };

    typedef struct Message Message;
    typedef std::vector<Message> MessageList;

} // namespace AwsMock::Database::Entity::S3

#endif // AWSMOCK_DB_ENTITY_SQS_MESSAGE_H
