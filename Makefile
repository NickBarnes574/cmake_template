# BRIEF -- Makefile:
# ----------------------------------------------------------------------------
# This Makefile provides commands to build and deploy the project in various
# configurations.
#
# Targets:
#   all:		Builds the project for all architectures.
#   default:    Builds the project in the release configuration for x86_64.
#   release:    Builds and installs the project in release mode for x86_64.
#   debug:      Builds and installs the project in debug mode for x86_64.
#   aarch64:    Builds and installs the project for the aarch64 architecture.
#   clean:      Cleans up the build directory.
#
# Usage:
#   make [target] - Build the project using the specified target.
#                   If no target is specified, 'default' is used.
# ----------------------------------------------------------------------------

MAKEFLAGS += --no-print-directory # Suppress 'Entering directory' messages.

# Project configuration
BUILD_DIR = build/artifacts

# Toolchains
AARCH64_TOOLCHAIN = cmake/toolchains/aarch64-glibc-toolchain.cmake

default: release

all: 
	@$(MAKE) release
	@$(MAKE) debug
	@$(MAKE) aarch64

# x86_64
release: BUILD_TYPE = Release
release: ARCH = x86_64
release: build

debug: BUILD_TYPE = Debug
debug: ARCH = x86_64
debug: build

# aarch64
aarch64: BUILD_TYPE = Release
aarch64: ARCH = aarch64
aarch64: TOOLCHAIN = -DCMAKE_TOOLCHAIN_FILE=$(AARCH64_TOOLCHAIN)
aarch64: build

build:
	@cmake -S . -B $(BUILD_DIR)/$(ARCH) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(TOOLCHAIN)
	@cmake --build $(BUILD_DIR)/$(ARCH) --target install
	@rm -rf $(BUILD_DIR) # Remove artifacts temporarily

clean:
	@rm -rf build tests

.PHONY: release debug clean aarch64 all build

# *** end of file ***