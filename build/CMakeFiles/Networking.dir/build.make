# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/nick/Programming/CMake_Build_System

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/nick/Programming/CMake_Build_System/build

# Include any dependencies generated for this target.
include CMakeFiles/Networking.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/Networking.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/Networking.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Networking.dir/flags.make

CMakeFiles/Networking.dir/libraries/4_Networking/src/socket_io.c.o: CMakeFiles/Networking.dir/flags.make
CMakeFiles/Networking.dir/libraries/4_Networking/src/socket_io.c.o: ../libraries/4_Networking/src/socket_io.c
CMakeFiles/Networking.dir/libraries/4_Networking/src/socket_io.c.o: CMakeFiles/Networking.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nick/Programming/CMake_Build_System/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/Networking.dir/libraries/4_Networking/src/socket_io.c.o"
	/usr/bin/clang-14 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/Networking.dir/libraries/4_Networking/src/socket_io.c.o -MF CMakeFiles/Networking.dir/libraries/4_Networking/src/socket_io.c.o.d -o CMakeFiles/Networking.dir/libraries/4_Networking/src/socket_io.c.o -c /home/nick/Programming/CMake_Build_System/libraries/4_Networking/src/socket_io.c

CMakeFiles/Networking.dir/libraries/4_Networking/src/socket_io.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Networking.dir/libraries/4_Networking/src/socket_io.c.i"
	/usr/bin/clang-14 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/nick/Programming/CMake_Build_System/libraries/4_Networking/src/socket_io.c > CMakeFiles/Networking.dir/libraries/4_Networking/src/socket_io.c.i

CMakeFiles/Networking.dir/libraries/4_Networking/src/socket_io.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Networking.dir/libraries/4_Networking/src/socket_io.c.s"
	/usr/bin/clang-14 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/nick/Programming/CMake_Build_System/libraries/4_Networking/src/socket_io.c -o CMakeFiles/Networking.dir/libraries/4_Networking/src/socket_io.c.s

CMakeFiles/Networking.dir/libraries/4_Networking/src/tcp_server.c.o: CMakeFiles/Networking.dir/flags.make
CMakeFiles/Networking.dir/libraries/4_Networking/src/tcp_server.c.o: ../libraries/4_Networking/src/tcp_server.c
CMakeFiles/Networking.dir/libraries/4_Networking/src/tcp_server.c.o: CMakeFiles/Networking.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nick/Programming/CMake_Build_System/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/Networking.dir/libraries/4_Networking/src/tcp_server.c.o"
	/usr/bin/clang-14 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/Networking.dir/libraries/4_Networking/src/tcp_server.c.o -MF CMakeFiles/Networking.dir/libraries/4_Networking/src/tcp_server.c.o.d -o CMakeFiles/Networking.dir/libraries/4_Networking/src/tcp_server.c.o -c /home/nick/Programming/CMake_Build_System/libraries/4_Networking/src/tcp_server.c

CMakeFiles/Networking.dir/libraries/4_Networking/src/tcp_server.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Networking.dir/libraries/4_Networking/src/tcp_server.c.i"
	/usr/bin/clang-14 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/nick/Programming/CMake_Build_System/libraries/4_Networking/src/tcp_server.c > CMakeFiles/Networking.dir/libraries/4_Networking/src/tcp_server.c.i

CMakeFiles/Networking.dir/libraries/4_Networking/src/tcp_server.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Networking.dir/libraries/4_Networking/src/tcp_server.c.s"
	/usr/bin/clang-14 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/nick/Programming/CMake_Build_System/libraries/4_Networking/src/tcp_server.c -o CMakeFiles/Networking.dir/libraries/4_Networking/src/tcp_server.c.s

# Object files for target Networking
Networking_OBJECTS = \
"CMakeFiles/Networking.dir/libraries/4_Networking/src/socket_io.c.o" \
"CMakeFiles/Networking.dir/libraries/4_Networking/src/tcp_server.c.o"

# External object files for target Networking
Networking_EXTERNAL_OBJECTS =

libNetworking.a: CMakeFiles/Networking.dir/libraries/4_Networking/src/socket_io.c.o
libNetworking.a: CMakeFiles/Networking.dir/libraries/4_Networking/src/tcp_server.c.o
libNetworking.a: CMakeFiles/Networking.dir/build.make
libNetworking.a: CMakeFiles/Networking.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/nick/Programming/CMake_Build_System/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C static library libNetworking.a"
	$(CMAKE_COMMAND) -P CMakeFiles/Networking.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Networking.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Networking.dir/build: libNetworking.a
.PHONY : CMakeFiles/Networking.dir/build

CMakeFiles/Networking.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Networking.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Networking.dir/clean

CMakeFiles/Networking.dir/depend:
	cd /home/nick/Programming/CMake_Build_System/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/nick/Programming/CMake_Build_System /home/nick/Programming/CMake_Build_System /home/nick/Programming/CMake_Build_System/build /home/nick/Programming/CMake_Build_System/build /home/nick/Programming/CMake_Build_System/build/CMakeFiles/Networking.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Networking.dir/depend

