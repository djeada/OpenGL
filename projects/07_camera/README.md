# 07 — An interactive orbit camera


Three cubes viewed with an interactive orbit camera.

## Controls

| Input | Action |
| --- | --- |
| Left drag | Orbit around the origin. |
| Mouse wheel | Change the camera distance. |
| Arrow keys | Change yaw and pitch. |
| + / - | Zoom in / out. |
| R | Reset the camera. |

## What you learn

Moving a camera changes the view matrix, while the objects' model matrices
can remain unchanged. This orbit camera converts yaw, pitch, and distance
into an eye position on a sphere around the origin.

## Read the source

1. Convert degrees to radians before evaluating sine and cosine for the eye position.
2. Use `lookAt(eye, origin, up)` to build the view matrix and keep the perspective projection separate.
3. Qt input events update camera variables and schedule a repaint. OpenGL calls
   stay inside the methods with a current context.
4. Clamp pitch to -80 to +80 degrees to avoid the up-vector singularity at the poles.
   Clamp distance to keep zoom within a useful range.
5. Reuse the cube buffers while translating the model matrix to three different positions.

This is an orbit camera, rather than a first-person movement controller. It
provides a small, inspectable example of mapping Qt input onto OpenGL matrix uniforms.

## Try it

- Change the orbit target from the origin to one of the side cubes.
- Add keyboard controls to move the target, creating a camera pan.
- Explain why removing the pitch clamp causes trouble near a straight-up view.
- Compare camera movement with applying the opposite translation to every object.


## Build this project on its own

Requirements: C++17, CMake 3.16+, Qt 6.2+ (`Widgets`, `OpenGL`, `OpenGLWidgets`),
and a desktop OpenGL 3.3+ implementation. See the [course setup](../../README.md)
for installation and Qt Creator instructions. After copying this folder elsewhere,
all source and build files needed by this project remain inside the folder.

From this directory:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
./build/opengl_07
```

On Windows with a multi-configuration generator, add `--config Debug` to the
build command and run `build/Debug/opengl_07.exe`; pass `-C Debug` to CTest.
Set `CMAKE_PREFIX_PATH`
to your Qt installation if CMake cannot find it. Press Escape to close the window.

## Rendering check

```sh
ctest --test-dir build --output-on-failure
```

This runs the executable with `--smoke-test`, verifies a desktop context,
visible geometry, framebuffer dimensions and GL errors, then resizes the
window and exercises this lesson's keyboard controls where present. A desktop
display is required; on Linux CI use `xvfb-run -a ctest --test-dir build --output-on-failure`.
The local `smoke_test.h` contains only the optional test driver.
