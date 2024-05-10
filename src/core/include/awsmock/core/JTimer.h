//
// Created by vogje01 on 4/12/24.
//

#ifndef AWSMOCK_CORE_JTIMER_H
#define AWSMOCK_CORE_JTIMER_H

// C++ standard includes
#include <future>
#include <iostream>
#include <stop_token>
#include <thread>


// AwsMock includes
#include <awsmock/core/LogStream.h>

namespace AwsMock::Core {

    /**
     * @brief General asynchronous job execution class
     *
     * This is supposed to stop immediately, in case a stop signal is send to the thread.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class JTimer {

      public:

        /**
         * Constructor
         *
         * @param name timer name
         * @param timeout timeout in seconds
         */
        explicit JTimer(std::string name, int timeout) : _name(std::move(name)), _timeout(timeout) {}

        /**
         * Constructor
         *
         * @param name timer name
         */
        explicit JTimer(std::string name) : _name(std::move(name)) {}

        /**
         * Start the timer
         */
        void Start();

        /**
         * Start the task
         */
        void Start(int timeout);

        /**
         * Restart the timer
         */
        void Restart();

        /**
         * Stop the task
         */
        void Stop();

        /**
         * Initialization
         */
        virtual void Initialize() = 0;

        /**
         * Main loop
         */
        virtual void Run() = 0;

        /**
         * Shutdown
         */
        virtual void Shutdown() = 0;

        /**
         * Sets the timeout.
         *
         * <p>
         * The time will be stopped and restarted
         * </p>
         */
        void SetTimeout(int timeout);

        void DoWork(std::stop_source stopSource);

      private:

        /**
         * Timer name
         */
        std::string _name;

        /**
         * Loop timeout
         */
        int _timeout{};

        /**
         * Promise for stopping thread
         */
        std::promise<void> _stop;

        /**
         * Thread handle
         */
        std::future<void> _thread_handle;

        /**
         * Stopped flag
         */
        bool _stopped = false;

        std::stop_source _stopSource;

        std::jthread _thread;
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_JTIMER_H
