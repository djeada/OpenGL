# 01 — VAO, VBO, EBO and shaders


An indexed blue quad, built from four vertices and two triangles. This is the
starting point for the course and follows the supplied Qt widget example.

## What you learn

| Object | Purpose in this program |
| --- | --- |
| VBO (vertex buffer object) | Stores the eight position floats in GPU memory. |
| EBO (element buffer object) | Stores six integer indices selecting vertices for two triangles. |
| VAO (vertex array object) | Remembers how attributes read the VBO and which EBO is attached. |
| Shader program | Runs the vertex shader for positions and the fragment shader for colors. |

## Read the source in this order

1. `main()` requests a desktop OpenGL 3.3 core context **before** creating
   `QApplication`. Qt owns the window, event loop, context, and presentation.
2. `initializeGL()` resolves the OpenGL functions when a context is current.
   The local arrays live on the CPU; `glBufferData` copies their contents to GPU buffers.
3. Bind the VAO before describing the buffers. Attribute location 0 contains
   two `GL_FLOAT` values per vertex. The stride is two floats and the offset is zero.
   `glVertexAttribPointer` captures the VBO binding for that attribute;
   `glEnableVertexAttribArray(0)` enables reading it.
4. The indices `{0, 1, 3, 1, 2, 3}` form two triangles with shared corners.
   The EBO binding is VAO state. Unbind the VAO before changing an element-buffer
   binding; otherwise you can accidentally detach its EBO.
5. The vertex shader writes `gl_Position = vec4(position, 0.0, 1.0)`.
   With `w = 1`, these positions are already in normalized device coordinates
   after the perspective divide. Both x and y range from -1 to +1 across the viewport.
6. The fragment shader outputs the same opaque blue color for every covered fragment.
   Compilation and linking failures print the shader log and stop the program.
7. `paintGL()` clears the frame, binds the program and VAO, and calls
   `glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)`.
   The count is **indices**, not vertices or bytes. `nullptr` means offset zero
   into the bound EBO. Qt presents the widget's framebuffer after rendering.
8. `cleanup()` makes the context current before deleting the shader and buffers.

The quad stretches when the window's aspect ratio changes because coordinates
are mapped directly to the viewport. Project 05 introduces an aspect-aware projection.

## Try it

- Change the fragment color without changing the vertex data.
- Change the indices to `{0, 1, 2}` and the draw count to 3. Explain which triangle remains.
- Move one corner in the vertex array and identify both triangles that use it.
- Change the shader's input location and the corresponding attribute location together.
- Introduce a GLSL typo to see how the error log identifies the failing shader.

## Common mistakes

- Uploading the index array to `GL_ARRAY_BUFFER` rather than `GL_ELEMENT_ARRAY_BUFFER`.
- Treating a VAO as storage for the vertex bytes.
- Giving `glDrawElements` a byte size rather than the number of indices.
- Compiling shaders in the constructor before Qt has made an OpenGL context current.


## Build this project on its own

Requirements: C++17, CMake 3.16+, Qt 6.2+ (`Widgets`, `OpenGL`, `OpenGLWidgets`),
and a desktop OpenGL 3.3+ implementation. See the [course setup](../../README.md)
for installation and Qt Creator instructions. After copying this folder elsewhere,
all source and build files needed by this project remain inside the folder.

From this directory:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
./build/opengl_01
```

On Windows with a multi-configuration generator, add `--config Debug` to the
build command and run `build/Debug/opengl_01.exe`; pass `-C Debug` to CTest.
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
