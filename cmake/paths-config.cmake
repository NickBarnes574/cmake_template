# ----------------------------------------------------------------------------
# BRIEF -- paths-config.cmake
# ----------------------------------------------------------------------------
# Defines logical output paths for applications, libraries, and test binaries.
#
# Features:
# - Supports remote/local scope, per-architecture output
# - Shared deploy-level shared libs (e.g. .so) for multi-project reuse
# - Internal directory for non-deployed artifacts
#
# Usage:
# - Call `set_project_output_paths(<project_name>)` in each CMakeLists.txt
# ----------------------------------------------------------------------------

function(set_project_output_paths project_name)
    if(NOT DEFINED ARCH)
        set(ARCH "x86_64")
    endif()

    # ----------------------------------------------------------
    # CPU architecture path
    # ----------------------------------------------------------

    # Base deploy directory for current arch
    set(DEPLOY_ARCH_BASE "${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/deploy/${ARCH}")

    # ----------------------------------------------------------
    # Library paths
    # ----------------------------------------------------------

    # Shared deploy-level libraries (used by multiple projects)
    set(DEPLOY_SHARED_LIB_DIR "${DEPLOY_ARCH_BASE}/libs/shared" CACHE INTERNAL "")
    set(DEPLOY_STATIC_LIB_DIR "${DEPLOY_ARCH_BASE}/libs/static" CACHE INTERNAL "")

    # Set output paths for CMake (static = .a, shared = .so)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DEPLOY_STATIC_LIB_DIR} CACHE INTERNAL "")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DEPLOY_SHARED_LIB_DIR} CACHE INTERNAL "")

    # ----------------------------------------------------------
    # Project paths
    # ----------------------------------------------------------

    # Project-specific deploy root (e.g., deploy/x86_64/project_1)
    set(PROJECT_DEPLOY_BASE "${DEPLOY_ARCH_BASE}/${project_name}")

    # Output directories for apps and tests (remote + local scopes)
    set(REMOTE_BIN_DIR         "${PROJECT_DEPLOY_BASE}/remote/bin"       CACHE INTERNAL "")
    set(REMOTE_APPS_OUTPUT_DIR "${REMOTE_BIN_DIR}/apps"                  CACHE INTERNAL "")
    set(REMOTE_TESTS_OUTPUT_DIR "${REMOTE_BIN_DIR}/tests"                CACHE INTERNAL "")

    set(LOCAL_BIN_DIR          "${PROJECT_DEPLOY_BASE}/local/bin"        CACHE INTERNAL "")
    set(LOCAL_APPS_OUTPUT_DIR  "${LOCAL_BIN_DIR}/apps"                   CACHE INTERNAL "")
    set(LOCAL_TESTS_OUTPUT_DIR "${LOCAL_BIN_DIR}/tests"                  CACHE INTERNAL "")

    # ----------------------------------------------------------
    # Internal output for support tools, unit tests, or analysis
    # ----------------------------------------------------------
    set(INTERNAL_OUTPUT_BASE "${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/detach/${ARCH}")
    set(INTERNAL_APP_DIR     "${INTERNAL_OUTPUT_BASE}/apps"   CACHE INTERNAL "")
    set(INTERNAL_LIB_DIR     "${INTERNAL_OUTPUT_BASE}/libs"   CACHE INTERNAL "")
    set(INTERNAL_DOC_DIR     "${INTERNAL_OUTPUT_BASE}/docs"   CACHE INTERNAL "")
    set(INTERNAL_TEST_DIR    "${INTERNAL_OUTPUT_BASE}/tests"  CACHE INTERNAL "")
endfunction()

# # *** END OF FILE ***