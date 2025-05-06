include(${CMAKE_SOURCE_DIR}/cmake/add-localized-lib.cmake)

add_localized_lib(
    TARGET      Logging
    TYPE        SHARED
    SOURCES
        src/runtime_log.c
    INCLUDES
        include
)

# Link any dependencies if needed (e.g., Threads, Math)
target_link_libraries(Logging PUBLIC Core IO System)