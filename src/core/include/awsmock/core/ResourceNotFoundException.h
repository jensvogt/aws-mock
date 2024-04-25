//
// Created by vogje01 on 02/09/2022.
//

#ifndef AWSMOCK_CORE_RESOURCE_NOT_FOUND_EXCEPTION_H
#define AWSMOCK_CORE_RESOURCE_NOT_FOUND_EXCEPTION_H

#include "Poco/Exception.h"
#include "Poco/Net/HTTPResponse.h"

namespace AwsMock::Core {

    /**
   * Resource not found exception class.
   *
   * @author jens.vogt@opitz-consulting.com
   */
    class ResourceNotFoundException : public Poco::Exception {

      public:
        /**
       * Constructor.
       *
       * @param code exception code, default: 0
       */
        explicit ResourceNotFoundException(int code = Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);

        /**
       * Constructor.
       *
       * @param msg exception message
       * @param code exception code, default: 0
       */
        explicit ResourceNotFoundException(const std::string &msg,
                                           int code = Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);

        /**
       * Constructor.
       *
       * @param msg exception message
       * @param arg exception argument, will be appended to the message, separated with a ':'.
       * @param code exception code, default: 0
       */
        ResourceNotFoundException(const std::string &msg,
                                  const std::string &arg,
                                  int code = Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);

        /**
       * Constructor.
       *
       * @param msg exception message
       * @param exc parent exception.
       * @param code exception code, default: 0
       */
        ResourceNotFoundException(const std::string &msg,
                                  const Poco::Exception &exc,
                                  int code = Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);

        /**
       * Copy constructor.
       *
       * @param exc parent exception.
       */
        ResourceNotFoundException(const ResourceNotFoundException &exc);

        /**
       * Destructor
       */
        ~ResourceNotFoundException() noexcept override;

        /**
       * Assigment operator.
       */
        ResourceNotFoundException &operator=(const ResourceNotFoundException &exc);

        /**
       * Returns the exception name.
       */
        const char *name() const noexcept;

        /**
       * Returns the exception class name.
       */
        const char *className() const noexcept;

        /**
       * Returns a clone of the exception
       */
        Poco::Exception *clone() const;

        /**
       * Rethrows the exception.
       */
        void rethrow() const;
    };

}// namespace AwsMock::Core

#endif//AWSMOCK_CORE_RESOURCE_NOT_FOUND_EXCEPTION_H
