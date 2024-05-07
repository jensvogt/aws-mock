cmake_minimum_required(VERSION 3.22)
project(awsmock)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_STANDARD 20)
if (NOT WIN32)
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH " ..." FORCE)
else ()
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(AWSMOCK_WIN32_DIR "D:\\work\\win32")
    #    set(CMAKE_CXX_FLAGS "/cgthreads8")
endif ()
set(CMAKE_BUILD_PARALLEL_LEVEL 30)

# Doxygen
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen and dot) " OFF)

# Current version
file(READ "src/core/include/awsmock/core/Version.h" ver)
string(REGEX MATCH "#define PROJECT_VERSION \"([0-9\.]+)\"" _ ${ver})
set(PROJECT_VERSION ${CMAKE_MATCH_1})
string(TIMESTAMP PROJECT_BUILDDATE "%d-%m-%Y")

# Build message
message("-- Building ${PROJECT_NAME} v${PROJECT_VERSION} type: ${CMAKE_BUILD_TYPE}, install prefix: ${CMAKE_INSTALL_PREFIX}")

# Set default build type release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE PATH "" FORCE)
endif ()

# Testing
enable_testing()

add_subdirectory(src/core)
add_subdirectory(src/dto)
add_subdirectory(src/db)
add_subdirectory(src/service)
add_subdirectory(src/manager)
add_subdirectory(src/controller)
if (!WIN32)
    add_subdirectory(docs)
endif ()

install(FILES dist/etc/awsmock.properties DESTINATION /etc)
install(FILES dist/etc/systemd/system/awsmock.service DESTINATION /etc/systemd/system)
install(FILES dist/bin/awslocal DESTINATION ${CMAKE_INSTALL_PREFIX}/bin PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE)
