
set(LIBRARY awsmockcore)
set(LIBRARY_STATIC awsmockcore_static)

set(UTILS_SOURCES src/utils/StringUtils.cpp src/utils/FileUtils.cpp src/utils/DirUtils.cpp src/utils/DateTimeUtils.cpp
        src/utils/CryptoUtils.cpp src/utils/DirectoryWatcher.cpp src/utils/AwsUtils.cpp src/utils/SystemUtils.cpp
        src/logging/LibriJsonFormatter.cpp src/utils/TarUtils.cpp src/utils/RandomUtils.cpp)

set(EXCEPTION_SOURCES src/exception/CoreException.cpp src/exception/DatabaseException.cpp
        src/exception/ServiceException.cpp src/exception/ResourceNotFoundException.cpp)

set(SOURCES src/config/Configuration.cpp src/logging/Logger.cpp src/monitoring/MetricService.cpp
        src/monitoring/MetricSystemCollector.cpp ${UTILS_SOURCES} ${EXCEPTION_SOURCES})

# Test directory
add_subdirectory(tst)

# Includes
include_directories(include ../core/include /usr/include /usr/local/include)

add_library(${LIBRARY} SHARED ${SOURCES})
add_library(${LIBRARY_STATIC} STATIC ${SOURCES})
set_target_properties(${LIBRARY_STATIC} PROPERTIES OUTPUT_NAME ${LIBRARY})

install(TARGETS ${LIBRARY} DESTINATION lib)
