# 06 — Perspective and depth testing


Two overlapping rotating cubes rendered with a perspective camera.

## Controls

| Key | Action |
| --- | --- |
| D | Toggle depth testing to compare correct occlusion with draw-order artifacts. |

## What you learn

3D vertices pass through model, view, and projection matrices. The shader
computes `projection * view * model * position`. Perspective makes distant
objects appear smaller; the depth buffer decides which surface is visible.

## Read the source

1. Eight vertices and 36 indices describe six cube faces, each made of two triangles.
2. `lookAt` builds a fixed view matrix from an eye position, a target, and an up vector.
3. `perspective` uses a 45-degree vertical field of view, the current window aspect,
   and positive near/far distances of 0.1 and 100.
4. Enable `GL_DEPTH_TEST` and clear **both color and depth** each frame.
   The default `GL_LESS` test keeps fragments closer than the stored depth.
5. Draw the same mesh twice with different model matrices. The buffers are reused;
   the uniform state changes for each draw.

Draw order alone cannot correctly hide all faces of overlapping 3D objects.
Press D to inspect why an opaque scene needs a depth buffer.

## Try it

- Move one cube along z and compare perspective and occlusion.
- Replace the perspective projection with an orthographic projection.
- Change the near distance and observe clipping; a near distance of zero is invalid.
- Reverse the cube draw order with depth testing enabled. The visible result should remain correct.


## Build this project on its own

Requirements: C++17, CMake 3.16+, Qt 6.2+ (`Widgets`, `OpenGL`, `OpenGLWidgets`),
and a desktop OpenGL 3.3+ implementation. See the [course setup](../../README.md)
for installation and Qt Creator instructions. After copying this folder elsewhere,
all source and build files needed by this project remain inside the folder.

From this directory:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
./build/opengl_06
```

On Windows with a multi-configuration generator, add `--config Debug` to the
build command and run `build/Debug/opengl_06.exe`; pass `-C Debug` to CTest.
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
