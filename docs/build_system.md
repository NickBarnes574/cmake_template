# CMake Build System

## Specific Requirements

Break down your CMake build system into:

- A top level CMakeLists.txt, which includes the cmake/buildsystem.cmake file containing the build system and calls `add_all_targets()` and `add_all_tests()`
- A top level Makefile which specifies targets such as release, debug, and clean. Later, you can add labels to build a particular architecture (if cross-compiling) or to build and run unit or functional test suites.
- A `cmake/` directory, which houses the various CMake files (limited to one CMake function per file):
	- `buildsystem.cmake`: contains includes of all other .cmake files in this directory (a single place where everything is included)
	- `add_all_targets.cmake`: adds the source files and include for each target (calling `add_exe()` or `add_so()` as needed)
	- `add_all_tests.cmake`: contains a function to add GTest suite per target
	- `add_exe.cmake`: adds an executable file given a target name, source files, and include files
	- `add_gtest.cmake`: adds a GTest suite given a target name, C source files, test source files, and includes
	- `add_so.cmake`: same as `add_exe()` but build as a shared library
	- `disable_warnings.cmake`: function to disable lint / clang-tidy warnings for a particular file, to be used sparingly (useful as you will be using outside libraries such as TweetNacl)
	- `install_target.cmake`: install a target in directory in release/deploy/remote file structure.
	- `set_default_debug_options.cmake`, and a similar `set_default_release_options`: set any compiler or linking options in the appropriate mode
	- `strip_target.cmake`: performs stripping on an executable to be used in release mode
	- `tidy-checks.cmake`: where all your clang-tidy chekcs and configuration lives. You can extend this to add in other static analysis tools if desired.