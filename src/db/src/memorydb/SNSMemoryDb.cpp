//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/memorydb/SNSMemoryDb.h>

namespace AwsMock::Database {

  SNSMemoryDb::SNSMemoryDb() : _logger(Poco::Logger::get("SQSMemoryDb")) {}

  bool SNSMemoryDb::TopicExists(const std::string &region, const std::string &name) {

    return find_if(_topics.begin(), _topics.end(), [region, name](const std::pair<std::string, Entity::SNS::Topic> &topic) {
      return topic.second.region == region && topic.second.topicName == name;
    }) != _topics.end();

  }

  bool SNSMemoryDb::TopicExists(const std::string &arn) {

    return find_if(_topics.begin(), _topics.end(), [arn](const std::pair<std::string, Entity::SNS::Topic> &topic) {
      return topic.second.topicArn == arn;
    }) != _topics.end();

  }

  Entity::SNS::Topic SNSMemoryDb::GetTopicById(const std::string &oid) {

    auto it = find_if(_topics.begin(), _topics.end(), [oid](const std::pair<std::string, Entity::SNS::Topic> &topic) {
      return topic.first == oid;
    });

    if (it != _topics.end()) {
      it->second.oid = oid;
      return it->second;
    }
    return {};
  }

  Entity::SNS::Topic SNSMemoryDb::GetTopicByArn(const std::string &topicArn) {

    auto it = find_if(_topics.begin(), _topics.end(), [topicArn](const std::pair<std::string, Entity::SNS::Topic> &topic) {
      return topic.second.topicArn == topicArn;
    });

    if (it != _topics.end()) {
      it->second.oid = it->first;
      return it->second;
    }
    return {};
  }

  Entity::SNS::TopicList SNSMemoryDb::GetTopicsBySubscriptionArn(const std::string &subscriptionArn) {

    Entity::SNS::TopicList topics;
    for (const auto &topic : _topics) {
      if (topic.second.subscriptions.size() > 0) {
        auto it = find_if(topic.second.subscriptions.begin(), topic.second.subscriptions.end(), [subscriptionArn](const Entity::SNS::Subscription &subcription) {
          return subcription.subscriptionArn == subscriptionArn;
        });
        if (it != topic.second.subscriptions.end()) {
          topics.emplace_back(topic.second);
        }
      }
    }
    return topics;
  }

  Entity::SNS::Topic SNSMemoryDb::CreateTopic(const Entity::SNS::Topic &topic) {
    Poco::ScopedLock loc(_topicMutex);

    std::string oid = Poco::UUIDGenerator().createRandom().toString();
    _topics[oid] = topic;
    log_trace_stream(_logger) << "Topic created, oid: " << oid << std::endl;
    return GetTopicById(oid);

  }

  Entity::SNS::Topic SNSMemoryDb::UpdateTopic(const Entity::SNS::Topic &topic) {
    Poco::ScopedLock loc(_topicMutex);

    std::string region = topic.region;
    std::string name = topic.topicName;
    auto it = find_if(_topics.begin(), _topics.end(), [region, name](const std::pair<std::string, Entity::SNS::Topic> &topic) {
      return topic.second.region == region && topic.second.topicName == name;
    });
    _topics[it->first] = topic;
    return _topics[it->first];

  }

  Entity::SNS::TopicList SNSMemoryDb::ListTopics(const std::string &region) {

    Entity::SNS::TopicList topicList;
    if (region.empty()) {

      for (const auto &topic : _topics) {
        topicList.emplace_back(topic.second);
      }

    } else {

      for (const auto &topic : _topics) {
        if (topic.second.region == region) {
          topicList.emplace_back(topic.second);
        }
      }

    }

    log_trace_stream(_logger) << "Got topic list, size: " << topicList.size() << std::endl;
    return topicList;
  }

  long SNSMemoryDb::CountTopics(const std::string &region) {

    long count = 0;
    if (region.empty()) {

      count = static_cast<long>(_topics.size());

    } else {

      for (const auto &topic : _topics) {
        if (topic.second.region == region) {
          count++;
        }
      }
    }
    return count;
  }

  void SNSMemoryDb::DeleteTopic(const Entity::SNS::Topic &topic) {
    Poco::ScopedLock loc(_topicMutex);

    std::string region = topic.region;
    std::string arn = topic.topicArn;
    const auto count = std::erase_if(_topics, [region, arn](const auto &item) {
      auto const &[key, value] = item;
      return value.region == region && value.topicArn == arn;
    });
    log_debug_stream(_logger) << "Topic deleted, count: " << count << std::endl;
  }

  void SNSMemoryDb::DeleteAllTopics() {
    Poco::ScopedLock loc(_topicMutex);

    log_debug_stream(_logger) << "All topics deleted, count: " << _topics.size() << std::endl;
    _topics.clear();
  }

  bool SNSMemoryDb::MessageExists(const std::string &id) {

    return find_if(_messages.begin(), _messages.end(), [id](const std::pair<std::string, Entity::SNS::Message> &message) {
      return message.first == id;
    }) != _messages.end();

  }

  Entity::SNS::Message SNSMemoryDb::CreateMessage(const Entity::SNS::Message &message) {
    Poco::ScopedLock loc(_messageMutex);

    std::string oid = Poco::UUIDGenerator().createRandom().toString();
    _messages[oid] = message;
    log_trace_stream(_logger) << "Message created, oid: " << oid << std::endl;
    return GetMessageById(oid);

  }

  Entity::SNS::Message SNSMemoryDb::GetMessageById(const std::string &oid) {

    auto it = find_if(_messages.begin(), _messages.end(), [oid](const std::pair<std::string, Entity::SNS::Message> &message) {
      return message.first == oid;
    });

    if (it != _messages.end()) {
      it->second.oid = oid;
      return it->second;
    }
    return {};
  }

  long SNSMemoryDb::CountMessages(const std::string &region, const std::string &topicArn) {

    long count = 0;
    for (const auto &message : _messages) {
      if (!region.empty() && message.second.region == region && !topicArn.empty() && message.second.topicArn == topicArn) {
        count++;
      } else if (!region.empty() && message.second.region == region) {
        count++;
      } else if (!topicArn.empty() && message.second.topicArn == topicArn) {
        count++;
      } else {
        count++;
      }
    }
    return count;
  }

  long SNSMemoryDb::CountMessagesByStatus(const std::string &region, const std::string &topicArn, Entity::SNS::MessageStatus status) {

    long count = 0;
    for (const auto &message : _messages) {
      if (!region.empty() && message.second.region == region && !topicArn.empty() && message.second.topicArn == topicArn && message.second.status == status) {
        count++;
      }
    }
    return count;
  }

  Entity::SNS::MessageList SNSMemoryDb::ListMessages(const std::string &region) {

    Entity::SNS::MessageList messageList;
    if (region.empty()) {

      for (const auto &message : _messages) {
        messageList.emplace_back(message.second);
      }

    } else {

      for (const auto &message : _messages) {
        if (message.second.region == region) {
          messageList.emplace_back(message.second);
        }
      }
    }

    log_trace_stream(_logger) << "Got message list, size: " << messageList.size() << std::endl;
    return messageList;
  }

  Entity::SNS::Message SNSMemoryDb::UpdateMessage(Entity::SNS::Message &message) {
    Poco::ScopedLock lock(_messageMutex);

    std::string oid = message.oid;
    auto it = find_if(_messages.begin(), _messages.end(), [oid](const std::pair<std::string, Entity::SNS::Message> &message) {
      return message.second.oid == oid;
    });
    _messages[it->first] = message;
    return _messages[it->first];
  }

  void SNSMemoryDb::DeleteMessage(const Entity::SNS::Message &message) {
    Poco::ScopedLock loc(_messageMutex);

    std::string messageId = message.messageId;
    const auto count = std::erase_if(_messages, [messageId](const auto &item) {
      auto const &[key, value] = item;
      return value.messageId == messageId;
    });
    log_debug_stream(_logger) << "Message deleted, messageId: " << message.messageId << " count: " << count << std::endl;
  }

  void SNSMemoryDb::DeleteMessages(const std::string &region, const std::string &topicArn, const std::vector<std::string> &messageIds) {
    Poco::ScopedLock loc(_messageMutex);

    long count = 0;
    for (auto &messageId : messageIds) {
      count += static_cast<long>(std::erase_if(_messages, [region, topicArn, messageId](const auto &item) {
        auto const &[key, value] = item;
        return value.region == region && value.topicArn == topicArn && value.messageId == messageId;
      }));
    }
    log_debug_stream(_logger) << "Messages deleted, count: " << count << " count: " << count << std::endl;
  }

  void SNSMemoryDb::DeleteAllMessages() {
    Poco::ScopedLock loc(_messageMutex);

    log_debug_stream(_logger) << "All messages deleted, count: " << _messages.size() << std::endl;
    _messages.clear();
  }
}