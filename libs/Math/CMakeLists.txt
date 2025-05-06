include(${CMAKE_SOURCE_DIR}/cmake/add-localized-lib.cmake)

add_localized_lib(
    TARGET      Math
    TYPE        SHARED
    SOURCES
        src/number_converter.c
        src/number_generator.c
    INCLUDES
        include
)

# Link any dependencies if needed (e.g., Threads, Math)
target_link_libraries(Math PUBLIC Core)
