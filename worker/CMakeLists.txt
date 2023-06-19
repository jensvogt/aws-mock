
set(LIBRARY awsmockwrk)
set(LIBRARY_STATIC awsmockwrk_static)

set(SOURCES src/S3Worker.cpp src/SQSWorker.cpp src/LambdaWorker.cpp)
set(HEADERS include/awsmock/worker/S3Worker.h include/awsmock/worker/SQSWorker.h include/awsmock/worker/LambdaWorker.h)

# Includes
include_directories(include ../core/include ../db/include ../dto/include ../service/include
        /usr/include/bsoncxx/v_noabi /usr/include/mongocxx/v_noabi /usr/include /usr/local/include)

add_library(${LIBRARY} SHARED ${SOURCES})
add_library(${LIBRARY_STATIC} STATIC ${SOURCES})
set_target_properties(${LIBRARY_STATIC} PROPERTIES OUTPUT_NAME ${LIBRARY})

install(TARGETS ${LIBRARY} DESTINATION lib)
