//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/kms/KMSCreator.h>

namespace AwsMock::Service {

    void KMSCreator::CreateKmsKey(const std::string &keyId) {

        log_debug << "Start creating KMS key, keyId: " << keyId;

        // Get database connection
        Database::KMSDatabase &kmsDatabase = Database::KMSDatabase::instance();

        // Make local copy
        Database::Entity::KMS::Key key = kmsDatabase.GetKeyByKeyId(keyId);
        key.keyState = Dto::KMS::KeyStateToString(Dto::KMS::KeyState::ENABLED);

        switch (Dto::KMS::KeySpecFromString(key.keySpec)) {

            case Dto::KMS::KeySpec::SYMMETRIC_DEFAULT: {
                GenerateAes256Key(key);
                break;
            }

            case Dto::KMS::KeySpec::RSA_2048: {
                GenerateRsaKeyPair(key, 2048);
                break;
            }

            case Dto::KMS::KeySpec::RSA_3072: {
                GenerateRsaKeyPair(key, 3072);
                break;
            }

            case Dto::KMS::KeySpec::RSA_4096: {
                GenerateRsaKeyPair(key, 4096);
                break;
            }

            case Dto::KMS::KeySpec::HMAC_224:
            case Dto::KMS::KeySpec::HMAC_256:
            case Dto::KMS::KeySpec::HMAC_384:
            case Dto::KMS::KeySpec::HMAC_512:
            case Dto::KMS::KeySpec::ECC_NIST_P256:
            case Dto::KMS::KeySpec::ECC_NIST_P384:
            case Dto::KMS::KeySpec::ECC_NIST_P521:
            case Dto::KMS::KeySpec::ECC_SECG_P256K1: {
                log_warning << "Encryption algorithm not yet implemented, name: " << key.keySpec;
                break;
            }
        }

        // Save to database
        key = kmsDatabase.UpdateKey(key);

        log_debug << "KMS key created, keyId: " << keyId;
    }

    void KMSCreator::GenerateAes256Key(Database::Entity::KMS::Key &key) {
        log_debug << "Start creating AES256 KMS key, keyId: " << key.keyId;

        unsigned char keyMaterial[32], iv[32];
        Core::Crypto::CreateAes256Key(keyMaterial, iv);

        // Base64 hashing
        std::string heyKex = Core::Crypto::HexEncode(keyMaterial, 32);
        std::string hexIv = Core::Crypto::HexEncode(iv, 16);
        log_debug << "KMS keys created";

        key.aes256Key = heyKex;
        key.aes256Iv = hexIv;

        log_debug << "AES256 KMS key created, keyId: " << key.keyId;
    }

    void KMSCreator::GenerateRsaKeyPair(Database::Entity::KMS::Key &key, int length) {
        log_debug << "Start creating RSA KMS key pair, keyId: " << key.keyId << " length: " << length;

        EVP_PKEY *keyMaterial = Core::Crypto::GenerateRsaKeys(length);
        std::string publicKey = Core::Crypto::GetRsaPublicKey(keyMaterial);
        std::string privateKey = Core::Crypto::GetRsaPrivateKey(keyMaterial);

        key.rsaPublicKey = publicKey;
        key.rsaPrivateKey = privateKey;

        log_debug << "AES256 KMS key created, keyId: " << key.keyId;
    }

}// namespace AwsMock::Service
