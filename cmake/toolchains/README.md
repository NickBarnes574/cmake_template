# Toolchain Configuration

## Overview
The `toolchain` directory contains CMake toolchain files. These files are used to specify the settings and compilers for cross-compiling for different platforms and architectures.

## Files
- `aarch64-glibc-toolchain.cmake`: Toolchain file for cross-compiling for the AArch64 architecture with glibc.

## Usage
Specify the appropriate toolchain file when configuring your project with CMake for cross-compilation. For example, use the following command to configure a build for AArch64:

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=path/to/aarch64-glibc-toolchain.cmake ..
