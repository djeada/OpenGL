# 09 — Alpha blending and draw order


Three translucent colored panels overlapping an opaque checkerboard.

## Controls

| Key | Action |
| --- | --- |
| O | Compare back-to-front order with deliberately reversed order. |
| Up / Down | Increase / decrease panel opacity. |

## What you learn

Straight-alpha blending combines source and destination RGB as
`source * alpha + destination * (1 - alpha)`. The result depends on the order
in which translucent surfaces are drawn.

## Read the source

1. Draw the opaque checkerboard with depth testing and depth writes enabled.
2. Enable `GL_BLEND` and choose `GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA`.
3. Keep depth testing enabled so opaque surfaces can occlude translucent ones,
   but disable depth writes with `glDepthMask(GL_FALSE)` for the transparent pass.
4. Draw the panels from farthest to nearest. The camera is fixed, so their
   z positions provide an explicit order. A moving camera would require recomputing it.
5. Restore depth writes and disable blending after the pass. A disabled depth
   write mask would also prevent the next frame's depth clear from working.

Disabling depth writes does not disable the depth test. These settings solve
different problems. Simple sorting is sufficient for these separated panels;
intersecting transparent geometry needs more sophisticated treatment.

## Try it

- Press O and identify the changed colors where all three panels overlap.
- Change the alpha values to 1.0 and to 0.0, predicting the results first.
- Re-enable depth writes for the transparent pass and draw front-to-back to expose lost layers.
- Add an opaque object between two panels and observe which transparent fragments are rejected.


## Build this project on its own

Requirements: C++17, CMake 3.16+, Qt 6.2+ (`Widgets`, `OpenGL`, `OpenGLWidgets`),
and a desktop OpenGL 3.3+ implementation. See the [course setup](../../README.md)
for installation and Qt Creator instructions. After copying this folder elsewhere,
all source and build files needed by this project remain inside the folder.

From this directory:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
./build/opengl_09
```

On Windows with a multi-configuration generator, add `--config Debug` to the
build command and run `build/Debug/opengl_09.exe`; pass `-C Debug` to CTest.
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
