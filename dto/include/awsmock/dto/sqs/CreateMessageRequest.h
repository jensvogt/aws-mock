//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_CREATEMESSAGEREQUEST_H
#define AWSMOCK_DTO_SQS_CREATEMESSAGEREQUEST_H

// C++ standard includes
#include <string>

namespace AwsMock::Dto::SQS {

    struct MessageAttribute {

      /**
       * MessageAttribute name
       */
      std::string attributeName;

      /**
       * MessageAttribute value
       */
      std::string attributeValue;

      /**
       * Logical data type
       */
      std::string type;

      /**
       * Transport data type
       */
      std::string transportType;

      /**
       * Converts the DTO to a string representation.
       *
       * @return DTO as string for logging.
       */
      [[nodiscard]] std::string ToString() const {
          std::stringstream ss;
          ss << (*this);
          return ss.str();
      }

      /**
       * Stream provider.
       *
       * @return output stream
       */
      friend std::ostream &operator<<(std::ostream &os, const MessageAttribute &r) {
          os << "MessageAttribute={attributeName='" + r.attributeName + "' attributeValue='" + r.attributeValue + "' type='" + r.type + "' transportType='"
              + r.transportType + "'}";
          return os;
      }

    };

    typedef std::vector<MessageAttribute> MessageAttributeList;

    struct CreateMessageRequest {

      /**
       * Region
       */
      std::string region;

      /**
       * Queue URL
       */
      std::string queueUrl;

      /**
       * Queue ARN
       */
      std::string queueArn;

      /**
       * Message body
       */
      std::string body;

      /**
       * Message attributes
       */
      MessageAttributeList messageAttributes;

      /**
       * Converts the DTO to a string representation.
       *
       * @return DTO as string for logging.
       */
      [[nodiscard]] std::string ToString() const {
          std::stringstream ss;
          ss << (*this);
          return ss.str();
      }

      /**
       * Stream provider.
       *
       * @return output stream
       */
      friend std::ostream &operator<<(std::ostream &os, const CreateMessageRequest &r) {
          os << "CreateMessageRequest={region='" + r.region + "' queueUrl='" + r.queueUrl + "' queueArn='" + r.queueArn + "' body: '" + r.body + "' attributes=[";
          for (const auto &a : r.messageAttributes) {
              os << "attributeName='" + a.attributeName + "' attributeValue='" + a.attributeValue + "' ";
          }
          os << "]}";
          return os;
      }

    };

} // namespace AwsMock::Dto::SQS

#endif // AWSMOCK_DTO_SQS_CREATEMESSAGEREQUEST_H
