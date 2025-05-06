include(${CMAKE_SOURCE_DIR}/cmake/add-localized-lib.cmake)

add_localized_lib(
    TARGET      Threading
    TYPE        SHARED
    SOURCES
        src/thread_pool.c
    INCLUDES
        include
)

# Link any dependencies if needed (e.g., Threads, Math)
target_link_libraries(Threading PUBLIC Core DSA pthread)