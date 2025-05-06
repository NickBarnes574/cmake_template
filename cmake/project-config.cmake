# ----------------------------------------------------------------------------
# BRIEF -- project-config.cmake
# ----------------------------------------------------------------------------
# Provides utility functions for project configuration and setup.
#
# Features:
# - Uses centralized output paths from paths-config.cmake
# - Provides `set_project_includes()` and flexible `configure_executable()` function
#
# Usage:
# - Set EXECUTABLE_NAME to the name of the binary
# - Optionally set TARGET_INSTALL_SCOPE to either "remote" (default) or "local"
# ----------------------------------------------------------------------------

include(${CMAKE_CURRENT_LIST_DIR}/paths-config.cmake)

# ----------------------------------------------------------------------------
# FUNCTION - set_project_includes()
#
# Description:
# Adds standard include path to the specified target for generated config headers.
#
# Usage:
# set_project_includes(<target>)
# ----------------------------------------------------------------------------
function(set_project_includes target)
    target_include_directories(${target} PRIVATE "${CMAKE_BINARY_DIR}/cmake/config")
endfunction()

# ----------------------------------------------------------------------------
# FUNCTION - configure_executable()
#
# Description:
# Sets up output and install behavior for the app executable defined by
# EXECUTABLE_NAME and optionally TARGET_INSTALL_SCOPE.
#
# Usage:
# - Defaults TARGET_INSTALL_SCOPE to "remote" if not provided
# - Called from add_project_app()
# ----------------------------------------------------------------------------
function(configure_executable target_name scope)
    if(COMPILE_EXECUTABLE)
        if(NOT target_name)
            message(FATAL_ERROR "target_name is not defined but COMPILE_EXECUTABLE is ON.")
        endif()

        if(${scope} STREQUAL "remote")
            set(INSTALL_PATH "${REMOTE_APPS_OUTPUT_DIR}")
        elseif(${scope} STREQUAL "local")
            set(INSTALL_PATH "${LOCAL_APPS_OUTPUT_DIR}")
        elseif(${scope} STREQUAL "internal")
            set(INSTALL_PATH "${INTERNAL_APP_DIR}")
        else()
            message(FATAL_ERROR "Invalid scope '${scope}' — expected 'local', 'remote', or 'internal'")
        endif()

        set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${INSTALL_PATH}
        )
        install(TARGETS ${target_name}
                DESTINATION ${INSTALL_PATH})
    endif()
endfunction()

# ----------------------------------------------------------------------------
# FUNCTION - configure_library()
#
# Description:
# Sets up output and install behavior for the library
#
# Usage:
# - Called from add_project_app()
# ----------------------------------------------------------------------------
function(configure_library target_name)
    if(NOT TARGET ${target_name})
        message(FATAL_ERROR "Target '${target_name}' not found in configure_library()")
    endif()

    get_target_property(target_type ${target_name} TYPE)

    if(target_type STREQUAL "STATIC_LIBRARY")
        set(OUTDIR "${DEPLOY_STATIC_LIB_DIR}")
    elseif(target_type STREQUAL "SHARED_LIBRARY")
        set(OUTDIR "${DEPLOY_SHARED_LIB_DIR}")
    else()
        # Fallback to internal for utility or unsupported types
        set(OUTDIR "${INTERNAL_LIB_DIR}")
    endif()

    set_target_properties(${target_name} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${OUTDIR}
        LIBRARY_OUTPUT_DIRECTORY ${OUTDIR}
    )
endfunction()



# ----------------------------------------------------------------------------
# FUNCTION - configure_test_executable()
#
# Description:
# Sets runtime and install directories for test executables based on
# TEST_INSTALL_SCOPE.
#
# Usage:
# - Defaults to remote if TEST_INSTALL_SCOPE is not set
# - Supports optional installation if INSTALL_TEST_EXECUTABLE is set
#
# Parameters:
# target     The name of the test target to configure
# ----------------------------------------------------------------------------
function(configure_test_executable target_name scope)
    if(NOT TARGET ${target_name})
        message(FATAL_ERROR "Target '${target_name}' not found in configure_library()")
    endif()

    if(${scope} STREQUAL "remote")
        set(OUTDIR "${REMOTE_TESTS_OUTPUT_DIR}")
    elseif(${scope} STREQUAL "local")
        set(OUTDIR "${LOCAL_TESTS_OUTPUT_DIR}")
    elseif(${scope} STREQUAL "internal")
        set(OUTDIR "${INTERNAL_TEST_DIR}")
    else()
        message(FATAL_ERROR "Invalid scope '${scope}' — expected 'local', 'remote', or 'internal'")
    endif()

    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${OUTDIR}
    )

    if(DEFINED INSTALL_TEST_EXECUTABLE AND INSTALL_TEST_EXECUTABLE)
        install(TARGETS ${target_name} DESTINATION ${OUTDIR})
    endif()
endfunction()

# ----------------------------------------------------------------------------
# FUNCTION - add_project_app()
# 
# Description:
# Adds a project app target with proper install path and includes.
#
# Usage:
# add_project_app(
#     TARGET <name>
#     SOURCES <src1> <src2> ...
#     INCLUDES <include1> <include2> ...
#     INSTALL_SCOPE <remote|local> [DEFAULT: remote]
#     LIBRARIES <lib1> <lib2> ...
# )
# ----------------------------------------------------------------------------
function(add_project_app)
    set(options)
    set(single_args TARGET INSTALL_SCOPE)
    set(multi_args SOURCES INCLUDES LIBRARIES)

    cmake_parse_arguments(APP "${options}" "${single_args}" "${multi_args}" ${ARGN})

    if(NOT APP_TARGET)
        message(FATAL_ERROR "add_project_app(): TARGET is required")
    endif()

    if(NOT APP_SOURCES)
        message(FATAL_ERROR "add_project_app(): SOURCES are required")
    endif()

    set(EXECUTABLE_NAME ${APP_TARGET})
    add_executable(${EXECUTABLE_NAME} ${APP_SOURCES} ${APP_INCLUDES})

    if(DEFINED APP_INCLUDES)
        target_include_directories(${EXECUTABLE_NAME} PUBLIC ${APP_INCLUDES})
    endif()

    if(DEFINED APP_LIBRARIES)
        target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${APP_LIBRARIES})
    endif()

    if(NOT APP_INSTALL_SCOPE)
        set(TARGET_INSTALL_SCOPE "remote")
    else()
        set(TARGET_INSTALL_SCOPE "${APP_INSTALL_SCOPE}")
    endif()

    configure_executable()
endfunction()


# *** END OF FILE ***
