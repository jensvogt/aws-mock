//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/core/DirectoryWatcher.h>

namespace AwsMock::Core {

    DirectoryWatcher::DirectoryWatcher(std::string rootDir) : _logger(Poco::Logger::get("DirectoryWatcher")), _rootDir(std::move(rootDir)) {
        Core::Logger::SetDefaultConsoleLogger("DirectoryWatcher");
        Initialize();
    }

    void DirectoryWatcher::Initialize() {

        fd = inotify_init();
        int wd = inotify_add_watch(fd, _rootDir.c_str(), IN_CREATE | IN_MODIFY | IN_DELETE);
        _watcherMap[wd] = _rootDir;

        Poco::DirectoryIterator it(_rootDir);
        Poco::DirectoryIterator end;
        while (it != end) {
            _logger.debug() << "Adding directory, path: " << it.name() << std::endl;
            wd = inotify_add_watch(fd, it.path().toString().c_str(), IN_CREATE | IN_MODIFY | IN_DELETE);
            _watcherMap[wd] = it.path().toString();
            ++it;
        }
        _logger.debug() << "File _watcher initialized, path: " << _rootDir << std::endl;
    }

    void DirectoryWatcher::Lock() {
        _mutex.lock();
    }

    void DirectoryWatcher::Unlock() {
        _mutex.unlock();
    }

    [[noreturn]] void DirectoryWatcher::run() {

        int i, wd;
        char buffer[BUF_LEN];

        while (true) {
            i = 0;
            ssize_t length = read(fd, buffer, BUF_LEN);
            if (length < 0) {
                _logger.debug() << "Invalid _watcher struct length, path: " << length << std::endl;
            }
            while (i < length) {
                auto *event = (struct inotify_event *) &buffer[i];
                if (event->len) {
                    if (_mutex.tryLock()) {
                        if (event->mask & IN_CREATE) {
                            std::string rootDir = _watcherMap[event->wd];
                            std::string fileName = GetFilename(rootDir, event->name);
                            if (event->mask & IN_ISDIR) {
                                wd = inotify_add_watch(fd, fileName.c_str(), IN_CREATE | IN_MODIFY | IN_DELETE);
                                _watcherMap[wd] = fileName;
                            }
                            Poco::File f(fileName);
                            DirectoryEvent ev(f, DW_ITEM_ADDED);
                            this->itemAdded(this, ev);
                        }

                        if (event->mask & IN_MODIFY) {
                            std::string rootDir = _watcherMap[event->wd];
                            std::string fileName = GetFilename(rootDir, event->name);
                            Poco::File f(fileName);
                            DirectoryEvent ev(f, DW_ITEM_MODIFIED);
                            this->itemModified(this, ev);
                        }

                        if (event->mask & IN_DELETE) {
                            if (event->mask & IN_ISDIR) {
                                auto it = _watcherMap.find(event->wd);
                                if (it != _watcherMap.end()) {
                                    _watcherMap.erase(it);
                                }
                                inotify_rm_watch(fd, event->wd);
                            }
                            std::string rootDir = _watcherMap[event->wd];
                            std::string fileName = GetFilename(rootDir, event->name);
                            Poco::File f(fileName);
                            DirectoryEvent ev(f, DW_ITEM_REMOVED);
                            this->itemDeleted(this, ev);
                        }
                    }
                }
                i += (int)EVENT_SIZE + event->len;
            }
        }
    }

    std::string DirectoryWatcher::GetFilename(const std::string &rootDir, const char* fileName) {
        char p[100] = " ";
        strcpy(p, rootDir.c_str());
        strcat(p, "/");
        strcat(p, fileName);
        return {p};
    }
}