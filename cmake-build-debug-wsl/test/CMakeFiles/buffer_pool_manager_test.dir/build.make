# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/d/DESKTOP/study/DBS/minisql

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl

# Include any dependencies generated for this target.
include test/CMakeFiles/buffer_pool_manager_test.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/buffer_pool_manager_test.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/buffer_pool_manager_test.dir/flags.make

test/CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.o: test/CMakeFiles/buffer_pool_manager_test.dir/flags.make
test/CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.o: ../test/buffer/buffer_pool_manager_test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.o"
	cd /mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.o -c /mnt/d/DESKTOP/study/DBS/minisql/test/buffer/buffer_pool_manager_test.cpp

test/CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.i"
	cd /mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/DESKTOP/study/DBS/minisql/test/buffer/buffer_pool_manager_test.cpp > CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.i

test/CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.s"
	cd /mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/DESKTOP/study/DBS/minisql/test/buffer/buffer_pool_manager_test.cpp -o CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.s

# Object files for target buffer_pool_manager_test
buffer_pool_manager_test_OBJECTS = \
"CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.o"

# External object files for target buffer_pool_manager_test
buffer_pool_manager_test_EXTERNAL_OBJECTS =

test/buffer_pool_manager_test: test/CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.o
test/buffer_pool_manager_test: test/CMakeFiles/buffer_pool_manager_test.dir/build.make
test/buffer_pool_manager_test: bin/libminisql_shared.so
test/buffer_pool_manager_test: lib/libgtestd.a
test/buffer_pool_manager_test: test/libminisql_test_main.so
test/buffer_pool_manager_test: glog-build/libglogd.so.0.6.0
test/buffer_pool_manager_test: lib/libgtestd.a
test/buffer_pool_manager_test: test/CMakeFiles/buffer_pool_manager_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable buffer_pool_manager_test"
	cd /mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/buffer_pool_manager_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/buffer_pool_manager_test.dir/build: test/buffer_pool_manager_test

.PHONY : test/CMakeFiles/buffer_pool_manager_test.dir/build

test/CMakeFiles/buffer_pool_manager_test.dir/clean:
	cd /mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl/test && $(CMAKE_COMMAND) -P CMakeFiles/buffer_pool_manager_test.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/buffer_pool_manager_test.dir/clean

test/CMakeFiles/buffer_pool_manager_test.dir/depend:
	cd /mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/d/DESKTOP/study/DBS/minisql /mnt/d/DESKTOP/study/DBS/minisql/test /mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl /mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl/test /mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl/test/CMakeFiles/buffer_pool_manager_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/buffer_pool_manager_test.dir/depend

