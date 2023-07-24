//
// Created by vogje01 on 23/07/2023.
//

#ifndef AWSMOCK_CORE_CURLUTILS_H
#define AWSMOCK_CORE_CURLUTILS_H

// C++ standard includes
#include <string>
#include <iostream>
#include <utility>
#include <sys/stat.h>

// Curl includes
#include <curl/curl.h>

// AwsMock includes
#include "awsmock/core/Logger.h"
#include "awsmock/core/LogStream.h"

#define DOCKER_SOCKET "/var/run/docker.sock"

typedef enum {
  GET,
  POST,
  DELETE,
  PUT
} Methods;

namespace AwsMock::Core {

    /**
     * CURL utilities.
     *
     * @author jens.vogt@opitz-consulting.com
     */
    class CurlUtils {

    public:

      /**
       * Constructor
       */
      CurlUtils();

      /*
       * Constructor
       */
      CurlUtils(std::string host);

      /**
       * Destructor
       */
      ~CurlUtils();

      /**
       * Send request
       *
       * @param method HTTP method
       * @param path request path
       * @return response string
       */
      std::string SendRequest(const std::string &method, const std::string &path);

      /**
       * Send request
       *
       * @param method HTTP method
       * @param path request path
       * @param body request body
       * @return response string
       */
      std::string SendRequest(const std::string &method, const std::string &path, const std::string &body);

      /**
       * Send request
       *
       * @param method HTTP method
       * @param path request path
       * @param headers request extra headers
       * @param fileName filename to send as request body
       * @return response string
       */
      std::string SendFileRequest(const std::string &method, const std::string &path, const std::string &header, const std::string &fileName);

    private:

      /**
       * Logger
       */
      Core::LogStream _logger;

      /**
       * Curl
       */
      CURL *curl{};

      /**
       * Curl response codes
       */
      CURLcode res{};

      /**
       * Host URI
       */
      std::string _hostUri;

      /**
       * Curl headers
       */
      struct curl_slist *headers = nullptr;

      /**
       * Remote flag
       */
      bool _isRemote = false;

      std::string _readBuffer;

      /**
       * Write callback
       *
       * @param contents
       * @param size
       * @param nmemb
       * @param userp
       * @return
       */
      static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){
          ((std::string*)userp)->append((char*)contents, size * nmemb);
          return size * nmemb;
      }
    };
}

#endif //AWSMOCK_CORE_CURLUTILS_H