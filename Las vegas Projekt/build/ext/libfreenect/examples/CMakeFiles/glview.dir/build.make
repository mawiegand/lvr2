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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/student/d/dofeldsc/Arbeitsfläche/meshing.dofeldsc

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/student/d/dofeldsc/Arbeitsfläche/meshing.dofeldsc/build

# Include any dependencies generated for this target.
include ext/libfreenect/examples/CMakeFiles/glview.dir/depend.make

# Include the progress variables for this target.
include ext/libfreenect/examples/CMakeFiles/glview.dir/progress.make

# Include the compile flags for this target's objects.
include ext/libfreenect/examples/CMakeFiles/glview.dir/flags.make

ext/libfreenect/examples/CMakeFiles/glview.dir/glview.c.o: ext/libfreenect/examples/CMakeFiles/glview.dir/flags.make
ext/libfreenect/examples/CMakeFiles/glview.dir/glview.c.o: ../ext/libfreenect/examples/glview.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/student/d/dofeldsc/Arbeitsfläche/meshing.dofeldsc/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object ext/libfreenect/examples/CMakeFiles/glview.dir/glview.c.o"
	cd /home/student/d/dofeldsc/Arbeitsfläche/meshing.dofeldsc/build/ext/libfreenect/examples && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/glview.dir/glview.c.o   -c /home/student/d/dofeldsc/Arbeitsfläche/meshing.dofeldsc/ext/libfreenect/examples/glview.c

ext/libfreenect/examples/CMakeFiles/glview.dir/glview.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/glview.dir/glview.c.i"
	cd /home/student/d/dofeldsc/Arbeitsfläche/meshing.dofeldsc/build/ext/libfreenect/examples && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/student/d/dofeldsc/Arbeitsfläche/meshing.dofeldsc/ext/libfreenect/examples/glview.c > CMakeFiles/glview.dir/glview.c.i

ext/libfreenect/examples/CMakeFiles/glview.dir/glview.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/glview.dir/glview.c.s"
	cd /home/student/d/dofeldsc/Arbeitsfläche/meshing.dofeldsc/build/ext/libfreenect/examples && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/student/d/dofeldsc/Arbeitsfläche/meshing.dofeldsc/ext/libfreenect/examples/glview.c -o CMakeFiles/glview.dir/glview.c.s

ext/libfreenect/examples/CMakeFiles/glview.dir/glview.c.o.requires:
.PHONY : ext/libfreenect/examples/CMakeFiles/glview.dir/glview.c.o.requires

ext/libfreenect/examples/CMakeFiles/glview.dir/glview.c.o.provides: ext/libfreenect/examples/CMakeFiles/glview.dir/glview.c.o.requires
	$(MAKE) -f ext/libfreenect/examples/CMakeFiles/glview.dir/build.make ext/libfreenect/examples/CMakeFiles/glview.dir/glview.c.o.provides.build
.PHONY : ext/libfreenect/examples/CMakeFiles/glview.dir/glview.c.o.provides

ext/libfreenect/examples/CMakeFiles/glview.dir/glview.c.o.provides.build: ext/libfreenect/examples/CMakeFiles/glview.dir/glview.c.o

# Object files for target glview
glview_OBJECTS = \
"CMakeFiles/glview.dir/glview.c.o"

# External object files for target glview
glview_EXTERNAL_OBJECTS =

../bin/glview: ext/libfreenect/examples/CMakeFiles/glview.dir/glview.c.o
../bin/glview: lib/libfreenect.so.0.1.2
../bin/glview: /usr/lib/x86_64-linux-gnu/libGLU.so
../bin/glview: /usr/lib/x86_64-linux-gnu/libGL.so
../bin/glview: /usr/lib/x86_64-linux-gnu/libSM.so
../bin/glview: /usr/lib/x86_64-linux-gnu/libICE.so
../bin/glview: /usr/lib/x86_64-linux-gnu/libX11.so
../bin/glview: /usr/lib/x86_64-linux-gnu/libXext.so
../bin/glview: /usr/lib/libglut.so
../bin/glview: /usr/lib/libXmu.so
../bin/glview: /usr/lib/x86_64-linux-gnu/libXi.so
../bin/glview: /usr/lib/x86_64-linux-gnu/libusb-1.0.so
../bin/glview: ext/libfreenect/examples/CMakeFiles/glview.dir/build.make
../bin/glview: ext/libfreenect/examples/CMakeFiles/glview.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable ../../../../bin/glview"
	cd /home/student/d/dofeldsc/Arbeitsfläche/meshing.dofeldsc/build/ext/libfreenect/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/glview.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
ext/libfreenect/examples/CMakeFiles/glview.dir/build: ../bin/glview
.PHONY : ext/libfreenect/examples/CMakeFiles/glview.dir/build

ext/libfreenect/examples/CMakeFiles/glview.dir/requires: ext/libfreenect/examples/CMakeFiles/glview.dir/glview.c.o.requires
.PHONY : ext/libfreenect/examples/CMakeFiles/glview.dir/requires

ext/libfreenect/examples/CMakeFiles/glview.dir/clean:
	cd /home/student/d/dofeldsc/Arbeitsfläche/meshing.dofeldsc/build/ext/libfreenect/examples && $(CMAKE_COMMAND) -P CMakeFiles/glview.dir/cmake_clean.cmake
.PHONY : ext/libfreenect/examples/CMakeFiles/glview.dir/clean

ext/libfreenect/examples/CMakeFiles/glview.dir/depend:
	cd /home/student/d/dofeldsc/Arbeitsfläche/meshing.dofeldsc/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/student/d/dofeldsc/Arbeitsfläche/meshing.dofeldsc /home/student/d/dofeldsc/Arbeitsfläche/meshing.dofeldsc/ext/libfreenect/examples /home/student/d/dofeldsc/Arbeitsfläche/meshing.dofeldsc/build /home/student/d/dofeldsc/Arbeitsfläche/meshing.dofeldsc/build/ext/libfreenect/examples /home/student/d/dofeldsc/Arbeitsfläche/meshing.dofeldsc/build/ext/libfreenect/examples/CMakeFiles/glview.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : ext/libfreenect/examples/CMakeFiles/glview.dir/depend

