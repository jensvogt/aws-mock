//
// Created by vogje01 on 29/05/2023.
//

#include "awsmock/repository/SNSDatabase.h"

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::builder::stream::document;

    SNSDatabase::SNSDatabase(const Core::Configuration &configuration) : Database(configuration), _logger(Poco::Logger::get("SNSDatabase")) {

        // Get collections
        _topicCollection = GetConnection()["sns_topic"];
        _messageCollection = GetConnection()["sns_message"];
    }

    bool SNSDatabase::TopicExists(const std::string &topicArn) {

        int64_t count = _topicCollection.count_documents(make_document(kvp("topicArn", topicArn)));
        log_trace_stream(_logger) << "Topic exists: " << (count > 0 ? "true" : "false") << std::endl;

        return count > 0;
    }

    bool SNSDatabase::TopicExists(const std::string &region, const std::string &topicName) {

        int64_t count = _topicCollection.count_documents(make_document(kvp("region", region), kvp("topicName", topicName)));
        log_trace_stream(_logger) << "Topic exists: " << (count > 0 ? "true" : "false") << std::endl;
        return count > 0;
    }

    Entity::SNS::Topic SNSDatabase::CreateTopic(const Entity::SNS::Topic &topic) {

        auto result = _topicCollection.insert_one(topic.ToDocument());
        log_trace_stream(_logger) << "Topic created, oid: " << result->inserted_id().get_oid().value.to_string() << std::endl;

        return GetTopicById(result->inserted_id().get_oid().value);
    }

    Entity::SNS::Topic SNSDatabase::GetTopicById(bsoncxx::oid oid) {

        mongocxx::stdx::optional<bsoncxx::document::value> mResult = _topicCollection.find_one(make_document(kvp("_id", oid)));

        if (mResult->empty()) {
            return {};
        }

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

        log_trace_stream(_logger) << "Got topic list, size:" << topicList.size() << std::endl;
        return topicList;
    }

    Entity::SNS::Topic SNSDatabase::UpdateTopic(const Entity::SNS::Topic &topic) {

        auto result = _topicCollection.replace_one(make_document(kvp("region", topic.region), kvp("topicArn", topic.topicArn)), topic.ToDocument());

        log_trace_stream(_logger) << "Topic updated: " << topic.ToString() << std::endl;

        return GetTopicByArn(topic.topicArn);
    }

    long SNSDatabase::CountTopics(const std::string &region) {

        long count = _topicCollection.count_documents(make_document(kvp("region", region)));
        log_trace_stream(_logger) << "Count topics, result: " << count << std::endl;

        return count;
    }

    void SNSDatabase::DeleteTopic(const Entity::SNS::Topic &topic) {

        auto result = _topicCollection.delete_many(make_document(kvp("topicArn", topic.topicArn)));
        log_debug_stream(_logger) << "Topic deleted, count: " << result->deleted_count() << std::endl;
    }

    void SNSDatabase::DeleteAllTopics() {

        auto result = _topicCollection.delete_many({});
        log_debug_stream(_logger) << "All topics deleted, count: " << result->deleted_count() << std::endl;
    }

    Entity::SNS::Message SNSDatabase::CreateMessage(const Entity::SNS::Message &message) {

        try {

            auto result = _messageCollection.insert_one(message.ToDocument());
            log_trace_stream(_logger) << "Message created, oid: " << result->inserted_id().get_oid().value.to_string() << std::endl;

            return GetMessageById(result->inserted_id().get_oid().value);

        } catch (const mongocxx::exception& exc) {
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

    long SNSDatabase::CountMessages(const std::string &region, const std::string &topicArn) {

        long count = _messageCollection.count_documents(make_document(kvp("region", region),kvp("topicArn", topicArn)));
        log_trace_stream(_logger) << "Count messages, result: " << count << std::endl;

        return count;
    }

    void SNSDatabase::DeleteMessage(const Entity::SNS::Message &message) {

        auto result = _messageCollection.delete_one(make_document(kvp("messageId", message.messageId)));
        log_debug_stream(_logger) << "Messages deleted, messageId: " << message.messageId << " count: " << result->deleted_count() << std::endl;
    }

    void SNSDatabase::DeleteMessages(const std::string &region, const std::string &topicArn, const std::vector<std::string> &receipts) {

        bsoncxx::builder::basic::array array{};
        for (const auto &receipt : receipts) {
            array.append(receipt);
        }

        auto result = _messageCollection.delete_many(make_document(kvp("region", region), kvp("topicArn", topicArn), kvp("messageId", make_document(kvp("$in", array)))));
        log_debug_stream(_logger) << "Messages deleted, count: " << result->result().deleted_count() << std::endl;
    }

    void SNSDatabase::DeleteAllMessages() {
        auto result = _messageCollection.delete_many({});
        log_debug_stream(_logger) << "All messages deleted, count: " << result->deleted_count() << std::endl;
    }

} // namespace AwsMock::Database
