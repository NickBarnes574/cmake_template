# FILENAME: buildsystem.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Serves as the central configuration file for the CMake build
# process, and defines different build configurations.
# -----------------------------------------------------------------------------

# Build destination
set(DEST_BUILD "${CMAKE_SOURCE_DIR}/build")

# Local destinations
set(DEST_RELEASE_LOCAL "/release/local")
set(DEST_DEBUG_LOCAL "/debug/local")

# Remote destinations
set(DEST_RELEASE_REMOTE "/release/remote")
set(DEST_DEBUG_REMOTE "/debug/remote")

include_cmake_modules("checks")     # Clang-Tidy checks
include_cmake_modules("options")    # Debug and Release options
include_cmake_modules("libraries")  # Custom user-made libraries
include_cmake_modules("target")     # Target management
include_cmake_modules("install")    # Installation configuration
include_cmake_modules("testing")    # GTest configuration

# Create target 1
configure_target(
#  |Parameter|----------|Value|
    TARGET_NAME         "maze"       # Name of the target
    ENDPOINT            "LOCAL"        # Determines whether the target is remote or local
    TARGET_TYPE         "EXE"           # Can be an executable or an SO
    SOURCE_DIR          "projects/maze"     # Top-level directory for the project source files
    DESTINATION_DIR     "projects"      # Top-level destination project directory
    LIBRARIES Common Math DSA Threading Networking
)

# *** end of file ***
