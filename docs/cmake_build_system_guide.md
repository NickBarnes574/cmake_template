# CMake Build System

### 1. Top-Level `CMakeLists.txt`

This file is the entry point of your CMake build system. It should:

- Include the `cmake/buildsystem.cmake` file.
- Call the `add_all_targets()` and `add_all_tests()` functions.

Example structure:

```cmake
cmake_minimum_required(VERSION 3.15) # specify minimum CMake version
project(YourProjectName)

include(cmake/buildsystem.cmake)

add_all_targets()
add_all_tests()
```

### 2. Top-Level `Makefile`

This file defines high-level targets for building your project. You'll create targets such as:

- `release`
- `debug`
- `clean`

For cross-compiling, you can later add specific targets.

Example structure:

```makefile
.PHONY: release debug clean

release:
	cmake --build . --config Release

debug:
	cmake --build . --config Debug

clean:
	cmake --build . --target clean
```

### 3. `cmake/` Directory

This directory contains your modular CMake scripts. Each script should focus on a single aspect of the build process:

#### `buildsystem.cmake`

- Includes all other `.cmake` files in the directory.
  
#### `add_all_targets.cmake`

- Adds source files and includes for each target using `add_exe()` or `add_so()`.

#### `add_all_tests.cmake`

- Adds a GTest suite per target.

#### `add_exe.cmake` and `add_so.cmake`

- These files define functions to add an executable (`add_exe()`) or a shared library (`add_so()`).

#### `disable_warnings.cmake`

- Function to disable specific compiler warnings.

#### `install_target.cmake`

- Installs a target in the desired directory structure.

#### `set_default_debug_options.cmake` and `set_default_release_options.cmake`

- Set compiler or linking options for debug and release builds.

#### `strip_target.cmake`

- Strips executable in release mode.

#### `tidy-checks.cmake`

- Configuration for clang-tidy and possibly other static analysis tools.

### 4. Toolchain Files

For cross-compiling, use toolchain files for different architectures. For example, you can have separate toolchain files for Ubuntu, PowerPC, MIPS, ARM, etc. These files specify the compilers, flags, and any other settings needed for each architecture.

Refer to Bootlin's website for pre-made toolchain files for various architectures, which can significantly simplify setting up your cross-compiling environment.

### Cross-Compiling with ARM 64-bit Toolchain

#### Creating the Toolchain File
- **Toolchain File Name:** Create a file named `aarch64-glibc-toolchain.cmake`.
- **Configure Compiler and Tools:** Set paths for the cross compilers and tools specific to ARM 64-bit.
- **Toolchain File Content:** Set system name, processor, and paths to the compilers.

Example Toolchain File:
```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(tools /path/to/your/toolchain)
set(CMAKE_C_COMPILER ${tools}/bin/aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER ${tools}/bin/aarch64-linux-gnu-g++)
set(CMAKE_FIND_ROOT_PATH ${tools})
```

#### Using the Toolchain File in the Build System
- **CMake Configuration:** Use the toolchain file when configuring your project for ARM 64-bit.
- **Command Example:** 
  ```bash
  cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/aarch64-glibc-toolchain.cmake /path/to/your/source
  ```

#### Adding a Cross-Compile Label in the Makefile
- **Define a New Target:** Add a target like `arm64` in your Makefile.
- **Specify Toolchain and Build Directory:** Use the ARM toolchain file and specify a build directory.
- **Makefile Target Example:**
  ```makefile
  .PHONY: arm64
  
  arm64:
      cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/aarch64-glibc-toolchain.cmake -Bbuild/arm64 -H.
      cmake --build build/arm64
  ```

#### Usage
- **Compile for ARM 64-bit:** Run `make arm64` to start the ARM build.
- **Simple and Consistent:** This aligns with other build targets in your Makefile.

This command makes the build process straightforward and consistent with other build targets in your Makefile. 

Remember to adjust the paths and options according to your project's structure and requirements.

### 5. Integrating Unit Tests

- Utilize `add_all_tests.cmake` to integrate GTest for unit testing.
- Define how each test suite is built and linked with your project's source files.

### 6. Considerations for Different Target Formats

- Configure your CMake to build both executables (ELF) and shared libraries (.so).
- This can be done within the `add_exe.cmake` and `add_so.cmake` scripts.

### 7. Continuous Integration and Testing

Consider setting up a continuous integration (CI) system that can build and test your code on various architectures and platforms. This ensures that your codebase remains stable and compatible across all intended targets.

By following this breakdown, you'll have a structured approach to setting up a CMake build system that is scalable, flexible, and suitable for a project that targets multiple platforms and architectures.

To use the `/opt` directory for storing your toolchain in a Linux environment, follow these steps:

1. **Download the Toolchain**:
   - First, you need to download the specific version of the toolchain you require. Since you mentioned using a toolchain from bootlin.com, visit their website, find the appropriate toolchain (like `aarch64--glibc--stable-2023.11-1`), and download it.

2. **Extract the Toolchain**:
   - Once downloaded, you can extract the toolchain to the `/opt` directory. This process typically involves uncompressing the toolchain archive. For example, if your downloaded toolchain is a tar.gz file, you can use the following command in the terminal:
     ```bash
     sudo tar -xzvf downloaded_toolchain.tar.gz -C /opt/
     ```
   - This command will extract the toolchain to the `/opt` directory. You might need to replace `downloaded_toolchain.tar.gz` with the actual filename of your downloaded toolchain.

3. **Set Permissions (if necessary)**:
   - You may need to set appropriate permissions on the toolchain directory to ensure that all required users have access to it. For example:
     ```bash
     sudo chmod -R u=rwx,g=rx,o=rx /opt/toolchain_directory
     ```
   - Replace `toolchain_directory` with the actual directory name of the toolchain in `/opt`.

4. **Update Environment Variables**:
   - To use the toolchain, update the PATH environment variable to include the toolchain’s bin directory. Add the following line to your `.bashrc` or `.bash_profile` (or the configuration file of your shell):
     ```bash
     export PATH=/opt/toolchain_directory/bin:$PATH
     ```
   - Again, replace `toolchain_directory` with the actual directory name. After editing the file, apply the changes by running `source ~/.bashrc` or simply reopen the terminal.

5. **Configure the Project**:
   - When configuring your project (e.g., running `cmake`), specify the toolchain file if needed. This is often done with a flag like `-DCMAKE_TOOLCHAIN_FILE=/path/to/toolchainfile.cmake`.

6. **Document the Setup**:
   - Provide clear documentation for your project, indicating that the toolchain is located in `/opt` and how to set it up, including any environment variable changes and permissions settings.

7. **Gitignore the Toolchain (if using Git)**:
   - If your project is in a Git repository, make sure that the toolchain directory is listed in your `.gitignore` file to prevent it from being accidentally committed to your repository.
