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
CMAKE_SOURCE_DIR = /home/imacsimus/MyProjects/noapi-v2.0

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/imacsimus/MyProjects/noapi-v2.0/build

# Include any dependencies generated for this target.
include CMakeFiles/litemath.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/litemath.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/litemath.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/litemath.dir/flags.make

CMakeFiles/litemath.dir/external/litemath/Image2d.cpp.o: CMakeFiles/litemath.dir/flags.make
CMakeFiles/litemath.dir/external/litemath/Image2d.cpp.o: ../external/litemath/Image2d.cpp
CMakeFiles/litemath.dir/external/litemath/Image2d.cpp.o: CMakeFiles/litemath.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/imacsimus/MyProjects/noapi-v2.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/litemath.dir/external/litemath/Image2d.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/litemath.dir/external/litemath/Image2d.cpp.o -MF CMakeFiles/litemath.dir/external/litemath/Image2d.cpp.o.d -o CMakeFiles/litemath.dir/external/litemath/Image2d.cpp.o -c /home/imacsimus/MyProjects/noapi-v2.0/external/litemath/Image2d.cpp

CMakeFiles/litemath.dir/external/litemath/Image2d.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/litemath.dir/external/litemath/Image2d.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/imacsimus/MyProjects/noapi-v2.0/external/litemath/Image2d.cpp > CMakeFiles/litemath.dir/external/litemath/Image2d.cpp.i

CMakeFiles/litemath.dir/external/litemath/Image2d.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/litemath.dir/external/litemath/Image2d.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/imacsimus/MyProjects/noapi-v2.0/external/litemath/Image2d.cpp -o CMakeFiles/litemath.dir/external/litemath/Image2d.cpp.s

# Object files for target litemath
litemath_OBJECTS = \
"CMakeFiles/litemath.dir/external/litemath/Image2d.cpp.o"

# External object files for target litemath
litemath_EXTERNAL_OBJECTS =

liblitemath.a: CMakeFiles/litemath.dir/external/litemath/Image2d.cpp.o
liblitemath.a: CMakeFiles/litemath.dir/build.make
liblitemath.a: CMakeFiles/litemath.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/imacsimus/MyProjects/noapi-v2.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library liblitemath.a"
	$(CMAKE_COMMAND) -P CMakeFiles/litemath.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/litemath.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/litemath.dir/build: liblitemath.a
.PHONY : CMakeFiles/litemath.dir/build

CMakeFiles/litemath.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/litemath.dir/cmake_clean.cmake
.PHONY : CMakeFiles/litemath.dir/clean

CMakeFiles/litemath.dir/depend:
	cd /home/imacsimus/MyProjects/noapi-v2.0/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/imacsimus/MyProjects/noapi-v2.0 /home/imacsimus/MyProjects/noapi-v2.0 /home/imacsimus/MyProjects/noapi-v2.0/build /home/imacsimus/MyProjects/noapi-v2.0/build /home/imacsimus/MyProjects/noapi-v2.0/build/CMakeFiles/litemath.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/litemath.dir/depend

