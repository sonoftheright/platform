This is a simple platform layer that provides simple inputs and an OpenGL rendering capabilities for either MacOS or Windows.

In order to compile the example program, you should just run `./compile_and_run.sh` in either your Mac console or `./compile_and_run.bat` in Windows.

Requires clang to be installed, as well as the standard OS development packages.

All that is expected of the example program right now is:
- a simple FPS counter rendered in the window (in the upper left)
- a 'toggle mouselock' instruction text
- a vertex-colored cube at the origin
- a "Quit" button rendered in the upper right.

This is based on a [stream](https://www.youtube.com/watch?v=CJSvTqgBkQk) that [Sean Barrett](https://github.com/nothings) did, where he was working on a single-file platform library.
This repo is a sample program to "prove out" and serve as a test for the platform layer.
The philosophy behind the platform header file is to provide a single include that provides immediate access to building a native program. While it is very large, it has no other dependencies (besides OS libraries), and can be included easily into a project.
