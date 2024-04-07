//
// Created by vogje01 on 29/05/2023.
//

#include "awsmock/repository/SQSDatabase.h"

namespace AwsMock::Database {

  using bsoncxx::builder::basic::kvp;
  using bsoncxx::builder::basic::make_array;
  using bsoncxx::builder::basic::make_document;

  SQSDatabase::SQSDatabase() : _logger(Poco::Logger::get("SQSDatabase")), _memoryDb(SQSMemoryDb::instance()), _useDatabase(HasDatabase()), _databaseName(GetDatabaseName()) {
  }

  bool SQSDatabase::QueueExists(const std::string &region, const std::string &name) {

    if (_useDatabase) {

      auto client = GetClient();
      mongocxx::collection _queueCollection = (*client)[_databaseName]["sqs_queue"];

      int64_t count = _queueCollection.count_documents(make_document(kvp("region", region), kvp("name", name)));
      log_trace_stream(_logger) << "Queue exists: " << (count > 0 ? "true" : "false") << std::endl;
      return count > 0;

    } else {

      return _memoryDb.QueueExists(region, name);

    }
  }

  bool SQSDatabase::QueueUrlExists(const std::string &region, const std::string &queueUrl) {

    if (_useDatabase) {

      auto client = GetClient();
      mongocxx::collection _queueCollection = (*client)[_databaseName]["sqs_queue"];

      int64_t count = _queueCollection.count_documents(make_document(kvp("region", region), kvp("queueUrl", queueUrl)));
      log_trace_stream(_logger) << "Queue exists: " << (count > 0 ? "true" : "false") << std::endl;
      return count > 0;

    } else {

      return _memoryDb.QueueUrlExists(region, queueUrl);

    }
  }

  bool SQSDatabase::QueueArnExists(const std::string &queueArn) {

    if (_useDatabase) {

      auto client = GetClient();
      mongocxx::collection _queueCollection = (*client)[_databaseName]["sqs_queue"];

      int64_t count = _queueCollection.count_documents(make_document(kvp("queueArn", queueArn)));
      log_trace_stream(_logger) << "Queue exists: " << (count > 0 ? "true" : "false") << std::endl;
      return count > 0;

    } else {

      return _memoryDb.QueueArnExists(queueArn);

    }
  }

  Entity::SQS::Queue SQSDatabase::CreateQueue(const Entity::SQS::Queue &queue) {

    if (_useDatabase) {

      auto client = GetClient();
      mongocxx::collection _queueCollection = (*client)[_databaseName]["sqs_queue"];
      auto session = client->start_session();

      try {

        session.start_transaction();
        auto result = _queueCollection.insert_one(queue.ToDocument());
        log_trace_stream(_logger) << "Queue created, oid: " << result->inserted_id().get_oid().value.to_string() << std::endl;
        session.commit_transaction();

        return GetQueueById(result->inserted_id().get_oid().value);

      } catch (mongocxx::exception &e) {
        session.abort_transaction();
        log_error_stream(_logger) << "Collection transaction exception: " << e.what() << std::endl;
        throw Core::DatabaseException("Insert queue failed, region: " + queue.region + " queueUrl: " + queue.queueUrl + " message: " + e.what());
      }

    } else {

      return _memoryDb.CreateQueue(queue);

    }
  }

  Entity::SQS::Queue SQSDatabase::GetQueueById(bsoncxx::oid oid) {

    auto client = GetClient();
    mongocxx::collection _queueCollection = (*client)[_databaseName]["sqs_queue"];

    mongocxx::stdx::optional<bsoncxx::document::value> mResult = _queueCollection.find_one(make_document(kvp("_id", oid)));
    if (!mResult) {
      return {};
    }

    Entity::SQS::Queue result;
    result.FromDocument(mResult);
    return result;
  }

  Entity::SQS::Queue SQSDatabase::GetQueueById(const std::string &oid) {

    if (_useDatabase) {

      return GetQueueById(bsoncxx::oid(oid));

    } else {

      return _memoryDb.GetQueueById(oid);

    }
  }

  Entity::SQS::Queue SQSDatabase::GetQueueByArn(const std::string &queueArn) {

    Entity::SQS::Queue result;
    if (_useDatabase) {

      auto client = GetClient();
      mongocxx::collection _queueCollection = (*client)[_databaseName]["sqs_queue"];

      mongocxx::stdx::optional<bsoncxx::document::value> mResult = _queueCollection.find_one(make_document(kvp("queueArn", queueArn)));

      if (!mResult) {
        return {};
      }

      result.FromDocument(mResult);

    } else {

      return _memoryDb.GetQueueByArn(queueArn);

    }
    return result;
  }

  Entity::SQS::Queue SQSDatabase::GetQueueByUrl(const std::string &region, const std::string &queueUrl) {

    Entity::SQS::Queue result;
    if (_useDatabase) {

      auto client = GetClient();
      mongocxx::collection _queueCollection = (*client)[_databaseName]["sqs_queue"];

      mongocxx::stdx::optional<bsoncxx::document::value> mResult = _queueCollection.find_one(make_document(kvp("region", region), kvp("queueUrl", queueUrl)));
      if (!mResult) {
        return {};
      }

      result.FromDocument(mResult);

    } else {

      return _memoryDb.GetQueueByUrl(queueUrl);

    }
    return result;
  }

  Entity::SQS::Queue SQSDatabase::GetQueueByName(const std::string &region, const std::string &name) {

    Entity::SQS::Queue result;
    if (_useDatabase) {

      auto client = GetClient();
      mongocxx::collection _queueCollection = (*client)[_databaseName]["sqs_queue"];

      mongocxx::stdx::optional<bsoncxx::document::value> mResult = _queueCollection.find_one(make_document(kvp("region", region), kvp("name", name)));
      if (!mResult) {
        log_warning_stream(_logger) << "GetQueueByName failed, name: " << name << std::endl;
        return {};
      }

      result.FromDocument(mResult);

    } else {

      return _memoryDb.GetQueueByName(region, name);

    }
    return result;
  }

  Entity::SQS::QueueList SQSDatabase::ListQueues(const std::string &region) {

    Entity::SQS::QueueList queueList;
    if (_useDatabase) {

      auto client = GetClient();
      mongocxx::collection _queueCollection = (*client)[_databaseName]["sqs_queue"];

      if (region.empty()) {

        auto queueCursor = _queueCollection.find({});
        for (auto queue : queueCursor) {
          Entity::SQS::Queue result;
          result.FromDocument(queue);
          queueList.push_back(result);
        }

      } else {

        auto queueCursor = _queueCollection.find(make_document(kvp("region", region)));
        for (auto queue : queueCursor) {
          Entity::SQS::Queue result;
          result.FromDocument(queue);
          queueList.push_back(result);
        }

      }

    } else {

      queueList = _memoryDb.ListQueues(region);

    }
    log_trace_stream(_logger) << "Got queue list, size: " << queueList.size() << std::endl;
    return queueList;
  }

  void SQSDatabase::PurgeQueue(const std::string &region, const std::string &queueUrl) {

    if (_useDatabase) {

      auto client = GetClient();
      auto messageCollection = (*client)[_databaseName]["sqs_message"];
      auto session = client->start_session();

      try {

        session.start_transaction();
        auto result = messageCollection.delete_many(make_document(kvp("region", region), kvp("queueUrl", queueUrl)));
        session.commit_transaction();
        log_debug_stream(_logger) << "Purged queue, count: " << result->deleted_count() << " url: " << queueUrl << std::endl;

      } catch (const mongocxx::exception &exc) {
        session.abort_transaction();
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException(exc.what(), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      log_debug_stream(_logger) << "Purged queue, count: " << _messages.size() << std::endl;
      _memoryDb.PurgeQueue(region, queueUrl);

    }
  }

  Entity::SQS::Queue SQSDatabase::UpdateQueue(Entity::SQS::Queue &queue) {

    if (_useDatabase) {

      mongocxx::options::find_one_and_update opts{};
      opts.return_document(mongocxx::options::return_document::k_after);

      auto client = GetClient();
      mongocxx::collection _queueCollection = (*client)[_databaseName]["sqs_queue"];
      auto session = client->start_session();

      try {

        session.start_transaction();
        auto mResult = _queueCollection.find_one_and_update(make_document(kvp("region", queue.region), kvp("name", queue.name)), queue.ToDocument(), opts);
        session.commit_transaction();
        log_trace_stream(_logger) << "Queue updated: " << queue.ToString() << std::endl;

        queue.FromDocument(mResult->view());
        return queue;

      } catch (const mongocxx::exception &exc) {
        session.abort_transaction();
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException(exc.what(), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      return _memoryDb.UpdateQueue(queue);

    }
  }

  Entity::SQS::Queue SQSDatabase::CreateOrUpdateQueue(Entity::SQS::Queue &queue) {

    if (QueueUrlExists(queue.region, queue.queueUrl)) {

      return UpdateQueue(queue);

    } else {

      return CreateQueue(queue);

    }
  }

  long SQSDatabase::CountQueues(const std::string &region) {

    long count = 0;
    if (_useDatabase) {

      auto client = GetClient();
      mongocxx::collection _queueCollection = (*client)[_databaseName]["sqs_queue"];

      if (region.empty()) {
        count = _queueCollection.count_documents({});
      } else {
        count = _queueCollection.count_documents(make_document(kvp("region", region)));
      }

    } else {

      return _memoryDb.CountQueues(region);

    }
    log_trace_stream(_logger) << "Count queues, result: " << count << std::endl;
    return count;
  }

  void SQSDatabase::DeleteQueue(const Entity::SQS::Queue &queue) {

    if (_useDatabase) {

      auto client = GetClient();
      mongocxx::collection _queueCollection = (*client)[_databaseName]["sqs_queue"];
      auto session = client->start_session();

      try {

        session.start_transaction();
        auto result = _queueCollection.delete_many(make_document(kvp("region", queue.region), kvp("queueUrl", queue.queueUrl)));
        session.commit_transaction();
        log_debug_stream(_logger) << "Queue deleted, count: " << result->deleted_count() << std::endl;

      } catch (const mongocxx::exception &exc) {
        session.abort_transaction();
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException(exc.what(), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      _memoryDb.DeleteQueue(queue);

    }
  }

  void SQSDatabase::DeleteAllQueues() {

    if (_useDatabase) {

      auto client = GetClient();
      mongocxx::collection _queueCollection = (*client)[_databaseName]["sqs_queue"];
      auto session = client->start_session();

      try {

        session.start_transaction();
        auto result = _queueCollection.delete_many({});
        session.commit_transaction();
        log_debug_stream(_logger) << "All queues deleted, count: " << result->deleted_count() << std::endl;

      } catch (const mongocxx::exception &exc) {
        session.abort_transaction();
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException(exc.what(), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      _memoryDb.DeleteAllQueues();

    }
  }

  Entity::SQS::Message SQSDatabase::CreateMessage(const Entity::SQS::Message &message) {

    if (_useDatabase) {

      auto client = GetClient();
      auto messageCollection = (*client)[_databaseName]["sqs_message"];
      auto session = client->start_session();

      try {

        session.start_transaction();
        auto result = messageCollection.insert_one(message.ToDocument());
        session.commit_transaction();
        log_trace_stream(_logger) << "Message created, oid: " << result->inserted_id().get_oid().value.to_string() << std::endl;
        return GetMessageById(result->inserted_id().get_oid().value);

      } catch (const mongocxx::exception &exc) {
        session.abort_transaction();
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException(exc.what(), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      return _memoryDb.CreateMessage(message);

    }
  }

  bool SQSDatabase::MessageExists(const std::string &receiptHandle) {

    if (_useDatabase) {

      auto client = GetClient();
      auto messageCollection = (*client)[_databaseName]["sqs_message"];

      int64_t count = messageCollection.count_documents(make_document(kvp("receiptHandle", receiptHandle)));
      log_trace_stream(_logger) << "Message exists: " << (count > 0 ? "true" : "false") << std::endl;
      return count > 0;

    } else {

      return _memoryDb.MessageExists(receiptHandle);

    }
  }

  Entity::SQS::Message SQSDatabase::GetMessageById(bsoncxx::oid oid) {

    auto client = GetClient();
    auto messageCollection = (*client)[_databaseName]["sqs_message"];

    mongocxx::stdx::optional<bsoncxx::document::value> mResult = messageCollection.find_one(make_document(kvp("_id", oid)));
    Entity::SQS::Message result;
    result.FromDocument(mResult);

    return result;
  }

  Entity::SQS::Message SQSDatabase::GetMessageByReceiptHandle(const std::string &receiptHandle) {

    if (_useDatabase) {

      auto client = GetClient();
      auto messageCollection = (*client)[_databaseName]["sqs_message"];

      mongocxx::stdx::optional<bsoncxx::document::value> mResult = messageCollection.find_one(make_document(kvp("receiptHandle", receiptHandle)));
      Entity::SQS::Message result;
      result.FromDocument(mResult);
      return result;

    } else {

      return _memoryDb.GetMessageByReceiptHandle(receiptHandle);

    }
  }

  Entity::SQS::Message SQSDatabase::GetMessageById(const std::string &oid) {

    if (_useDatabase) {

      return GetMessageById(bsoncxx::oid(oid));

    } else {

      return _memoryDb.GetMessageById(oid);

    }
  }

  Entity::SQS::Message SQSDatabase::UpdateMessage(Entity::SQS::Message &message) {

    if (_useDatabase) {

      mongocxx::options::find_one_and_update opts{};
      opts.return_document(mongocxx::options::return_document::k_after);

      auto client = GetClient();
      auto messageCollection = (*client)[_databaseName]["sqs_message"];
      auto session = client->start_session();

      try {

        session.start_transaction();
        auto mResult = messageCollection.find_one_and_update(make_document(kvp("_id", bsoncxx::oid{message.oid})), message.ToDocument(), opts);
        session.commit_transaction();
        log_trace_stream(_logger) << "Message updated: " << ConvertMessageToJson(mResult.value()) << std::endl;
        if (mResult) {
          message.FromDocument(mResult->view());
          return message;
        }

      } catch (const mongocxx::exception &exc) {
        session.abort_transaction();
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException(exc.what(), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      return _memoryDb.UpdateMessage(message);

    }
    return {};
  }

  Entity::SQS::Message SQSDatabase::CreateOrUpdateMessage(Entity::SQS::Message &message) {

    if (MessageExists(message.receiptHandle)) {

      return UpdateMessage(message);

    } else {

      return CreateMessage(message);

    }
  }

  Entity::SQS::MessageList SQSDatabase::ListMessages(const std::string &region) {

    Entity::SQS::MessageList messageList;
    if (_useDatabase) {

      auto client = GetClient();
      auto messageCollection = (*client)[_databaseName]["sqs_message"];

      if (region.empty()) {

        auto messageCursor = messageCollection.find({});
        for (auto message : messageCursor) {
          Entity::SQS::Message result;
          result.FromDocument(message);
          messageList.push_back(result);
        }

      } else {

        auto messageCursor = messageCollection.find(make_document(kvp("region", region)));
        for (auto message : messageCursor) {
          Entity::SQS::Message result;
          result.FromDocument(message);
          messageList.push_back(result);
        }
      }

    } else {

      messageList = _memoryDb.ListMessages(region);

    }
    log_trace_stream(_logger) << "Got message list, size: " << messageList.size() << std::endl;
    return messageList;
  }

  void SQSDatabase::ReceiveMessages(const std::string &region, const std::string &queueUrl, int visibility, int maxMessages, Entity::SQS::MessageList &messageList) {

    // First rest messages
    ResetMessages(queueUrl, visibility);

    auto reset = std::chrono::high_resolution_clock::now() + std::chrono::seconds(visibility);

    if (_useDatabase) {

      auto client = GetClient();
      auto messageCollection = (*client)[_databaseName]["sqs_message"];
      auto session = client->start_session();

      try {

        session.start_transaction();

        mongocxx::options::find opts;
        opts.limit(maxMessages);

        // Get the cursor
        auto messageCursor = messageCollection.find(make_document(kvp("queueUrl", queueUrl),
                                                                   kvp("status", Entity::SQS::MessageStatusToString(Entity::SQS::MessageStatus::INITIAL))),
                                                     opts);

        for (auto message : messageCursor) {

          Entity::SQS::Message result;
          result.FromDocument(message);

          result.retries++;
          result.receiptHandle = Core::AwsUtils::CreateSqsReceiptHandler();
          messageList.push_back(result);

          // Update values
          messageCollection.update_one(make_document(kvp("_id", message["_id"].get_oid())),
                                        make_document(kvp("$set",
                                                          make_document(kvp("status", Entity::SQS::MessageStatusToString(Entity::SQS::MessageStatus::INVISIBLE)),
                                                                        kvp("reset", bsoncxx::types::b_date(reset)),
                                                                        kvp("receiptHandle", result.receiptHandle),
                                                                        kvp("retries", result.retries)))));
        }

        // Commit
        session.commit_transaction();

      } catch (mongocxx::exception &e) {
        log_error_stream(_logger) << "Collection transaction exception: " << e.what() << std::endl;
        session.abort_transaction();
      }

    } else {

      _memoryDb.ReceiveMessages(region, queueUrl, visibility, maxMessages, messageList);

    }
    log_trace_stream(_logger) << "Messages received, region: " << region << " queue: " << queueUrl + " count: " << messageList.size() << std::endl;
  }

  void SQSDatabase::ResetMessages(const std::string &queueUrl, long visibility) {

    if (_useDatabase) {

      auto client = GetClient();
      auto messageCollection = (*client)[_databaseName]["sqs_message"];
      auto session = client->start_session();

      try {

        session.start_transaction();
        auto result = messageCollection.update_many(
          make_document(
            kvp("queueUrl", queueUrl),
            kvp("status", Entity::SQS::MessageStatusToString(Entity::SQS::MessageStatus::INVISIBLE)),
            kvp("reset", make_document(
              kvp("$lt", bsoncxx::types::b_date(std::chrono::system_clock::now()))))),
          make_document(kvp("$set",
                            make_document(
                              kvp("status", Entity::SQS::MessageStatusToString(Entity::SQS::MessageStatus::INITIAL)),
                              kvp("receiptHandle", ""),
                              kvp("reset", bsoncxx::types::b_null())))));
        session.commit_transaction();

        log_trace_stream(_logger) << "Message reset, updated: " << result->upserted_count() << " queue: " << queueUrl << std::endl;

      } catch (mongocxx::exception &e) {
        log_error_stream(_logger) << "Collection transaction exception: " << e.what() << std::endl;
        session.abort_transaction();
      }

    } else {

      _memoryDb.ResetMessages(queueUrl, visibility);

    }
  }

  void SQSDatabase::RedriveMessages(const std::string &queueUrl, const Entity::SQS::RedrivePolicy &redrivePolicy) {

    if (_useDatabase) {

      auto client = GetClient();
      auto messageCollection = (*client)[_databaseName]["sqs_message"];
      auto session = client->start_session();

      try {

        session.start_transaction();
        std::string dlqQueueUrl = Core::AwsUtils::ConvertSQSQueueArnToUrl(Core::Configuration::instance(), redrivePolicy.deadLetterTargetArn);
        auto result = messageCollection.update_many(make_document(kvp("queueUrl", queueUrl),
                                                                   kvp("status", Entity::SQS::MessageStatusToString(Entity::SQS::MessageStatus::INITIAL)),
                                                                   kvp("retries", make_document(
                                                                     kvp("$gt", redrivePolicy.maxReceiveCount)))),
                                                     make_document(kvp("$set", make_document(kvp("retries", 0),
                                                                                             kvp("queueArn",
                                                                                                 redrivePolicy.deadLetterTargetArn),
                                                                                             kvp("queueUrl",
                                                                                                 dlqQueueUrl)))));
        session.commit_transaction();
        log_trace_stream(_logger) << "Message redrive, arn: " << redrivePolicy.deadLetterTargetArn << " updated: " << result->modified_count() << " queue: "
                                  << queueUrl << std::endl;

      } catch (mongocxx::exception &e) {
        log_error_stream(_logger) << "Collection transaction exception: " << e.what() << std::endl;
        session.abort_transaction();
      }

    } else {

      _memoryDb.RedriveMessages(queueUrl, redrivePolicy, Core::Configuration::instance());

    }
  }

  void SQSDatabase::ResetDelayedMessages(const std::string &queueUrl, long delay) {

    if (_useDatabase) {

      auto client = GetClient();
      auto messageCollection = (*client)[_databaseName]["sqs_message"];
      auto session = client->start_session();

      try {

        session.start_transaction();
        auto now = std::chrono::high_resolution_clock::now();
        auto result = messageCollection.update_many(
          make_document(
            kvp("queueUrl", queueUrl),
            kvp("status", Entity::SQS::MessageStatusToString(Entity::SQS::MessageStatus::DELAYED)),
            kvp("reset", make_document(
              kvp("$lt", bsoncxx::types::b_date(now))))),
          make_document(
            kvp("$set",
                make_document(
                  kvp("status", Entity::SQS::MessageStatusToString(Entity::SQS::MessageStatus::INITIAL))))));
        session.commit_transaction();

        log_trace_stream(_logger) << "Delayed message reset, updated: " << result->upserted_count() << " queue: " << queueUrl << std::endl;

      } catch (mongocxx::exception &exc) {
        session.abort_transaction();
        log_error_stream(_logger) << "Collection transaction exception: " << exc.what() << std::endl;
        throw Core::DatabaseException(exc.what(), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      _memoryDb.ResetDelayedMessages(queueUrl, delay);

    }
  }

  void SQSDatabase::MessageRetention(const std::string &queueUrl, long retentionPeriod) {

    auto reset = std::chrono::high_resolution_clock::now() - std::chrono::seconds{retentionPeriod};

    if (_useDatabase) {

      auto client = GetClient();
      auto messageCollection = (*client)[_databaseName]["sqs_message"];
      auto session = client->start_session();

      try {

        session.start_transaction();
        auto now = std::chrono::high_resolution_clock::now();
        auto result = messageCollection.delete_many(
          make_document(
            kvp("queueUrl", queueUrl),
            kvp("created", make_document(
              kvp("$lt", bsoncxx::types::b_date(reset))))));
        session.commit_transaction();

        log_trace_stream(_logger) << "Message retention reset, deleted: " << result->deleted_count() << " queue: " << queueUrl << std::endl;

      } catch (const mongocxx::exception &exc) {
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException(exc.what(), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      _memoryDb.MessageRetention(queueUrl, retentionPeriod);

    }
  }

  long SQSDatabase::CountMessages(const std::string &region, const std::string &queueUrl) {

    if (_useDatabase) {

      long count = 0;
      auto client = GetClient();
      auto messageCollection = (*client)[_databaseName]["sqs_message"];

      if (!region.empty() && !queueUrl.empty()) {
        count = messageCollection.count_documents(make_document(kvp("region", region), kvp("queueUrl", queueUrl)));
        log_trace_stream(_logger) << "Count messages, region: " << region << " url: " << queueUrl << " result: " << count << std::endl;
      } else if (!region.empty()) {
        count = messageCollection.count_documents(make_document(kvp("region", region)));
        log_trace_stream(_logger) << "Count messages, region: " << region << " result: " << count << std::endl;
      } else {
        count = messageCollection.count_documents({});
        log_trace_stream(_logger) << "Count messages, result: " << count << std::endl;
      }
      return count;

    } else {

      return _memoryDb.CountMessages(region, queueUrl);

    }
  }

  long SQSDatabase::CountMessagesByStatus(const std::string &region, const std::string &queueUrl, Entity::SQS::MessageStatus status) {

    if (_useDatabase) {

      auto client = GetClient();
      auto messageCollection = (*client)[_databaseName]["sqs_message"];

      long count = messageCollection.count_documents(make_document(kvp("region", region),
                                                                    kvp("queueUrl", queueUrl),
                                                                    kvp("status", Entity::SQS::MessageStatusToString(status))));
      log_trace_stream(_logger) << "Count messages by status, status: " << Entity::SQS::MessageStatusToString(status) << " result: " << count << std::endl;
      return count;

    } else {

      return _memoryDb.CountMessagesByStatus(region, queueUrl, status);

    }
  }

  void SQSDatabase::DeleteMessages(const std::string &queueUrl) {

    if (_useDatabase) {

      auto client = GetClient();
      auto messageCollection = (*client)[_databaseName]["sqs_message"];
      auto session = client->start_session();

      try {

        session.start_transaction();
        auto result = messageCollection.delete_many(make_document(kvp("queueUrl", queueUrl)));
        session.commit_transaction();
        log_debug_stream(_logger) << "Messages deleted, queue: " << queueUrl << " count: " << result->deleted_count() << std::endl;

      } catch (const mongocxx::exception &exc) {
        session.abort_transaction();
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException(exc.what(), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      _memoryDb.DeleteMessages(queueUrl);

    }
  }

  void SQSDatabase::DeleteMessage(const Entity::SQS::Message &message) {

    if (_useDatabase) {

      auto client = GetClient();
      auto messageCollection = (*client)[_databaseName]["sqs_message"];
      auto session = client->start_session();

      try {

        session.start_transaction();
        auto result = messageCollection.delete_one(make_document(kvp("receiptHandle", message.receiptHandle)));
        session.commit_transaction();
        log_debug_stream(_logger) << "Messages deleted, receiptHandle: " << Core::StringUtils::SubString(message.receiptHandle, 0, 40) << "... count: "
                                  << result->deleted_count() << std::endl;
      } catch (const mongocxx::exception &exc) {
        session.abort_transaction();
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException(exc.what(), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      _memoryDb.DeleteMessage(message);

    }
  }

  void SQSDatabase::DeleteAllMessages() {

    if (_useDatabase) {

      auto client = GetClient();
      auto messageCollection = (*client)[_databaseName]["sqs_message"];
      auto session = client->start_session();

      try {

        session.start_transaction();
        auto result = messageCollection.delete_many({});
        session.commit_transaction();
        log_debug_stream(_logger) << "All messages deleted, count: " << result->deleted_count() << std::endl;

      } catch (const mongocxx::exception &exc) {
        session.abort_transaction();
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException(exc.what(), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      _memoryDb.DeleteAllMessages();

    }
  }

  std::string SQSDatabase::ConvertMessageToJson(mongocxx::stdx::optional<bsoncxx::document::value> document) {
    return bsoncxx::to_json(document->view());
  }

} // namespace AwsMock::Database
