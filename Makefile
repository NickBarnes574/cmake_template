# Suppress 'Entering directory' messages.
MAKEFLAGS += --no-print-directory

# Default build type
BUILD_TYPE ?= Release

# Default architecture (user can override: `make ARCH=aarch64`)
ARCH ?= x86_64

default: release

all: 
	@$(MAKE) release
	@$(MAKE) debug
	@$(MAKE) aarch64

# Build for x86-64 based systems
release: BUILD_TYPE = Release
release: build

debug: BUILD_TYPE = Debug
debug: build

# Build for ARM-64 based systems
aarch64: BUILD_TYPE = Release
aarch64: ARCH = aarch64
aarch64: build

build:
	@cmake -S . -B build/$(ARCH) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DARCH=$(ARCH)
	@cmake --build build/$(ARCH)

install:
	@cmake --install build/$(ARCH) --prefix install_dir

clean:
	@rm -rf build tests Release Debug

help:
	@echo "Available targets:"
	@echo "  make default    - Builds the project in release mode (x86-64)."
	@echo "  make release    - Builds and installs the project in release mode (x86-64)."
	@echo "  make debug      - Builds and installs the project in debug mode (x86-64)."
	@echo "  make aarch64    - Builds and installs the project for ARM-64 architecture."
	@echo "  make all        - Builds the project for all configurations."
	@echo "  make clean      - Cleans up the build directory."
	@echo "  make help       - Displays this help message."

.PHONY: release debug clean aarch64 all build

