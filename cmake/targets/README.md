# CMake Target Configuration

This directory contains the CMake scripts responsible for configuring and adding build targets to the project. It is organized hierarchically to ensure clarity and ease of navigation, with each subdirectory and file serving a specific role in the target configuration process.

## Directory Structure

- `add_all_targets.cmake`: The entry point script for adding all defined build targets to the project. It orchestrates the inclusion of individual target configuration scripts.
- `core/`: This directory contains core scripts that are essential for defining and configuring any type of build target.
  - `add_target.cmake`: A foundational script that provides the primary function for adding a new build target. It handles the basic configuration common to all target types.
- `core/types/`: Contains scripts for adding specific types of targets, such as executables and shared objects (libraries).
  - `add_exe.cmake`: Defines the process for configuring and adding an executable target to the build system.
  - `add_so.cmake`: Defines the process for configuring and adding a shared object (library) target to the build system.
- `core/types/utilities/`: Houses utility scripts that support target configuration, such as stripping unnecessary information from binaries.
  - `strip_target.cmake`: A utility script for stripping debugging information from the target, typically used in release builds to reduce the size of the generated binaries.

## Usage

To add a new build target to the project, from inside the `add_all_targets()` function, make a call to `add_target()`. Specify the target type (`EXE` or `SO`), along with any necessary parameters such as the target name, source directories, and libraries to link against. For detailed usage instructions, refer to the comments within `add_target.cmake`.

`Adding a local target executable`
```cmake
    add_target(
    #   [ FIELD ]-----------[ VALUE ]
        TARGET_NAME         test_client
        ENDPOINT            LOCAL
        TARGET_TYPE         EXE
        SOURCE_DIR          projects/test
        DESTINATION_DIR     projects/test
        LIBRARIES           # No custom libraries added
    )
```
`Adding a remote target executable`
```cmake
    add_target(
    #   [ FIELD ]-----------[ VALUE ]
        TARGET_NAME         test_server
        ENDPOINT            REMOTE
        TARGET_TYPE         EXE
        SOURCE_DIR          projects/test
        DESTINATION_DIR     projects/test
        LIBRARIES           # No custom libraries added
    )
```
`Adding a local shared object library`
```cmake
    add_target(
    #   [ FIELD ]-----------[ VALUE ]
        TARGET_NAME         test_library
        ENDPOINT            REMOTE
        TARGET_TYPE         SO
        SOURCE_DIR          projects/test
        DESTINATION_DIR     projects/test
        LIBRARIES           # No custom libraries added
    )
```