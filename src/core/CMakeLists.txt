
set(LIBRARY awsmockcore)
set(LIBRARY_STATIC awsmockcore_static)

set(UTILS_SOURCES src/utils/StringUtils.cpp src/utils/FileUtils.cpp src/utils/DirUtils.cpp src/utils/DateTimeUtils.cpp
        src/utils/CryptoUtils.cpp src/utils/AwsUtils.cpp src/utils/SystemUtils.cpp
        src/utils/CurlUtils.cpp src/utils/TarUtils.cpp src/utils/RandomUtils.cpp src/utils/JsonUtils.cpp
        src/config/Configuration.cpp src/utils/TestUtils.cpp src/utils/HttpUtils.cpp src/utils/NumberUtils.cpp
        src/utils/MemoryMappedFile.cpp src/utils/Timer.cpp src/utils/Task.cpp src/utils/XmlUtils.cpp
        src/utils/Win32Utils.cpp)

set(EXCEPTION_SOURCES src/exception/CoreException.cpp src/exception/DatabaseException.cpp
        src/exception/ServiceException.cpp src/exception/ResourceNotFoundException.cpp
        src/exception/InvalidMethodCallException.cpp src/exception/JsonException.cpp)
set(LOGGING_SOURCES src/logging/LogStream.cpp)
set(MONITORING_SOURCES src/monitoring/MetricService.cpp src/monitoring/MetricSystemCollector.cpp)

set(SOURCES ${UTILS_SOURCES} ${EXCEPTION_SOURCES} ${LOGGING_SOURCES} ${MONITORING_SOURCES})

# Test directory
if (NOT CYGWIN)
add_subdirectory(tst)
endif()

# Includes
include_directories(./include)

add_library(${LIBRARY} SHARED ${SOURCES})
add_library(${LIBRARY_STATIC} STATIC ${SOURCES})
set_target_properties(${LIBRARY_STATIC} PROPERTIES OUTPUT_NAME ${LIBRARY})

if (CYGWIN)
    link_directories(${LIBRARY} /usr/lib)
    target_link_libraries(${LIBRARY} libssl.dll.a libcrypto.dll.a libcurl.dll.a libarchive.dll.a libiconv.dll.a
            libPocoPrometheus.dll.a libPocoJSON.dll.a libPocoXML.dll.a libPocoNet.dll.a libPocoZip.dll.a
            libPocoUtil.dll.a libPocoFoundation.dll.a)
endif ()

install(TARGETS ${LIBRARY} DESTINATION lib)
