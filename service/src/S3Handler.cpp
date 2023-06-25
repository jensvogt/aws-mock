
#include "awsmock/service/S3Handler.h"

namespace AwsMock::Service {

    S3Handler::S3Handler(Core::Configuration &configuration, Core::MetricService &metricService)
        : AbstractHandler(), _logger(Poco::Logger::get("S3ServiceHandler")), _configuration(configuration), _metricService(metricService), _s3Service(configuration) {

        // Set console logger
        Core::Logger::SetDefaultConsoleLogger("S3ServiceHandler");
    }

    void S3Handler::handleGet(Poco::Net::HTTPServerRequest &request,
                              Poco::Net::HTTPServerResponse &response,
                              [[maybe_unused]]const std::string &region,
                              [[maybe_unused]]const std::string &user) {
        Core::MetricServiceTimer measure(_metricService, HTTP_GET_TIMER);
        log_debug_stream(_logger) << "S3 GET request, URI: " + request.getURI() << " region: " << region << " user: " + user << std::endl;

        try {

            std::string bucket, key;
            GetBucketKeyFromUri(request.getURI(), bucket, key);

            bool isListRequest = QueryParameterExists("list-type");

            if(bucket.empty()) {

                // Return bucket list
                Dto::S3::ListAllBucketResponse s3Response = _s3Service.ListAllBuckets();
                SendOkResponse(response, s3Response.ToXml());

            } else if(!bucket.empty() && !key.empty()) {

                // Get object request
                log_debug_stream(_logger) << "S3 get object request, bucket: " << bucket << " key: " << key << std::endl;
                Dto::S3::GetObjectRequest s3Request = {.region=region, .bucket=bucket, .key=key};

                Dto::S3::GetObjectResponse s3Response = _s3Service.GetObject(s3Request);
                std::ifstream ifs (s3Response.filename);

                HeaderMap headerMap;
                headerMap.emplace_back("ETag", Core::StringUtils::GenerateRandomString(32));
                headerMap.emplace_back("Content-Type", s3Response.contentType);
                headerMap.emplace_back("Content-Length", std::to_string(s3Response.size));
                headerMap.emplace_back("Last-Modified", Poco::DateTimeFormatter().format(s3Response.modified, Poco::DateTimeFormat::HTTP_FORMAT));

                SendOkResponse(response, s3Response.filename, s3Response.size, &headerMap);

            } else if(isListRequest) {

                std::string prefix;
                if(QueryParameterExists("prefix")) {
                    prefix = GetQueryParameter("prefix");
                }

                // Return object list
                Dto::S3::ListBucketRequest s3Request = {.region=region, .name=bucket, .prefix=prefix};
                Dto::S3::ListBucketResult result = _s3Service.ListBucket(s3Request);
                SendOkResponse(response, result.ToXml());
            }

        } catch (Core::ServiceException &exc) {
            SendErrorResponse("S3", response, exc);
        } catch (Core::ResourceNotFoundException &exc) {
            SendErrorResponse("S3", response, exc);
        }
    }

    void S3Handler::handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, [[maybe_unused]]const std::string &region, [[maybe_unused]]const std::string &user) {
        Core::MetricServiceTimer measure(_metricService, HTTP_PUT_TIMER);
        log_debug_stream(_logger) << "S3 PUT request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl;

        try {
            std::string bucket, key;
            GetBucketKeyFromUri(request.getURI(), bucket, key);
            log_debug_stream(_logger) << "Found bucket/key, bucket: " << bucket << " key: " << key << std::endl << std::endl;

            bool isMultipartUpload = QueryParameterExists("uploadId");
            bool isNotification = QueryParameterExists("notification");

            if (isMultipartUpload) {

                // S3 initial multipart upload
                std::string partNumber = GetQueryParameter("partNumber", false);
                std::string uploadId = GetQueryParameter("uploadId", false);
                log_debug_stream(_logger) << "Initial S3 multipart upload part: " << partNumber << std::endl << std::endl;

                std::string eTag = _s3Service.UploadPart(request.stream(), std::stoi(partNumber), uploadId);

                HeaderMap headerMap;
                headerMap.emplace_back("ETag", eTag);

                SendOkResponse(response, {}, &headerMap);
                log_debug_stream(_logger) << "Finished S3 multipart upload part: " << partNumber << std::endl << std::endl;

            } else if(isNotification) {

                log_debug_stream(_logger) << "Bucket notification request, bucket: " << bucket << std::endl << std::endl;

                // S3 notification setup
                Dto::S3::PutBucketNotificationRequest s3Request = Dto::S3::PutBucketNotificationRequest(GetPayload(request), region, bucket);

                _s3Service.PutBucketNotification(s3Request);

                SendOkResponse(response);

            } else if(!key.empty()) {

                // S3 put object request
                Dto::S3::PutObjectRequest putObjectRequest;

                putObjectRequest.bucket = bucket;
                putObjectRequest.key = key;
                putObjectRequest.owner = user;
                putObjectRequest.region = region;
                putObjectRequest.contentType = request.get("Content-Type");
                putObjectRequest.md5Sum = request.get("Content-MD5");
                putObjectRequest.size = std::stol(request.get("Content-Length"));

                // Set the write file = true, as the file is coming over the REST interface and not from the file watcher.
                putObjectRequest.writeFile = request.get("WriteFile") == "true";

                Dto::S3::PutObjectResponse putObjectResponse = _s3Service.PutObject(putObjectRequest, &request.stream());

                HeaderMap headerMap;
                headerMap.emplace_back("ETag", putObjectResponse.etag);

                SendOkResponse(response, {}, &headerMap);

            } else if(!bucket.empty()){

                // S3 create bucket request
                std::string name = Core::DirUtils::RelativePath(request.getURI());
                std::string payload = GetPayload(request);
                Dto::S3::CreateBucketRequest s3Request(payload);
                Dto::S3::CreateBucketResponse s3Response = _s3Service.CreateBucket(name, user, s3Request);

                SendOkResponse(response, s3Response.ToXml());

            }

        } catch (Poco::Exception &exc) {
            SendErrorResponse("S3", response, exc);
        }
    }

    void S3Handler::handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, [[maybe_unused]]const std::string &region, [[maybe_unused]]const std::string &user) {
        Core::MetricServiceTimer measure(_metricService, HTTP_POST_TIMER);
        log_debug_stream(_logger) << "S3 POST request, URI: " << request.getURI() << " region: " << region << " user: " << user << std::endl << std::endl;

        try {

            std::string bucket, key;
            GetBucketKeyFromUri(request.getURI(), bucket, key);

            bool isMultipartUpload = QueryParameterExists("uploads");
            bool isDeleteObjects = QueryParameterExists("delete");

            if (isMultipartUpload) {

                log_debug_stream(_logger) << "Starting multipart upload" << std::endl << std::endl;

                Dto::S3::InitiateMultipartUploadResult result = _s3Service.CreateMultipartUpload(bucket, key, region, user);
                SendOkResponse(response, result.ToXml());

            } else if(isDeleteObjects) {

                log_debug_stream(_logger) << "Starting delete objects request" << std::endl;

                std::string payload = GetPayload(request);

                auto s3Request = Dto::S3::DeleteObjectsRequest(payload);
                Dto::S3::DeleteObjectsResponse s3Response = _s3Service.DeleteObjects(s3Request);
                SendOkResponse(response, s3Response.ToXml());

            } else {

                std::string uploadId = GetQueryParameter("uploadId", true);
                log_debug_stream(_logger) << "Finish multipart upload request, uploadId: " << uploadId << std::endl << std::endl;

                Dto::S3::CompleteMultipartUploadResult result = _s3Service.CompleteMultipartUpload(uploadId, bucket, key, region, user);
                SendOkResponse(response, result.ToXml());

            }

        } catch (Poco::Exception &exc) {
            SendErrorResponse("S3", response, exc);
        }
    }

    void S3Handler::handleDelete(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, [[maybe_unused]]const std::string &region, [[maybe_unused]]const std::string &user) {
        Core::MetricServiceTimer measure(_metricService, HTTP_DELETE_TIMER);
        log_debug_stream(_logger) << "S3 DELETE request, URI: " + request.getURI() << " region: " << region << " user: " << user << std::endl << std::endl;

        try {
            std::string bucket, key;
            GetBucketKeyFromUri(request.getURI(), bucket, key);

            if(!bucket.empty() && !key.empty()) {
                _s3Service.DeleteObject({.region=region, .user=user, .bucket=bucket, .key=key});
                SendDeleteResponse(response);
            } else if(!bucket.empty()) {
                _s3Service.DeleteBucket(region, bucket);
                SendDeleteResponse(response);
            }

        } catch (Core::ServiceException &exc) {
            SendErrorResponse("S3", response, exc);
        }
    }

    void S3Handler::handleOptions(Poco::Net::HTTPServerResponse &response) {
        Core::MetricServiceTimer measure(_metricService, HTTP_OPTIONS_TIMER);
        log_debug_stream(_logger) << "S3 OPTIONS request" << std::endl << std::endl;

        response.set("Allow", "GET, PUT, POST, DELETE, OPTIONS");
        response.setContentType("text/plain; charset=utf-8");

        handleHttpStatusCode(response, 200);
        std::ostream &outputStream = response.send();
        outputStream.flush();
    }

    void S3Handler::handleHead(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
        Core::MetricServiceTimer measure(_metricService, HTTP_HEAD_TIMER);
        log_debug_stream(_logger) << "S3 HEAD request, address: " << request.clientAddress().toString() << std::endl << std::endl;

        try {

            std::string bucket, key;
            GetBucketKeyFromUri(request.getURI(), bucket, key);
            log_debug_stream(_logger) << "S3 HEAD request, bucket: " << bucket << " key: " << key << std::endl << std::endl;

            Dto::S3::GetMetadataRequest s3Request = {.bucket=bucket, .key=key};
            Dto::S3::GetMetadataResponse s3Response = _s3Service.GetMetadata(s3Request);

            HeaderMap headerMap;
            headerMap.emplace_back("Last-Modified", Poco::DateTimeFormatter().format(s3Response.modified, Poco::DateTimeFormat::HTTP_FORMAT));
            headerMap.emplace_back("Content-Length", std::to_string(s3Response.size));
            headerMap.emplace_back("Content-Type", s3Response.contentType);
            headerMap.emplace_back("Connection", "closed");
            headerMap.emplace_back("Server", "AmazonS3");

            SendOkResponse(response, {}, &headerMap);

        } catch (Poco::Exception &exc) {
            SendErrorResponse("S3", response, exc);
        }
    }

    void S3Handler::GetBucketKeyFromUri(const std::string &uri, std::string &bucket, std::string &key) {

        Poco::RegularExpression::MatchVec posVec;
        Poco::RegularExpression pattern(R"(/([a-zA-Z0-9-.]+)?/?([a-zA-Z0-9-_/.*'()]+)?\??.*$)");
        if (!pattern.match(uri, 0, posVec)) {
            log_error_stream(_logger) << "Could not get bucket/key from URI, uri: " << uri << std::endl;
            throw Core::ResourceNotFoundException("Could not extract bucket/key from URI");
        }

        if (posVec.size() > 1) {
            bucket = uri.substr(posVec[1].offset, posVec[1].length);
        }
        if (posVec.size() > 2) {
            key = uri.substr(posVec[2].offset, posVec[2].length);
        }
    }
}
