//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LISTQUEUERESPONSE_H
#define AWSMOCK_DTO_SQS_LISTQUEUERESPONSE_H

// C++ standard includes
#include <string>
#include <sstream>
#include <iostream>

// Poco includes
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/UUID.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/DOMWriter.h"
#include "Poco/XML/XMLWriter.h"

// AwsMock includes
#include "awsmock/entity/sqs/Queue.h"

namespace AwsMock::Dto::SQS {

    class ListQueueResponse {

    public:

      /**
       * Constructor
       *
       * @param queueList list of queue entities.
       */
      explicit ListQueueResponse(Database::Entity::SQS::QueueList queueList);

      /**
       * Convert to XML representation
       *
       * @return XML string
       */
      std::string ToXml();

      /**
       * Converts the DTO to a string representation.
       *
       * @return DTO as string for logging.
       */
      [[nodiscard]] std::string ToString() const;

    private:

      /**
       * List of buckets
       */
      Database::Entity::SQS::QueueList _queueList;

      /**
       * Stream provider.
       *
       * @return output stream
       */
      friend std::ostream &operator<<(std::ostream &, const ListQueueResponse &);
    };

} // namespace AwsMock::Dto::SQS

#endif // AWSMOCK_DTO_SQS_LISTQUEUERESPONSE_H