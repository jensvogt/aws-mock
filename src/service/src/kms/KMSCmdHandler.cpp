
#include <awsmock/service/kms/KMSCmdHandler.h>

namespace AwsMock::Service {

    void KMSCmdHandler::handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const Dto::Common::KMSClientCommand &kmsClientCommand) {
        log_trace << "KMS POST request: " << kmsClientCommand.ToJson();

        std::string requestId = GetHeaderValue(request, "RequestId", Poco::UUIDGenerator().createRandom().toString());

        switch (kmsClientCommand.command) {

            case Dto::Common::KMSCommandType::CREATE_KEY: {

                Dto::KMS::CreateKeyRequest kmsRequest;
                kmsRequest.FromJson(kmsClientCommand.payload);
                kmsRequest.region = kmsClientCommand.region;

                Dto::KMS::CreateKeyResponse kmsResponse = _kmsService.CreateKey(kmsRequest);

                SendOkResponse(response, kmsResponse.ToJson());
                log_info << "Key created, keyId: " << kmsResponse.key.keyId;

                break;
            }

            case Dto::Common::KMSCommandType::SCHEDULE_KEY_DELETION: {

                Dto::KMS::ScheduleKeyDeletionRequest kmsRequest;
                kmsRequest.FromJson(kmsClientCommand.payload);
                kmsRequest.region = kmsClientCommand.region;

                Dto::KMS::ScheduledKeyDeletionResponse kmsResponse = _kmsService.ScheduleKeyDeletion(kmsRequest);

                SendOkResponse(response, kmsResponse.ToJson());
                log_info << "Key deletion scheduled, keyId: " << kmsResponse.keyId;

                break;
            }

            case Dto::Common::KMSCommandType::LIST_KEYS: {

                Dto::KMS::ListKeysRequest kmsRequest;
                kmsRequest.FromJson(kmsClientCommand.payload);
                kmsRequest.region = kmsClientCommand.region;

                Dto::KMS::ListKeysResponse kmsResponse = _kmsService.ListKeys(kmsRequest);

                SendOkResponse(response, kmsResponse.ToJson());
                log_info << "List keys received, count: " << kmsResponse.keys.size();

                break;
            }

            case Dto::Common::KMSCommandType::DESCRIBE_KEY: {

                Dto::KMS::DescribeKeyRequest kmsRequest;
                kmsRequest.FromJson(kmsClientCommand.payload);
                kmsRequest.region = kmsClientCommand.region;

                Dto::KMS::DescribeKeyResponse kmsResponse = _kmsService.DescribeKey(kmsRequest);

                SendOkResponse(response, kmsResponse.ToJson());
                log_info << "Describe key received, count: " << kmsResponse.key.keyId;

                break;
            }

            case Dto::Common::KMSCommandType::ENCRYPT: {

                Dto::KMS::EncryptRequest kmsRequest;
                kmsRequest.FromJson(kmsClientCommand.payload);
                kmsRequest.region = kmsClientCommand.region;

                Dto::KMS::EncryptResponse kmsResponse = _kmsService.Encrypt(kmsRequest);

                SendOkResponse(response, kmsResponse.ToJson());
                log_info << "Encrypt received, size: " << kmsResponse.ciphertext.length();

                break;
            }

            case Dto::Common::KMSCommandType::DECRYPT: {

                Dto::KMS::DecryptRequest kmsRequest;
                kmsRequest.FromJson(kmsClientCommand.payload);
                kmsRequest.region = kmsClientCommand.region;

                Dto::KMS::DecryptResponse kmsResponse = _kmsService.Decrypt(kmsRequest);

                SendOkResponse(response, kmsResponse.ToJson());
                log_info << "Decrypt received, size: " << kmsResponse.plaintext.length();

                break;
            }

            case Dto::Common::KMSCommandType::UNKNOWN: {
                log_error << "Unimplemented command called";
                throw Core::ServiceException("Unimplemented command called");
            }
        }
    }
}// namespace AwsMock::Service
