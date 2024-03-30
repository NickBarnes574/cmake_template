# CMake Configuration

## Overview
This directory contains the central CMake scripts and configurations for the project. It includes various subdirectories categorizing different aspects of the build process, as well as the main `buildsystem.cmake` file.

## Subdirectories
- `target/`: Scripts for target creation and configuration.
- `testing/`: Scripts for setting up and managing tests.
- `checks/`: Scripts for various build checks.
- `options/`: Scripts for setting build options and flags.
- `install/`: Scripts related to the installation of build targets.
- `toolchain/`: Toolchain files for cross-compiling.

## Main File
- `buildsystem.cmake`: The central configuration file for the CMake build process. This file includes scripts from the subdirectories and sets up the overall build system.

## Usage
Include `buildsystem.cmake` in your main `CMakeLists.txt` file to utilize the configurations and scripts organized in this directory. The subdirectories contain specialized scripts that are included and used by `buildsystem.cmake`.
