# 02 — Vertex attributes and interpolation


A quad with four corner colors and smooth color transitions across its triangles.

## What you learn

One vertex can carry multiple attributes. The VBO stores an interleaved sequence
of **x, y, red, green, blue**. Attribute 0 reads the first two floats and attribute
1 reads the following three; both use a five-float stride.

## Read the source

1. Compare the vertex array with each `glVertexAttribPointer` call. The color
   offset is `2 * sizeof(float)`, expressed as a byte offset into the bound buffer.
2. The vertex shader accepts both attributes and passes `vertexColor` to its output.
3. The rasterizer interpolates the color across each triangle. The fragment shader
   receives the matching input and writes it to the framebuffer.
4. The EBO still lets the two triangles share four corners, including their colors.

Shader stage outputs and inputs must agree in type and name. Attribute locations
describe vertex inputs; they are a different interface from the interpolated values
between shader stages.

## Try it

- Make all four colors identical. The gradient should disappear.
- Change the color at one corner and identify which triangles are affected.
- Add an alpha component: update the array, stride, color attribute size, and shader type.
- Add the `flat` qualifier to both sides of the color interface to compare flat and smooth shading.


## Build this project on its own

Requirements: C++17, CMake 3.16+, Qt 6.2+ (`Widgets`, `OpenGL`, `OpenGLWidgets`),
and a desktop OpenGL 3.3+ implementation. See the [course setup](../../README.md)
for installation and Qt Creator instructions. After copying this folder elsewhere,
all source and build files needed by this project remain inside the folder.

From this directory:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
./build/opengl_02
```

On Windows with a multi-configuration generator, add `--config Debug` to the
build command and run `build/Debug/opengl_02.exe`; pass `-C Debug` to CTest.
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
