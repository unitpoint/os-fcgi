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
include mysql/vio/CMakeFiles/vio.dir/depend.make

# Include the progress variables for this target.
include mysql/vio/CMakeFiles/vio.dir/progress.make

# Include the compile flags for this target's objects.
include mysql/vio/CMakeFiles/vio.dir/flags.make

mysql/vio/CMakeFiles/vio.dir/vio.c.o: mysql/vio/CMakeFiles/vio.dir/flags.make
mysql/vio/CMakeFiles/vio.dir/vio.c.o: ../mysql/vio/vio.c
	$(CMAKE_COMMAND) -E cmake_progress_report /tmp/os-igor/ncurses-5.9/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object mysql/vio/CMakeFiles/vio.dir/vio.c.o"
	cd /tmp/os-igor/ncurses-5.9/mysql/vio && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -fPIC -o CMakeFiles/vio.dir/vio.c.o   -c /tmp/os-igor/mysql/vio/vio.c

mysql/vio/CMakeFiles/vio.dir/vio.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/vio.dir/vio.c.i"
	cd /tmp/os-igor/ncurses-5.9/mysql/vio && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -fPIC -E /tmp/os-igor/mysql/vio/vio.c > CMakeFiles/vio.dir/vio.c.i

mysql/vio/CMakeFiles/vio.dir/vio.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/vio.dir/vio.c.s"
	cd /tmp/os-igor/ncurses-5.9/mysql/vio && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -fPIC -S /tmp/os-igor/mysql/vio/vio.c -o CMakeFiles/vio.dir/vio.c.s

mysql/vio/CMakeFiles/vio.dir/vio.c.o.requires:
.PHONY : mysql/vio/CMakeFiles/vio.dir/vio.c.o.requires

mysql/vio/CMakeFiles/vio.dir/vio.c.o.provides: mysql/vio/CMakeFiles/vio.dir/vio.c.o.requires
	$(MAKE) -f mysql/vio/CMakeFiles/vio.dir/build.make mysql/vio/CMakeFiles/vio.dir/vio.c.o.provides.build
.PHONY : mysql/vio/CMakeFiles/vio.dir/vio.c.o.provides

mysql/vio/CMakeFiles/vio.dir/vio.c.o.provides.build: mysql/vio/CMakeFiles/vio.dir/vio.c.o
.PHONY : mysql/vio/CMakeFiles/vio.dir/vio.c.o.provides.build

mysql/vio/CMakeFiles/vio.dir/viosocket.c.o: mysql/vio/CMakeFiles/vio.dir/flags.make
mysql/vio/CMakeFiles/vio.dir/viosocket.c.o: ../mysql/vio/viosocket.c
	$(CMAKE_COMMAND) -E cmake_progress_report /tmp/os-igor/ncurses-5.9/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object mysql/vio/CMakeFiles/vio.dir/viosocket.c.o"
	cd /tmp/os-igor/ncurses-5.9/mysql/vio && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -fPIC -o CMakeFiles/vio.dir/viosocket.c.o   -c /tmp/os-igor/mysql/vio/viosocket.c

mysql/vio/CMakeFiles/vio.dir/viosocket.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/vio.dir/viosocket.c.i"
	cd /tmp/os-igor/ncurses-5.9/mysql/vio && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -fPIC -E /tmp/os-igor/mysql/vio/viosocket.c > CMakeFiles/vio.dir/viosocket.c.i

mysql/vio/CMakeFiles/vio.dir/viosocket.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/vio.dir/viosocket.c.s"
	cd /tmp/os-igor/ncurses-5.9/mysql/vio && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -fPIC -S /tmp/os-igor/mysql/vio/viosocket.c -o CMakeFiles/vio.dir/viosocket.c.s

mysql/vio/CMakeFiles/vio.dir/viosocket.c.o.requires:
.PHONY : mysql/vio/CMakeFiles/vio.dir/viosocket.c.o.requires

mysql/vio/CMakeFiles/vio.dir/viosocket.c.o.provides: mysql/vio/CMakeFiles/vio.dir/viosocket.c.o.requires
	$(MAKE) -f mysql/vio/CMakeFiles/vio.dir/build.make mysql/vio/CMakeFiles/vio.dir/viosocket.c.o.provides.build
.PHONY : mysql/vio/CMakeFiles/vio.dir/viosocket.c.o.provides

mysql/vio/CMakeFiles/vio.dir/viosocket.c.o.provides.build: mysql/vio/CMakeFiles/vio.dir/viosocket.c.o
.PHONY : mysql/vio/CMakeFiles/vio.dir/viosocket.c.o.provides.build

mysql/vio/CMakeFiles/vio.dir/viossl.c.o: mysql/vio/CMakeFiles/vio.dir/flags.make
mysql/vio/CMakeFiles/vio.dir/viossl.c.o: ../mysql/vio/viossl.c
	$(CMAKE_COMMAND) -E cmake_progress_report /tmp/os-igor/ncurses-5.9/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object mysql/vio/CMakeFiles/vio.dir/viossl.c.o"
	cd /tmp/os-igor/ncurses-5.9/mysql/vio && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -fPIC -o CMakeFiles/vio.dir/viossl.c.o   -c /tmp/os-igor/mysql/vio/viossl.c

mysql/vio/CMakeFiles/vio.dir/viossl.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/vio.dir/viossl.c.i"
	cd /tmp/os-igor/ncurses-5.9/mysql/vio && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -fPIC -E /tmp/os-igor/mysql/vio/viossl.c > CMakeFiles/vio.dir/viossl.c.i

mysql/vio/CMakeFiles/vio.dir/viossl.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/vio.dir/viossl.c.s"
	cd /tmp/os-igor/ncurses-5.9/mysql/vio && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -fPIC -S /tmp/os-igor/mysql/vio/viossl.c -o CMakeFiles/vio.dir/viossl.c.s

mysql/vio/CMakeFiles/vio.dir/viossl.c.o.requires:
.PHONY : mysql/vio/CMakeFiles/vio.dir/viossl.c.o.requires

mysql/vio/CMakeFiles/vio.dir/viossl.c.o.provides: mysql/vio/CMakeFiles/vio.dir/viossl.c.o.requires
	$(MAKE) -f mysql/vio/CMakeFiles/vio.dir/build.make mysql/vio/CMakeFiles/vio.dir/viossl.c.o.provides.build
.PHONY : mysql/vio/CMakeFiles/vio.dir/viossl.c.o.provides

mysql/vio/CMakeFiles/vio.dir/viossl.c.o.provides.build: mysql/vio/CMakeFiles/vio.dir/viossl.c.o
.PHONY : mysql/vio/CMakeFiles/vio.dir/viossl.c.o.provides.build

mysql/vio/CMakeFiles/vio.dir/viosslfactories.c.o: mysql/vio/CMakeFiles/vio.dir/flags.make
mysql/vio/CMakeFiles/vio.dir/viosslfactories.c.o: ../mysql/vio/viosslfactories.c
	$(CMAKE_COMMAND) -E cmake_progress_report /tmp/os-igor/ncurses-5.9/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object mysql/vio/CMakeFiles/vio.dir/viosslfactories.c.o"
	cd /tmp/os-igor/ncurses-5.9/mysql/vio && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -fPIC -o CMakeFiles/vio.dir/viosslfactories.c.o   -c /tmp/os-igor/mysql/vio/viosslfactories.c

mysql/vio/CMakeFiles/vio.dir/viosslfactories.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/vio.dir/viosslfactories.c.i"
	cd /tmp/os-igor/ncurses-5.9/mysql/vio && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -fPIC -E /tmp/os-igor/mysql/vio/viosslfactories.c > CMakeFiles/vio.dir/viosslfactories.c.i

mysql/vio/CMakeFiles/vio.dir/viosslfactories.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/vio.dir/viosslfactories.c.s"
	cd /tmp/os-igor/ncurses-5.9/mysql/vio && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -fPIC -S /tmp/os-igor/mysql/vio/viosslfactories.c -o CMakeFiles/vio.dir/viosslfactories.c.s

mysql/vio/CMakeFiles/vio.dir/viosslfactories.c.o.requires:
.PHONY : mysql/vio/CMakeFiles/vio.dir/viosslfactories.c.o.requires

mysql/vio/CMakeFiles/vio.dir/viosslfactories.c.o.provides: mysql/vio/CMakeFiles/vio.dir/viosslfactories.c.o.requires
	$(MAKE) -f mysql/vio/CMakeFiles/vio.dir/build.make mysql/vio/CMakeFiles/vio.dir/viosslfactories.c.o.provides.build
.PHONY : mysql/vio/CMakeFiles/vio.dir/viosslfactories.c.o.provides

mysql/vio/CMakeFiles/vio.dir/viosslfactories.c.o.provides.build: mysql/vio/CMakeFiles/vio.dir/viosslfactories.c.o
.PHONY : mysql/vio/CMakeFiles/vio.dir/viosslfactories.c.o.provides.build

# Object files for target vio
vio_OBJECTS = \
"CMakeFiles/vio.dir/vio.c.o" \
"CMakeFiles/vio.dir/viosocket.c.o" \
"CMakeFiles/vio.dir/viossl.c.o" \
"CMakeFiles/vio.dir/viosslfactories.c.o"

# External object files for target vio
vio_EXTERNAL_OBJECTS =

mysql/vio/libvio.a: mysql/vio/CMakeFiles/vio.dir/vio.c.o
mysql/vio/libvio.a: mysql/vio/CMakeFiles/vio.dir/viosocket.c.o
mysql/vio/libvio.a: mysql/vio/CMakeFiles/vio.dir/viossl.c.o
mysql/vio/libvio.a: mysql/vio/CMakeFiles/vio.dir/viosslfactories.c.o
mysql/vio/libvio.a: mysql/vio/CMakeFiles/vio.dir/build.make
mysql/vio/libvio.a: mysql/vio/CMakeFiles/vio.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C static library libvio.a"
	cd /tmp/os-igor/ncurses-5.9/mysql/vio && $(CMAKE_COMMAND) -P CMakeFiles/vio.dir/cmake_clean_target.cmake
	cd /tmp/os-igor/ncurses-5.9/mysql/vio && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/vio.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
mysql/vio/CMakeFiles/vio.dir/build: mysql/vio/libvio.a
.PHONY : mysql/vio/CMakeFiles/vio.dir/build

mysql/vio/CMakeFiles/vio.dir/requires: mysql/vio/CMakeFiles/vio.dir/vio.c.o.requires
mysql/vio/CMakeFiles/vio.dir/requires: mysql/vio/CMakeFiles/vio.dir/viosocket.c.o.requires
mysql/vio/CMakeFiles/vio.dir/requires: mysql/vio/CMakeFiles/vio.dir/viossl.c.o.requires
mysql/vio/CMakeFiles/vio.dir/requires: mysql/vio/CMakeFiles/vio.dir/viosslfactories.c.o.requires
.PHONY : mysql/vio/CMakeFiles/vio.dir/requires

mysql/vio/CMakeFiles/vio.dir/clean:
	cd /tmp/os-igor/ncurses-5.9/mysql/vio && $(CMAKE_COMMAND) -P CMakeFiles/vio.dir/cmake_clean.cmake
.PHONY : mysql/vio/CMakeFiles/vio.dir/clean

mysql/vio/CMakeFiles/vio.dir/depend:
	cd /tmp/os-igor/ncurses-5.9 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/os-igor /tmp/os-igor/mysql/vio /tmp/os-igor/ncurses-5.9 /tmp/os-igor/ncurses-5.9/mysql/vio /tmp/os-igor/ncurses-5.9/mysql/vio/CMakeFiles/vio.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : mysql/vio/CMakeFiles/vio.dir/depend

