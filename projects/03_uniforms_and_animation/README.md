# 03 — Uniforms and animation


A quad that moves from side to side while its color changes continuously.

## What you learn

A uniform is program state supplied by the CPU. Its value is constant for an
individual draw, unlike per-vertex attributes. Here the `time` uniform is used
by both shader stages, so a single update drives motion and color.

## Read the source

1. A `QElapsedTimer` measures real elapsed milliseconds. Convert to seconds
   when sending the value to GLSL with `setUniformValue`.
2. A `QTimer` requests a repaint about every 16 ms by calling `update()`.
   This schedules work through Qt; it is not a promise of exactly 60 frames per second.
3. Bind the shader program before setting its uniform. The vertex shader adds
   a sine-wave offset; the fragment shader creates three phase-shifted color channels.
4. The VBO is uploaded only during initialization. Shader arithmetic changes
   the displayed positions without re-uploading the vertex data every frame.

Qt calls `paintGL()` with the context current. Timer callbacks only request
updates; they do not call OpenGL directly.

## Try it

- Double the speed by multiplying time in the shader.
- Use cosine for the y offset to make the quad follow a circle.
- Replace the sine color with a CPU-provided `vec3` uniform.
- Stop the repaint timer and observe the difference between elapsed time and displayed frames.


## Build this project on its own

Requirements: C++17, CMake 3.16+, Qt 6.2+ (`Widgets`, `OpenGL`, `OpenGLWidgets`),
and a desktop OpenGL 3.3+ implementation. See the [course setup](../../README.md)
for installation and Qt Creator instructions. After copying this folder elsewhere,
all source and build files needed by this project remain inside the folder.

From this directory:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
./build/opengl_03
```

On Windows with a multi-configuration generator, add `--config Debug` to the
build command and run `build/Debug/opengl_03.exe`; pass `-C Debug` to CTest.
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
