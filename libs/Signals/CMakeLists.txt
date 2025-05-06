include(${CMAKE_SOURCE_DIR}/cmake/add-localized-lib.cmake)

add_localized_lib(
    TARGET      Signals
    TYPE        SHARED
    SOURCES
        src/signal_handler_single_thread.c
        src/signal_handler_multi_thread.c
    INCLUDES
        include
)

# Link any dependencies if needed (e.g., Threads, Math)
target_link_libraries(Signals PUBLIC Core)

add_cunit_test(
    TARGET      signal_handler_tests
    SCOPE       internal
    SOURCES
        tests/signal_handler_single_thread_tests.c
        tests/signal_handler_multi_thread_tests.c
        tests/test_runner.c
    DEPENDENCIES
        Core Signals
    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/signal_handler/include
)

target_compile_definitions(signal_handler_tests PRIVATE _XOPEN_SOURCE=700)