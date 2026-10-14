# 05 — 2D transformations


A colored quad that rotates, changes scale, and moves horizontally.

## What you learn

Homogeneous coordinates let a 4 by 4 matrix express translation as well as
rotation and scale. `QMatrix4x4` provides the CPU-side mathematics; the actual
vertex transformation still happens in the GLSL vertex shader.

## Read the source

1. An orthographic projection maps a world rectangle to the viewport. Its
   horizontal range uses `width / height`, keeping the quad's proportions stable on resize.
2. Build a model matrix by calling `translate`, `rotate`, and `scale`.
   These operations compose as `T * R * S` in this program.
3. Send `projection * model` as the `transform` uniform. GLSL multiplies
   a column vector on the right, so scale acts first, then rotation, then translation.
4. The vertex data remains fixed. Only the matrix uniform changes per frame.

`QMatrix4x4::rotate` takes degrees. The sine functions in C++ take radians;
in this example their argument is elapsed seconds used as an animation phase.

## Try it

- Exchange the translate and rotate calls. Describe orbiting versus rotating in place.
- Use a nonuniform scale and watch how the shape changes.
- Remove the aspect-ratio correction and compare wide and tall windows.
- Replace the animation with fixed values and calculate the transformed origin by hand.


## Build this project on its own

Requirements: C++17, CMake 3.16+, Qt 6.2+ (`Widgets`, `OpenGL`, `OpenGLWidgets`),
and a desktop OpenGL 3.3+ implementation. See the [course setup](../../README.md)
for installation and Qt Creator instructions. After copying this folder elsewhere,
all source and build files needed by this project remain inside the folder.

From this directory:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
./build/opengl_05
```

On Windows with a multi-configuration generator, add `--config Debug` to the
build command and run `build/Debug/opengl_05.exe`; pass `-C Debug` to CTest.
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
