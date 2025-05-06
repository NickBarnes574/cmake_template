include(${CMAKE_SOURCE_DIR}/cmake/add-localized-lib.cmake)

add_localized_lib(
    TARGET      IO
    TYPE        SHARED
    SOURCES
        src/file_io.c
        src/threadsafe_io.c
    INCLUDES
        include
)

# Link any dependencies if needed (e.g., Threads, Math)
target_link_libraries(IO PUBLIC Core)
