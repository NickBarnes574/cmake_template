# cmake/add-localized-lib.cmake

include_guard(GLOBAL)
include(${CMAKE_SOURCE_DIR}/cmake/paths-config.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/project-config.cmake)

function(add_localized_lib)
    cmake_parse_arguments(AL_LIB "" "TARGET;TYPE" "SOURCES;INCLUDES" ${ARGN})

    if(NOT AL_LIB_TARGET)
        message(FATAL_ERROR "add_localized_lib: TARGET is required")
    endif()

    if(NOT AL_LIB_SOURCES)
        message(FATAL_ERROR "add_localized_lib: SOURCES must not be empty for target ${AL_LIB_TARGET}")
    endif()

    if(NOT AL_LIB_TYPE)
        set(AL_LIB_TYPE "STATIC")
    endif()

    string(TOUPPER "${AL_LIB_TYPE}" AL_LIB_TYPE_UPPER)
    if(NOT AL_LIB_TYPE_UPPER STREQUAL "STATIC" AND NOT AL_LIB_TYPE_UPPER STREQUAL "SHARED")
        message(FATAL_ERROR "add_localized_lib: TYPE must be STATIC or SHARED (got '${AL_LIB_TYPE}')")
    endif()

    # Note: set_project_output_paths() should already be called by the parent scope once

    add_library(${AL_LIB_TARGET} ${AL_LIB_TYPE_UPPER} ${AL_LIB_SOURCES})
    target_compile_features(${AL_LIB_TARGET} PRIVATE c_std_17)
    target_link_libraries(${AL_LIB_TARGET} PRIVATE Warnings)

    if(AL_LIB_INCLUDES)
        target_include_directories(${AL_LIB_TARGET} PUBLIC ${AL_LIB_INCLUDES})
    endif()

    configure_library(${AL_LIB_TARGET})
endfunction()
