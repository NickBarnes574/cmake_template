include(${CMAKE_SOURCE_DIR}/cmake/add-localized-lib.cmake)

add_localized_lib(
    TARGET      Core
    TYPE        SHARED
    SOURCES
        src/default_free.c
        src/comparisons.c
    INCLUDES
        include
)
