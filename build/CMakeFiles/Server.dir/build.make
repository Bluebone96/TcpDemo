# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/blue/Code/TcpDemo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/blue/Code/TcpDemo/build

# Include any dependencies generated for this target.
include CMakeFiles/Server.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Server.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Server.dir/flags.make

CMakeFiles/Server.dir/Server/main.cpp.o: CMakeFiles/Server.dir/flags.make
CMakeFiles/Server.dir/Server/main.cpp.o: ../Server/main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/blue/Code/TcpDemo/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Server.dir/Server/main.cpp.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Server.dir/Server/main.cpp.o -c /home/blue/Code/TcpDemo/Server/main.cpp

CMakeFiles/Server.dir/Server/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Server.dir/Server/main.cpp.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/blue/Code/TcpDemo/Server/main.cpp > CMakeFiles/Server.dir/Server/main.cpp.i

CMakeFiles/Server.dir/Server/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Server.dir/Server/main.cpp.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/blue/Code/TcpDemo/Server/main.cpp -o CMakeFiles/Server.dir/Server/main.cpp.s

CMakeFiles/Server.dir/Server/main.cpp.o.requires:
.PHONY : CMakeFiles/Server.dir/Server/main.cpp.o.requires

CMakeFiles/Server.dir/Server/main.cpp.o.provides: CMakeFiles/Server.dir/Server/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/Server.dir/build.make CMakeFiles/Server.dir/Server/main.cpp.o.provides.build
.PHONY : CMakeFiles/Server.dir/Server/main.cpp.o.provides

CMakeFiles/Server.dir/Server/main.cpp.o.provides.build: CMakeFiles/Server.dir/Server/main.cpp.o

CMakeFiles/Server.dir/Player/Player.cpp.o: CMakeFiles/Server.dir/flags.make
CMakeFiles/Server.dir/Player/Player.cpp.o: ../Player/Player.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/blue/Code/TcpDemo/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Server.dir/Player/Player.cpp.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Server.dir/Player/Player.cpp.o -c /home/blue/Code/TcpDemo/Player/Player.cpp

CMakeFiles/Server.dir/Player/Player.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Server.dir/Player/Player.cpp.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/blue/Code/TcpDemo/Player/Player.cpp > CMakeFiles/Server.dir/Player/Player.cpp.i

CMakeFiles/Server.dir/Player/Player.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Server.dir/Player/Player.cpp.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/blue/Code/TcpDemo/Player/Player.cpp -o CMakeFiles/Server.dir/Player/Player.cpp.s

CMakeFiles/Server.dir/Player/Player.cpp.o.requires:
.PHONY : CMakeFiles/Server.dir/Player/Player.cpp.o.requires

CMakeFiles/Server.dir/Player/Player.cpp.o.provides: CMakeFiles/Server.dir/Player/Player.cpp.o.requires
	$(MAKE) -f CMakeFiles/Server.dir/build.make CMakeFiles/Server.dir/Player/Player.cpp.o.provides.build
.PHONY : CMakeFiles/Server.dir/Player/Player.cpp.o.provides

CMakeFiles/Server.dir/Player/Player.cpp.o.provides.build: CMakeFiles/Server.dir/Player/Player.cpp.o

CMakeFiles/Server.dir/Server/Server.cpp.o: CMakeFiles/Server.dir/flags.make
CMakeFiles/Server.dir/Server/Server.cpp.o: ../Server/Server.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/blue/Code/TcpDemo/build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Server.dir/Server/Server.cpp.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Server.dir/Server/Server.cpp.o -c /home/blue/Code/TcpDemo/Server/Server.cpp

CMakeFiles/Server.dir/Server/Server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Server.dir/Server/Server.cpp.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/blue/Code/TcpDemo/Server/Server.cpp > CMakeFiles/Server.dir/Server/Server.cpp.i

CMakeFiles/Server.dir/Server/Server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Server.dir/Server/Server.cpp.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/blue/Code/TcpDemo/Server/Server.cpp -o CMakeFiles/Server.dir/Server/Server.cpp.s

CMakeFiles/Server.dir/Server/Server.cpp.o.requires:
.PHONY : CMakeFiles/Server.dir/Server/Server.cpp.o.requires

CMakeFiles/Server.dir/Server/Server.cpp.o.provides: CMakeFiles/Server.dir/Server/Server.cpp.o.requires
	$(MAKE) -f CMakeFiles/Server.dir/build.make CMakeFiles/Server.dir/Server/Server.cpp.o.provides.build
.PHONY : CMakeFiles/Server.dir/Server/Server.cpp.o.provides

CMakeFiles/Server.dir/Server/Server.cpp.o.provides.build: CMakeFiles/Server.dir/Server/Server.cpp.o

CMakeFiles/Server.dir/Handler/Handle.cpp.o: CMakeFiles/Server.dir/flags.make
CMakeFiles/Server.dir/Handler/Handle.cpp.o: ../Handler/Handle.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/blue/Code/TcpDemo/build/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Server.dir/Handler/Handle.cpp.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Server.dir/Handler/Handle.cpp.o -c /home/blue/Code/TcpDemo/Handler/Handle.cpp

CMakeFiles/Server.dir/Handler/Handle.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Server.dir/Handler/Handle.cpp.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/blue/Code/TcpDemo/Handler/Handle.cpp > CMakeFiles/Server.dir/Handler/Handle.cpp.i

CMakeFiles/Server.dir/Handler/Handle.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Server.dir/Handler/Handle.cpp.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/blue/Code/TcpDemo/Handler/Handle.cpp -o CMakeFiles/Server.dir/Handler/Handle.cpp.s

CMakeFiles/Server.dir/Handler/Handle.cpp.o.requires:
.PHONY : CMakeFiles/Server.dir/Handler/Handle.cpp.o.requires

CMakeFiles/Server.dir/Handler/Handle.cpp.o.provides: CMakeFiles/Server.dir/Handler/Handle.cpp.o.requires
	$(MAKE) -f CMakeFiles/Server.dir/build.make CMakeFiles/Server.dir/Handler/Handle.cpp.o.provides.build
.PHONY : CMakeFiles/Server.dir/Handler/Handle.cpp.o.provides

CMakeFiles/Server.dir/Handler/Handle.cpp.o.provides.build: CMakeFiles/Server.dir/Handler/Handle.cpp.o

CMakeFiles/Server.dir/Handler/EventHandler.cpp.o: CMakeFiles/Server.dir/flags.make
CMakeFiles/Server.dir/Handler/EventHandler.cpp.o: ../Handler/EventHandler.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/blue/Code/TcpDemo/build/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Server.dir/Handler/EventHandler.cpp.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Server.dir/Handler/EventHandler.cpp.o -c /home/blue/Code/TcpDemo/Handler/EventHandler.cpp

CMakeFiles/Server.dir/Handler/EventHandler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Server.dir/Handler/EventHandler.cpp.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/blue/Code/TcpDemo/Handler/EventHandler.cpp > CMakeFiles/Server.dir/Handler/EventHandler.cpp.i

CMakeFiles/Server.dir/Handler/EventHandler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Server.dir/Handler/EventHandler.cpp.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/blue/Code/TcpDemo/Handler/EventHandler.cpp -o CMakeFiles/Server.dir/Handler/EventHandler.cpp.s

CMakeFiles/Server.dir/Handler/EventHandler.cpp.o.requires:
.PHONY : CMakeFiles/Server.dir/Handler/EventHandler.cpp.o.requires

CMakeFiles/Server.dir/Handler/EventHandler.cpp.o.provides: CMakeFiles/Server.dir/Handler/EventHandler.cpp.o.requires
	$(MAKE) -f CMakeFiles/Server.dir/build.make CMakeFiles/Server.dir/Handler/EventHandler.cpp.o.provides.build
.PHONY : CMakeFiles/Server.dir/Handler/EventHandler.cpp.o.provides

CMakeFiles/Server.dir/Handler/EventHandler.cpp.o.provides.build: CMakeFiles/Server.dir/Handler/EventHandler.cpp.o

CMakeFiles/Server.dir/Handler/Dispatcher.cpp.o: CMakeFiles/Server.dir/flags.make
CMakeFiles/Server.dir/Handler/Dispatcher.cpp.o: ../Handler/Dispatcher.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/blue/Code/TcpDemo/build/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Server.dir/Handler/Dispatcher.cpp.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Server.dir/Handler/Dispatcher.cpp.o -c /home/blue/Code/TcpDemo/Handler/Dispatcher.cpp

CMakeFiles/Server.dir/Handler/Dispatcher.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Server.dir/Handler/Dispatcher.cpp.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/blue/Code/TcpDemo/Handler/Dispatcher.cpp > CMakeFiles/Server.dir/Handler/Dispatcher.cpp.i

CMakeFiles/Server.dir/Handler/Dispatcher.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Server.dir/Handler/Dispatcher.cpp.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/blue/Code/TcpDemo/Handler/Dispatcher.cpp -o CMakeFiles/Server.dir/Handler/Dispatcher.cpp.s

CMakeFiles/Server.dir/Handler/Dispatcher.cpp.o.requires:
.PHONY : CMakeFiles/Server.dir/Handler/Dispatcher.cpp.o.requires

CMakeFiles/Server.dir/Handler/Dispatcher.cpp.o.provides: CMakeFiles/Server.dir/Handler/Dispatcher.cpp.o.requires
	$(MAKE) -f CMakeFiles/Server.dir/build.make CMakeFiles/Server.dir/Handler/Dispatcher.cpp.o.provides.build
.PHONY : CMakeFiles/Server.dir/Handler/Dispatcher.cpp.o.provides

CMakeFiles/Server.dir/Handler/Dispatcher.cpp.o.provides.build: CMakeFiles/Server.dir/Handler/Dispatcher.cpp.o

# Object files for target Server
Server_OBJECTS = \
"CMakeFiles/Server.dir/Server/main.cpp.o" \
"CMakeFiles/Server.dir/Player/Player.cpp.o" \
"CMakeFiles/Server.dir/Server/Server.cpp.o" \
"CMakeFiles/Server.dir/Handler/Handle.cpp.o" \
"CMakeFiles/Server.dir/Handler/EventHandler.cpp.o" \
"CMakeFiles/Server.dir/Handler/Dispatcher.cpp.o"

# External object files for target Server
Server_EXTERNAL_OBJECTS =

../bin/Server: CMakeFiles/Server.dir/Server/main.cpp.o
../bin/Server: CMakeFiles/Server.dir/Player/Player.cpp.o
../bin/Server: CMakeFiles/Server.dir/Server/Server.cpp.o
../bin/Server: CMakeFiles/Server.dir/Handler/Handle.cpp.o
../bin/Server: CMakeFiles/Server.dir/Handler/EventHandler.cpp.o
../bin/Server: CMakeFiles/Server.dir/Handler/Dispatcher.cpp.o
../bin/Server: CMakeFiles/Server.dir/build.make
../bin/Server: ../lib/libDemoLib.a
../bin/Server: CMakeFiles/Server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../bin/Server"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Server.dir/build: ../bin/Server
.PHONY : CMakeFiles/Server.dir/build

CMakeFiles/Server.dir/requires: CMakeFiles/Server.dir/Server/main.cpp.o.requires
CMakeFiles/Server.dir/requires: CMakeFiles/Server.dir/Player/Player.cpp.o.requires
CMakeFiles/Server.dir/requires: CMakeFiles/Server.dir/Server/Server.cpp.o.requires
CMakeFiles/Server.dir/requires: CMakeFiles/Server.dir/Handler/Handle.cpp.o.requires
CMakeFiles/Server.dir/requires: CMakeFiles/Server.dir/Handler/EventHandler.cpp.o.requires
CMakeFiles/Server.dir/requires: CMakeFiles/Server.dir/Handler/Dispatcher.cpp.o.requires
.PHONY : CMakeFiles/Server.dir/requires

CMakeFiles/Server.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Server.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Server.dir/clean

CMakeFiles/Server.dir/depend:
	cd /home/blue/Code/TcpDemo/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/blue/Code/TcpDemo /home/blue/Code/TcpDemo /home/blue/Code/TcpDemo/build /home/blue/Code/TcpDemo/build /home/blue/Code/TcpDemo/build/CMakeFiles/Server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Server.dir/depend

