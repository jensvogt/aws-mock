//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_S3_DATABASE_H
#define AWSMOCK_REPOSITORY_S3_DATABASE_H

// C++ standard includes
#include <iostream>
#include <string>
#include <vector>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/document.hpp>

// AwsMock includes
#include "awsmock/core/config/Configuration.h"
#include "awsmock/core/exception/DatabaseException.h"
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/entity/s3/Bucket.h>
#include <awsmock/entity/s3/Object.h>
#include <awsmock/memorydb/S3MemoryDb.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

    /**
     * @brief S3 MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class S3Database : public DatabaseBase {

      public:

        /**
         * @brief Constructor
         */
        explicit S3Database();

        /**
         * @brief Singleton instance
         */
        static S3Database &instance() {
            static S3Database s3Database;
            return s3Database;
        }

        /**
         * @brief Bucket exists
         *
         * @param region AWS region
         * @param name bucket name
         * @return true if bucket exists
         * @throws DatabaseException
         */
        bool BucketExists(const std::string &region, const std::string &name);

        /**
         * @brief Bucket exists
         *
         * @param bucket bucket entity
         * @return true if bucket exists
         * @throws DatabaseException
         */
        bool BucketExists(const Entity::S3::Bucket &bucket);

        /**
         * @brief Returns the total number of buckets
         *
         * @return total number of buckets
         * @throws DatabaseException
         */
        long BucketCount();

        /**
         * @brief Returns the bucket by userPoolId
         *
         * @param oid bucket oid
         * @return bucket, if existing
         * @throws DatabaseException
         */
        Entity::S3::Bucket GetBucketById(bsoncxx::oid oid);

        /**
         * @brief Returns the bucket by userPoolId
         *
         * @param oid bucket oid
         * @return bucket, if existing
         * @throws DatabaseException
         */
        Entity::S3::Bucket GetBucketById(const std::string &oid);

        /**
         * @brief Returns the bucket by region and name.
         *
         * @param region AWS region
         * @param name bucket name
         * @return bucket entity
         */
        Entity::S3::Bucket GetBucketByRegionName(const std::string &region, const std::string &name);

        /**
         * @brief Create a new bucket in the S3 bucket table
         *
         * @param bucket bucket entity
         * @return created bucket entity
         * @throws DatabaseException
         */
        Entity::S3::Bucket CreateBucket(const Entity::S3::Bucket &bucket);

        /**
         * @brief List all buckets
         *
         * @return BucketList
         */
        Entity::S3::BucketList ListBuckets();

        /**
         * @brief Check whether the bucket has still objects
         *
         * @param bucket bucket entity
         * @return true if bucket exists
         * @throws DatabaseException
         */
        bool HasObjects(const Entity::S3::Bucket &bucket);

        /**
         * @brief List objects on a bucket.
         *
         * @param region AWS region
         * @param bucket bucket name
         * @param maxKeys maximal number of return elements
         * @return list of S3 objects
         */
        std::vector<Entity::S3::Object> GetBucketObjectList(const std::string &region, const std::string &bucket, long maxKeys);

        /**
         * @brief Updates a bucket
         *
         * @param bucket bucket entity
         * @return created bucket entity
         * @throws DatabaseException
         */
        Entity::S3::Bucket UpdateBucket(const Entity::S3::Bucket &bucket);

        /**
         * @brief Create a new bucket or updated a existing bucket
         *
         * @param bucket bucket entity
         * @return created bucket entity
         * @throws DatabaseException
         */
        Entity::S3::Bucket CreateOrUpdateBucket(const Entity::S3::Bucket &bucket);

        /**
         * @brief Create a new S3 object in the S3 object table
         *
         * @param object object entity
         * @return created object entity
         * @throws DatabaseException
         */
        Entity::S3::Object CreateObject(const Entity::S3::Object &object);

        /**
         * @brief Create a new S3 object in the S3 object table, if it does not exist, otherwise update the exiting object.
         *
         * @param object object entity
         * @return created or updated object entity
         * @throws DatabaseException
         */
        Entity::S3::Object CreateOrUpdateObject(const Entity::S3::Object &object);

        /**
         * @brief Updates an existing object in the S3 object table
         *
         * @param object object entity
         * @return updated object entity
         * @throws DatabaseException
         */
        Entity::S3::Object UpdateObject(const Entity::S3::Object &object);

        /**
         * @brief Check the existence of an object
         *
         * @param object object entity
         * @return true if existing otherwise false
         * @throws DatabaseException
         */
        bool ObjectExists(const Entity::S3::Object &object);

        /**
         * @brief Check the existence of an object by internal name
         *
         * @param filename object internal name
         * @return true if existing otherwise false
         * @throws DatabaseException
         */
        bool ObjectExists(const std::string &filename);

        /**
         * @brief Gets an object from an bucket
         *
         * @param region AWS S3 region name
         * @param bucket object bucket
         * @param key object key
         * @return S3 object
         * @throws DatabaseException
         */
        Entity::S3::Object GetObject(const std::string &region, const std::string &bucket, const std::string &key);

        /**
         * @brief Gets an object from a bucket using the bucket, key and MD5 sum as query parameter
         *
         * @param region AWS S3 region name
         * @param bucket object bucket
         * @param key object key
         * @param md5sum MD5 sum of object
         * @return S3 object
         * @throws DatabaseException
         */
        Entity::S3::Object GetObjectMd5(const std::string &region, const std::string &bucket, const std::string &key, const std::string &md5sum);

        /**
         * @brief Gets an object from a bucket using the bucket, key and MD5 sum as query parameter
         *
         * @param region AWS S3 region name
         * @param bucket object bucket
         * @param key object key
         * @param version version ID
         * @return S3 object
         * @throws DatabaseException
         */
        Entity::S3::Object GetObjectVersion(const std::string &region, const std::string &bucket, const std::string &key, const std::string &version);

        /**
         * @brief Gets an object from an bucket
         *
         * @param oid object ID
         * @return S3 object
         * @throws DatabaseException
         */
        Entity::S3::Object GetObjectById(bsoncxx::oid oid);

        /**
         * @brief Gets an object from an bucket
         *
         * @param oid object ID
         * @return S3 object
         * @throws DatabaseException
         */
        Entity::S3::Object GetObjectById(const std::string &oid);

        /**
         * @brief List all objects of a bucket
         *
         * @param bucket S3 bucket name
         * @param prefix S3 key prefix
         * @return ObjectList
         */
        Entity::S3::ObjectList ListBucket(const std::string &bucket, const std::string &prefix = {});

        /**
         * @brief List all objects.
         *
         * @param prefix S3 key prefix
         * @return ObjectList
         */
        Entity::S3::ObjectList ListObjects(const std::string &prefix = {});

        /**
         * @brief Counts the number of keys in a bucket
         *
         * @param region AWS region
         * @param bucket bucket name
         * @return number of object in bucket
         * @throws DatabaseException
         */
        long ObjectCount(const std::string &region = {}, const std::string &bucket = {});

        /**
         * @brief Creates a bucket notification-
         *
         * <p>In case of a wildcard notification all notifications are added.</p>
         *
         * @param bucket S3 bucket
         * @param bucketNotification bucket notification
         * @return updated Bucket entity
         */
        Entity::S3::Bucket CreateBucketNotification(const Entity::S3::Bucket &bucket, const Entity::S3::BucketNotification &bucketNotification);

        /**
         * @brief Creates a bucket notification-
         *
         * @param bucket S3 bucket
         * @param bucketNotification bucket notification
         * @return updated Bucket entity
         */
        Entity::S3::Bucket DeleteBucketNotifications(const Entity::S3::Bucket &bucket, const Entity::S3::BucketNotification &bucketNotification);

        /**
         * @brief Delete a bucket.
         *
         * @param bucket bucket entity
         * @throws DatabaseException
         */
        void DeleteBucket(const Entity::S3::Bucket &bucket);

        /**
         * @brief Deletes all buckets
         */
        void DeleteAllBuckets();

        /**
         * @brief Delete an object.
         *
         * @param object object entity
         * @throws DatabaseException
         */
        void DeleteObject(const Entity::S3::Object &object);

        /**
         * @brief Updates an existing object in the S3 object table
         *
         * @param bucket bucket to delete from
         * @param keys vector of object keys
         * @throws DatabaseException
         */
        void DeleteObjects(const std::string &bucket, const std::vector<std::string> &keys);

        /**
         * @brief Deletes all objects
         */
        void DeleteAllObjects();

      private:

        /**
         * Allowed event types
         */
        static std::map<std::string, std::vector<std::string>> allowedEventTypes;

        /**
         * Use MongoDB
         */
        bool _useDatabase;

        /**
         * Database name
         */
        std::string _databaseName;

        /**
         * Bucket collection name
         */
        std::string _bucketCollectionName;

        /**
         * Object collection name
         */
        std::string _objectCollectionName;

        /**
         * S3 in-memory database
         */
        S3MemoryDb &_memoryDb;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_S3_DATABASE_H
