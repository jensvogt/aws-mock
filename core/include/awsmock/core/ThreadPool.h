//
// Created by vogje01 on 22/05/2023.
//

#ifndef AWSMOCK_CORE_THREADPOOL_H
#define AWSMOCK_CORE_THREADPOOL_H

// Poco includes
#include <Poco/Logger.h>
#include <Poco/ThreadPool.h>

// AwsMock includes
#include <awsmock/core/Configuration.h>

#define CONFIG_NAME_TAG "awsmock.threadpool.name"
#define CONFIG_INITIAL_TAG "awsmock.threadpool.min"
#define CONFIG_MAX_TAG "awsmock.threadpool.max"
#define CONFIG_WAIT_TIME_TAG "awsmock.threadpool.idletime"

#define DEFAULT_NAME "worker-pool"
#define DEFAULT_INITIAL 4
#define DEFAULT_MAX 32
#define DEFAULT_WAITTIME 1000
#define DEFAULT_IDLETIME 60000

namespace AwsMock::Core {

    template<typename C>
    class ThreadPool : public Poco::ThreadPool {

    public:

      /**
       * Constructor.
       *
       * @param configuration thread configuration
       */
      [[maybe_unused]]
      explicit ThreadPool(const Configuration &configuration) : ThreadPool(configuration.getString(CONFIG_NAME_TAG, DEFAULT_NAME),
                                                                           configuration.getInt(CONFIG_INITIAL_TAG, DEFAULT_INITIAL),
                                                                           configuration.getInt(CONFIG_MAX_TAG, DEFAULT_MAX),
                                                                           configuration.getInt(CONFIG_WAIT_TIME_TAG, DEFAULT_IDLETIME)) {};
      /**
       * Constructor.
       *
       * @param waitTime thread wait time
       * @param initial initial size
       * @param max max size
       * @param waitTime thread wait time
x       */
      explicit ThreadPool(const std::string &name = DEFAULT_NAME, int initial = DEFAULT_INITIAL, int max = DEFAULT_MAX, int waitTime = DEFAULT_WAITTIME)
          : _logger(Poco::Logger::get("ThreadPool")), Poco::ThreadPool(name, initial, max, waitTime), _initial(initial), _max(max), _idleTime(waitTime) {
          poco_debug(_logger,
                     "Thread pool initialized, name: " + name + " initial: " + std::to_string(initial) + " max: " + std::to_string(max) + " waitTime: "
                         + std::to_string(waitTime));
      }

      /**
       * Destructor
       */
      virtual ~ThreadPool() {
          poco_debug(_logger,"Shutting down thread pool, name: " + _name);
          joinAll();
          _threads.clear();
      }

      /**
       * Set name.
       *
       * @param name name of the threads
       */
      void SetName(const std::string &name) {
          _name = name;
      }

      /**
       * Start thread.
       *
       * @param args variable list of process arguments
       */
      template<class... Args>
      void StartThread(Args&&... args) {
          Poco::ScopedLock lock(_mutex);
          WaitForAvailableThread();
          const std::string threadName = _name + "-" + std::to_string(used());
          _threads[threadName] = std::make_unique<C>(std::forward<Args>(args)...);
          start(*_threads[threadName], threadName);
      }

      /**
       * Dumps thread list to the logger.
       */
      void DumpThreads() {
          for (auto &it : _threads) {
              poco_information(_logger, "Thread, name: " + it.first + " running: " + std::to_string(it.second->GetRunning()));
          }
      }

    private:

      /**
       * Cleanup the internal thread map
       */
      void CleanupThreads() {
          for (auto &it : _threads) {
              if (!it.second->GetRunning()) {
                  _threads.erase(it.first);
              }
          }
      }

      /**
       * Blocks until a thread is available
       */
      void WaitForAvailableThread() {
          while (!available()) {
              poco_debug(_logger, "Waiting for thread");
              Poco::Thread::sleep(_idleTime);
              collect();
              CleanupThreads();
          }
      }

      /**
       * Logger
       */
      Poco::Logger &_logger;

      /**
       * Name
       */
      std::string _name;

      /**
       * Initial size
       */
      int _initial;

      /**
       * Maximal size
       */
      int _max;

      /**
       * Thread wait time
       */
      int _idleTime;

      /**
       * Thread vector
       */
      std::map<std::string, std::unique_ptr<C>> _threads;

      /**
       * Mutex
       */
       Poco::Mutex _mutex;
    };

} // namespace AwsMock::Core

#endif // AWSMOCK_CORE_THREADPOOL_H
