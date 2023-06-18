
set(LIBRARY awsmocksrv)
set(LIBRARY_STATIC awsmocksrv_static)

set(S3_SOURCES src/S3Server.cpp src/S3Handler.cpp src/S3Service.cpp)
set(SQS_SOURCES src/SQSServer.cpp src/SQSHandler.cpp src/SQSService.cpp)
set(SNS_SOURCES src/SNSServer.cpp src/SNSHandler.cpp src/SNSService.cpp)
set(LAMBDA_SOURCES src/LambdaServer.cpp src/LambdaHandler.cpp src/LambdaService.cpp)
set(DOCKER_SOURCES src/DockerService.cpp)

set(SOURCES src/AbstractHandler.cpp ${S3_SOURCES} ${SQS_SOURCES} ${SNS_SOURCES} ${LAMBDA_SOURCES} ${DOCKER_SOURCES})

# Test directory
add_subdirectory(tst)

# Includes
include_directories(include ../core/include ../db/include ../dto/include ../dto/include /usr/include
        /usr/include/bsoncxx/v_noabi /usr/include/mongocxx/v_noabi)

add_library(${LIBRARY} SHARED ${SOURCES})
add_library(${LIBRARY_STATIC} STATIC ${SOURCES})
set_target_properties(${LIBRARY_STATIC} PROPERTIES OUTPUT_NAME ${LIBRARY})

install(TARGETS ${LIBRARY} ${LIBRARY_STATIC} DESTINATION lib)
