//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_SNS_MESSAGE_H
#define AWSMOCK_DB_ENTITY_SNS_MESSAGE_H

// C++ includes
#include <string>
#include <sstream>
#include <vector>

// Poco includes
#include <Poco/DateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <mongocxx/stdx.hpp>

namespace AwsMock::Database::Entity::SNS {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::view_or_value;
    using bsoncxx::document::view;
    using bsoncxx::document::value;

    enum STATUS { INITIAL, SEND, RESEND };

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
       * Topic ARN
       */
      std::string topicArn;

      /**
       * Target ARN
       */
      std::string targetArn;

      /**
       * Message body
       */
      std::string message;

      /**
       * Message ID
       */
      std::string messageId;

      /**
       * List of sqs
       */
      MessageAttributeList attributes;

      /**
       * Last send datetime
       */
      Poco::DateTime lastSend;

      /**
       * Creation datetime
       */
      Poco::DateTime created = Poco::DateTime();

      /**
       * Last modified datetime
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
              kvp("topicArn", topicArn),
              kvp("targetArn", targetArn),
              kvp("message", message),
              kvp("messageId", messageId),
              kvp("attributes", messageAttributesDoc),
              kvp("reset", bsoncxx::types::b_date(std::chrono::milliseconds(lastSend.timestamp().epochMicroseconds() / 1000))),
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
          topicArn = mResult.value()["topicArn"].get_string().value.to_string();
          targetArn = mResult.value()["targetArn"].get_string().value.to_string();
          message = mResult.value()["message"].get_string().value.to_string();
          messageId = mResult.value()["messageId"].get_string().value.to_string();
          lastSend = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["reset"].get_date().value) / 1000));
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
          topicArn = mResult.value()["topicArn"].get_string().value.to_string();
          targetArn = mResult.value()["targetArn"].get_string().value.to_string();
          message = mResult.value()["message"].get_string().value.to_string();
          messageId = mResult.value()["messageId"].get_string().value.to_string();
          lastSend = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["reset"].get_date().value) / 1000));
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
          os << "Message={oid='" + m.oid + "' topicArn: " + m.topicArn + "' targetArn='" + m.targetArn + "'message='" + m.message + "'messageId='" + m.messageId
              + "' reset='" + Poco::DateTimeFormatter().format(m.lastSend, Poco::DateTimeFormat::HTTP_FORMAT) + "' created='" +
              Poco::DateTimeFormatter().format(m.created, Poco::DateTimeFormat::HTTP_FORMAT) + "' modified='" +
              Poco::DateTimeFormatter().format(m.modified, Poco::DateTimeFormat::HTTP_FORMAT) + "'}";
          return os;
      }

    };

    typedef struct Message Message;
    typedef std::vector<Message> MessageList;

} // namespace AwsMock::Database::Entity::SNS

#endif // AWSMOCK_DB_ENTITY_SNS_MESSAGE_H
