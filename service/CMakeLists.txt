
set(LIBRARY awsmocksrv)
set(LIBRARY_STATIC awsmocksrv_static)

set(S3_SOURCES src/S3Server.cpp src/S3Handler.cpp src/S3Service.cpp)
set(SQS_SOURCES src/SQSServer.cpp src/SQSHandler.cpp src/SQSService.cpp)

set(SOURCES src/AbstractHandler.cpp ${S3_SOURCES} ${SQS_SOURCES} src/SNSService.cpp
        src/LambdaService.cpp src/DockerService.cpp)

# Test directory
add_subdirectory(tst)

# Includes
include_directories(include ../core/include ../db/include ../dto/include ../dto/include /usr/include
        /usr/include/bsoncxx/v_noabi /usr/include/mongocxx/v_noabi)

add_library(${LIBRARY} SHARED ${SOURCES})
add_library(${LIBRARY_STATIC} STATIC ${SOURCES})
set_target_properties(${LIBRARY_STATIC} PROPERTIES OUTPUT_NAME ${LIBRARY})

install(TARGETS ${LIBRARY} ${LIBRARY_STATIC} DESTINATION lib)
