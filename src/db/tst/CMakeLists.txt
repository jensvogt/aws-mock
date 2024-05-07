set(BINARY db_tst)
set(STATIC_LIB awsmockdb)

set(SQS_SOURCES SQSDatabaseTests.cpp SQSMemoryDbTests.cpp)
set(SNS_SOURCES SNSDatabaseTests.cpp SNSMemoryDbTests.cpp)
set(S3_SOURCES S3DatabaseTests.cpp S3MemoryDbTests.cpp)
set(MODULE_SOURCES ModuleDatabaseTests.cpp ModuleMemoryDbTests.cpp)
set(LAMBDA_SOURCES LambdaDatabaseTests.cpp LambdaMemoryDbTests.cpp)
set(COGNITO_SOURCES CognitoDatabaseTests.cpp CognitoMemoryDbTests.cpp)
set(SECRETSMANAGER_SOURCES SecretsManagerDatabaseTests.cpp)
set(TRANSFER_SOURCES TransferDatabaseTest.cpp TransferMemoryDbTest.cpp)

# Includes
if (WIN32)
    include_directories(../include ../../core/include ${AWSMOCK_WIN32_DIR}/poco/include ${AWSMOCK_WIN32_DIR}/gnuwin32/include)
    include_directories(${AWSMOCK_WIN32_DIR}/plog/include ${AWSMOCK_WIN32_DIR}/openssl/include)
    include_directories(${AWSMOCK_WIN32_DIR}/curl/include ${AWSMOCK_WIN32_DIR}/boost)
    include_directories(${AWSMOCK_WIN32_DIR}/asio/include ${AWSMOCK_WIN32_DIR}/gtest/include)
    include_directories(${AWSMOCK_WIN32_DIR}/mongo-cxx-driver/include/bsoncxx/v_noabi)
    include_directories(${AWSMOCK_WIN32_DIR}/mongo-cxx-driver/include/mongocxx/v_noabi)
    link_directories(../ ../../core ${AWSMOCK_WIN32_DIR}/poco/lib)
    link_directories(${AWSMOCK_WIN32_DIR}/mongo-cxx-driver/lib)
else ()
    include_directories(../include ../../core/include /usr/include /usr/include/mongo /usr/include/bsoncxx/v_noabi /usr/include/mongocxx/v_noabi)
    link_directories(/usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib ../ ../../core)
endif ()


add_executable(${BINARY} DatabaseTests.cpp ${SQS_SOURCES} ${S3_SOURCES} ${SNS_SOURCES} ${MODULE_SOURCES}
        ${LAMBDA_SOURCES} ${TRANSFER_SOURCES} ${COGNITO_SOURCES} ${SECRETSMANAGER_SOURCES} main.cpp)

if (WIN32)
    target_link_libraries(${BINARY} PUBLIC ${STATIC_LIB} awsmockcore mongocxx bsoncxx PocoUtil PocoFoundation PocoNet PocoJSON
            PocoPrometheus PocoZip gtest ssl crypto z archive curl)
else ()
    target_link_libraries(${BINARY} PUBLIC ${STATIC_LIB} awsmockcore mongocxx bsoncxx PocoUtil PocoFoundation PocoNet PocoJSON
            PocoPrometheus PocoZip gtest ${CMAKE_DL_LIBS} pthread ssl crypto z archive curl tbb)
endif ()

add_test(NAME ${BINARY} COMMAND ${BINARY})