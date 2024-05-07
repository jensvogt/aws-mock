set(BINARY awsmockctl)

if (NOT WIN32)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif ()

set(SOURCES src/Configuration.cpp src/Controller.cpp src/main.cpp)

if (WIN32)
    include_directories(./include ../core/include ../db/include ../dto/include ${AWSMOCK_WIN32_DIR}/include)
    link_directories(${AWSMOCK_WIN32_DIR}/lib ../core/${CMAKE_BUILD_TYPE} ../db/${CMAKE_BUILD_TYPE})
else ()
    include_directories(include ../core/include ../dto/include ../db/include /usr/include /usr/include/mongo)
    include_directories(/usr/include/bsoncxx/v_noabi /usr/include/mongocxx/v_noabi)
    link_directories(/usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib ../core)
endif ()

add_executable(${BINARY} ${SOURCES})

if (WIN32)
    target_link_libraries(${BINARY} PUBLIC awsmockcore awsmockdto awsmockdb PocoPrometheus PocoNet
            PocoJSON PocoXML PocoZip PocoFoundation PocoUtil libcrypto.lib libssl.lib archive libcurl_imp.lib
            bsoncxx-v_noabi-rhb-x64-v143-md.lib mongocxx-v_noabi-rhb-x64-v143-md.lib)
else ()
    # Check systemd
    find_library(SYSTEMD NAMES systemd HINTS /usr/lib/x86_64-linux-gnu)
    if (SYSTEMD)
        # systemd available
        add_definitions(-DHAS_SYSTEMD)
        target_link_libraries(${BINARY} PUBLIC awsmockcore awsmockdto awsmockdb mongocxx bsoncxx PocoPrometheus PocoNet
                PocoJSON PocoXML PocoZip PocoFoundation PocoUtil pthread crypto ssl z archive curl tbb systemd)
    else ()
        # systemd not available
        target_link_libraries(${BINARY} PUBLIC awsmockcore awsmockdto awsmockdb mongocxx bsoncxx PocoPrometheus PocoNet
                PocoJSON PocoXML PocoZip PocoFoundation PocoUtil pthread crypto ssl z archive curl tbb)
    endif ()
endif ()

add_library(${BINARY}_lib STATIC ${SOURCES})

install(TARGETS ${BINARY} DESTINATION bin)
