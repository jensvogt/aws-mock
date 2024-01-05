//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/DynamoDbDatabase.h>

namespace AwsMock::Database {

  using bsoncxx::builder::basic::kvp;
  using bsoncxx::builder::basic::make_array;
  using bsoncxx::builder::basic::make_document;

  DynamoDbDatabase::DynamoDbDatabase(Core::Configuration &configuration) : Database(configuration), _logger(Poco::Logger::get("DynamoDbDatabase")), _memoryDb(DynamoDbMemoryDb::instance()) {

    if (HasDatabase()) {

      // Get collection
      _tableCollection = GetConnection()["dynamodb_table"];
      _itemCollection = GetConnection()["dynamodb_item"];

    }
  }

  Entity::DynamoDb::Table DynamoDbDatabase::CreateTable(const Entity::DynamoDb::Table &table) {

    if (HasDatabase()) {

      try {
        auto result = _tableCollection.insert_one(table.ToDocument());
        log_trace_stream(_logger) << "DynamoDb table created, oid: " << result->inserted_id().get_oid().value.to_string() << std::endl;
        return GetTableById(result->inserted_id().get_oid().value);

      } catch (const mongocxx::exception &exc) {
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException("Database exception " + std::string(exc.what()), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      return _memoryDb.CreateTable(table);

    }
  }

  Entity::DynamoDb::Table DynamoDbDatabase::GetTableById(bsoncxx::oid oid) {

    try {

      mongocxx::stdx::optional<bsoncxx::document::value> mResult = _tableCollection.find_one(make_document(kvp("_id", oid)));
      if (!mResult) {
        _logger.error() << "Database exception: Table not found " << std::endl;
        throw Core::DatabaseException("Database exception, Table not found ", Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

      Entity::DynamoDb::Table result;
      result.FromDocument(mResult);
      _logger.debug() << "Got table by ID, table: " << result.ToString() << std::endl;
      return result;

    } catch (const mongocxx::exception &exc) {
      _logger.error() << "Database exception " << exc.what() << std::endl;
      throw Core::DatabaseException("Database exception " + std::string(exc.what()), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
    }

  }

  Entity::DynamoDb::Table DynamoDbDatabase::GetTableById(const std::string &oid) {

    if (HasDatabase()) {

      return GetTableById(bsoncxx::oid(oid));

    } else {

      return _memoryDb.GetTableById(oid);
    }
  }

  bool DynamoDbDatabase::TableExists(const std::string &region, const std::string &tableName) {

    if (HasDatabase()) {

      try {

        int64_t count;
        if (!region.empty()) {
          count = _tableCollection.count_documents(make_document(kvp("region", region), kvp("name", tableName)));
        } else {
          count = _tableCollection.count_documents(make_document(kvp("name", tableName)));
        }
        log_trace_stream(_logger) << "DynamoDb table exists: " << (count > 0 ? "true" : "false") << std::endl;
        return count > 0;

      } catch (const mongocxx::exception &exc) {
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException("Database exception " + std::string(exc.what()), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      return _memoryDb.TableExists(region, tableName);

    }
  }

  std::vector<Entity::DynamoDb::Table> DynamoDbDatabase::ListTables(const std::string &region) {

    Entity::DynamoDb::TableList tables;
    if (HasDatabase()) {

      try {

        if (region.empty()) {

          auto tableCursor = _tableCollection.find({});
          for (auto table : tableCursor) {
            Entity::DynamoDb::Table result;
            result.FromDocument(table);
            tables.push_back(result);
          }

        } else {

          auto tableCursor = _tableCollection.find(make_document(kvp("region", region)));
          for (auto table : tableCursor) {
            Entity::DynamoDb::Table result;
            result.FromDocument(table);
            tables.push_back(result);
          }

        }

      } catch (const mongocxx::exception &exc) {
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException("Database exception " + std::string(exc.what()), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      tables = _memoryDb.ListTables(region);
    }

    log_trace_stream(_logger) << "Got DynamoDb table list, size:" << tables.size() << std::endl;
    return tables;
  }

  void DynamoDbDatabase::DeleteTable(const std::string &tableName) {

    if (HasDatabase()) {

      try {

        auto result = _tableCollection.delete_many(make_document(kvp("name", tableName)));
        log_debug_stream(_logger) << "DynamoDB table deleted, tableName: " << tableName << " count: " << result->deleted_count() << std::endl;

      } catch (const mongocxx::exception &exc) {
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException("Database exception " + std::string(exc.what()), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      _memoryDb.DeleteTable(tableName);

    }
  }

  void DynamoDbDatabase::DeleteAllTables() {

    if (HasDatabase()) {

      try {

        auto result = _tableCollection.delete_many({});
        log_debug_stream(_logger) << "All DynamoDb tables deleted, count: " << result->deleted_count() << std::endl;

      } catch (const mongocxx::exception &exc) {
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException("Database exception " + std::string(exc.what()), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      _memoryDb.DeleteAllTables();

    }
  }

  bool DynamoDbDatabase::ItemExists(const std::string &region, const std::string &tableName, const std::string &key) {

    if (HasDatabase()) {

      try {

        int64_t count;
        if (!region.empty()) {
          count = _tableCollection.count_documents(make_document(kvp("region", region), kvp("name", tableName)));
        } else {
          count = _tableCollection.count_documents(make_document(kvp("name", tableName)));
        }
        log_trace_stream(_logger) << "DynamoDb table exists: " << (count > 0 ? "true" : "false") << std::endl;
        return count > 0;

      } catch (const mongocxx::exception &exc) {
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException("Database exception " + std::string(exc.what()), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      return _memoryDb.ItemExists(region, tableName, key);

    }
  }

  Entity::DynamoDb::ItemList DynamoDbDatabase::ListItems(const std::string &region, const std::string &tableName) {

    Entity::DynamoDb::ItemList items;
    if (HasDatabase()) {

      try {

        if (region.empty() && tableName.empty()) {

          auto itemCursor = _itemCollection.find({});
          for (auto item : itemCursor) {
            Entity::DynamoDb::Item result;
            result.FromDocument(item);
            items.push_back(result);
          }

        } else if (tableName.empty()) {

          auto itemCursor = _itemCollection.find(make_document(kvp("region", region)));
          for (auto item : itemCursor) {
            Entity::DynamoDb::Item result;
            result.FromDocument(item);
            items.push_back(result);
          }

        } else {

          auto itemCursor = _itemCollection.find(make_document(kvp("region", region), kvp("name", tableName)));
          for (auto item : itemCursor) {
            Entity::DynamoDb::Item result;
            result.FromDocument(item);
            items.push_back(result);
          }

        }

      } catch (const mongocxx::exception &exc) {
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException("Database exception " + std::string(exc.what()), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      items = _memoryDb.ListItems(region, tableName);
    }

    log_trace_stream(_logger) << "Got DynamoDb item list, size:" << items.size() << std::endl;
    return items;
  }

  void DynamoDbDatabase::DeleteItem(const std::string &region, const std::string &tableName, const std::string &key) {

    if (HasDatabase()) {

      try {

        auto result = _itemCollection.delete_many(make_document(kvp("name", tableName)));
        log_debug_stream(_logger) << "DynamoDB item deleted, tableName: " << tableName << " count: " << result->deleted_count() << std::endl;

      } catch (const mongocxx::exception &exc) {
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException("Database exception " + std::string(exc.what()), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      _memoryDb.DeleteItem(region, tableName, key);

    }
  }

  void DynamoDbDatabase::DeleteAllItems() {

    if (HasDatabase()) {

      try {

        auto result = _itemCollection.delete_many({});
        log_debug_stream(_logger) << "DynamoDB items deleted, count: " << result->deleted_count() << std::endl;

      } catch (const mongocxx::exception &exc) {
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException("Database exception " + std::string(exc.what()), Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
      }

    } else {

      _memoryDb.DeleteAllItems();

    }
  }

} // namespace AwsMock::Database