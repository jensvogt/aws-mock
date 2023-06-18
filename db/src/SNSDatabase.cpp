//
// Created by vogje01 on 29/05/2023.
//

#include "awsmock/db/SNSDatabase.h"

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;

    SNSDatabase::SNSDatabase(const Core::Configuration &configuration) : Database(configuration), _logger(Poco::Logger::get("SNSDatabase")) {

        // Set default console logger
        Core::Logger::SetDefaultConsoleLogger("SNSDatabase");

        // Get collections
        _topicCollection = GetConnection()["sns_topic"];
        _messageCollection = GetConnection()["sns_message"];
    }

    bool SNSDatabase::TopicExists(const std::string &topicArn) {

        int64_t count = _topicCollection.count_documents(make_document(kvp("topicArn", topicArn)));
        _logger.trace() << "Topic exists: " << (count > 0 ? "true" : "false") << std::endl;
        return count > 0;
    }

    bool SNSDatabase::TopicExists(const std::string &region, const std::string &topicName) {

        int64_t count = _topicCollection.count_documents(make_document(kvp("region", region), kvp("topicName", topicName)));
        _logger.trace() << "Topic exists: " << (count > 0 ? "true" : "false") << std::endl;
        return count > 0;
    }

    Entity::SNS::Topic SNSDatabase::CreateTopic(const Entity::SNS::Topic &topic) {

        auto result = _topicCollection.insert_one(topic.ToDocument());
        _logger.trace() << "Topic created, oid: " << result->inserted_id().get_oid().value.to_string() << std::endl;

        return GetTopicById(result->inserted_id().get_oid().value);
    }

    Entity::SNS::Topic SNSDatabase::GetTopicById(bsoncxx::oid oid) {

        mongocxx::stdx::optional<bsoncxx::document::value> mResult = _topicCollection.find_one(make_document(kvp("_id", oid)));
        Entity::SNS::Topic result;
        result.FromDocument(mResult);

        return result;
    }

    Entity::SNS::Topic SNSDatabase::GetTopicById(const std::string &oid) {
        return GetTopicById(bsoncxx::oid(oid));
    }

    Entity::SNS::Topic SNSDatabase::GetTopicByArn(const std::string &topicArn) {

        mongocxx::stdx::optional<bsoncxx::document::value> mResult = _topicCollection.find_one(make_document(kvp("topicArn", topicArn)));
        Entity::SNS::Topic result;
        result.FromDocument(mResult);

        return result;
    }

    Entity::SNS::TopicList SNSDatabase::ListTopics(const std::string &region) {

        Entity::SNS::TopicList topicList;
        auto queueCursor = _topicCollection.find(make_document(kvp("region", region)));
        for (auto topic : queueCursor) {
            Entity::SNS::Topic result;
            result.FromDocument(topic);
            topicList.push_back(result);
        }

        _logger.trace() << "Got topic list, size:" << topicList.size() << std::endl;
        return topicList;
    }

    Entity::SNS::Topic SNSDatabase::UpdateTopic(const Entity::SNS::Topic &topic) {

        auto result = _topicCollection.replace_one(make_document(kvp("region", topic.region), kvp("topicArn", topic.topicArn)), topic.ToDocument());

        _logger.trace() << "Topic updated: " << topic.ToString() << std::endl;

        return GetTopicByArn(topic.topicArn);
    }

    long SNSDatabase::CountTopics(const std::string &region) {

        long count = _topicCollection.count_documents(make_document(kvp("region", region)));
        _logger.trace() << "Count topics, result: " << count << std::endl;
        return count;
    }

    void SNSDatabase::DeleteTopic(const Entity::SNS::Topic &topic) {
       auto result = _topicCollection.delete_many(make_document(kvp("topicArn", topic.topicArn)));
       _logger.debug() << "Topic deleted, count: " << result->deleted_count() << std::endl;
   }

    void SNSDatabase::DeleteAllTopics() {
        auto result = _topicCollection.delete_many({});
        _logger.debug() << "All topics deleted, count: " << result->deleted_count() << std::endl;
    }

    Entity::SNS::Message SNSDatabase::CreateMessage(const Entity::SNS::Message &message) {

        try {

            auto result = _messageCollection.insert_one(message.ToDocument());
            _logger.trace() << "Message created, oid: " << result->inserted_id().get_oid().value.to_string() << std::endl;

            return GetMessageById(result->inserted_id().get_oid().value);

        } catch (const mongocxx::exception& exc) {
            std::cerr << exc.what() << std::endl;
            _logger.error() << "Database exception " << exc.what() << std::endl;
            throw Core::DatabaseException(exc.what(), 500);
        }
    }

    Entity::SNS::Message SNSDatabase::GetMessageById(bsoncxx::oid oid) {

        mongocxx::stdx::optional<bsoncxx::document::value> mResult = _messageCollection.find_one(make_document(kvp("_id", oid)));
        Entity::SNS::Message result;
        result.FromDocument(mResult);

        return result;
    }

    Entity::SNS::Message SNSDatabase::GetMessageById(const std::string &oid) {
        return GetMessageById(bsoncxx::oid(oid));
    }

    /*Entity::SQS::Message SNSDatabase::GetMessageByReceiptHandle(const std::string &receiptHandle) {

        mongocxx::stdx::optional<bsoncxx::document::value> mResult = _messageCollection.find_one(make_document(kvp("receiptHandle", receiptHandle)));
        Entity::SQS::Message result;
        result.FromDocument(mResult);

        return result;
    }

    Entity::SQS::Message SNSDatabase::GetMessageById(const std::string &oid) {
        return GetMessageById(bsoncxx::oid(oid));
    }

    void SNSDatabase::ReceiveMessages(const std::string &region, const std::string &queueUrl, Entity::SQS::MessageList &messageList) {
        auto now = std::chrono::high_resolution_clock::now();

        auto messageCursor = _messageCollection.find(make_document(kvp("queueUrl", queueUrl), kvp("status", Entity::SQS::INITIAL)));
        for (auto message : messageCursor) {
            Entity::SQS::Message result;
            result.FromDocument(message);
            result.receiptHandle = Core::StringUtils::GenerateRandomString(120);
            messageList.push_back(result);
            _messageCollection.update_one(make_document(kvp("_id", message["_id"].get_oid())),
                                          make_document(kvp("$set", make_document(kvp("status", Entity::SQS::SEND),
                                                                                  kvp("reset", bsoncxx::types::b_date(now)),
                                                                                  kvp("receiptHandle", result.receiptHandle)))));
        }
        _logger.trace() << "Messages received, region: " << region << " queue: " << queueUrl + " count: " << messageList.size() << std::endl;
    }

    void SNSDatabase::ResetMessages(const std::string &queueUrl, long visibility) {

        long updated = 0;
        auto messageCursor = _messageCollection.find(make_document(kvp("queueUrl", queueUrl), kvp("status", Entity::SQS::SEND)));
        for (auto message : messageCursor) {
            _messageCollection.update_one(make_document(kvp("_id", message["_id"].get_oid())),
                                          make_document(kvp("$set",
                                                            make_document(kvp("status", Entity::SQS::INITIAL), kvp("receiptHandle", "")))));
            updated++;
        }
        _logger.trace() << "Message reset, visibility: " << visibility << " updated: " << updated << std::endl;
    }*/

    long SNSDatabase::CountMessages(const std::string &region, const std::string &topicArn) {

        long count = _messageCollection.count_documents(make_document(kvp("region", region), kvp("topicArn", topicArn)));
        _logger.trace() << "Count messages, result: " << count << std::endl;
        return count;
    }
/*
    long SNSDatabase::CountMessagesByStatus(const std::string &region, const std::string &queueUrl, int status) {

        long count = _messageCollection.count_documents(make_document(kvp("region", region), kvp("queueUrl", queueUrl), kvp("status", status)));
        _logger.trace() << "Count messages by status, status: " << status << " result: " << count << std::endl;
        return count;
    }*/

    void SNSDatabase::DeleteMessage(const Entity::SNS::Message &message) {
        auto result = _messageCollection.delete_one(make_document(kvp("messageId", message.messageId)));
        _logger.debug() << "Messages deleted, messageId: " << message.messageId << " count: " << result->deleted_count() << std::endl;
    }

    void SNSDatabase::DeleteAllMessages() {
        auto result = _messageCollection.delete_many({});
        _logger.debug() << "All messages deleted, count: " << result->deleted_count() << std::endl;
    }

} // namespace AwsMock::Database
