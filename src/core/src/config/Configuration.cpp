//
// Created by vogje01 on 22/08/2022.
//

#include <awsmock/core/Configuration.h>

namespace AwsMock::Core {

    Configuration::Configuration() {
        Initialize();
    }

    Configuration::Configuration(const std::string &filename) {
        SetFilename(filename);
        Initialize();
    }

    void Configuration::Initialize() {

        // General
        DefineStringProperty("awsmock.region", "AWSMOCK_REGION", "eu-central-1");
        DefineStringProperty("awsmock.account.userPoolId", "AWSMOCK_ACCOUNT_ID", "000000000000");
        DefineStringProperty("awsmock.client.userPoolId", "AWSMOCK_CLIENT_ID", "00000000");
        DefineStringProperty("awsmock.user", "AWSMOCK_USER", "none");
        DefineStringProperty("awsmock.data.dir", "AWSMOCK_DATA_DIR", "/tmp/awsmock/data");
        DefineStringProperty("awsmock.temp.dir", "AWSMOCK_TEMP_DIR", "/tmp/awsmock/tmp");
        DefineBoolProperty("awsmock.pretty", "AWSMOCK_PRETTY", false);

        // Manager
        DefineStringProperty("awsmock.manager.host", "AWSMOCK_MANAGER_HOST", "localhost");
        DefineIntProperty("awsmock.manager.port", "AWSMOCK_MANAGER_PORT", 4567);
        DefineIntProperty("awsmock.manager.http.max.queue", "AWSMOCK_MANAGER_MAX_QUEUE", 50);
        DefineIntProperty("awsmock.manager.http.max.threads", "AWSMOCK_MANAGER_MAX_THREADS", 10);

        // Gateway
        DefineBoolProperty("awsmock.service.gateway.active", "AWSMOCK_SERVICE_GATEWAY_ACTIVE", true);
        DefineStringProperty("awsmock.service.gateway.http.host", "AWSMOCK_SERVICE_GATEWAY_HOST", "localhost");
        DefineIntProperty("awsmock.service.gateway.http.port", "AWSMOCK_SERVICE_GATEWAY_PORT", 4566);
        DefineIntProperty("awsmock.service.gateway.http.max.queue", "AWSMOCK_SERVICE_GATEWAY_MAX_QUEUE", 250);
        DefineIntProperty("awsmock.service.gateway.http.max.threads", "AWSMOCK_SERVICE_GATEWAY_MAX_THREADS", 50);
        DefineIntProperty("awsmock.service.gateway.http.timeout", "AWSMOCK_SERVICE_GATEWAY_TIMEOUT", 900);

        // S3
        DefineBoolProperty("awsmock.service.s3.active", "AWSMOCK_SERVICE_S3_ACTIVE", true);
        DefineStringProperty("awsmock.service.s3.http.host", "AWSMOCK_SERVICE_S3_HOST", "localhost");
        DefineIntProperty("awsmock.service.s3.http.port", "AWSMOCK_SERVICE_S3_PORT", 9500);
        DefineIntProperty("awsmock.service.s3.http.max.queue", "AWSMOCK_SERVICE_S3_MAX_QUEUE", 250);
        DefineIntProperty("awsmock.service.s3.http.max.threads", "AWSMOCK_SERVICE_S3_MAX_THREADS", 50);
        DefineIntProperty("awsmock.service.s3.http.timeout", "AWSMOCK_SERVICE_S3_TIMEOUT", 900);
        DefineIntProperty("awsmock.service.s3.monitoring.period", "AWSMOCK_SERVICE_S3_MONITORING_PERIOD", 900);

        // SQS
        DefineBoolProperty("awsmock.service.sqs.active", "AWSMOCK_SERVICE_SQS_ACTIVE", true);
        DefineStringProperty("awsmock.service.sqs.http.host", "AWSMOCK_SERVICE_SQS_HOST", "localhost");
        DefineIntProperty("awsmock.service.sqs.http.port", "AWSMOCK_SERVICE_SQS_PORT", 9501);
        DefineIntProperty("awsmock.service.sqs.http.max.queue", "AWSMOCK_SERVICE_SQS_MAX_QUEUE", 250);
        DefineIntProperty("awsmock.service.sqs.http.max.threads", "AWSMOCK_SERVICE_SQS_MAX_THREADS", 50);
        DefineIntProperty("awsmock.service.sqs.http.timeout", "AWSMOCK_SERVICE_SQS_TIMEOUT", 900);
        DefineStringProperty("awsmock.service.sqs.hostname", "AWSMOCK_SERVICE_SQS_HOSTNAME", "localstack");
        DefineIntProperty("awsmock.service.s3.monitoring.period", "AWSMOCK_MONITORING_SQS_PERIOD", 300);
        DefineIntProperty("awsmock.service.s3.worker.period", "AWSMOCK_WORKER_SQS_PERIOD", 30);

        // SNS
        DefineBoolProperty("awsmock.service.sns.active", "AWSMOCK_SERVICE_SNS_ACTIVE", true);
        DefineStringProperty("awsmock.service.sns.http.host", "AWSMOCK_SERVICE_SNS_HOST", "localhost");
        DefineIntProperty("awsmock.service.sns.http.port", "AWSMOCK_SERVICE_SNS_PORT", 9502);
        DefineIntProperty("awsmock.service.sns.http.max.queue", "AWSMOCK_SERVICE_SNS_MAX_QUEUE", 250);
        DefineIntProperty("awsmock.service.sns.http.max.threads", "AWSMOCK_SERVICE_SNS_MAX_THREADS", 50);
        DefineIntProperty("awsmock.service.sns.http.timeout", "AWSMOCK_SERVICE_SNS_TIMEOUT", 900);
        DefineIntProperty("awsmock.service.sns.message.timeout", "AWSMOCK_SERVICE_SNS_MESSAGE_TIMEOUT", 14);
        DefineStringProperty("awsmock.service.sns.hostname", "AWSMOCK_SERVICE_SNS_HOSTNAME", "localhost");
        DefineIntProperty("awsmock.monitoring.sns.worker.period", "AWSMOCK_SNS_WORKER_PERIOD", 300);
        DefineIntProperty("awsmock.monitoring.sns.monitoring.period", "AWSMOCK_SNS_MONITORING_PERIOD", 300);

        // Lambda
        DefineBoolProperty("awsmock.service.lambda.active", "AWSMOCK_SERVICE_LAMBDA_ACTIVE", true);
        DefineStringProperty("awsmock.service.lambda.http.host", "AWSMOCK_SERVICE_LAMBDA_HOST", "localhost");
        DefineIntProperty("awsmock.service.lambda.http.port", "AWSMOCK_SERVICE_LAMBDA_PORT", 9503);
        DefineIntProperty("awsmock.service.lambda.http.max.queue", "AWSMOCK_SERVICE_LAMBDA_MAX_QUEUE", 250);
        DefineIntProperty("awsmock.service.lambda.http.max.threads", "AWSMOCK_SERVICE_LAMBDA_MAX_THREADS", 50);
        DefineIntProperty("awsmock.service.lambda.http.timeout", "AWSMOCK_SERVICE_LAMBDA_TIMEOUT", 900);
        DefineIntProperty("awsmock.monitoring.lambda.period", "AWSMOCK_MONITORING_LAMBDA_PERIOD", 300);

        // Transfer server
        DefineBoolProperty("awsmock.service.transfer.active", "AWSMOCK_SERVICE_TRANSFER_ACTIVE", true);
        DefineStringProperty("awsmock.service.transfer.http.host", "AWSMOCK_SERVICE_TRANSFER_HOST", "localhost");
        DefineIntProperty("awsmock.service.transfer.http.port", "AWSMOCK_SERVICE_TRANSFER_PORT", 9504);
        DefineIntProperty("awsmock.service.transfer.http.max.queue", "AWSMOCK_SERVICE_TRANSFER_MAX_QUEUE", 250);
        DefineIntProperty("awsmock.service.transfer.http.max.threads", "AWSMOCK_SERVICE_TRANSFER_MAX_THREADS", 50);
        DefineIntProperty("awsmock.service.transfer.http.timeout", "AWSMOCK_SERVICE_TRANSFER_TIMEOUT", 900);
        DefineIntProperty("awsmock.service.transfer.ftp.port", "AWSMOCK_SERVICE_TRANSFER_FTP_PORT", 21);
        DefineStringProperty("awsmock.service.transfer.bucket", "AWSMOCK_SERVICE_TRANSFER_BUCKET", "transfer-server");
        DefineStringProperty("awsmock.service.transfer.base.dir", "AWSMOCK_SERVICE_TRANSFER_BASE_DIR", "/tmp/awsmock/data/transfer");
        DefineIntProperty("awsmock.service.transfer.monitoring.period", "AWSMOCK_SERVICE_TRANSFER_MONITORING_PERIOD", 300);

        // Cognito
        DefineBoolProperty("awsmock.service.cognito.active", "AWSMOCK_SERVICE_COGNITO_ACTIVE", true);
        DefineStringProperty("awsmock.service.cognito.http.host", "AWSMOCK_SERVICE_COGNITO_HOST", "localhost");
        DefineIntProperty("awsmock.service.cognito.http.port", "AWSMOCK_SERVICE_COGNITO_PORT", 9505);
        DefineIntProperty("awsmock.service.cognito.http.max.queue", "AWSMOCK_SERVICE_COGNITO_MAX_QUEUE", 250);
        DefineIntProperty("awsmock.service.cognito.http.max.threads", "AWSMOCK_SERVICE_COGNITO_MAX_THREADS", 50);
        DefineIntProperty("awsmock.service.cognito.http.timeout", "AWSMOCK_SERVICE_COGNITO_TIMEOUT", 120);
        DefineIntProperty("awsmock.service.cognito.monitoring.period", "AWSMOCK_SERVICE_COGNITO_MONITORING_PERIOD", 300);

        // DynamoDB
        DefineBoolProperty("awsmock.service.dynamodb.active", "AWSMOCK_SERVICE_DYNAMODB_ACTIVE", true);
        DefineStringProperty("awsmock.service.dynamodb.http.host", "AWSMOCK_SERVICE_DYNAMODB_HOST", "localhost");
        DefineIntProperty("awsmock.service.dynamodb.http.port", "AWSMOCK_SERVICE_DYNAMODB_PORT", 9506);
        DefineIntProperty("awsmock.service.dynamodb.http.max.queue", "AWSMOCK_SERVICE_DYNAMODB_MAX_QUEUE", 250);
        DefineIntProperty("awsmock.service.dynamodb.http.max.threads", "AWSMOCK_SERVICE_DYNAMODB_MAX_THREADS", 50);
        DefineIntProperty("awsmock.service.dynamodb.http.timeout", "AWSMOCK_SERVICE_DYNAMODB_TIMEOUT", 120);
        DefineIntProperty("awsmock.monitoring.dynamodb.period", "AWSMOCK_MONITORING_DYNAMODB_PERIOD", 300);

        // SecretsManager
        DefineBoolProperty("awsmock.service.secretsmanager.active", "AWSMOCK_SERVICE_SECRETSMANAGER_ACTIVE", true);
        DefineStringProperty("awsmock.service.secretsmanager.http.host", "AWSMOCK_SERVICE_SECRETSMANAGER_HOST", "localhost");
        DefineIntProperty("awsmock.service.secretsmanager.http.port", "AWSMOCK_SERVICE_SECRETSMANAGER_PORT", 9507);
        DefineIntProperty("awsmock.service.secretsmanager.http.max.queue", "AWSMOCK_SERVICE_SECRETSMANAGER_MAX_QUEUE", 250);
        DefineIntProperty("awsmock.service.secretsmanager.http.max.threads", "AWSMOCK_SERVICE_SECRETSMANAGER_MAX_THREADS", 50);
        DefineIntProperty("awsmock.service.secretsmanager.http.timeout", "AWSMOCK_SERVICE_SECRETSMANAGER_TIMEOUT", 120);
        DefineIntProperty("awsmock.service.secretsmanager.monitoring.period", "AWSMOCK_SERVICE_SECRETSMANAGER_MONITORING_PERIOD", 300);

        // KMS
        DefineBoolProperty("awsmock.service.kms.active", "AWSMOCK_SERVICE_KMS_ACTIVE", true);
        DefineStringProperty("awsmock.service.kms.http.host", "AWSMOCK_SERVICE_KMS_HOST", "localhost");
        DefineIntProperty("awsmock.service.kms.http.port", "AWSMOCK_SERVICE_KMS_PORT", 9508);
        DefineIntProperty("awsmock.service.kms.http.max.queue", "AWSMOCK_SERVICE_KMS_MAX_QUEUE", 250);
        DefineIntProperty("awsmock.service.kms.http.max.threads", "AWSMOCK_SERVICE_KMS_MAX_THREADS", 50);
        DefineIntProperty("awsmock.service.kms.http.timeout", "AWSMOCK_SERVICE_KMS_TIMEOUT", 900);
        DefineIntProperty("awsmock.service.kms.monitoring.period", "AWSMOCK_SERVICEK_KMS_MONITORING_PERIOD", 300);
        DefineIntProperty("awsmock.service.kms.worker.period", "AWSMOCK_WORKER_KMS_PERIOD", 300);

        // Docker
        DefineStringProperty("awsmock.docker.network.mode", "AWSMOCK_DOCKER_NETWORK_MODE", "bridge");
        DefineStringProperty("awsmock.docker.network.name", "AWSMOCK_DOCKER_NETWORK_NAME", ".dockerhost.net");
        DefineIntProperty("awsmock.docker.default.memory.size", "AWSMOCK_DOCKER_DEFAULT_MEMORY_SIZE", 512);
        DefineIntProperty("awsmock.docker.default.temp.size", "AWSMOCK_DOCKER_DEFAULT_TEMP_SIZE", 10240);
        DefineIntProperty("awsmock.docker.container.port", "AWSMOCK_DOCKER_CONTAINER_PORT", 8080);

        // Monitoring
        DefineIntProperty("awsmock.monitoring.port", "AWSMOCK_CORE_METRIC_PORT", 9100);
        DefineIntProperty("awsmock.monitoring.timeout", "AWSMOCK_CORE_METRIC_TIMEOUT", 60000);

        // Logging
        DefineStringProperty("awsmock.log.level", "AWSMOCK_LOG_LEVEL", "information");

        // Database
        DefineBoolProperty("awsmock.mongodb.active", "AWSMOCK_MONGODB_ACTIVE", true);
        DefineStringProperty("awsmock.mongodb.name", "AWSMOCK_MONGODB_NAME", "awsmock");
        DefineStringProperty("awsmock.mongodb.host", "AWSMOCK_MONGODB_HOST", "vogje01-nuc");
        DefineIntProperty("awsmock.mongodb.port", "AWSMOCK_MONGODB_PORT", 27017);
        DefineStringProperty("awsmock.mongodb.user", "AWSMOCK_MONGODB_USER", "admin");
        DefineStringProperty("awsmock.mongodb.password", "AWSMOCK_MONGODB_PASSWORD", "admin");
    }

    void Configuration::DefineStringProperty(const std::string &key, const std::string &envProperty, const std::string &defaultValue) {
        if (getenv(envProperty.c_str()) != nullptr) {
            setString(key, getenv(envProperty.c_str()));
        } else if (!has(key)) {
            setString(key, defaultValue);
        }
        log_trace << "Defined property, key: " << key << " property: " << envProperty << " default: " << defaultValue;
    }

    void Configuration::DefineBoolProperty(const std::string &key, const std::string &envProperty, bool defaultValue) {
        if (getenv(envProperty.c_str()) != nullptr) {
            setBool(key, std::string(getenv(envProperty.c_str())) == "true");
        } else if (!has(key)) {
            setBool(key, defaultValue);
        }
        log_trace << "Defined property, key: " << key << " property: " << envProperty << " default: " << defaultValue;
    }

    void Configuration::DefineIntProperty(const std::string &key, const std::string &envProperty, int defaultValue) {
        if (getenv(envProperty.c_str()) != nullptr) {
            setInt(key, std::stoi(getenv(envProperty.c_str())));
        } else if (!has(key)) {
            setInt(key, defaultValue);
        }
        log_trace << "Defined property, key: " << key << " property: " << envProperty << " default: " << defaultValue;
    }

    std::string Configuration::GetFilename() const {
        if (_filename.empty()) {
            throw CoreException("Filename not set");
        }
        return _filename;
    }

    void Configuration::SetFilename(const std::string &filename) {
        if (filename.empty()) {
            throw CoreException("Empty filename");
        }
        if (!Core::FileUtils::FileExists(filename)) {
            log_warning << "Configuration file '" << filename << "' does not exist, Will use defaults.";
            return;
        }
        _filename = filename;
        load(_filename);
    }

    void Configuration::SetValue(const std::string &key, const std::string &value) {
        if (!hasProperty(key)) {
            throw CoreException("Property not found, key: " + key);
        }
        setString(key, value);
        log_trace << "Value set, key: " << key;
    }

    void Configuration::SetValue(const std::string &key, bool value) {
        if (!hasProperty(key)) {
            throw CoreException("Property not found, key: " + key);
        }
        setBool(key, value);
        log_trace << "Value set, key: " << key;
    }

    void Configuration::SetValue(const std::string &key, int value) {
        if (!hasProperty(key)) {
            throw CoreException("Property not found, key: " + key);
        }
        setInt(key, value);
        log_trace << "Value set, key: " << key;
    }

    std::string Configuration::GetAppName() {
        return PROJECT_NAME;
    }

    std::string Configuration::GetVersion() {
        return PROJECT_VERSION;
    }

    std::string Configuration::ToString() const {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    void Configuration::WriteFile(const std::string &filename) {
        if (filename.empty()) {
            log_warning << "Configuration filename empty.";
            return;
        }
        if (Core::FileUtils::FileExists(filename)) {
            log_warning << "Configuration filename exists already. File will be truncated, before writing.";
        }
        std::vector<std::string> pKeys;
        this->keys(pKeys);
        std::ofstream ofs(filename, std::ofstream::trunc);
        for (const auto &key: *this) {
            ofs << key.first << "=" << key.second;
        }
    }

    std::ostream &operator<<(std::ostream &os, const Configuration &s) {
        os << "Configuration={";
        for (const auto &it: s) {
            os << it.first << "={" << it.second + "}, ";
        }
        return os;
    }

}// namespace AwsMock::Core
