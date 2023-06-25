//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_WORKER_S3WORKER_H
#define AWSMOCK_WORKER_S3WORKER_H

// C++ standard includes
#include <string>
#include <sys/inotify.h>

// Poco includes
#include "Poco/Delegate.h"
#include <Poco/DirectoryWatcher.h>
#include <Poco/Logger.h>
#include <Poco/LogStream.h>
#include <Poco/Path.h>
#include <Poco/Runnable.h>
#include <Poco/ScopedLock.h>

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/DirectoryWatcher.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/db/ServiceDatabase.h>
#include <awsmock/dto/s3/PutObjectRequest.h>
#include <awsmock/service/S3Service.h>

namespace AwsMock::Worker {

    class S3Worker : public Poco::Runnable {

    public:

      /**
       * Constructor
       */
      explicit S3Worker(const Core::Configuration &configuration);

      /**
       * Destructor
       */
      ~S3Worker() override;

      /**
       * Main method
       */
      void run() override;

    private:

      /**
       * Initialization
       */
      void Initialize();

      /**
       * Callback for the directory watcher add events.
       *
       * @param addEvent directory watcher add event.
       */
      void OnFileAdded(const Core::DirectoryEvent &addEvent);

      /**
       * Callback for the directory watcher change events.
       *
       * @param modifiedEvent directory watcher change event.
       */
      void OnFileModified(const Core::DirectoryEvent &modifiedEvent);

      /**
       * Callback for the directory watcher delete events.
       *
       * @param deleteEvent directory watcher delete event.
       */
      void OnFileDeleted(const Core::DirectoryEvent &deleteEvent);

      /**
       * Gets the bucket and object key from the file name.
       *
       * @param fileName absolute file file name
       * @param bucket bucket name
       * @param key object key
       */
      void GetBucketKeyFromFile(const std::string &fileName, std::string &bucket, std::string &key);

      /**
       * Sends a put object request to the S3 service
       *
       * @param bucket S3 bucket name
       * @param key S3 object key
       * @param md5Sum MD5 hash
       * @param contentType content type
       * @param fileSize size of the file
       */
      void SendPutObjectRequest(const std::string &bucket, const std::string &key, const std::string &md5Sum, const std::string &contentType, long fileSize);

      /**
       * Logger
       */
      Core::LogStream _logger;

      /**
       * Configuration
       */
      const Core::Configuration &_configuration;

      /**
       * Service database
       */
      std::unique_ptr<Database::ServiceDatabase> _serviceDatabase;

      /**
       * S3 service
       */
      std::unique_ptr<Service::S3Service> _s3Service;

      /**
       * Data directory
       */
      std::string _dataDir;

      /**
       * Watcher directory
       */
      std::string _watcherDir;

      /**
       * Directory _watcher thread
       */
      Poco::Thread _watcherThread;

      /**
       * AWS region
       */
      std::string _region;

      /**
       * Directory _watcher
       */
      Core::DirectoryWatcher *_watcher;

      /**
       * Running flag
       */
      bool _running;

      /**
       * Sleeping period in ms
       */
      bool _period;

      /**
       * S3 service host
       */
      std::string _s3ServiceHost;

      /**
       * S3 service port
       */
      int _s3ServicePort;
    };

} // namespace AwsMock::Worker

#endif //AWSMOCK_WORKER_S3WORKER_H
