//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DB_ENTITY_S3_BUCKET_H
#define AWSMOCK_DB_ENTITY_S3_BUCKET_H

// C++ includes
#include <string>
#include <chrono>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>

namespace AwsMock::Database::Entity::S3 {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::view_or_value;
    using bsoncxx::document::view;
    using bsoncxx::document::value;

    struct BucketNotification {

      /**
       * Event
       */
      std::string event;

      /**
       * Notification ID
       */
      std::string notificationId;

      /**
       * Queue ARN
       */
      std::string queueArn;

      /**
       * Lambda ARN
       */
      std::string lambdaArn;

      /**
       * Converts the entity to a MongoDB document
       *
       * @return entity as MongoDB document.
       */
      [[maybe_unused]] [[nodiscard]] view_or_value<view, value> ToDocument() const {

          view_or_value<view, value> notificationDoc = make_document(
              kvp("notificationId", notificationId),
              kvp("event", event),
              kvp("queueArn", queueArn),
              kvp("lambdaArn", lambdaArn));

          return notificationDoc;
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
      friend std::ostream &operator<<(std::ostream &os, const BucketNotification &q) {
          os << "BucketNotification={notificationId='" + q.notificationId + "' queueArn='" + q.queueArn + "' lambdaArn='" + q.lambdaArn + "' event='" + q.event + "'}";
          return os;
      }

    };

    typedef struct BucketNotification BucketNotification;

    struct Bucket {

      /**
       * ID
       */
      std::string oid;

      /**
       * Bucket region
       */
      std::string region;

      /**
       * Bucket name
       */
      std::string name;

      /**
       * Bucket owner
       */
      std::string owner;

      /**
       * Bucket notifications
       */
      std::vector<BucketNotification> notifications;

      /**
       * Creation date
       */
      Poco::DateTime created;

      /**
       * Last modification date
       */
      Poco::DateTime modified;

      /**
       * Checks whether a notification with the given event name exists.
       *
       * @param eventName name of the event
       * @return true if notification with the given event name exists.
       */
      bool HasNotification(const std::string &eventName) {
          return find_if(notifications.begin(), notifications.end(), [eventName](const BucketNotification &eventNotification) {
            return eventNotification.event == eventName;
          }) != notifications.end();
      }

      /**
       * Returns a given notification by name
       *
       * @param eventName name of the event
       * @return found notification or notifications.end().
       */
      BucketNotification GetNotification(const std::string &eventName) {
          auto it = find_if(notifications.begin(), notifications.end(), [eventName](const BucketNotification &eventNotification) {
            return eventNotification.event == eventName;
          });
          return *it;
      }

      /**
       * Converts the entity to a MongoDB document
       *
       * @return entity as MongoDB document.
       */
      [[maybe_unused]] [[nodiscard]] view_or_value <view, value> ToDocument() const {

          auto notificationsDoc = bsoncxx::builder::basic::array{};
          for (const auto &notification : notifications) {
              notificationsDoc.append(notification.ToDocument());
          }

          view_or_value < view, value > bucketDoc = make_document(
              kvp("region", region),
              kvp("name", name),
              kvp("owner", owner),
              kvp("notifications", notificationsDoc),
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
          name = mResult.value()["name"].get_string().value.to_string();
          owner = mResult.value()["owner"].get_string().value.to_string();
          created = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["created"].get_date().value) / 1000));
          modified = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["modified"].get_date().value) / 1000));

          bsoncxx::array::view notificationView{mResult.value()["notifications"].get_array().value};
          for (bsoncxx::array::element notificationElement : notificationView) {
              BucketNotification notification{
                  .event=notificationElement["event"].get_string().value.to_string(),
                  .notificationId=notificationElement["notificationId"].get_string().value.to_string(),
                  .queueArn=notificationElement["queueArn"].get_string().value.to_string(),
                  .lambdaArn=notificationElement["lambdaArn"].get_string().value.to_string()
              };
              notifications.push_back(notification);
          }
      }

      /**
       * Converts the MongoDB document to an entity
       *
       * @return entity.
       */
      [[maybe_unused]] void FromDocument(mongocxx::stdx::optional<bsoncxx::document::view> mResult) {

          oid = mResult.value()["_id"].get_oid().value.to_string();
          region = mResult.value()["region"].get_string().value.to_string();
          name = mResult.value()["name"].get_string().value.to_string();
          owner = mResult.value()["owner"].get_string().value.to_string();
          created = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["created"].get_date().value) / 1000));
          modified = Poco::DateTime(Poco::Timestamp::fromEpochTime(bsoncxx::types::b_date(mResult.value()["modified"].get_date().value) / 1000));

          bsoncxx::array::view notificationView{mResult.value()["notifications"].get_array().value};
          for (bsoncxx::array::element notificationElement : notificationView) {
              BucketNotification notification{
                  .event=notificationElement["event"].get_string().value.to_string(),
                  .notificationId=notificationElement["notificationId"].get_string().value.to_string(),
                  .queueArn=notificationElement["queueArn"].get_string().value.to_string(),
                  .lambdaArn=notificationElement["lambdaArn"].get_string().value.to_string()
              };
              notifications.push_back(notification);
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
      friend std::ostream &operator<<(std::ostream &os, const Bucket &q) {
          os << "Bucket={oid='" + q.oid + "' region='" + q.region + "' name='" + q.name + "' owner='" + q.owner +
              "' created='" + Poco::DateTimeFormatter().format(q.created, Poco::DateTimeFormat::HTTP_FORMAT) +
              "' modified='" + Poco::DateTimeFormatter().format(q.created, Poco::DateTimeFormat::HTTP_FORMAT) + "'}";
          return os;
      }

    };

    typedef struct Bucket Bucket;
    typedef std::vector<Bucket> BucketList;

} // namespace AwsMock::Database::S3::Entity

#endif //AWSMOCK_DB_ENTITY_S3_BUCKET_H
