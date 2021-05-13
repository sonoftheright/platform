This is a sample application built on top of a simple platform layer that provides input handling and an OpenGL rendering capabilities for either MacOS or Windows.

Currently all the core platform code resides in the `platform/platform.h` header file. It includes a few other separate headers for various internal things. The rest of this repository is the "user" code for the sample application.

In order to compile the example program, you should just run `./compile_and_run.sh` in either your Mac console or `./compile_and_run.bat` in Windows.

Passing in `--production` to the script will build the app in production-mode (alters some compiler flags, and for MacOS it will build the `.app` package).

Requires clang to be installed, as well as the standard OS tools (ie, XCode, Visual Studio).

All that is expected of the example program right now is:
- a simple FPS counter rendered in the window (in the upper left)
- a 'toggle mouselock' instruction text
- flying 3D camera controlled by wasd & mouse (once mouselock is toggled on)
- a vertex-colored cube at the origin
- a "Quit" button rendered in the upper right.

This is based on a [stream](https://www.youtube.com/watch?v=CJSvTqgBkQk) that [Sean Barrett](https://github.com/nothings) did, where he was working on a single-file platform library.
This repo is a sample program to "prove out" and serve as a test for the platform layer.
The philosophy behind the platform header file is to provide a single entry point to #include that provides immediate access to building a native program.
While it is very large, it has no other dependencies (besides OS/standard libraries), and can be included easily into a project.

Other references this is based on:
- [TIGR](https://github.com/erkkah/tigr)
- [GLFW](https://github.com/glfw/glfw)
- [linmath.h](https://github.com/datenwolf/linmath.h)
- [STB's API file](https://gist.github.com/nothings/ef38135f4aa4799e8f09069a44ded5a2)
- [Per Vognsen's Mu](https://gist.github.com/pervognsen/6a67966c5dc4247a0021b95c8d0a7b72)

@NOTE: This project is in constant flux and development. There are no guarantees that any of this code works, _at all_.

@TODO:
- Audio support (currently working on using miniaudio for the time being, until we build-in audio to platform.h)
