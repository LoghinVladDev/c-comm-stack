cmake_minimum_required(VERSION 3.10)
project(c_com_stack C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(dependencies/c-thread)
include_directories(dependencies/c-thread/include)
include_directories(
        src/connection
        src/util
        src/com
)

set(
        C_COM_STACK_SRC
        src/connection/connection.c
        src/connection/connection.h
        src/util/com_stack_types.h
        src/connection/connection-private.h

        src/com/com.c
        src/com/com.h
        src/com/com-private.h
)

if ( NOT TARGET c-com-stack )
    add_executable(c-com-stack src/tests/comStackTest.c ${C_COM_STACK_SRC})
    target_link_libraries(c-com-stack c-thread-lib)
endif()

if ( NOT TARGET comStackClientTest )
    add_executable(comStackClientTest src/tests/comStackTestClient.c ${C_COM_STACK_SRC})
    target_link_libraries(comStackClientTest c-thread-lib)
endif()

if ( NOT TARGET comStackServerTest )
    add_executable(comStackServerTest src/tests/comStackTestServer.c ${C_COM_STACK_SRC})
    target_link_libraries(comStackServerTest c-thread-lib)
endif()

if ( NOT TARGET c-com-stack-lib )
    add_library(c-com-stack-lib STATIC ${C_COM_STACK_SRC})
    target_link_libraries(c-com-stack-lib c-thread-lib)
endif()