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
include glog-build/CMakeFiles/signalhandler_unittest.dir/depend.make

# Include the progress variables for this target.
include glog-build/CMakeFiles/signalhandler_unittest.dir/progress.make

# Include the compile flags for this target's objects.
include glog-build/CMakeFiles/signalhandler_unittest.dir/flags.make

glog-build/CMakeFiles/signalhandler_unittest.dir/src/signalhandler_unittest.cc.o: glog-build/CMakeFiles/signalhandler_unittest.dir/flags.make
glog-build/CMakeFiles/signalhandler_unittest.dir/src/signalhandler_unittest.cc.o: ../thirdparty/glog/src/signalhandler_unittest.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object glog-build/CMakeFiles/signalhandler_unittest.dir/src/signalhandler_unittest.cc.o"
	cd /mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl/glog-build && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/signalhandler_unittest.dir/src/signalhandler_unittest.cc.o -c /mnt/d/DESKTOP/study/DBS/minisql/thirdparty/glog/src/signalhandler_unittest.cc

glog-build/CMakeFiles/signalhandler_unittest.dir/src/signalhandler_unittest.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/signalhandler_unittest.dir/src/signalhandler_unittest.cc.i"
	cd /mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl/glog-build && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/DESKTOP/study/DBS/minisql/thirdparty/glog/src/signalhandler_unittest.cc > CMakeFiles/signalhandler_unittest.dir/src/signalhandler_unittest.cc.i

glog-build/CMakeFiles/signalhandler_unittest.dir/src/signalhandler_unittest.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/signalhandler_unittest.dir/src/signalhandler_unittest.cc.s"
	cd /mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl/glog-build && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/DESKTOP/study/DBS/minisql/thirdparty/glog/src/signalhandler_unittest.cc -o CMakeFiles/signalhandler_unittest.dir/src/signalhandler_unittest.cc.s

# Object files for target signalhandler_unittest
signalhandler_unittest_OBJECTS = \
"CMakeFiles/signalhandler_unittest.dir/src/signalhandler_unittest.cc.o"

# External object files for target signalhandler_unittest
signalhandler_unittest_EXTERNAL_OBJECTS =

glog-build/signalhandler_unittest: glog-build/CMakeFiles/signalhandler_unittest.dir/src/signalhandler_unittest.cc.o
glog-build/signalhandler_unittest: glog-build/CMakeFiles/signalhandler_unittest.dir/build.make
glog-build/signalhandler_unittest: glog-build/libglogtestd.a
glog-build/signalhandler_unittest: glog-build/CMakeFiles/signalhandler_unittest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable signalhandler_unittest"
	cd /mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl/glog-build && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/signalhandler_unittest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
glog-build/CMakeFiles/signalhandler_unittest.dir/build: glog-build/signalhandler_unittest

.PHONY : glog-build/CMakeFiles/signalhandler_unittest.dir/build

glog-build/CMakeFiles/signalhandler_unittest.dir/clean:
	cd /mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl/glog-build && $(CMAKE_COMMAND) -P CMakeFiles/signalhandler_unittest.dir/cmake_clean.cmake
.PHONY : glog-build/CMakeFiles/signalhandler_unittest.dir/clean

glog-build/CMakeFiles/signalhandler_unittest.dir/depend:
	cd /mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/d/DESKTOP/study/DBS/minisql /mnt/d/DESKTOP/study/DBS/minisql/thirdparty/glog /mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl /mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl/glog-build /mnt/d/DESKTOP/study/DBS/minisql/cmake-build-debug-wsl/glog-build/CMakeFiles/signalhandler_unittest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : glog-build/CMakeFiles/signalhandler_unittest.dir/depend

