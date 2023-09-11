//
// Created by vogje01 on 23/07/2023.
//

#include <awsmock/core/CurlUtils.h>

namespace AwsMock::Core {

    CurlUtils::CurlUtils() : _logger(Poco::Logger::get("CurlUtils")) {}

    CurlResponse CurlUtils::SendRequest(const std::string &method, const std::string &path) {

        _readBuffer={};
        curl = curl_easy_init();
        if (!curl) {
            log_error_stream(_logger) << "Error while initiating curl" << std::endl;
            curl_easy_cleanup(curl);
            return {};
        }

        // Set headers
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");

        // Set options
        //std::string url = HOST_URI + path;
        curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, DOCKER_SOCKET);
        curl_easy_setopt(curl, CURLOPT_URL, path.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &_readBuffer);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            log_error_stream(_logger) << "Request send failed, url: " << path << " error: " << curl_easy_strerror(res) << std::endl;
        }

        int status = 0;
        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &status);

        CurlResponse response = {.statusCode = status, .statusReason=curl_easy_strerror(res), .output=_readBuffer};
        curl_easy_cleanup(curl);

        return response;
    }

    CurlResponse CurlUtils::SendRequest(const std::string &method, const std::string &path, const std::string &body) {

        _readBuffer={};
        curl = curl_easy_init();
        if (!curl) {
            log_error_stream(_logger) << "Error while initiating curl" << std::endl;
            curl_easy_cleanup(curl);
            return {};
        }

        // Set headers
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");

        // Set options
        curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, DOCKER_SOCKET);
        curl_easy_setopt(curl, CURLOPT_URL, path.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &_readBuffer);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            log_error_stream(_logger) << "Request send failed, path: " << path << " error: " << curl_easy_strerror(res) << std::endl;
        }

        int status = 0;
        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &status);

        CurlResponse response = {.statusCode = status, .statusReason=curl_easy_strerror(res), .output=_readBuffer};
        curl_easy_cleanup(curl);

        return response;
    }

    CurlResponse CurlUtils::SendFileRequest(const std::string &method, const std::string &url, const std::string &header, const std::string &fileName) {

        _readBuffer={};
        curl = curl_easy_init();
        if (!curl) {
            log_error_stream(_logger) << "Error while initiating curl" << std::endl;
            curl_easy_cleanup(curl);
            return {};
        }

        FILE *fd;
        fd = fopen(fileName.c_str(), "rb");
        if(!fd) {
            return {};
        }

        /* to get the file size */
        struct stat fileInfo{};
        if(fstat(fileno(fd), &fileInfo) != 0)
            return {};

        // Set headers
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");

        // Set options
        curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, DOCKER_SOCKET);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &_readBuffer);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_READDATA, fd);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)fileInfo.st_size);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            log_error_stream(_logger) << "Request send failed, url: " << url << " error: " << curl_easy_strerror(res) << std::endl;
        }
        free(fd);

        int status = 0;
        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &status);

        CurlResponse response = {.statusCode = status, .statusReason=curl_easy_strerror(res), .output=_readBuffer};
        curl_easy_cleanup(curl);

        return response;
    }
}