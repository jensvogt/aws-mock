//
// Created by vogje01 on 29/05/2023.
//

#include "awsmock/repository/TransferDatabase.h"

namespace AwsMock::Database {

  using bsoncxx::builder::basic::kvp;
  using bsoncxx::builder::basic::make_array;
  using bsoncxx::builder::basic::make_document;

  TransferDatabase::TransferDatabase() : _logger(Poco::Logger::get("TransferDatabase")), _memoryDb(TransferMemoryDb::instance()) {}

  bool TransferDatabase::TransferExists(const std::string &region, const std::string &serverId) {

    if (HasDatabase()) {

      auto client = GetClient();
      mongocxx::collection _transferCollection = (*client)["awsmock"]["transfer"];
      int64_t count = _transferCollection.count_documents(make_document(kvp("region", region), kvp("serverId", serverId)));
      log_trace_stream(_logger) << "Transfer manager exists: " << (count > 0 ? "true" : "false") << std::endl;
      return count > 0;

    } else {

      return _memoryDb.TransferExists(region, serverId);

    }
  }

  bool TransferDatabase::TransferExists(const Entity::Transfer::Transfer &transfer) {

    return TransferExists(transfer.region, transfer.serverId);
  }

  bool TransferDatabase::TransferExists(const std::string &serverId) {

    if (HasDatabase()) {

      auto client = GetClient();
      mongocxx::collection _transferCollection = (*client)["awsmock"]["transfer"];
      int64_t count = _transferCollection.count_documents(make_document(kvp("serverId", serverId)));
      log_trace_stream(_logger) << "Transfer manager exists: " << (count > 0 ? "true" : "false") << std::endl;
      return count > 0;

    } else {

      return _memoryDb.TransferExists(serverId);

    }
  }

  bool TransferDatabase::TransferExists(const std::string &region, const std::vector<std::string> &protocols) {

    if (HasDatabase()) {

      bsoncxx::builder::basic::array mProtocol{};
      for (const auto &p : protocols) {
        mProtocol.append(p);
      }

      auto client = GetClient();
      mongocxx::collection _transferCollection = (*client)["awsmock"]["transfer"];
      int64_t count = _transferCollection.count_documents(make_document(kvp("region", region), kvp("protocols", make_document(kvp("$all", mProtocol)))));
      log_trace_stream(_logger) << "Transfer manager exists: " << (count > 0 ? "true" : "false") << std::endl;
      return count > 0;

    } else {

      return _memoryDb.TransferExists(region, protocols);

    }
  }

  Entity::Transfer::Transfer TransferDatabase::CreateTransfer(const Entity::Transfer::Transfer &transfer) {

    if (HasDatabase()) {

      auto client = GetClient();
      mongocxx::collection _transferCollection = (*client)["awsmock"]["transfer"];
      auto result = _transferCollection.insert_one(transfer.ToDocument());
      log_trace_stream(_logger) << "Bucket created, oid: " << result->inserted_id().get_oid().value.to_string() << std::endl;

      return GetTransferById(result->inserted_id().get_oid().value);

    } else {

      return _memoryDb.CreateTransfer(transfer);

    }
  }

  Entity::Transfer::Transfer TransferDatabase::GetTransferById(bsoncxx::oid oid) {

    auto client = GetClient();
    mongocxx::collection _transferCollection = (*client)["awsmock"]["transfer"];
    mongocxx::stdx::optional<bsoncxx::document::value> mResult = _transferCollection.find_one(make_document(kvp("_id", oid)));
    Entity::Transfer::Transfer result;
    result.FromDocument(mResult);
    return result;

  }

  Entity::Transfer::Transfer TransferDatabase::GetTransferById(const std::string &oid) {

    if (HasDatabase()) {

      return GetTransferById(bsoncxx::oid(oid));

    } else {

      return _memoryDb.GetTransferById(oid);

    }
  }

  Entity::Transfer::Transfer TransferDatabase::GetTransferByServerId(const std::string &serverId) {

    if (HasDatabase()) {

      auto client = GetClient();
      mongocxx::collection _transferCollection = (*client)["awsmock"]["transfer"];
      mongocxx::stdx::optional<bsoncxx::document::value> mResult = _transferCollection.find_one(make_document(kvp("serverId", serverId)));
      Entity::Transfer::Transfer result;
      result.FromDocument(mResult);
      return result;

    } else {

      return _memoryDb.GetTransferByServerId(serverId);

    }
  }

  Entity::Transfer::Transfer TransferDatabase::CreateOrUpdateTransfer(const Entity::Transfer::Transfer &lambda) {

    if (TransferExists(lambda)) {
      return UpdateTransfer(lambda);
    } else {
      return CreateTransfer(lambda);
    }
  }

  Entity::Transfer::Transfer TransferDatabase::UpdateTransfer(const Entity::Transfer::Transfer &transfer) {

    if (HasDatabase()) {

      auto client = GetClient();
      mongocxx::collection _transferCollection = (*client)["awsmock"]["transfer"];
      auto result = _transferCollection.replace_one(make_document(kvp("region", transfer.region), kvp("serverId", transfer.serverId)), transfer.ToDocument());
      log_trace_stream(_logger) << "Transfer updated: " << transfer.ToString() << std::endl;
      return GetTransferByServerId(transfer.serverId);

    } else {

      return _memoryDb.UpdateTransfer(transfer);

    }
  }

  Entity::Transfer::Transfer TransferDatabase::GetTransferByArn(const std::string &arn) {

    if (HasDatabase()) {

      auto client = GetClient();
      mongocxx::collection _transferCollection = (*client)["awsmock"]["transfer"];
      mongocxx::stdx::optional<bsoncxx::document::value> mResult = _transferCollection.find_one(make_document(kvp("arn", arn)));
      Entity::Transfer::Transfer result;
      result.FromDocument(mResult);
      return result;

    } else {

      return _memoryDb.GetTransferByArn(arn);

    }
  }

  std::vector<Entity::Transfer::Transfer> TransferDatabase::ListServers(const std::string &region) {

    std::vector<Entity::Transfer::Transfer> transfers;

    if (HasDatabase()) {

      try {

        auto client = GetClient();
        mongocxx::collection _transferCollection = (*client)["awsmock"]["transfer"];
        if (region.empty()) {

          auto transferCursor = _transferCollection.find(make_document());
          for (auto transfer : transferCursor) {
            Entity::Transfer::Transfer result;
            result.FromDocument(transfer);
            transfers.push_back(result);
          }

        } else {

          auto transferCursor = _transferCollection.find(make_document(kvp("region", region)));
          for (auto transfer : transferCursor) {
            Entity::Transfer::Transfer result;
            result.FromDocument(transfer);
            transfers.push_back(result);
          }
        }
        log_trace_stream(_logger) << "Got transfer list, size:" << transfers.size() << std::endl;

      } catch (mongocxx::exception::system_error &e) {
        log_error_stream(_logger) << "List servers failed, error: " << e.what() << std::endl;
      }

    } else {

      return _memoryDb.ListServers(region);

    }
    return transfers;
  }

  void TransferDatabase::DeleteTransfer(const std::string &serverId) {

    if (HasDatabase()) {

      auto client = GetClient();
      mongocxx::collection _transferCollection = (*client)["awsmock"]["transfer"];
      auto result = _transferCollection.delete_many(make_document(kvp("serverId", serverId)));
      log_debug_stream(_logger) << "Transfer deleted, serverId: " << serverId << " count: " << result->deleted_count() << std::endl;

    } else {

      _memoryDb.DeleteTransfer(serverId);

    }
  }

  void TransferDatabase::DeleteAllTransfers() {

    if (HasDatabase()) {

      auto client = GetClient();
      mongocxx::collection _transferCollection = (*client)["awsmock"]["transfer"];
      auto result = _transferCollection.delete_many({});
      log_debug_stream(_logger) << "All transfers deleted, count: " << result->deleted_count() << std::endl;

    } else {

      _memoryDb.DeleteAllTransfers();

    }
  }

} // namespace AwsMock::Database
