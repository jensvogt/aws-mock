//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/memorydb/SecretsManagerMemoryDb.h>

namespace AwsMock::Database {

  SecretsManagerMemoryDb::SecretsManagerMemoryDb() : _logger(Poco::Logger::get("SecretsManagerDb")) {}

  bool SecretsManagerMemoryDb::SecretExists(const std::string &region, const std::string &name) {

    return find_if(_secrets.begin(), _secrets.end(), [region, name](const std::pair<std::string, Entity::SecretsManager::Secret> &secret) {
      return secret.second.region == region && secret.second.name == name;
    }) != _secrets.end();
  }

  bool SecretsManagerMemoryDb::SecretExists(const Entity::SecretsManager::Secret &secret) {
    return SecretExists(secret.region, secret.name);
  }

  Entity::SecretsManager::Secret SecretsManagerMemoryDb::GetSecretById(const std::string &oid) {

    auto it = find_if(_secrets.begin(), _secrets.end(), [oid](const std::pair<std::string, Entity::SecretsManager::Secret> &secret) {
      return secret.first == oid;
    });

    if (it != _secrets.end()) {
      it->second.oid = oid;
      return it->second;
    }
    return {};
  }

  Entity::SecretsManager::Secret SecretsManagerMemoryDb::CreateSecret(const Entity::SecretsManager::Secret &secret) {
    Poco::ScopedLock lock(_secretMutex);

    std::string oid = Poco::UUIDGenerator().createRandom().toString();
    _secrets[oid] = secret;
    log_trace_stream(_logger) << "Secret created, oid: " << oid << std::endl;
    return GetSecretById(oid);

  }

}