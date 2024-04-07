# Cross-Compilation Guide for AARCH64 Using Make and CMake

This guide provides step-by-step instructions on how to set up your environment for cross-compiling applications for AARCH64 architecture using Make and CMake tools. It's tailored for developers who need to build their projects for different architectures from a single codebase.

## Prerequisites

Before starting, ensure you have CMake and Make installed on your development machine. These tools are essential for the build and compilation process described in this guide.

## Step 1: Download the AARCH64 Toolchain

1. Navigate to [Bootlin's toolchain repository](https://toolchains.bootlin.com) to download the latest `aarch64-glibc` toolchain.
2. Select the toolchain version that best suits your requirements. For most purposes, the latest stable version is recommended.

## Step 2: Install the Toolchain

1. Extract the downloaded toolchain archive.
2. Move the extracted folder to the `/opt` directory. This location is typical for toolchains and external libraries, ensuring system-wide accessibility.

   ```bash
   sudo mv <your-extracted-folder> /opt/aarch64--glibc--stable-2024.02-1
   ```

## Step 3: Project Directory Structure

Ensure your main project directory is organized as follows, which is a common structure for CMake projects:

```
.
├── cmake
├── CMakeLists.txt
├── docs
├── libraries
├── Makefile
└── projects
```

## Step 4: Configure the CMake Toolchain File

1. Within the `cmake` directory of your project, create a new `toolchains` directory if it doesn't already exist.
2. Inside the `toolchains` directory, create a new file named `aarch64-glibc-toolchain.cmake`.
3. Populate `aarch64-glibc-toolchain.cmake` with the following content, adjusting the `tools` variable to match the toolchain version you installed:

```cmake
# FILENAME: aarch64-glibc-toolchain.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: CMake toolchain file for cross-compiling using the aarch64
# toolchain, targeting Linux systems with the aarch64 architecture.
#
# NOTES: Download the aarch64 toolchain from https://toolchains.bootlin.com.
# Install by unzipping and moving it to the /opt directory.
# -----------------------------------------------------------------------------

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(tools /opt/aarch64--glibc--stable-2024.02-1)
set(CMAKE_C_COMPILER ${tools}/bin/aarch64-linux-gcc)
set(CMAKE_CXX_COMPILER ${tools}/bin/aarch64-linux-g++)
set(CMAKE_FIND_ROOT_PATH ${tools})
set(CMAKE_STRIP ${tools}/bin/aarch64-linux-strip)
```

## Step 5: Modify the Makefile for Cross-Compilation

Update your project's `Makefile` to include a target for the `aarch64` architecture. This addition allows you to build your project for `aarch64` using the specified toolchain file. Replace your existing `Makefile` content with the following, making any necessary adjustments to fit your project:

```makefile
# BRIEF -- Makefile:
# ----------------------------------------------------------------------------
# This Makefile provides commands to build and deploy the project for various
# configurations, including cross-compilation for the aarch64 architecture.
#
# Targets:
#   all:      Builds the project for all architectures.
#   default:  Builds the project in release configuration for x86_64.
#   release:  Builds and installs the project in release mode for x86_64.
#   debug:    Builds and installs the project in debug mode for x86_64.
#   aarch64:  Builds and installs the project for aarch64 architecture.
#   clean:    Cleans up the build directory.
#
# Usage:
#   make [target] - Builds the project using the specified target.
#                   'default' is used if no target is specified.
# ----------------------------------------------------------------------------

MAKEFLAGS += --no-print-directory # Suppress 'Entering directory' messages.

# Project configuration
BUILD_DIR = build/artifacts
AARCH64_TOOLCHAIN = cmake/toolchains/aarch64-glibc-toolchain.cmake

default: release

all: 
	@$(MAKE) release
	@$(MAKE) debug
	@$(MAKE) aarch64

# x86_64 targets
release: BUILD_TYPE = Release
release: ARCH = x86_64
release: build

debug: BUILD_TYPE = Debug
debug: ARCH = x86

_64
debug: build

# aarch64 target
aarch64: BUILD_TYPE = Release
aarch64: ARCH = aarch64
aarch64: TOOLCHAIN = -DCMAKE_TOOLCHAIN_FILE=$(AARCH64_TOOLCHAIN)
aarch64: build

build:
	@cmake -S . -B $(BUILD_DIR)/$(ARCH) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(TOOLCHAIN)
	@cmake --build $(BUILD_DIR)/$(ARCH) --target install
	@rm -rf $(BUILD_DIR) # Remove build artifacts

clean:
	@rm -rf build tests .vscode

.PHONY: release debug clean aarch64 all build
```

## Conclusion

You've now set up your environment for cross-compiling projects using CMake and Make for the AARCH64 architecture. This setup provides a flexible foundation for developing applications across different platforms from a single codebase. Remember to adjust the paths and toolchain versions according to your specific project requirements and toolchain updates.