# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.21

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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.21.4/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.21.4/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/xlzz/Downloads/PathTracer-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/xlzz/Downloads/PathTracer-master/build

# Utility rule file for PathTracer_autogen.

# Include any custom commands dependencies for this target.
include CMakeFiles/PathTracer_autogen.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/PathTracer_autogen.dir/progress.make

CMakeFiles/PathTracer_autogen:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/xlzz/Downloads/PathTracer-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Automatic MOC for target PathTracer"
	/opt/homebrew/Cellar/cmake/3.21.4/bin/cmake -E cmake_autogen /Users/xlzz/Downloads/PathTracer-master/build/CMakeFiles/PathTracer_autogen.dir/AutogenInfo.json ""

PathTracer_autogen: CMakeFiles/PathTracer_autogen
PathTracer_autogen: CMakeFiles/PathTracer_autogen.dir/build.make
.PHONY : PathTracer_autogen

# Rule to build all files generated by this target.
CMakeFiles/PathTracer_autogen.dir/build: PathTracer_autogen
.PHONY : CMakeFiles/PathTracer_autogen.dir/build

CMakeFiles/PathTracer_autogen.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/PathTracer_autogen.dir/cmake_clean.cmake
.PHONY : CMakeFiles/PathTracer_autogen.dir/clean

CMakeFiles/PathTracer_autogen.dir/depend:
	cd /Users/xlzz/Downloads/PathTracer-master/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/xlzz/Downloads/PathTracer-master /Users/xlzz/Downloads/PathTracer-master /Users/xlzz/Downloads/PathTracer-master/build /Users/xlzz/Downloads/PathTracer-master/build /Users/xlzz/Downloads/PathTracer-master/build/CMakeFiles/PathTracer_autogen.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/PathTracer_autogen.dir/depend

