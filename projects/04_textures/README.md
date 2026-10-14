# 04 — Textures, sampling and wrapping


A repeated checkerboard generated in memory, with filtering and wrapping controls.

## Controls

| Key | Action |
| --- | --- |
| F | Switch nearest/linear sampling, including the mipmap minification filter. |
| W | Switch repeat/clamp-to-edge wrapping. |

## What you learn

Texture coordinates are a vertex attribute. They are interpolated across each
triangle and used to sample a texture in the fragment shader. Coordinates in
this example span 0 to 2, so repeat wrapping shows two repetitions along each axis.

## Read the source

1. A `QImage` creates a 64 by 64 RGBA checkerboard. No image file or external asset is needed.
2. `glTexImage2D` uploads RGBA bytes with `GL_RGBA8` internal storage. The image
   is vertically mirrored because its first row is the top, while OpenGL's first row is the bottom.
3. `glGenerateMipmap` creates progressively smaller levels for minification.
4. Bind the texture to `GL_TEXTURE0` and set the sampler uniform to **0**.
   The sampler takes a texture-unit index, not the object ID returned by `glGenTextures`.
5. Filtering controls how texels are combined. Wrapping controls what happens
   outside the 0-to-1 range. Keyboard handlers change CPU flags; `paintGL()` applies GL state.

## Try it

- Change the UV range to 0 to 1 and then to 0 to 4.
- Add a colored corner to the generated image to make its orientation obvious.
- Compare nearest and linear filters while resizing the window.
- Remove the mipmaps and use `GL_LINEAR` as the minification filter. Explain
  why using a mipmap filter without a complete mip chain can make a texture incomplete.


## Build this project on its own

Requirements: C++17, CMake 3.16+, Qt 6.2+ (`Widgets`, `OpenGL`, `OpenGLWidgets`),
and a desktop OpenGL 3.3+ implementation. See the [course setup](../../README.md)
for installation and Qt Creator instructions. After copying this folder elsewhere,
all source and build files needed by this project remain inside the folder.

From this directory:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
./build/opengl_04
```

On Windows with a multi-configuration generator, add `--config Debug` to the
build command and run `build/Debug/opengl_04.exe`; pass `-C Debug` to CTest.
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
