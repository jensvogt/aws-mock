//
// Created by vogje01 on 4/25/24.
//

#include <awsmock/dto/kms/DecryptRequest.h>

namespace AwsMock::Dto::KMS {

    void DecryptRequest::FromJson(const std::string &jsonString) {

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(jsonString);
        const auto &rootObject = result.extract<Poco::JSON::Object::Ptr>();

        try {

            // Attributes
            std::string tmpStr;
            Core::JsonUtils::GetJsonValueString("Region", rootObject, region);
            Core::JsonUtils::GetJsonValueString("KeyId", rootObject, keyId);
            Core::JsonUtils::GetJsonValueBool("DryRun", rootObject, dryRun);
            Core::JsonUtils::GetJsonValueString("CiphertextBlob", rootObject, ciphertext);
            Core::JsonUtils::GetJsonValueString("EncryptionAlgorithm", rootObject, tmpStr);
            if (!tmpStr.empty()) {
                encryptionAlgorithm = Dto::KMS::EncryptionAlgorithmsFromString(tmpStr);
            }

            // Grant tokens
            if (rootObject->has("GrantTokens")) {
                Poco::JSON::Array::Ptr jsonTokenArray = rootObject->getArray("GrantTokens");
                for (int i = 0; i < jsonTokenArray->size(); i++) {
                    grantTokens.emplace_back(jsonTokenArray->getElement<std::string>(i));
                }
            }

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    std::string DecryptRequest::ToJson() const {

        try {
            Poco::JSON::Object rootJson;
            rootJson.set("Region", region);
            rootJson.set("KeyId", keyId);
            rootJson.set("DryRun", dryRun);
            rootJson.set("CiphertextBlob", ciphertext);
            rootJson.set("EncryptionAlgorithm", Dto::KMS::EEncryptionAlgorithmsToString(encryptionAlgorithm));

            // Algorithms
            Poco::JSON::Array grantsArray;
            for (const auto &token: grantTokens) {
                grantsArray.add(token);
            }
            rootJson.set("GrantTokens", grantsArray);

            return Core::JsonUtils::ToJsonString(rootJson);

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    std::string DecryptRequest::ToString() const {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const DecryptRequest &r) {
        os << "DecryptRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::KMS
