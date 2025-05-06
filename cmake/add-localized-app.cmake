# cmake/add-localized-app.cmake

include_guard(GLOBAL)
include(${CMAKE_SOURCE_DIR}/cmake/paths-config.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/project-config.cmake) # for configure_executable()

function(add_localized_app)
    cmake_parse_arguments(AL_APP "" "PROJECT;TARGET;SCOPE" "SOURCES;INCLUDES" ${ARGN})

    if(NOT AL_APP_PROJECT)
        message(FATAL_ERROR "add_localized_app: PROJECT is required")
    endif()

    if(NOT AL_APP_TARGET)
        message(FATAL_ERROR "add_localized_app: TARGET is required")
    endif()

    if(NOT AL_APP_SOURCES)
        message(FATAL_ERROR "add_localized_app: SOURCES must not be empty for target ${AL_APP_TARGET}")
    endif()

    set_project_output_paths(${AL_APP_PROJECT})

    add_executable(${AL_APP_TARGET} ${AL_APP_SOURCES})
    target_compile_features(${AL_APP_TARGET} PRIVATE c_std_17)

    if(AL_APP_INCLUDES)
        target_include_directories(${AL_APP_TARGET} PUBLIC ${AL_APP_INCLUDES})
    endif()

    configure_executable(${AL_APP_TARGET} ${AL_APP_SCOPE})

    # Optional RPATH logic (can be parameterized later)
    set_target_properties(${AL_APP_TARGET} PROPERTIES
        BUILD_RPATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/../libs"
        INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/../libs"
        INSTALL_RPATH_USE_LINK_PATH TRUE
    )
endfunction()
