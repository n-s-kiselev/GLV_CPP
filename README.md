# GLV (Graphics Library of Views)
### GUI Building Toolkit


1. About
========================================
GLV (Graphics Library of Views) is a GUI building toolkit written in C++ for Linux, OSX, and Win32. GLV is specially designed for creating interfaces to real-time, multimedia applications using hardware accelerated graphics. GLV has no dependencies on other libraries other than OpenGL which is provided by all modern operating systems. Although windowing is technically not a part of GLV, it does provide an abstraction layer for creating bindings to a particular windowing system for creating an OpenGL context and getting mouse and keyboard input. A binding to GLUT is currently provided. 


2. Compilation Instructions
========================================

The source code can either be built into a library or directly compiled from source into an application. In the following instructions, the base directory is where this README file is located.

2.1 Building a Library
----------------------------------------

GLV is built with a single cross-platform [nob.h](https://github.com/tsoding/nob.h) build script, `nob.c`, on Linux, macOS and Windows (MinGW). It needs only a C/C++ compiler: no Make, CMake or IDE project. First bootstrap the build program once:

	cc nob.c -o nob        (or: gcc nob.c -o nob)

After that, `nob` rebuilds itself automatically whenever `nob.c` changes. The available commands are:

	./nob                    - builds the library
	./nob -examples-glut     - builds the GLUT examples (requires ./nob first)
	./nob -examples-glfw     - builds the GLFW3 examples
	./nob -examples-sdl      - builds the SDL3 examples
	./nob -examples-sfml     - builds the SFML3 examples
	./nob -examples-<b> -dynamic - same, but links against the shared library
	./nob -test              - builds and runs test/test_units.cpp
	./nob -clean             - removes the build folder
	./nob -help              - lists all options

The same 25 examples exist once per windowing toolkit, as examples/<toolkit>/<name>_<toolkit>.cpp. The GLUT examples use GLV's own Window/Application binding. The GLFW3, SDL3 and SFML3 examples create their own window and run their own main loop, and pass events to GLV through the small header-only helper in their folder (glv_glfw.h, glv_sdl.h, glv_sfml.h). GLFW3, SDL3 and SFML3 are vendored in vendor/ and built from source; the GLV library itself links none of them.

Everything the build produces goes into ./build:

	build/lib/libGLV.a              static library
	build/lib/libfreeglut.a         vendored FreeGLUT (link together with libGLV.a)
	build/lib/libGLV.{so,dylib,dll} shared library (FreeGLUT included)
	build/include/GLV/              public headers
	build/examples/static-<toolkit>/ statically linked examples (glut, glfw, sdl, sfml)
	build/examples/shared-<toolkit>/ dynamically linked examples
	build/tests/                    test programs

Programs linked with `-dynamic` are not self-contained: unlike the static build, they have to find
`build/lib/libGLV.{dll,so,dylib}` at run time. Rather than copying that file next to every executable or
permanently adding `build/lib` to your system-wide search path, point the loader at it for the current
shell session or command only. The snippets below use the GLFW3 `widgets` example; the same pattern
applies to any example under any toolkit's `shared-<toolkit>` folder.

**Windows (Command Prompt)**

	cd build\examples\shared-glfw
	set PATH=..\..\lib;%PATH%
	widgets_glfw.exe

**Windows (PowerShell)**

	cd build\examples\shared-glfw
	$env:PATH = "..\..\lib;$env:PATH"
	.\widgets_glfw.exe

**Linux (bash)**

	cd build/examples/shared-glfw
	LD_LIBRARY_PATH=../../lib ./widgets_glfw

**macOS (bash)**

	cd build/examples/shared-glfw
	DYLD_LIBRARY_PATH=../../lib ./widgets_glfw

The `set PATH=`/`$env:PATH` assignment only lasts for the current Command Prompt/PowerShell session; the
`LD_LIBRARY_PATH`/`DYLD_LIBRARY_PATH` prefix form only applies to that single command. Either way your
system-wide search path is left untouched, and no `.dll`/`.so`/`.dylib` has to be copied anywhere.

On Windows that same session-scoped `PATH` prepend is also how you make sure the right GCC runtime gets
loaded. Both the static and the dynamic examples need MinGW's own `libstdc++-6.dll` and
`libgcc_s_seh-1.dll`, and unrelated programs (gnuplot, for one) install older copies of those under
directories that can come earlier on `PATH`. An example that starts and exits immediately with code
`0xC0000139` (`STATUS_ENTRYPOINT_NOT_FOUND`) is loading one of those stale copies; put your MinGW `bin`
directory first to fix it:

	set PATH=C:\mingw64\bin;..\..\lib;%PATH%


2.2 Compiling Direct From Source
----------------------------------------
GLV can easily be compiled directly from source into an existing project.

Make sure to pass in the following flags to the compiler:

	-finline-functions (or -O3)
	-fpeel-loops


2.3 Dependencies
----------------------------------------
GLV requires only OpenGL. On Linux and Windows the OpenGL functions are loaded with GLAD, which is vendored in vendor/glad and compiled into the library. The GLUT window binding uses FreeGLUT, which is vendored in vendor/freeglut and built from source by `nob.c` on every platform, so no GLUT installation is needed. The GLFW3, SDL3 and SFML3 toolkits used by the examples are vendored as well.

- macOS: nothing to install besides the Xcode command line tools.
- Linux (Debian/Ubuntu): `sudo apt install libgl-dev libx11-dev libxrandr-dev libxi-dev libxxf86vm-dev` for the library itself. The example toolkits need some more X11 development headers on top of that:

	- GLFW3: `libxcursor-dev libxinerama-dev libxext-dev`
	- SDL3: `libxcursor-dev libxext-dev libxfixes-dev libxss-dev`
	- SFML3: `libxcursor-dev libudev-dev`

	`./nob` checks for every header it needs before compiling and names the package that provides any missing one. Only the headers are required for SDL3: it loads the X11 extension libraries at run time, so a machine without, say, libXcursor installed loses that feature instead of failing to start. The X11 backend is the only one built for SDL3 and SFML3; both run under Wayland through XWayland.
- Windows (MSYS2 MinGW-w64): `pacman -S mingw-w64-x86_64-gcc`.



3. File Organization
========================================

	GLV/		GLV headers
	src/		GLV source
	nob.c		build script (see section 2.1)
	vendor/		vendored third-party code (nob.h, GLAD, FreeGLUT, GLFW3, SDL3, SFML3)

	examples/	example source demonstrating various features of GLV,
			one folder per windowing toolkit (glut, glfw, sdl, sfml)
	test/		unit and visual testing source

	doc/		documentation of GLV source, design, etc.


