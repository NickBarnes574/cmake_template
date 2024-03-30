# FILENAME: utilities.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Provides utility functions used during the CMake build process.
# -----------------------------------------------------------------------------

function(include_cmake_modules subdir)
    file(GLOB MODULE_FILES "${CMAKE_SOURCE_DIR}/cmake/${subdir}/*.cmake")
    foreach(MODULE_FILE IN LISTS MODULE_FILES)
        include(${MODULE_FILE})
    endforeach()
endfunction()

# *** end of file ***