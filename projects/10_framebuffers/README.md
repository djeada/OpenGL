# 10 — Framebuffers and post-processing


A rotating cube rendered to a texture, then displayed through a second shader.

## Controls

| Key | Effect |
| --- | --- |
| 1 | Original color. |
| 2 | Weighted grayscale. |
| 3 | Inverted color. |

## What you learn

A framebuffer object selects the destination for rendering. Attach a color
texture so a later pass can sample the image, and a depth renderbuffer so the
3D scene still has depth testing. The screen quad uses a separate VAO and program.

## Read the source

1. Create the FBO, color texture, and depth renderbuffer in `initializeGL()`.
2. `ensureFramebuffer()` allocates storage using **physical pixel** dimensions:
   widget size multiplied by `devicePixelRatioF()`. It runs on resize and also
   checks for display scale changes during painting.
3. Attach the texture at `GL_COLOR_ATTACHMENT0` and the renderbuffer at
   `GL_DEPTH_ATTACHMENT`. Stop with a clear error if the framebuffer is incomplete.
4. In pass one, bind the custom FBO, set its viewport, enable depth testing,
   clear both attachments, and draw the cube.
5. In pass two, bind **`defaultFramebufferObject()`**, set the viewport, and
   disable depth testing. QOpenGLWidget renders into a Qt-owned FBO whose ID
   can change; binding framebuffer zero would target the wrong destination.
6. Bind the rendered color texture to unit 0. Draw a quad covering the viewport
   with UVs spanning 0 to 1 and let the fragment shader transform each sampled color.
7. Resize the color and depth storage together. Delete every attachment,
   framebuffer, buffer, and program with the context current during cleanup.

The second pass samples a texture written by the **first** pass. Sampling an
attachment while rendering into that same attachment would create a feedback loop.

## Try it

- Resize the window and verify the effect fills the viewport without distortion.
- Render at half resolution and upscale during the second pass.
- Add a threshold effect or a 3 by 3 blur using neighboring texels.
- Preserve the original image on the left half of the screen and apply an effect on the right.


## Build this project on its own

Requirements: C++17, CMake 3.16+, Qt 6.2+ (`Widgets`, `OpenGL`, `OpenGLWidgets`),
and a desktop OpenGL 3.3+ implementation. See the [course setup](../../README.md)
for installation and Qt Creator instructions. After copying this folder elsewhere,
all source and build files needed by this project remain inside the folder.

From this directory:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
./build/opengl_10
```

On Windows with a multi-configuration generator, add `--config Debug` to the
build command and run `build/Debug/opengl_10.exe`; pass `-C Debug` to CTest.
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
