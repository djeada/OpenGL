# 08 — Ambient, diffuse and specular lighting


A rotating blue box lit by a point light, marked by a small white cube.

## Controls

| Key | Lighting stages |
| --- | --- |
| 1 | Ambient only. |
| 2 | Ambient + diffuse. |
| 3 | Ambient + diffuse + specular (initial mode). |

## What you learn

Surface normals describe orientation. Diffuse lighting uses the dot product
of a normalized normal and the light direction. Specular lighting also depends
on the viewer direction. All those vectors must use the same coordinate space.

## Read the source

1. Each cube face has its own normal. The mesh duplicates corner positions,
   giving 24 vertices so adjacent faces can have different normals.
2. The vertex shader computes world-space positions and normals. Because the
   model uses a nonuniform scale, transform normals with the inverse-transpose
   3 by 3 matrix from `model.normalMatrix()`.
3. Normalize the interpolated normal in the fragment shader. Interpolation
   generally changes its length.
4. Ambient light adds a constant base. Diffuse light uses `max(dot(n, lightDirection), 0)`.
   A reflected light vector and a power of 32 produce a Phong specular highlight.
   Suppress that highlight when the surface faces away from the light.
5. Draw a scaled white cube at the light position using the same geometry
   and shader with an `unlit` uniform.

This is a teaching lighting model: it has no distance attenuation, shadows,
or material textures. Its small shader makes each lighting contribution visible.

## Try it

- Move the point light and predict which face becomes brightest.
- Change the shininess exponent to 8 or 128 and compare the highlight size.
- Compare the inverse-transpose normal matrix with a naive model-matrix transform
  while retaining the nonuniform scale.
- Add distance attenuation or a second light after identifying the existing three terms.


## Build this project on its own

Requirements: C++17, CMake 3.16+, Qt 6.2+ (`Widgets`, `OpenGL`, `OpenGLWidgets`),
and a desktop OpenGL 3.3+ implementation. See the [course setup](../../README.md)
for installation and Qt Creator instructions. After copying this folder elsewhere,
all source and build files needed by this project remain inside the folder.

From this directory:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
./build/opengl_08
```

On Windows with a multi-configuration generator, add `--config Debug` to the
build command and run `build/Debug/opengl_08.exe`; pass `-C Debug` to CTest.
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
