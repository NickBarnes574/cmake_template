include(${CMAKE_SOURCE_DIR}/cmake/add-localized-lib.cmake)

add_localized_lib(
    TARGET      Networking
    TYPE        SHARED
    SOURCES
        common/src/socket_io.c
        tcp_server/src/event_handler.c
        tcp_server/src/server_utils.c
        tcp_server/src/socket_manager.c
        tcp_server/src/tcp_server.c
    INCLUDES
        common/include
        tcp_server/include
)

# Link any dependencies if needed (e.g., Threads, Math)
target_link_libraries(Networking PUBLIC Core IO Threading Logging System)