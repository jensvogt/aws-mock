//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/S3Service.h>

namespace AwsMock::Service {

    S3Service::S3Service(const Core::Configuration &configuration) : _logger(Poco::Logger::get("S3Service")), _configuration(configuration) {

        // Set console logger
        Core::Logger::SetDefaultConsoleLogger("S3Service");

        // Initialize environment
        _dataDir = _configuration.getString("awsmock.data.dir", "/tmp/awsmock/data");
        _tempDir = _dataDir + Poco::Path::separator() + "tmp";
        _database = std::make_unique<Database::S3Database>(_configuration);

        // SQS service connection
        _sqsServiceHost = _configuration.getString("awsmock.service.sqs.host", "localhost");
        _sqsServicePort = _configuration.getInt("awsmock.service.sqs.port", 9501);

        // Lambda service connection
        _lambdaServiceHost = _configuration.getString("awsmock.service.lambda.host", "localhost");
        _lambdaServicePort = _configuration.getInt("awsmock.service.lambda.port", 9503);

        // Create temp directory
        if (!Core::DirUtils::DirectoryExists(_tempDir)) {
            Core::DirUtils::MakeDirectory(_tempDir);
        }
    }

    Dto::S3::CreateBucketResponse S3Service::CreateBucket(const std::string &name, const std::string &owner, const Dto::S3::CreateBucketRequest &s3Request) {
        log_trace_stream(_logger) << "Create bucket request, s3Request: " << s3Request.ToString() << std::endl;

        Dto::S3::CreateBucketResponse createBucketResponse;
        try {
            // Get region
            std::string region = s3Request._locationConstraint;

            // Check existence
            if (_database->BucketExists({.region=region, .name=name})) {
                throw Core::ServiceException("Bucket exists already", 500);
            }

            // Create directory
            std::string bucketDir = _dataDir + Poco::Path::separator() + "s3" + Poco::Path::separator() + name;
            if (!Core::DirUtils::DirectoryExists(bucketDir)) {
                Core::DirUtils::MakeDirectory(bucketDir);
            }

            // Update database
            _database->CreateBucket({.region=region, .name=name, .owner=owner});

            createBucketResponse = Dto::S3::CreateBucketResponse(region, "arn");
            log_trace_stream(_logger) << "S3 create bucket response: " << createBucketResponse.ToXml() << std::endl;

        } catch (Poco::Exception &exc) {
            log_error_stream(_logger) << "S3 create bucket failed, message: " << exc.message() << std::endl;
            throw Core::ServiceException(exc.message(), 500);
        }
        return createBucketResponse;
    }

    Dto::S3::GetMetadataResponse S3Service::GetMetadata(Dto::S3::GetMetadataRequest &request) {
        log_trace_stream(_logger) << "Get metadata request, s3Request: " << request.ToString() << std::endl;

        Dto::S3::GetMetadataResponse getMetadataResponse;
        try {

            Database::Entity::S3::Object object = _database->GetObject(request.bucket, request.bucket, request.key);

            getMetadataResponse.bucket = object.bucket;
            getMetadataResponse.key = object.key;
            getMetadataResponse.size = object.size;
            getMetadataResponse.md5Sum = object.md5sum;
            getMetadataResponse.contentType = object.contentType;
            getMetadataResponse.modified = object.modified;
            getMetadataResponse.created = object.modified;
            log_trace_stream(_logger) << "S3 get metadata response: " + getMetadataResponse.ToString() << std::endl;

        } catch (Poco::Exception &ex) {
            log_error_stream(_logger) << "S3 create bucket failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500);
        }
        return getMetadataResponse;
    }

    Dto::S3::GetObjectResponse S3Service::GetObject(Dto::S3::GetObjectRequest &request) {
        log_trace_stream(_logger) << "Get object request, s3Request: " << request.ToString() << std::endl;

        Dto::S3::GetObjectResponse getObjectResponse;
        try {

            Database::Entity::S3::Object object = _database->GetObject(request.bucket, request.bucket, request.key);

            std::string filename = _dataDir + Poco::Path::separator() + "s3" + Poco::Path::separator() + request.bucket + Poco::Path::separator() + request.key;
            getObjectResponse.bucket = object.bucket;
            getObjectResponse.key = object.key;
            getObjectResponse.size = object.size;
            getObjectResponse.contentType = object.contentType;
            getObjectResponse.filename = filename;
            getObjectResponse.modified = object.modified;
            log_trace_stream(_logger) << "S3 get object response: " << getObjectResponse.ToString() << std::endl;

        } catch (Poco::Exception &ex) {
            log_error_stream(_logger) << "S3 get object failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500);
        }
        return getObjectResponse;
    }

    Dto::S3::ListAllBucketResponse S3Service::ListAllBuckets() {
        log_trace_stream(_logger) << "List all buckets request" << std::endl;

        try {

            Database::Entity::S3::BucketList bucketList = _database->ListBuckets();
            auto listAllBucketResponse = Dto::S3::ListAllBucketResponse(bucketList);
            log_trace_stream(_logger) << "S3 Create Bucket List outcome: " + listAllBucketResponse.ToXml() << std::endl;
            return listAllBucketResponse;

        } catch (Poco::Exception &ex) {
            log_error_stream(_logger) << "S3 Create Bucket failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500);
        }
    }

    Dto::S3::ListBucketResult S3Service::ListBucket(const Dto::S3::ListBucketRequest &request) {
        log_trace_stream(_logger) << "List bucket request: " + request.ToString() << std::endl;

        try {

            Database::Entity::S3::ObjectList objectList = _database->ListBucket(request.name, request.prefix);
            Dto::S3::ListBucketResult listBucketResult = Dto::S3::ListBucketResult(request.name, objectList);
            log_trace_stream(_logger) << "S3 list bucket result: " << listBucketResult.ToXml() << std::endl;
            return listBucketResult;

        } catch (Poco::Exception &ex) {
            log_error_stream(_logger) << "S3 list bucket failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500);
        }
    }

    Dto::S3::InitiateMultipartUploadResult S3Service::CreateMultipartUpload(std::string &bucket, std::string &key, const std::string &region, const std::string &user) {
        log_trace_stream(_logger) << "CreateMultipartUpload request, bucket: " + bucket << " key: " << key << " region: " << region << " user: " << user << std::endl;

        // Check existence
        if (!_database->BucketExists({.region=region, .name=bucket})) {
            throw Core::ServiceException("Bucket does not exist", 500);
        }

        std::string uploadId = Core::StringUtils::GenerateRandomString(58);

        // Create upload directory, if not existing
        std::string uploadDir = GetMultipartUploadDirectory(uploadId);
        if (!Core::DirUtils::DirectoryExists(uploadDir)) {
            Core::DirUtils::MakeDirectory(uploadDir);
        }

        return {.bucket=bucket, .key=key, .uploadId=uploadId};
    }

    std::string S3Service::UploadPart(std::istream &stream, int part, const std::string &uploadId) {
        log_trace_stream(_logger) << "UploadPart request, part: " << part << " updateId: " << uploadId << std::endl;

        std::string uploadDir = GetMultipartUploadDirectory(uploadId);
        log_trace_stream(_logger) << "Using uploadDir: " << uploadDir << std::endl;

        std::ofstream ofs(uploadDir + Poco::Path::separator() + uploadId + "-" + std::to_string(part));
        ofs << stream.rdbuf();
        log_trace_stream(_logger) << "Part uploaded, part: " << part << " dir: " << uploadDir << std::endl;

        return Core::StringUtils::GenerateRandomString(40);
    }

    Dto::S3::CompleteMultipartUploadResult S3Service::CompleteMultipartUpload(const std::string &uploadId,
                                                                              const std::string &bucket,
                                                                              const std::string &key,
                                                                              const std::string &region,
                                                                              const std::string &user) {
        log_trace_stream(_logger) << "CompleteMultipartUpload request, uploadId: " << uploadId << " bucket: " << bucket << " key: " << key << " region: " << region
                                  << " user: "
                                  << user;

        // Get all file parts
        std::string uploadDir = GetMultipartUploadDirectory(uploadId);
        std::vector<std::string> files = Core::DirUtils::ListFilesByPrefix(uploadDir, uploadId);

        // Create bucket directory, if not existing
        std::string bucketDir = _dataDir + Poco::Path::separator() + "s3" + Poco::Path::separator() + bucket;
        if (!Core::DirUtils::DirectoryExists(bucketDir)) {
            Core::DirUtils::MakeDirectory(bucketDir);
        }

        // Output file
        std::string outFile = bucketDir + Poco::Path::separator() + GetDirFromKey(key);
        log_trace_stream(_logger) << "Output file, outFile: " << outFile << std::endl;

        // Append all parts to the output file
        Core::FileUtils::AppendBinaryFiles(outFile, _tempDir, files);
        log_trace_stream(_logger) << "Input files appended to outfile, outFile: " << outFile << std::endl;

        // Get file size, MD5 sum
        long fileSize = Core::FileUtils::FileSize(outFile);
        std::string md5sum = Core::Crypto::GetMd5FromFile(outFile);
        log_trace_stream(_logger) << "Got file metadata, md5sum: " << md5sum << " size: " << fileSize << " outFile: " << outFile << std::endl;

        // Create database object
        Database::Entity::S3::Object object = _database->CreateOrUpdateObject({.bucket=bucket, .key=key, .owner=user, .size=fileSize, .md5sum=md5sum});

        // Cleanup
        Core::DirUtils::DeleteDirectory(uploadDir);

        return {.location=region, .bucket=bucket, .key=key, .etag=Core::StringUtils::GenerateRandomString(40)};
    }

    Dto::S3::PutObjectResponse S3Service::PutObject(Dto::S3::PutObjectRequest &request, std::istream *stream) {
        log_trace_stream(_logger) << "Put object request: " << request.ToString() << std::endl;

        Dto::S3::PutObjectResponse response;
        try {
            // Check existence
            if (!_database->BucketExists({.region=request.region, .name=request.bucket})) {
                log_error_stream(_logger) << "Bucket does not exist, region: " << request.region + " bucket: " << request.bucket << std::endl;
                throw Core::ServiceException("Bucket does not exist", 500);
            }

            // Create directory, if not existing
            std::string directory = GetDirectory(request.bucket, request.key);
            if (!Core::DirUtils::DirectoryExists(directory)) {
                Core::DirUtils::MakeDirectory(directory);
                log_debug_stream(_logger) << "Local bucket directory created, path: " << directory << std::endl;
            }

            // Write file
            if (stream) {
                std::string fileName = GetFilename(request.bucket, request.key);
                std::ofstream ofs(fileName);
                ofs << stream->rdbuf();
                ofs.close();
            }

            // Update database
            Database::Entity::S3::Object object = {
                .bucket=request.bucket,
                .key=request.key,
                .owner=request.owner,
                .size=request.size,
                .md5sum=request.md5Sum,
                .contentType=request.contentType};
            object = _database->CreateOrUpdateObject(object);

            // Check notification
            Database::Entity::S3::Bucket bucket = _database->GetBucketByRegionName(request.region, request.bucket);
            auto notification =
                find_if(bucket.notifications.begin(), bucket.notifications.end(), [](const Database::Entity::S3::BucketNotification eventNotification) {
                  return eventNotification.event == "s3:ObjectCreated:Put";
                });
            if (notification != bucket.notifications.end()) {
                CheckNotifications(object, request.region, "s3:ObjectCreated:Put");
            }

        } catch (Poco::Exception &ex) {
            log_error_stream(_logger) << "S3 put object failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500);
        }
        return {.bucket=request.bucket, .key=request.key, .etag=request.md5Sum};
    }

    void S3Service::DeleteObject(const Dto::S3::DeleteObjectRequest &request) {
        log_trace_stream(_logger) << "Delete object request: " << request.ToString() << std::endl;

        try {
            // Check bucket existence
            if (!_database->BucketExists({.region=request.region, .name=request.bucket})) {
                throw Core::ServiceException("Bucket does not exist", 500);
            }

            if (!_database->ObjectExists({.bucket=request.bucket, .key=request.key})) {
                throw Core::ServiceException("Object does not exist", 500);
            }

            // Delete from database
            Database::Entity::S3::Object object = {.region=request.region, .bucket=request.bucket, .key=request.key};
            _database->DeleteObject(object);
            log_debug_stream(_logger) << "Database object deleted, key: " << request.key << std::endl;

            // Delete file system object
            DeleteObject(request.bucket, request.key);
            log_debug_stream(_logger) << "File system object deleted, key: " << request.key << std::endl;

            // Check notification
            Database::Entity::S3::Bucket bucket = _database->GetBucketByRegionName(request.region, request.bucket);
            auto notification =
                find_if(bucket.notifications.begin(), bucket.notifications.end(), [](const Database::Entity::S3::BucketNotification &eventNotification) {
                  return eventNotification.event == "s3:ObjectRemoved:Delete";
                });
            if (notification != bucket.notifications.end()) {
                CheckNotifications(object, request.region, "s3:ObjectRemoved:Delete");
            }

        } catch (Poco::Exception &exc) {
            log_error_stream(_logger) << "S3 delete object failed, message: " + exc.message() << std::endl;
            throw Core::ServiceException(exc.message(), 500);
        }
    }

    Dto::S3::DeleteObjectsResponse S3Service::DeleteObjects(const Dto::S3::DeleteObjectsRequest &request) {
        log_trace_stream(_logger) << "Delete objects request: " << request.ToString() << std::endl;

        Dto::S3::DeleteObjectsResponse response;
        try {
            // Check existence
            if (!_database->BucketExists({.region=request.region, .name=request.bucket})) {
                throw Core::ServiceException("Bucket does not exist", 500);
            }

            // Delete from database
            _database->DeleteObjects(request.bucket, request.keys);
            log_debug_stream(_logger) << "Database object deleted, count: " << request.keys.size() << std::endl;

            // Delete file system objects
            for(const auto &key: request.keys) {

                DeleteObject(request.bucket, key);
                log_debug_stream(_logger) << "File system object deleted: " << key << std::endl;

                // Check notifications
                Database::Entity::S3::Object object = _database->GetObject(request.region, request.bucket, key);
                Database::Entity::S3::Bucket bucket = _database->GetBucketByRegionName(request.region, request.bucket);
                if (bucket.HasNotification("s3:ObjectRemoved:Delete")) {
                    CheckNotifications(object, request.region, "s3:ObjectRemoved:Delete");
                }

            }

        } catch (Poco::Exception &ex) {
            log_error_stream(_logger) << "S3 delete objects failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500);
        }
        return response;
    }

    void S3Service::PutBucketNotification(const Dto::S3::PutBucketNotificationRequest &request) {
        log_trace_stream(_logger) << "Put bucket notification request, id: " << request.notificationId << std::endl;

        try {
            // Check bucket existence
            if (!_database->BucketExists({.region=request.region, .name=request.bucket})) {
                throw Core::ServiceException("Bucket does not exist", 500);
            }

            // Check notification existence
            Database::Entity::S3::Bucket bucket = _database->GetBucketByRegionName(request.region, request.bucket);
            if (bucket.HasNotification(request.event)) {
                throw Core::ServiceException("Bucket notification exists already", 500);
            }

            if (!request.lambdaArn.empty()) {
                CreateLambdaConfiguration(bucket, request);
            } else if (!request.queueArn.empty()) {
                CreateQueueConfiguration(bucket, request);
            }

        } catch (Poco::Exception &ex) {
            log_error_stream(_logger) << "S3 put bucket notification request failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500);
        }
    }

    void S3Service::DeleteBucket(const std::string &region, const std::string &name) {
        log_trace_stream(_logger) << "Delete bucket request, name: " << name << std::endl;

        try {
            Database::Entity::S3::Bucket bucket = {.region=region, .name=name};

            // Check existence
            if (!_database->BucketExists(bucket)) {
                throw Core::ServiceException("Bucket does not exist", 500);
            }

            // Check empty
            if (_database->HasObjects(bucket)) {
                throw Core::ServiceException("Bucket is not empty", 500);
            }

            // Delete directory
            DeleteBucket(name);

            // Delete bucket from database
            _database->DeleteBucket(bucket);

        } catch (Poco::Exception &ex) {
            log_error_stream(_logger) << "S3 Delete Bucket failed, message: " << ex.message() << std::endl;
            throw Core::ServiceException(ex.message(), 500);
        }
    }

    void S3Service::CheckNotifications(const Database::Entity::S3::Object& object, const std::string &region, const std::string &event) {

        Database::Entity::S3::Bucket dbBucket = _database->GetBucketByRegionName(region, object.bucket);

        if (dbBucket.HasNotification(event)) {

            Database::Entity::S3::BucketNotification notification = dbBucket.GetNotification(event);

            // Create the event record
            Dto::S3::Object obj = {.key=object.key, .etag=object.md5sum};
            Dto::S3::Bucket bucket = {.name=dbBucket.name};

            Dto::S3::S3 s3 = {.configurationId=notification.notificationId, .bucket=bucket, .object=obj,};

            Dto::S3::Record record = {.region=region, .s3=s3};
            Dto::S3::EventNotification eventNotification;

            eventNotification.records.push_back(record);
            log_debug_stream(_logger) << "Found record, count: " << eventNotification.records.size() << std::endl;

            if (!notification.queueArn.empty()) {

                // Queue notification
                SendQueueNotificationRequest(eventNotification, notification.queueArn);
                log_debug_stream(_logger) << "SQS message created, eventNotification: " + eventNotification.ToString() << std::endl;

            } else if (!notification.lambdaArn.empty()) {

                // Lambda notification
                SendLambdaInvocationRequest(eventNotification);
                log_debug_stream(_logger) << "Lambda function invoked, eventNotification: " + eventNotification.ToString() << std::endl;
            }
        }
    }

    Database::Entity::S3::Bucket S3Service::CreateQueueConfiguration(const Database::Entity::S3::Bucket &bucket, const Dto::S3::PutBucketNotificationRequest &request) {

        Database::Entity::S3::BucketNotification bucketNotification = {.event=request.event, .notificationId=request.notificationId, .queueArn = request.queueArn};
        return _database->CreateBucketNotification(bucket, bucketNotification);
    }

    Database::Entity::S3::Bucket S3Service::CreateLambdaConfiguration(const Database::Entity::S3::Bucket &bucket, const Dto::S3::PutBucketNotificationRequest &request) {

        Database::Entity::S3::BucketNotification bucketNotification = {.event=request.event, .notificationId=request.notificationId, .lambdaArn=request.lambdaArn};
        return _database->CreateBucketNotification(bucket, bucketNotification);
    }

    std::string S3Service::GetDirFromKey(const std::string &key) {

        if (key.find('/') != std::string::npos) {
            return key.substr(0, key.find_last_of('/'));
        }
        return {};
    }

    std::string S3Service::GetDirectory(const std::string &bucket, const std::string &key) {

        return _dataDir + Poco::Path::separator() + "s3" + Poco::Path::separator() + bucket + Poco::Path::separator() + GetDirFromKey(key);
    }

    std::string S3Service::GetFilename(const std::string &bucket, const std::string &key) {

        return _dataDir + Poco::Path::separator() + "s3" + Poco::Path::separator() + bucket + Poco::Path::separator() + key;
    }

    void S3Service::DeleteObject(const std::string &bucket, const std::string &key) {

        std::string filename = _dataDir + Poco::Path::separator() + "s3" + Poco::Path::separator() + bucket + Poco::Path::separator() + key;
        if(Core::FileUtils::FileExists(filename)) {
            Core::FileUtils::DeleteFile(filename);
        }

    }

    void S3Service::DeleteBucket(const std::string &bucket) {

        std::string dirname = _dataDir + Poco::Path::separator() + "s3" + Poco::Path::separator() + bucket;
        if (Core::DirUtils::DirectoryExists(dirname)) {
            Core::DirUtils::DeleteDirectory(dirname, true);
        }
    }

    std::string S3Service::GetMultipartUploadDirectory(const std::string &uploadId) {
        return _tempDir + Poco::Path::separator() + uploadId;
    }

    void S3Service::SendQueueNotificationRequest(const Dto::S3::EventNotification &eventNotification, const std::string &queueArn) {

        //"Credential=none/20230618/eu-central-1/s3/aws4_request, SignedHeaders=content-md5;content-type;host;x-amz-content-sha256;x-amz-date;x-amz-security-token, Signature=fe9766ea2c032ac7b17033a567f6b361192bddcf73f89d25c15019977c544e1c"
        Poco::URI uri("http://" + _sqsServiceHost + ":" + std::to_string(_sqsServicePort) + "/");
        std::string path(uri.getPathAndQuery());

        // Set payload
        std::string body = "Action=SendMessage&QueueArn=" + queueArn + "&MessageBody=" + eventNotification.ToJson();

        // Create HTTP request and set headers
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, path, Poco::Net::HTTPMessage::HTTP_1_1);
        request.setContentLength((long) body.length());
        request.add("Content-Type", "application/json");
        request.add("Authorization",
                    "AWS4-HMAC-SHA256 Credential=none/20230618/eu-central-1/lambda/aws4_request, SignedHeaders=host;x-amz-date;x-amz-security-token, Signature=90d0e45560fa4ce03e6454b7a7f2a949e0c98b46c35bccb47f666272ec572840");
        log_debug_stream(_logger) << "SQS message request created, body: " + body << std::endl;

        // Send request
        std::ostream &os = session.sendRequest(request);
        os << body;

        // Get the response status
        Poco::Net::HTTPResponse response;
        if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK) {
            log_error_stream(_logger) << "HTTP error, status: " + std::to_string(response.getStatus()) + " reason: " + response.getReason() << std::endl;
        }
        log_debug_stream(_logger) << "SQS message request send, status: " << response.getStatus() << std::endl;
    }

    void S3Service::SendLambdaInvocationRequest(const Dto::S3::EventNotification &eventNotification) {

        //"Credential=none/20230618/eu-central-1/s3/aws4_request, SignedHeaders=content-md5;content-type;host;x-amz-content-sha256;x-amz-date;x-amz-security-token, Signature=fe9766ea2c032ac7b17033a567f6b361192bddcf73f89d25c15019977c544e1c"
        Poco::URI uri("http://" + _lambdaServiceHost + ":" + std::to_string(_lambdaServicePort) + "/2015-03-31/functions/function/invocations");
        std::string path(uri.getPathAndQuery());

        // Set payload
        std::string body = eventNotification.ToJson();

        // Create HTTP request and set headers
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, path, Poco::Net::HTTPMessage::HTTP_1_1);
        request.setContentLength((long) body.length());
        request.add("Content-Type", "application/json");
        request.add("Authorization", "AWS4-HMAC-SHA256 Credential=none/20230618/eu-central-1/lambda/aws4_request, SignedHeaders=host;x-amz-date;x-amz-security-token, Signature=90d0e45560fa4ce03e6454b7a7f2a949e0c98b46c35bccb47f666272ec572840");
        log_debug_stream(_logger) << "Invocation request created, body: " + body << std::endl;

        // Send request
        std::ostream &os = session.sendRequest(request);
        os << body;

        // Get the response status
        Poco::Net::HTTPResponse response;
        if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK) {
            log_error_stream(_logger) << "HTTP error, status: " + std::to_string(response.getStatus()) + " reason: " + response.getReason() << std::endl;
        }
        log_debug_stream(_logger) << "Invocation request send, status: " << response.getStatus() << std::endl;
    }

} // namespace AwsMock::Service
