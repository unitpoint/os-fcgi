# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
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

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /tmp/os-igor

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/os-igor/ncurses-5.9

# Include any dependencies generated for this target.
include mysql/unittest/mysys/CMakeFiles/lf-t.dir/depend.make

# Include the progress variables for this target.
include mysql/unittest/mysys/CMakeFiles/lf-t.dir/progress.make

# Include the compile flags for this target's objects.
include mysql/unittest/mysys/CMakeFiles/lf-t.dir/flags.make

mysql/unittest/mysys/CMakeFiles/lf-t.dir/lf-t.c.o: mysql/unittest/mysys/CMakeFiles/lf-t.dir/flags.make
mysql/unittest/mysys/CMakeFiles/lf-t.dir/lf-t.c.o: ../mysql/unittest/mysys/lf-t.c
	$(CMAKE_COMMAND) -E cmake_progress_report /tmp/os-igor/ncurses-5.9/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object mysql/unittest/mysys/CMakeFiles/lf-t.dir/lf-t.c.o"
	cd /tmp/os-igor/ncurses-5.9/mysql/unittest/mysys && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/lf-t.dir/lf-t.c.o   -c /tmp/os-igor/mysql/unittest/mysys/lf-t.c

mysql/unittest/mysys/CMakeFiles/lf-t.dir/lf-t.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/lf-t.dir/lf-t.c.i"
	cd /tmp/os-igor/ncurses-5.9/mysql/unittest/mysys && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /tmp/os-igor/mysql/unittest/mysys/lf-t.c > CMakeFiles/lf-t.dir/lf-t.c.i

mysql/unittest/mysys/CMakeFiles/lf-t.dir/lf-t.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/lf-t.dir/lf-t.c.s"
	cd /tmp/os-igor/ncurses-5.9/mysql/unittest/mysys && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /tmp/os-igor/mysql/unittest/mysys/lf-t.c -o CMakeFiles/lf-t.dir/lf-t.c.s

mysql/unittest/mysys/CMakeFiles/lf-t.dir/lf-t.c.o.requires:
.PHONY : mysql/unittest/mysys/CMakeFiles/lf-t.dir/lf-t.c.o.requires

mysql/unittest/mysys/CMakeFiles/lf-t.dir/lf-t.c.o.provides: mysql/unittest/mysys/CMakeFiles/lf-t.dir/lf-t.c.o.requires
	$(MAKE) -f mysql/unittest/mysys/CMakeFiles/lf-t.dir/build.make mysql/unittest/mysys/CMakeFiles/lf-t.dir/lf-t.c.o.provides.build
.PHONY : mysql/unittest/mysys/CMakeFiles/lf-t.dir/lf-t.c.o.provides

mysql/unittest/mysys/CMakeFiles/lf-t.dir/lf-t.c.o.provides.build: mysql/unittest/mysys/CMakeFiles/lf-t.dir/lf-t.c.o
.PHONY : mysql/unittest/mysys/CMakeFiles/lf-t.dir/lf-t.c.o.provides.build

# Object files for target lf-t
lf__t_OBJECTS = \
"CMakeFiles/lf-t.dir/lf-t.c.o"

# External object files for target lf-t
lf__t_EXTERNAL_OBJECTS =

mysql/unittest/mysys/lf-t: mysql/unittest/mysys/CMakeFiles/lf-t.dir/lf-t.c.o
mysql/unittest/mysys/lf-t: mysql/unittest/mytap/libmytap.a
mysql/unittest/mysys/lf-t: mysql/mysys/libmysys.a
mysql/unittest/mysys/lf-t: mysql/strings/libstrings.a
mysql/unittest/mysys/lf-t: mysql/dbug/libdbug.a
mysql/unittest/mysys/lf-t: mysql/mysys/libmysys.a
mysql/unittest/mysys/lf-t: mysql/dbug/libdbug.a
mysql/unittest/mysys/lf-t: mysql/strings/libstrings.a
mysql/unittest/mysys/lf-t: mysql/zlib/libzlib.a
mysql/unittest/mysys/lf-t: mysql/unittest/mysys/CMakeFiles/lf-t.dir/build.make
mysql/unittest/mysys/lf-t: mysql/unittest/mysys/CMakeFiles/lf-t.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable lf-t"
	cd /tmp/os-igor/ncurses-5.9/mysql/unittest/mysys && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lf-t.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
mysql/unittest/mysys/CMakeFiles/lf-t.dir/build: mysql/unittest/mysys/lf-t
.PHONY : mysql/unittest/mysys/CMakeFiles/lf-t.dir/build

mysql/unittest/mysys/CMakeFiles/lf-t.dir/requires: mysql/unittest/mysys/CMakeFiles/lf-t.dir/lf-t.c.o.requires
.PHONY : mysql/unittest/mysys/CMakeFiles/lf-t.dir/requires

mysql/unittest/mysys/CMakeFiles/lf-t.dir/clean:
	cd /tmp/os-igor/ncurses-5.9/mysql/unittest/mysys && $(CMAKE_COMMAND) -P CMakeFiles/lf-t.dir/cmake_clean.cmake
.PHONY : mysql/unittest/mysys/CMakeFiles/lf-t.dir/clean

mysql/unittest/mysys/CMakeFiles/lf-t.dir/depend:
	cd /tmp/os-igor/ncurses-5.9 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/os-igor /tmp/os-igor/mysql/unittest/mysys /tmp/os-igor/ncurses-5.9 /tmp/os-igor/ncurses-5.9/mysql/unittest/mysys /tmp/os-igor/ncurses-5.9/mysql/unittest/mysys/CMakeFiles/lf-t.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : mysql/unittest/mysys/CMakeFiles/lf-t.dir/depend

