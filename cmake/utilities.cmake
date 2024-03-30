# FILENAME: utilities.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Provides utility functions used during the CMake build process.
# -----------------------------------------------------------------------------

# Projects directory
set(PROJECTS_DIR ${CMAKE_SOURCE_DIR}/projects)

# Build directory
set(DEST_BUILD "${CMAKE_SOURCE_DIR}/build")

# Debug destinations
set(DEBUG_LOCAL "/debug/local")
set(DEBUG_REMOTE "/debug/remote")

# Release destinations
set(RELEASE_LOCAL "/release/local")
set(RELEASE_REMOTE "/release/remote")

function(include_cmake_modules subdir)
    file(GLOB MODULE_FILES "${CMAKE_SOURCE_DIR}/cmake/${subdir}/*.cmake")
    foreach(MODULE_FILE IN LISTS MODULE_FILES)
        include(${MODULE_FILE})
    endforeach()
endfunction()

# *** end of file ***