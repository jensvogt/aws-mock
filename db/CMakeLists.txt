
set(LIBRARY awsmockdb)
set(LIBRARY_STATIC awsmockdb_static)

set(SOURCES src/Database.cpp src/ServiceDatabase.cpp src/S3Database.cpp src/SQSDatabase.cpp src/SNSDatabase.cpp
        src/LambdaDatabase.cpp)

# Test directory
add_subdirectory(tst)

# Includes
include_directories(include ../core/include /usr/include /usr/include/mongo /usr/include/bsoncxx/v_noabi
        /usr/include/mongocxx/v_noabi)

add_library(${LIBRARY} SHARED ${SOURCES})
add_library(${LIBRARY_STATIC} STATIC ${SOURCES})
set_target_properties(${LIBRARY_STATIC} PROPERTIES OUTPUT_NAME ${LIBRARY})

install(TARGETS ${LIBRARY} DESTINATION lib)
