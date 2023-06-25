//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/worker/S3Worker.h>
#include "awsmock/core/ResourceNotFoundException.h"

namespace AwsMock::Worker {

    S3Worker::S3Worker(const Core::Configuration &configuration) : _logger(Poco::Logger::get("S3Worker")), _configuration(configuration), _running(false) {
        Core::Logger::SetDefaultConsoleLogger("S3Worker");

        Initialize();

        _logger.debug() << "S3Worker initialized" << std::endl;
    }

    S3Worker::~S3Worker() {
        _watcherThread.wakeUp();
        delete _watcher;
    }

    void S3Worker::Initialize() {

        _dataDir = _configuration.getString("awsmock.data.dir") + Poco::Path::separator() + "s3";
        _watcherDir = _dataDir + Poco::Path::separator() + "watcher";
        _logger.debug() << "Watching path: " << _watcherDir << std::endl;

        // Sleeping period
        _period = _configuration.getInt("awsmock.worker.s3.period", 10000);

        // Create S3 directories
        if (!Core::DirUtils::DirectoryExists(_dataDir)) {
            Core::DirUtils::MakeDirectory(_dataDir);
        }
        if (!Core::DirUtils::DirectoryExists(_watcherDir)) {
            Core::DirUtils::MakeDirectory(_watcherDir);
        }

        // Create environment
        _region = _configuration.getString("awsmock.region");

        // SQS service connection
        _s3ServiceHost = _configuration.getString("awsmock.service.s3.host", "localhost");
        _s3ServicePort = _configuration.getInt("awsmock.service.s3.port", 9501);

        //_s3Service = std::make_unique<Service::S3Service>(_configuration);
        // Service database
        _serviceDatabase = std::make_unique<Database::ServiceDatabase>(_configuration);

        // Start _watcher
        _watcher = new Core::DirectoryWatcher(_watcherDir);
        _watcher->itemAdded += Poco::delegate(this, &S3Worker::OnFileAdded);
        //_watcher->itemModified += Poco::delegate(this, &S3Worker::OnFileModified);
        //_watcher->itemDeleted += Poco::delegate(this, &S3Worker::OnFileDeleted);
        _logger.debug() << "Directory _watcher added, path: " << _dataDir << std::endl;
    }

    void S3Worker::run() {

        // Check service active
//        if (!_serviceDatabase->IsActive("S3")) {
//            return;
//        }

        // Start file watcher, they will call the delegate methods, if they find a file system event.
        _watcherThread.start(_watcher);

        _running = true;
        while (_running) {
            Poco::Thread::sleep(_period);
        }
    }

    void S3Worker::OnFileAdded(const Core::DirectoryEvent &addedEvent) {
        _logger.debug() << "Added path: " << addedEvent.item.path() << std::endl;

        if (Core::DirUtils::IsDirectory(addedEvent.item.path())) {
            CreateBucket(addedEvent.item.path());
        } else {
            CreateObject(addedEvent.item.path());
        }
    }

    void S3Worker::OnFileModified(const Core::DirectoryEvent &modifiedEvent) {
        _logger.debug() << "Changed path: " << modifiedEvent.item.path() << std::endl;

        if(Core::DirUtils::IsDirectory(modifiedEvent.item.path())) {
            return;
        }

        // Get bucket, key
        std::string bucket, key;
        GetBucketKeyFromFile(modifiedEvent.item.path(), bucket, key);

        // Get file size, MD5 sum
        long size = Core::FileUtils::FileSize(modifiedEvent.item.path());
        std::string md5sum = Core::Crypto::GetMd5FromFile(modifiedEvent.item.path());
        std::string owner = Core::FileUtils::GetOwner(modifiedEvent.item.path());

        SendPutObjectRequest(bucket, key, md5sum, "application/octet-stream", size);
    }

    void S3Worker::OnFileDeleted(const Core::DirectoryEvent &deleteEvent) {
        _logger.debug() << "Deleted path: " << deleteEvent.item.path() << std::endl;

        if (Core::DirUtils::IsDirectory(deleteEvent.item.path())) {
            // TODO: Delete bucket
            return;
        }
        std::string bucketName, key;
        GetBucketKeyFromFile(deleteEvent.item.path(), bucketName, key);

        Database::Entity::S3::Bucket bucket = {.region=_region, .name=bucketName};
        Database::Entity::S3::Object object = {.bucket=bucketName, .key = key};

        //Dto::S3::DeleteObjectRequest request = {.region=_region, .bucket=bucketName, .key=key};
        //_s3Service->DeleteObject(request);
    }

    void S3Worker::GetBucketKeyFromFile(const std::string &fileName, std::string &bucket, std::string &key) {

        std::string file;
        if (Core::StringUtils::Contains(fileName, "watcher")) {
            file = Poco::replace(fileName, _watcherDir.c_str(), "");
        } else {
            file = Poco::replace(fileName, _dataDir.c_str(), "");
        }

        Poco::RegularExpression::MatchVec posVec;
        Poco::RegularExpression pattern(R"(/([a-zA-Z0-9-.]+)/?([a-zA-Z0-9-_/.*'()]+)?$)");
        if (!pattern.match(file, 0, posVec)) {
            throw Core::ResourceNotFoundException("Could not extract bucket and key");
        }

        if (posVec.size() > 1) {
            bucket = file.substr(posVec[1].offset, posVec[1].length);
        }
        if (posVec.size() > 2) {
            key = file.substr(posVec[2].offset, posVec[2].length);
        }
    }

    void S3Worker::CreateBucket(const std::string &dirPath) {

        // Get bucket, key
        std::string bucket, key;
        GetBucketKeyFromFile(dirPath, bucket, key);

        // Set parameter
        std::string owner = Core::FileUtils::GetOwner(dirPath);

        SendCreateBucketRequest(bucket, "application/octet-stream");
    }

    void S3Worker::CreateObject(const std::string &filePath) {

        // Get bucket, key
        std::string bucket, key;
        GetBucketKeyFromFile(filePath, bucket, key);

        // Get file size, MD5 sum
        long size = Core::FileUtils::FileSize(filePath);
        std::string md5sum = Core::Crypto::GetMd5FromFile(filePath);
        std::string owner = Core::FileUtils::GetOwner(filePath);

        Core::FileUtils::MoveTo(filePath, _dataDir + Poco::Path::separator() + bucket + Poco::Path::separator() + key, true);

        SendPutObjectRequest(bucket, key, md5sum, "application/octet-stream", size);
    }

    void S3Worker::SendCreateBucketRequest(const std::string &bucket, const std::string &contentType) {

        //"Credential=none/20230618/eu-central-1/s3/aws4_request, SignedHeaders=content-md5;content-type;host;x-amz-content-sha256;x-amz-date;x-amz-security-token, Signature=fe9766ea2c032ac7b17033a567f6b361192bddcf73f89d25c15019977c544e1c"
        Poco::URI uri("http://" + _s3ServiceHost + ":" + std::to_string(_s3ServicePort) + "/" + bucket);
        std::string path(uri.getPathAndQuery());

        // Get the body
        std::string body = std::string(
            "<CreateBucketConfiguration xmlns=\"http://s3.amazonaws.com/doc/2006-03-01/\">\n<LocationConstraint>" + bucket
                + "</LocationConstraint>\n</CreateBucketConfiguration>");

        // Create HTTP request and set headers
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_PUT, path, Poco::Net::HTTPMessage::HTTP_1_1);
        request.add("Content-Type", contentType);
        request.add("Authorization",
                    "AWS4-HMAC-SHA256 Credential=none/20230618/eu-central-1/lambda/aws4_request, SignedHeaders=host;x-amz-date;x-amz-security-token, Signature=90d0e45560fa4ce03e6454b7a7f2a949e0c98b46c35bccb47f666272ec572840");
        log_debug_stream(_logger) << "S3 create bucket message request created, bucket: " + bucket << std::endl;

        // Send request
        std::ostream &os = session.sendRequest(request);
        os << body;

        // Get the response status
        Poco::Net::HTTPResponse response;
        if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK) {
            log_error_stream(_logger) << "HTTP error, status: " + std::to_string(response.getStatus()) + " reason: " + response.getReason() << std::endl;
        }
        log_debug_stream(_logger) << "S3 create bucket message request send, status: " << response.getStatus() << std::endl;
    }

    void S3Worker::SendPutObjectRequest(const std::string &bucket, const std::string &key, const std::string &md5Sum, const std::string &contentType, long fileSize) {

        //"Credential=none/20230618/eu-central-1/s3/aws4_request, SignedHeaders=content-md5;content-type;host;x-amz-content-sha256;x-amz-date;x-amz-security-token, Signature=fe9766ea2c032ac7b17033a567f6b361192bddcf73f89d25c15019977c544e1c"
        Poco::URI uri("http://" + _s3ServiceHost + ":" + std::to_string(_s3ServicePort) + "/" + bucket + "/" + key);
        std::string path(uri.getPathAndQuery());

        // Create HTTP request and set headers
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_PUT, path, Poco::Net::HTTPMessage::HTTP_1_1);
        request.add("Content-Length", std::to_string(fileSize));
        request.add("Content-Type", contentType);
        request.add("Content-MD5", md5Sum);
        request.add("WriteFile", "false");
        request.add("Authorization",
                    "AWS4-HMAC-SHA256 Credential=none/20230618/eu-central-1/lambda/aws4_request, SignedHeaders=host;x-amz-date;x-amz-security-token, Signature=90d0e45560fa4ce03e6454b7a7f2a949e0c98b46c35bccb47f666272ec572840");
        log_debug_stream(_logger) << "S3 put object message request created, bucket: " + bucket << " key: " << key << std::endl;

        // Send request
        session.sendRequest(request);

        // Get the response status
        Poco::Net::HTTPResponse response;
        if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK) {
            log_error_stream(_logger) << "HTTP error, status: " + std::to_string(response.getStatus()) + " reason: " + response.getReason() << std::endl;
        }
        log_debug_stream(_logger) << "S3 put object message request send, status: " << response.getStatus() << std::endl;
    }

} // namespace AwsMock::Worker
