//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/core/DirectoryWatcher.h>
#include <sys/poll.h>

namespace AwsMock::Core {

    DirectoryWatcher::DirectoryWatcher(const std::string &rootDir) : _logger(Poco::Logger::get("DirectoryWatcher")) {
        Initialize(rootDir);
    }

    DirectoryWatcher::~DirectoryWatcher() noexcept {
        _running = false;
        Poco::Thread::sleep(_timeout);
        for (const auto& watcher: _watcherMap) {
            inotify_rm_watch(fd, watcher.first);
        }
        _watcherMap.clear();
        _lockedFiles.clear();
    }

    void DirectoryWatcher::Initialize(const std::string &rootDir) {

        fd = inotify_init();
        int wd = inotify_add_watch(fd, rootDir.c_str(), IN_CREATE | IN_CLOSE | IN_MODIFY | IN_DELETE);
        _watcherMap[wd] = rootDir;
        log_debug_stream(_logger) << "Root dir: " << rootDir << std::endl;

        Poco::RecursiveDirectoryIterator it(rootDir);
        Poco::RecursiveDirectoryIterator end;
        while (it != end) {
            if (it->isDirectory()) {
                log_debug_stream(_logger) << "Adding directory, path: " << it.name() << std::endl;
                wd = inotify_add_watch(fd, it.path().toString().c_str(), IN_CREATE | IN_CLOSE | IN_MODIFY | IN_DELETE);
                _watcherMap[wd] = it.path().toString();
            }
            ++it;
        }
        log_debug_stream(_logger) << "File watcher initialized, path: " << rootDir << std::endl;
    }

    void DirectoryWatcher::run() {

        int i, wd;
        char buffer[BUF_LEN];
        struct pollfd pollfd = {.fd = fd};
        _running = true;

        while (_running) {
            while (poll(&pollfd, 1, _timeout) < 0)
            {
                if (errno == EAGAIN || errno == EINTR)
                    continue;
            }
            i = 0;
            ssize_t length = read(fd, buffer, BUF_LEN);
            if (length < 0) {
                log_error_stream(_logger) << "Invalid _watcher struct length, path: " << length << std::endl;
            }
            while (i < length) {
                auto *event = (struct inotify_event *) &buffer[i];
                if (event->len) {

                    std::string rootDir = _watcherMap[event->wd];
                    std::string fileName = GetFilename(rootDir, event->name);

                    if (!IsLocked(fileName)) {

                        // Directory added
                        if (event->mask & IN_CREATE && event->mask & IN_ISDIR) {
                            wd = inotify_add_watch(fd, fileName.c_str(), IN_ALL_EVENTS | IN_ONESHOT);
                            _watcherMap[wd] = fileName;
                            DirectoryEvent ev(fileName, DW_ITEM_ADDED, DW_DIR_TYPE);
                            this->itemAdded(this, ev);
                        } else if (event->mask & IN_CLOSE && !(event->mask & IN_ISDIR)) {
                            // File written
                            DirectoryEvent ev(fileName, DW_ITEM_ADDED, DW_FILE_TYPE);
                            LockFile(fileName);
                            this->itemAdded(this, ev);
                        } else if (event->mask & IN_MODIFY) {
                            // Directory modified
                            DirectoryEvent ev(fileName, DW_ITEM_MODIFIED, DW_DIR_TYPE);
                            this->itemModified(this, ev);
                        }
                        if (event->mask & IN_DELETE && event->mask & IN_ISDIR) {
                            // Directory deleted
                            auto it = _watcherMap.find(event->wd);
                            if (it != _watcherMap.end()) {
                                _watcherMap.erase(it);
                            }
                            inotify_rm_watch(fd, event->wd);
                            DirectoryEvent ev(fileName, DW_ITEM_REMOVED, DW_DIR_TYPE);
                            this->itemDeleted(this, ev);
                        }
                        if (event->mask & IN_DELETE && !(event->mask & IN_ISDIR)) {
                            // File deleted
                            DirectoryEvent ev(fileName, DW_ITEM_REMOVED, DW_FILE_TYPE);
                            this->itemDeleted(this, ev);
                            //UnlockFile(fileName);
                        }
                    }
                }
                i += (int) (EVENT_SIZE + event->len);
            }
            memset(buffer, 0x00, BUFSIZ);
        }
    }

    std::string DirectoryWatcher::GetFilename(const std::string &rootDir, const char *fileName) {
        return {rootDir + "/" + std::string(fileName)};
    }

    void DirectoryWatcher::LockFile(const std::string &fileName) {
        // Cleanup
        for (const auto &it : _lockedFiles) {
            erase_if(_lockedFiles, [](const std::pair<const std::basic_string<char>,
                                                std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long, std::ratio<1, 1000000000>>>>& item) {
              return DURATION(item.second) > 5000;
            });
        }
        _lockedFiles[fileName] = Clock::now();
        std::cerr << "size: " << _lockedFiles.size() << std::endl;
    }

    bool DirectoryWatcher::IsLocked(const std::string &fileName) {
        return _lockedFiles.find(fileName) != _lockedFiles.end();
    }

    void DirectoryWatcher::ClearLocks() {
        _lockedFiles.clear();
        std::cerr << "size: " << _lockedFiles.size() << std::endl;
    }

    void DirectoryWatcher::ClearWatcher() {
        for (const auto &it : _watcherMap) {
            inotify_rm_watch(fd, it.first);
        }
        _watcherMap.clear();
    }
}