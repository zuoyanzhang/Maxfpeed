# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

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
CMAKE_SOURCE_DIR = /home/zzy/Maxfpeed

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zzy/Maxfpeed/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/ErrorDetection.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/ErrorDetection.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/ErrorDetection.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ErrorDetection.dir/flags.make

CMakeFiles/ErrorDetection.dir/maxFpeedMain/main.cpp.o: CMakeFiles/ErrorDetection.dir/flags.make
CMakeFiles/ErrorDetection.dir/maxFpeedMain/main.cpp.o: ../maxFpeedMain/main.cpp
CMakeFiles/ErrorDetection.dir/maxFpeedMain/main.cpp.o: CMakeFiles/ErrorDetection.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zzy/Maxfpeed/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ErrorDetection.dir/maxFpeedMain/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ErrorDetection.dir/maxFpeedMain/main.cpp.o -MF CMakeFiles/ErrorDetection.dir/maxFpeedMain/main.cpp.o.d -o CMakeFiles/ErrorDetection.dir/maxFpeedMain/main.cpp.o -c /home/zzy/Maxfpeed/maxFpeedMain/main.cpp

CMakeFiles/ErrorDetection.dir/maxFpeedMain/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ErrorDetection.dir/maxFpeedMain/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zzy/Maxfpeed/maxFpeedMain/main.cpp > CMakeFiles/ErrorDetection.dir/maxFpeedMain/main.cpp.i

CMakeFiles/ErrorDetection.dir/maxFpeedMain/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ErrorDetection.dir/maxFpeedMain/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zzy/Maxfpeed/maxFpeedMain/main.cpp -o CMakeFiles/ErrorDetection.dir/maxFpeedMain/main.cpp.s

# Object files for target ErrorDetection
ErrorDetection_OBJECTS = \
"CMakeFiles/ErrorDetection.dir/maxFpeedMain/main.cpp.o"

# External object files for target ErrorDetection
ErrorDetection_EXTERNAL_OBJECTS =

ErrorDetection: CMakeFiles/ErrorDetection.dir/maxFpeedMain/main.cpp.o
ErrorDetection: CMakeFiles/ErrorDetection.dir/build.make
ErrorDetection: detectModule/libdetectModule.a
ErrorDetection: CMakeFiles/ErrorDetection.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/zzy/Maxfpeed/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ErrorDetection"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ErrorDetection.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ErrorDetection.dir/build: ErrorDetection
.PHONY : CMakeFiles/ErrorDetection.dir/build

CMakeFiles/ErrorDetection.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ErrorDetection.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ErrorDetection.dir/clean

CMakeFiles/ErrorDetection.dir/depend:
	cd /home/zzy/Maxfpeed/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zzy/Maxfpeed /home/zzy/Maxfpeed /home/zzy/Maxfpeed/cmake-build-debug /home/zzy/Maxfpeed/cmake-build-debug /home/zzy/Maxfpeed/cmake-build-debug/CMakeFiles/ErrorDetection.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ErrorDetection.dir/depend
