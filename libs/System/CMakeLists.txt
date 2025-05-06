include(${CMAKE_SOURCE_DIR}/cmake/add-localized-lib.cmake)

add_localized_lib(
    TARGET      System
    TYPE        SHARED
    SOURCES
        src/system_info.c
    INCLUDES
        include
)

# Link any dependencies if needed (e.g., Threads, Math)
target_link_libraries(System PUBLIC Core IO)