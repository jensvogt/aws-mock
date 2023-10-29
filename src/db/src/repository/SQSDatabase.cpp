//
// Created by vogje01 on 29/05/2023.
//

#include "awsmock/repository/SQSDatabase.h"

namespace AwsMock::Database {

  using bsoncxx::builder::basic::kvp;
  using bsoncxx::builder::basic::make_array;
  using bsoncxx::builder::basic::make_document;

  SQSDatabase::SQSDatabase(const Core::Configuration &configuration) : Database(configuration), _logger(Poco::Logger::get("SQSDatabase")) {

    // Get collections
    _queueCollection = GetConnection()["sqs_queue"];
    _messageCollection = GetConnection()["sqs_message"];

    // Get end point
    _endpoint = configuration.getString("awsmock.rest.host", "localhost") + ":" + configuration.getString("awsmock.rest.port", "4567");
  }

  bool SQSDatabase::QueueUrlExists(const std::string &region, const std::string &queueUrl) {

    int64_t count = _queueCollection.count_documents(make_document(kvp("region", region), kvp("queueUrl", make_document(kvp("$regex", queueUrl)))));
    log_trace_stream(_logger) << "Queue exists: " << (count > 0 ? "true" : "false") << std::endl;

    return count > 0;
  }

  bool SQSDatabase::QueueExists(const std::string &region, const std::string &name) {

    int64_t count = _queueCollection.count_documents(make_document(kvp("region", region), kvp("name", name)));
    log_trace_stream(_logger) << "Queue exists: " << (count > 0 ? "true" : "false") << std::endl;

    return count > 0;
  }

  bool SQSDatabase::QueueArnExists(const std::string &queueArn) {

    int64_t count = _queueCollection.count_documents(make_document(kvp("queueArn", queueArn)));
    log_trace_stream(_logger) << "Queue exists: " << (count > 0 ? "true" : "false") << std::endl;

    return count > 0;
  }

  Entity::SQS::Queue SQSDatabase::CreateQueue(const Entity::SQS::Queue &queue) {

    auto result = _queueCollection.insert_one(queue.ToDocument());
    log_trace_stream(_logger) << "Queue created, oid: " << result->inserted_id().get_oid().value.to_string() << std::endl;

    return GetQueueById(result->inserted_id().get_oid().value);
  }

  Entity::SQS::Queue SQSDatabase::GetQueueById(bsoncxx::oid oid) {

    mongocxx::stdx::optional<bsoncxx::document::value> mResult = _queueCollection.find_one(make_document(kvp("_id", oid)));

    if (!mResult) {
      return {};
    }

    Entity::SQS::Queue result;
    result.FromDocument(mResult);
    return result;
  }

  Entity::SQS::Queue SQSDatabase::GetQueueById(const std::string &oid) {
    return GetQueueById(bsoncxx::oid(oid));
  }

  Entity::SQS::Queue SQSDatabase::GetQueueByArn(const std::string &queueArn) {

    mongocxx::stdx::optional<bsoncxx::document::value> mResult = _queueCollection.find_one(make_document(kvp("queueArn", queueArn)));

    if (!mResult) {
      return {};
    }

    Entity::SQS::Queue result;
    result.FromDocument(mResult);
    return result;
  }

  Entity::SQS::Queue SQSDatabase::GetQueueByUrl(const std::string &queueUrl) {

    bsoncxx::builder::stream::document filter{};
    filter << "queueUrl" << queueUrl << bsoncxx::builder::stream::finalize;

    mongocxx::stdx::optional<bsoncxx::document::value> mResult = _queueCollection.find_one(make_document(kvp("queueUrl", make_document(kvp("$regex", queueUrl)))));

    if (!mResult) {
      return {};
    }

    Entity::SQS::Queue result;
    result.FromDocument(mResult);
    return result;
  }

  Entity::SQS::Queue SQSDatabase::GetQueueByName(const std::string &region, const std::string &name) {

    mongocxx::stdx::optional<bsoncxx::document::value> mResult = _queueCollection.find_one(make_document(kvp("region", region), kvp("name", name)));
    if (!mResult) {
      return {};
    }

    Entity::SQS::Queue result;
    result.FromDocument(mResult);
    return result;
  }

  Entity::SQS::QueueList SQSDatabase::ListQueues(const std::string &region) {

    bsoncxx::builder::basic::document filter;
    if (!region.empty()) {
      filter.append(kvp("region", region));
    }

    auto queueCursor = _queueCollection.find({filter});

    Entity::SQS::QueueList queueList;
    for (auto queue : queueCursor) {
      Entity::SQS::Queue result;
      result.FromDocument(queue);
      queueList.push_back(result);
    }

    log_trace_stream(_logger) << "Got queue list, size: " << queueList.size() << std::endl;
    return queueList;
  }

  void SQSDatabase::PurgeQueue(const std::string &region, const std::string &queueUrl) {

    auto result = _messageCollection.delete_many(make_document(kvp("region", region), kvp("queueUrl", queueUrl)));
    log_debug_stream(_logger) << "Purged queue, count: " << result->deleted_count() << std::endl;
  }

  Entity::SQS::Queue SQSDatabase::UpdateQueue(const Entity::SQS::Queue &queue) {

    auto result = _queueCollection.replace_one(make_document(kvp("region", queue.region), kvp("name", queue.name)), queue.ToDocument());
    log_trace_stream(_logger) << "Queue updated: " << queue.ToString() << std::endl;

    return GetQueueById(queue.oid);
  }

  long SQSDatabase::CountQueues(const std::string &region) {

    long count=0;
    if(region.empty()) {
      count = _queueCollection.count_documents({});
    } else {
      count = _queueCollection.count_documents(make_document(kvp("region", region)));
    }
    log_trace_stream(_logger) << "Count queues, result: " << count << std::endl;

    return count;
  }

  void SQSDatabase::DeleteQueue(const Entity::SQS::Queue &queue) {

    auto result = _queueCollection.delete_many(make_document(kvp("region", queue.region), kvp("queueUrl", queue.queueUrl)));
    log_debug_stream(_logger) << "Queues deleted, count: " << result->deleted_count() << std::endl;
  }

  void SQSDatabase::DeleteAllQueues() {

    auto result = _queueCollection.delete_many({});
    log_debug_stream(_logger) << "All queues deleted, count: " << result->deleted_count() << std::endl;
  }

  Entity::SQS::Message SQSDatabase::CreateMessage(const Entity::SQS::Message &message) {

    auto result = _messageCollection.insert_one(message.ToDocument());
    log_trace_stream(_logger) << "Message created, oid: " << result->inserted_id().get_oid().value.to_string() << std::endl;

    return GetMessageById(result->inserted_id().get_oid().value);
  }

  bool SQSDatabase::MessageExists(const std::string &receiptHandle) {

    int64_t count = _messageCollection.count_documents(make_document(kvp("receiptHandle", receiptHandle)));
    log_trace_stream(_logger) << "Message exists: " << (count > 0 ? "true" : "false") << std::endl;
    return count > 0;
  }

  Entity::SQS::Message SQSDatabase::GetMessageById(bsoncxx::oid oid) {

    mongocxx::stdx::optional<bsoncxx::document::value> mResult = _messageCollection.find_one(make_document(kvp("_id", oid)));
    Entity::SQS::Message result;
    result.FromDocument(mResult);

    return result;
  }

  Entity::SQS::Message SQSDatabase::GetMessageByReceiptHandle(const std::string &receiptHandle) {

    mongocxx::stdx::optional<bsoncxx::document::value> mResult = _messageCollection.find_one(make_document(kvp("receiptHandle", receiptHandle)));
    Entity::SQS::Message result;
    result.FromDocument(mResult);

    return result;
  }

  Entity::SQS::Message SQSDatabase::GetMessageById(const std::string &oid) {
    return GetMessageById(bsoncxx::oid(oid));
  }

  void SQSDatabase::ReceiveMessages(const std::string &region, const std::string &queueUrl, int visibility, Entity::SQS::MessageList &messageList) {

    auto reset = std::chrono::high_resolution_clock::now() + std::chrono::seconds{visibility};

    auto session = GetSession();
    session.start_transaction();

    try {

      // Get the cursor
      auto messageCursor = _messageCollection.find(make_document(kvp("queueUrl", queueUrl), kvp("status", Entity::SQS::MessageStatusToString(Entity::SQS::MessageStatus::INITIAL))));
      for (auto message : messageCursor) {

        Entity::SQS::Message result;
        result.FromDocument(message);

        result.retries++;
        result.receiptHandle = Core::AwsUtils::CreateReceiptHandler();
        messageList.push_back(result);

        // Update values
        _messageCollection.update_one(make_document(kvp("_id", message["_id"].get_oid())),
                                      make_document(kvp("$set", make_document(kvp("status", Entity::SQS::MessageStatusToString(Entity::SQS::MessageStatus::INVISIBLE)),
                                                                              kvp("reset",
                                                                                  bsoncxx::types::b_date(reset)),
                                                                              kvp("receiptHandle",
                                                                                  result.receiptHandle))),
                                                    kvp("$inc", make_document(kvp("retries", 1)))));
      }

      // Commit
      session.commit_transaction();

    } catch (mongocxx::exception &e) {
      log_error_stream(_logger) << "Collection transaction exception: " << e.what() << std::endl;
      session.abort_transaction();
    }
    log_trace_stream(_logger) << "Messages received, region: " << region << " queue: " << queueUrl + " count: " << messageList.size() << std::endl;
  }

  void SQSDatabase::ResetMessages(const std::string &queueUrl, long visibility) {

    auto session = GetSession();
    session.start_transaction();
    try {

      auto now = std::chrono::high_resolution_clock::now();
      auto result = _messageCollection.update_many(
          make_document(
              kvp("queueUrl", queueUrl),
              kvp("status", Entity::SQS::MessageStatusToString(Entity::SQS::MessageStatus::INVISIBLE)),
              kvp("reset", make_document(
                  kvp("$lt", bsoncxx::types::b_date(now))))),
          make_document(kvp("$set",
                            make_document(
                                kvp("status", Entity::SQS::MessageStatusToString(Entity::SQS::MessageStatus::INITIAL)),
                                kvp("receiptHandle", "")))));
      // Commit
      session.commit_transaction();

      log_trace_stream(_logger) << "Message reset, visibility: " << visibility << " updated: " << result->upserted_count() << " queue: " << queueUrl << std::endl;

    } catch (mongocxx::exception &e) {
      log_error_stream(_logger) << "Collection transaction exception: " << e.what() << std::endl;
      session.abort_transaction();
    }
  }

  void SQSDatabase::RedriveMessages(const std::string &queueUrl, const Entity::SQS::RedrivePolicy &redrivePolicy) {
    auto session = GetSession();
    session.start_transaction();
    try {
      std::string dlqQueueUrl = Core::AwsUtils::ConvertSQSQueueArnToUrl(redrivePolicy.deadLetterTargetArn, _endpoint);
      auto result = _messageCollection.update_many(make_document(kvp("queueUrl", queueUrl),
                                                                 kvp("status", Entity::SQS::MessageStatusToString(Entity::SQS::MessageStatus::INITIAL)),
                                                                 kvp("retries", make_document(
                                                                     kvp("$gt", redrivePolicy.maxReceiveCount)))),
                                                   make_document(kvp("$set", make_document(kvp("retries", 0),
                                                                                           kvp("queueArn",
                                                                                               redrivePolicy.deadLetterTargetArn),
                                                                                           kvp("queueUrl",
                                                                                               dlqQueueUrl)))));
      // Commit
      session.commit_transaction();

      log_trace_stream(_logger) << "Message redrive, arn: " << redrivePolicy.deadLetterTargetArn << " updated: " << result->modified_count() << " queue: " << queueUrl << std::endl;

    } catch (mongocxx::exception &e) {
      log_error_stream(_logger) << "Collection transaction exception: " << e.what() << std::endl;
      session.abort_transaction();
    }
  }

  void SQSDatabase::ResetDelayedMessages(const std::string &queueUrl, long delay) {

    auto session = GetSession();
    session.start_transaction();
    try {

      auto now = std::chrono::high_resolution_clock::now();
      auto result = _messageCollection.update_many(
          make_document(
              kvp("queueUrl", queueUrl),
              kvp("status", Entity::SQS::MessageStatusToString(Entity::SQS::MessageStatus::DELAYED)),
              kvp("reset", make_document(
                  kvp("$lt", bsoncxx::types::b_date(now))))),
          make_document(
              kvp("$set",
                  make_document(
                      kvp("status", Entity::SQS::MessageStatusToString(Entity::SQS::MessageStatus::INITIAL))))));
      // Commit
      session.commit_transaction();

      int i = result->upserted_count();
      log_trace_stream(_logger) << "Delayed message reset, updated: " << result->upserted_count() << " queue: " << queueUrl << std::endl;

    } catch (mongocxx::exception &e) {
      log_error_stream(_logger) << "Collection transaction exception: " << e.what() << std::endl;
      session.abort_transaction();
    }
  }

  long SQSDatabase::CountMessages(const std::string &region, const std::string &queueUrl) {

    long count = _messageCollection.count_documents(make_document(kvp("region", region), kvp("queueUrl", queueUrl)));
    log_trace_stream(_logger) << "Count messages, region: " << region << " url: " << queueUrl << " result: " << count << std::endl;

    return count;
  }

  long SQSDatabase::CountMessagesByStatus(const std::string &region, const std::string &queueUrl, Entity::SQS::MessageStatus status) {

    long count = _messageCollection.count_documents(make_document(kvp("region", region), kvp("queueUrl", queueUrl), kvp("status", Entity::SQS::MessageStatusToString(status))));
    log_trace_stream(_logger) << "Count messages by status, status: " << Entity::SQS::MessageStatusToString(status) << " result: " << count << std::endl;

    return count;
  }

  void SQSDatabase::DeleteMessages(const std::string &queueUrl) {

    auto result = _messageCollection.delete_many(make_document(kvp("queueUrl", queueUrl)));

    log_debug_stream(_logger) << "Messages deleted, queue: " << queueUrl << " count: " << result->deleted_count() << std::endl;
  }

  void SQSDatabase::DeleteMessage(const Entity::SQS::Message &message) {

    auto result = _messageCollection.delete_one(make_document(kvp("receiptHandle", message.receiptHandle)));
    log_debug_stream(_logger) << "Messages deleted, receiptHandle: " << message.receiptHandle << " count: " << result->deleted_count() << std::endl;
  }

  void SQSDatabase::DeleteAllMessages() {

    auto result = _messageCollection.delete_many({});
    log_debug_stream(_logger) << "All messages deleted, count: " << result->deleted_count() << std::endl;
  }

} // namespace AwsMock::Database
