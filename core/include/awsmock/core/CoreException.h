//
// Created by vogje01 on 02/09/2022.
//

#ifndef AWSMOCK_CORE_COREEXCEPTION_H
#define AWSMOCK_CORE_COREEXCEPTION_H

#include "Poco/Exception.h"

namespace AwsMock::Core {

    /**
     * COM exception class. In case of a COM request failure a COMException is thrown.
     */
    class CoreException : public Poco::Exception {
     public:
        /**
         * Constructor.
         *
         * @param code exception code, default: 0
         */
        explicit CoreException(int code = 0);

        /**
         * Constructor.
         *
         * @param msg exception message
         * @param code exception code, default: 0
         */
        explicit CoreException(const std::string &msg, int code = 0);

        /**
         * Constructor.
         *
         * @param msg exception message
         * @param arg exception argument, will be appended to the message, separated with a ':'.
         * @param code exception code, default: 0
         */
        CoreException(const std::string &msg, const std::string &arg, int code = 0);

        /**
         * Constructor.
         *
         * @param msg exception message
         * @param exc parent exception.
         * @param code exception code, default: 0
         */
        CoreException(const std::string &msg, const Poco::Exception &exc, int code = 0);

        /**
         * Copy constructor.
         *
         * @param exc parent exception.
         */
        CoreException(const CoreException &exc);

        /**
         * Destructor
         */
        ~CoreException() noexcept override;

        /**
         * Assigment operator.
         */
        CoreException &operator=(const CoreException &exc);


      /**
       * Returns the exception name.
       */
      const char *name() const noexcept override;

      /**
       * Returns the exception class name.
       */
      const char *className() const noexcept override;

      /**
       * Returns a clone of the exception
       */
      Poco::Exception *clone() const override;

      /**
       * Rethrows the exception.
       */
      void rethrow() const override;
    };

} // namespace AwsMock::Core

#endif //AWSMOCK_CORE_COREEXCEPTION_H
