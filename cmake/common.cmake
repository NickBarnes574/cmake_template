# FILENAME: utilities.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Provides utility functions used during the CMake build process.
# -----------------------------------------------------------------------------

set(PROJECTS_BASE_DIR "${CMAKE_SOURCE_DIR}/projects")
set(BUILD_BASE_DIR "${CMAKE_BINARY_DIR}")

function(include_cmake_modules subdir)
    file(GLOB_RECURSE MODULE_FILES "${CMAKE_SOURCE_DIR}/cmake/${subdir}/*.cmake")
    foreach(MODULE_FILE IN LISTS MODULE_FILES)
        include(${MODULE_FILE})
    endforeach()
endfunction()

# *** end of file ***