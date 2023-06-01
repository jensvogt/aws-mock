//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_SQS_QUEUE_H
#define AWSMOCK_DB_ENTITY_SQS_QUEUE_H

// C++ includes
#include <string>
#include <vector>

// Poco includes
#include <Poco/DateTime.h>

namespace AwsMock::Database::Entity::SQS {

    struct Queue {

      /**
       * ID
       */
      long id;

      /**
       * Queue name
       */
      std::string name;

      /**
       * AWS region
       */
      std::string region;

      /**
       * Owner
       */
      std::string owner;

      /**
       * URL
       */
      std::string url;

      /**
       * Creation date
       */
      Poco::DateTime created;

      /**
       * Last modification date
       */
      Poco::DateTime modified;
    };

    typedef struct Queue Queue;
    typedef std::vector<Queue> QueueList;

} // namespace AwsMock::Database::Entity::S3
#endif // AWSMOCK_DB_ENTITY_SQS_QUEUE_H
