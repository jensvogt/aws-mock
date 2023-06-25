//
// Created by vogje01 on 21/12/2022.
// Copyright 2022, 2023 Jens Vogt
//
// This file is part of aws-mock.
//
// aws-mock is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// aws-mock is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with aws-mock.  If not, see <http://www.gnu.org/licenses/>.

#ifndef AWSMOCK_CORE_DIRECTORYWATCHER_H
#define AWSMOCK_CORE_DIRECTORYWATCHER_H

// C standard includes
#include <sys/inotify.h>
#include <unistd.h>

// C++ standard includes
#include <string>
#include <map>
#include <iostream>
#include <utility>

// Poco includes
#include <Poco/BasicEvent.h>
#include <Poco/File.h>
#include <Poco/Logger.h>
#include <Poco/LogStream.h>
#include <Poco/Mutex.h>
#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include <Poco/Delegate.h>
#include <Poco/DirectoryIterator.h>

// AwsMock includes
#include <awsmock/core/Logger.h>

#define MAX_EVENTS 1024 // Max. number of events to process at one go
#define LEN_NAME 16 // Assuming that the length of the filename won't exceed 16 bytes
#define EVENT_SIZE  (sizeof (struct inotify_event)) // size of one event
#define BUF_LEN     (MAX_EVENTS * (EVENT_SIZE + LEN_NAME)) // buffer to store the data of events

namespace AwsMock::Core {

    enum DirectoryEventType
    {
      DW_ITEM_ADDED = 1,
      /// A new item has been created and added to the directory.

      DW_ITEM_REMOVED = 2,
      /// An item has been removed from the directory.

      DW_ITEM_MODIFIED = 4,
      /// An item has been modified.

      DW_ITEM_MOVED_FROM = 8,
      /// An item has been renamed or moved. This event delivers the old name.

      DW_ITEM_MOVED_TO = 16
      /// An item has been renamed or moved. This event delivers the new name.
    };

    enum DirectoryEventMask
    {
      DW_FILTER_ENABLE_ALL = 31,
      /// Enables all event types.

      DW_FILTER_DISABLE_ALL = 0
      /// Disables all event types.
    };

    struct DirectoryEvent
    {
      DirectoryEvent(const Poco::File& f, DirectoryEventType ev): item(f), event(ev) {}

      /**
       * The directory or file that has been changed.
       */
      const Poco::File& item;

      /**
       * The kind of event.
       */
      DirectoryEventType event;

    };

    class DirectoryWatcher : public Poco::Runnable {

    public:

      /**
       * Constructor
       */
      explicit DirectoryWatcher(std::string rootDir);

      /**
       * Initialization
       */
      void Initialize();

      /**
       * Main thread running method
       */
      [[noreturn]] void run() override;

      /**
       * Lock the watcher.
       *
       * <p>Only when the lock is released the watcher escalates the events and calls the callbacks.</p>
       */
      void Lock();

      /**
       * Unlock the watcher.
       *
       * <p>Only in unlocked state the watcher events arse escalated.</p>
       */
      void Unlock();

      /**
       * Added event
       */
      Poco::BasicEvent<const DirectoryEvent> itemAdded;

      /**
       * Modified event
       */
      Poco::BasicEvent<const DirectoryEvent> itemModified;

      /**
       * Deleted event
       */
      Poco::BasicEvent<const DirectoryEvent> itemDeleted;

    private:

      /**
       * Returns th file name
       * @param rootDir root directory
       * @param fileName name of the file
       * @return absolute file name
       */
      static std::string GetFilename(const std::string &rootDir, const char* fileName);

      /**
       * Logger
       */
      Poco::LogStream _logger;

      /**
       * Root directory
       */
      std::string _rootDir;

      /**
       * File descriptor
       */
      int fd = 0;

      /**
       * Watcher map with absolute file pathes
       */
      std::map<int, std::string> _watcherMap;

      /**
       * Locking mechanism
       */
      Poco::Mutex _mutex;
    };

} // namespace AwsMock::Core

#endif // AWSMOCK_CORE_DIRECTORYWATCHER_H