//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_DB_S3DATABASE_H
#define AWSMOCK_DB_S3DATABASE_H

// C++ standard includes
#include <string>
#include <vector>
#include <iostream>

// Poco includes
#include "Poco/Logger.h"
#include "Poco/LogStream.h"

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>

// AwsMock includes
#include <awsmock/core/Logger.h>
#include <awsmock/core/Configuration.h>
#include <awsmock/core/DatabaseException.h>
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/db/Database.h>
#include <awsmock/entity/s3/Bucket.h>
#include <awsmock/entity/s3/Object.h>

namespace AwsMock::Database {

    class S3Database : public Database {

    public:

      /**
       * Constructor
       *
       * @param configuration configuration properties
       */
      explicit S3Database(const Core::Configuration &configuration);

      /**
       * Bucket exists
       *
       * @param region AWS region
       * @param name bucket name
       * @return true if bucket exists
       * @throws DatabaseException
       */
      bool BucketExists(const std::string &region, const std::string &name);

      /**
       * Bucket exists
       *
       * @param bucket bucket entity
       * @return true if bucket exists
       * @throws DatabaseException
       */
      bool BucketExists(const Entity::S3::Bucket &bucket);

      /**
       * Returns the bucket by id
       *
       * @param oid bucket oid
       * @return bucket, if existing
       * @throws DatabaseException
       */
      Entity::S3::Bucket GetBucketById(bsoncxx::oid oid);

      /**
       * Returns the bucket by id
       *
       * @param oid bucket oid
       * @return bucket, if existing
       * @throws DatabaseException
       */
      Entity::S3::Bucket GetBucketById(const std::string &oid);

      /**
       * Returns the bucket by region and name.
       *
       * @param region AWS region
       * @param name bucket name
       * @return bucket entity
       */
      Entity::S3::Bucket GetBucketByRegionName(const std::string &region, const std::string &name);

      /**
       * Create a new bucket in the S3 bucket table
       *
       * @param bucket bucket entity
       * @return created bucket entity
       * @throws DatabaseException
       */
      Entity::S3::Bucket CreateBucket(const Entity::S3::Bucket& bucket);

      /**
       * List all buckets
       *
       * @return BucketList
       */
      Entity::S3::BucketList ListBuckets();

      /**
       * CHeck whether the bucket has still objects
       *
       * @param bucket bucket entity
       * @return true if bucket exists
       * @throws DatabaseException
       */
      bool HasObjects(const Entity::S3::Bucket &bucket);

      /**
       * Updates a bucket
       *
       * @param bucket bucket entity
       * @return created bucket entity
       * @throws DatabaseException
       */
      Entity::S3::Bucket UpdateBucket(const Entity::S3::Bucket& bucket);

      /**
       * Create a new S3 object in the S3 object table
       *
       * @param object object entity
       * @return created object entity
       * @throws DatabaseException
       */
      Entity::S3::Object CreateObject(const Entity::S3::Object& object);

      /**
       * Create a new S3 object in the S3 object table, if it does not exist, otherwise update the exiting object.
       *
       * @param object object entity
       * @return created or updated object entity
       * @throws DatabaseException
       */
      Entity::S3::Object CreateOrUpdateObject(const Entity::S3::Object& object);

      /**
       * Updates an existing object in the S3 object table
       *
       * @param object object entity
       * @return updated object entity
       * @throws DatabaseException
       */
      Entity::S3::Object UpdateObject(const Entity::S3::Object& object);

      /**
       * Check the existence of an object
       *
       * @param object object entity
       * @return true if existing otherwise false
       * @throws DatabaseException
       */
      bool ObjectExists(const Entity::S3::Object& object);

      /**
       * Gets an object from an bucket
       *
       * @param region AWS S3 regsion name
       * @param bucket object bucket
       * @param key object key
       * @return S3 object
       * @throws DatabaseException
       */
      Entity::S3::Object GetObject(const std::string &region, const std::string &bucket, const std::string &key);

      /**
       * Gets an object from an bucket
       *
       * @param oid object ID
       * @return S3 object
       * @throws DatabaseException
       */
      Entity::S3::Object GetObjectById(bsoncxx::oid oid);

      /**
       * Gets an object from an bucket
       *
       * @param oid object ID
       * @return S3 object
       * @throws DatabaseException
       */
      Entity::S3::Object GetObjectById(const std::string &oid);

      /**
       * List all objects of a bucket
       *
       * @param bucket S3 bucket name
       * @param prefix S3 key prefix
       * @return ObjectList
       */
      Entity::S3::ObjectList ListBucket(const std::string &bucket, const std::string &prefix = {});

      /**
       * Counts the number of keys in a bucket
       *
       * @param bucket bucket entity
       * @return number of object in bucket
       * @throws DatabaseException
       */
      long ObjectCount(const Entity::S3::Bucket& bucket);

      /**
       * Creates a bucket notification-
       *
       * <p>In case of a wildcard notification all notifications are added.</p>
       *
       * @param bucket S3 bucket
       * @param bucketNotification bucket notification
       * @return updated Bucket entity
       */
      Entity::S3::Bucket CreateBucketNotification(const Entity::S3::Bucket &bucket, const Entity::S3::BucketNotification &bucketNotification);

      /**
       * Creates a bucket notification-
       *
       * @param bucket S3 bucket
       * @param bucketNotification bucket notification
       * @return updated Bucket entity
       */
      Entity::S3::Bucket DeleteBucketNotifications(const Entity::S3::Bucket &bucket, const Entity::S3::BucketNotification &bucketNotification);

      /**
       * Delete a bucket.
       *
       * @param bucket bucket entity
       * @throws DatabaseException
       */
      void DeleteBucket(const Entity::S3::Bucket& bucket);

      /**
       * Deletes all buckets
       */
      void DeleteAllBuckets();

      /**
       * Delete an object.
       *
       * @param object object entity
       * @throws DatabaseException
       */
      void DeleteObject(const Entity::S3::Object &object);

      /**
       * Updates an existing object in the S3 object table
       *
       * @param bucket bucket to delete from
       * @param keys vector of object keys
       * @throws DatabaseException
       */
      void DeleteObjects(const std::string &bucket, const std::vector<std::string> &keys);

      /**
       * Deletes all objects
       */
      void DeleteAllObjects();

    private:

      /**
       * Logger
       */
      Poco::LogStream _logger;

      /**
       * S3 bucket collection
       */
      mongocxx::collection _bucketCollection{};

      /**
       * S3 object collection
       */
      mongocxx::collection _objectCollection{};

      /**
       * Allowed event types
       */
      static std::map<std::string, std::vector<std::string>> allowedEventTypes;

    };

} // namespace AwsMock::Database

#endif //AWSMOCK_DB_S3DATABASE_H
