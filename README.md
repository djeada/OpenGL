# Learn OpenGL with Qt

Ten independent C++ projects teaching modern desktop OpenGL through Qt 6.
Start with a small `QOpenGLWidget` that draws an indexed quad using a **VAO,
VBO, EBO, vertex shader and fragment shader**, then work toward an interactive
3D camera, lighting, transparency and framebuffer effects.

Qt provides the window, event loop, input, context, shader compilation and
matrix helpers. The lessons use explicit OpenGL 3.3 core calls and GLSL 330
so the rendering pipeline stays visible. There are no GLFW, SDL, GLAD, GLEW,
GLM, Qt 3D, or external image-asset dependencies.

## The ten projects

| Project | Lesson | Main concepts |
| --- | --- | --- |
| 01 | [VAO, VBO, EBO and shaders](projects/01_buffers_and_shaders) | Indexed drawing, vertex layout, GLSL stages |
| 02 | [Vertex attributes and interpolation](projects/02_vertex_colors) | Interleaved attributes and rasterizer interpolation |
| 03 | [Uniforms and animation](projects/03_uniforms_and_animation) | CPU-to-shader values, elapsed time and Qt repaint scheduling |
| 04 | [Textures, sampling and wrapping](projects/04_textures) | UVs, image upload, texture units, filtering and wrapping |
| 05 | [2D transformations](projects/05_transformations) | Translation, rotation, scale and orthographic projection |
| 06 | [Perspective and depth testing](projects/06_depth_and_3d) | Model/view/projection matrices, perspective and occlusion |
| 07 | [An interactive orbit camera](projects/07_camera) | Mouse and keyboard input, orbiting, zoom and view matrices |
| 08 | [Ambient, diffuse and specular lighting](projects/08_lighting) | Normals, normal matrices, ambient/diffuse/specular terms |
| 09 | [Alpha blending and draw order](projects/09_blending) | Straight alpha, depth writes and transparent draw order |
| 10 | [Framebuffers and post-processing](projects/10_framebuffers) | Render targets, two rendering passes and screen effects |

The concepts progress in order, but the projects are independent. Each folder
contains its own `main.cpp`, `CMakeLists.txt`, README and optional rendering-test
header. Copy any one folder and build it without the repository root or another
lesson. Repeated setup code is intentional: no shared rendering engine hides
the OpenGL operations or creates dependencies between projects.

## Requirements and setup

- A C++17 compiler, CMake 3.16 or newer, and Qt 6.2 or newer.
- Qt modules: `Widgets`, `OpenGL`, `OpenGLWidgets`.
- Desktop OpenGL 3.3 or newer with a working graphical display. These projects
  use the desktop core API and GLSL 330, so an OpenGL ES-only Qt kit is unsuitable.

On Ubuntu 24.04:

```sh
sudo apt-get update
sudo apt-get install g++ cmake ninja-build qt6-base-dev libqt6opengl6-dev
```

On Windows or macOS, install a Qt 6 desktop kit and a matching C++ compiler.
In Qt Creator, open an individual project's `CMakeLists.txt`, select that Qt 6
kit, and build/run it. To use command-line CMake with a Qt installer build, add
`-DCMAKE_PREFIX_PATH="/path/to/Qt/6.x/your-desktop-kit"` during configuration.
The Qt kit architecture and compiler must match the application build.

## Build and run one project

From the repository root:

```sh
cmake -S projects/01_buffers_and_shaders -B build/01 -DCMAKE_BUILD_TYPE=Debug
cmake --build build/01 --parallel
./build/01/opengl_01
```

With a Windows multi-configuration generator, build using `--config Debug`
and run `build/01/Debug/opengl_01.exe`. If using `ctest`, also pass `-C Debug`.
Every lesson explains the expected image, its important calls and experiments
to try. Escape closes a window; each lesson README lists its additional controls.

## Build the whole course (optional)

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
./build/projects/01_buffers_and_shaders/opengl_01
```

The top-level CMake file only collects the ten independent targets. Each
project still works when configured directly. Executables are named
`opengl_01` through `opengl_10`.

## Rendering checks

```sh
ctest --test-dir build --output-on-failure
```

The local smoke tests create a real widget and OpenGL context, check for
visible geometry and GL errors, resize the framebuffer, and exercise each
lesson's keyboard modes. They exit automatically and require a working display.
To save an image while running one of them:

```sh
./build/01/opengl_01 --smoke-test --screenshot frame.png
```

Linux headless checks use Mesa software rendering with Xvfb:

```sh
sudo apt-get install xvfb xauth libgl1-mesa-dri
LIBGL_ALWAYS_SOFTWARE=1 xvfb-run -a -s "-screen 0 1280x1024x24" \
  ctest --test-dir build --output-on-failure
```

The GitHub Actions workflow builds the complete course, renders all lessons,
checks high DPI framebuffer resizing, and separately builds copied lesson
folders outside the repository to verify their independence. Set
`-DBUILD_TESTING=OFF` if you only want the applications.

## Qt and OpenGL lifecycle

All lessons request the core context before constructing `QApplication`.
They create GL objects in `initializeGL()`, draw in `paintGL()`, and delete
objects with the context current in `cleanup()`. Cleanup runs both from the
widget destructor and before Qt destroys its context. Animation and input
handlers schedule updates; they do not issue GL calls without a current context.

Qt owns presentation. With `QOpenGLWidget`, rendering targets a Qt-owned
framebuffer: the framebuffer lesson explicitly restores
`defaultFramebufferObject()` after its offscreen pass.

If CMake cannot find Qt, select a Qt 6 kit or set `CMAKE_PREFIX_PATH`. If context
creation fails, check the desktop OpenGL driver and display configuration.
Shader compilation and linking failures include the Qt shader log.

## References

- [QOpenGLWidget lifecycle and framebuffer behavior](https://doc.qt.io/qt-6/qopenglwidget.html)
- [OpenGL 3.3 core function wrapper](https://doc.qt.io/qt-6/qopenglfunctions-3-3-core.html)
- [QOpenGLShaderProgram](https://doc.qt.io/qt-6/qopenglshaderprogram.html)
- [OpenGL reference pages](https://registry.khronos.org/OpenGL-Refpages/gl4/)

## License

MIT; see [LICENSE](LICENSE). The existing license and copyright are retained.
