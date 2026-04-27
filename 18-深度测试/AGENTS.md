# AGENTS.md — Project Guide for AI Coding Agents

> This file documents the architecture, build process, and conventions of the `18-深度测试` project. It is intended for AI coding agents who need to modify or extend the codebase.

---

## Project Overview

`18-深度测试` is a **software rasterizer** (CPU-based 3D renderer) written in C++17. It implements a complete graphics pipeline that closely mimics OpenGL ES, including vertex shaders, clip-space clipping, perspective-correct interpolation, rasterization, fragment shaders, depth testing, and face culling. The rendered output is presented via **Slint UI** on Linux.

This project is **Lesson 18** of a progressive graphics-programming tutorial series. The directory name (`18-深度测试`) indicates its place in the series. The focus of this specific lesson is **depth testing** (`DEPTH_TEST`), demonstrated by rendering two overlapping triangles at different Z depths.

**Important:** The `slint/` directory contains a pre-built Slint C++ SDK (v1.15.1, Linux x86_64) which is now **actively used** for windowing and presentation.

---

## Technology Stack

| Layer | Technology |
|-------|------------|
| Language | C++17 |
| Build System | CMake (minimum 3.12) |
| Platform | Linux (x86_64) |
| UI Framework | Slint C++ SDK v1.15.1 (Qt6 backend) |
| Math | Custom template math library (`math::vec2f/3f/4f`, `math::mat3f/4f`) |
| Image Loading | Custom `Image` wrapper around `stb_image.h` |

---

## Build and Run Commands

Standard CMake out-of-source build:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

The top-level `CMakeLists.txt` creates an executable named `softRenderer` and links two static libraries plus Slint:
- `applicationLib` — from `application/image.cpp`
- `gpuLib` — from `gpu/*.cpp` and `gpu/shader/*.cpp`
- `Slint::Slint` — from the pre-built SDK in `./slint`

Assets in `./assets` are copied to the build directory at configure time:

```cmake
file(GLOB copyResources "./assets")
file(COPY ${copyResources} DESTINATION ${CMAKE_BINARY_DIR})
```

### Run

The project runs on Linux with a display server (X11 or Wayland). For headless testing, use Qt's offscreen platform:

```bash
./softRenderer
# or for headless environments:
QT_QPA_PLATFORM=offscreen ./softRenderer
```

---

## Code Organization

```
.
├── CMakeLists.txt                  # Root CMake config (integrates Slint)
├── main.cpp                        # Standard main(), Slint UI setup, render loop
├── ui/
│   └── app.slint                   # Slint UI: Window + Image element
├── global/
│   └── base.h                      # Common types, constants, macros (RGBA, buffer enums, draw modes)
├── application/
│   ├── image.h / .cpp              # Image loading wrapper (stb_image + Y-flip)
│   └── stb_image.h                 # Vendored single-header image loader
├── gpu/
│   ├── gpu.h / .cpp                # Singleton GPU: full rendering pipeline API
│   ├── frameBuffer.h / .cpp        # Color buffer (RGBA) + depth buffer (float)
│   ├── bufferObject.h / .cpp       # Raw byte buffer wrapper (VBO/EBO)
│   ├── vao.h / .cpp                # Vertex Array Object (attribute bindings)
│   ├── shader.h / .cpp             # Abstract Shader base class
│   ├── shader/defaultShader.h/.cpp # Concrete shader with MVP uniforms
│   ├── dataStructures.h            # VsOutput, FsOutput, BindingDescription
│   ├── raster.h / .cpp             # Bresenham line + triangle rasterization
│   └── clipper.h / .cpp            # Sutherland-Hodgman clipping + face culling
├── math/
│   ├── math.h                      # Aggregates vector.h, matrix.h, mathFunctions.h
│   ├── vector.h                    # Templated Vector2/3/4
│   ├── matrix.h                    # Templated Matrix33/44 (column-major)
│   └── mathFunctions.h             # translate, scale, rotate, perspective, inverse, lerp, dot, cross, etc.
└── slint/                          # Pre-built Slint C++ SDK (used)
```

---

## Architecture

### UI Layer (`ui/app.slint` + `main.cpp`)

- **`AppWindow`** (Slint component): A simple window containing an `Image` element that displays the rendered framebuffer.
  - Exposes `rendered-image` property for C++ code to update.
  - `image-rendering: pixelated` preserves crisp edges for the software-rendered output.

- **`main.cpp`**: Standard `main()` entry point.
  1. `AppWindow::create()` — instantiate Slint window.
  2. `sgl->initSurface(WIDTH, HEIGHT)` — GPU allocates its own framebuffer.
  3. `prepare()` — allocate shader, VBOs, VAOs, EBO; set GPU state.
  4. `slint::Timer` (16 ms interval) — drives the render loop.
  5. `slint::run_event_loop()` — blocks until window is closed.

### Application Layer (`application/`)

- **`Image`**: Wraps `stb_image`. Loads files as 4-channel RGBA with Y-flip. No channel swapping is needed because the engine uses standard RGBA layout.

### GPU Layer (`gpu/`)

- **`GPU`** (singleton, accessed via `sgl` macro): The core software renderer. Its public API mirrors OpenGL:
  - `genBuffer()` / `bindBuffer()` / `bufferData()` — VBO/EBO management
  - `genVertexArray()` / `bindVertexArray()` / `vertexAttributePointer()` — VAO management
  - `useProgram(Shader*)` — bind shader
  - `enable()` / `disable()` — toggle `CULL_FACE`, `DEPTH_TEST`
  - `frontFace()` / `cullFace()` / `depthFunc()` — rasterizer state
  - `drawElement(drawMode, first, count)` — execute the full pipeline
  - `clear()` — clears color to black and depth to `1.0f`
  - `getFrameBuffer()` — access the current `FrameBuffer*` for copying to Slint

**Pipeline stages inside `drawElement()`:**
1. **Vertex Shader Stage** — run vertex shader for each indexed vertex.
2. **Clip Space** — Sutherland-Hodgman clipping against the view frustum.
3. **Perspective Division** — divide by `w`, enter NDC.
4. **Face Culling** — remove triangles based on winding order.
5. **Screen Mapping** — NDC `[-1,1]` → screen pixels via `screenMatrix`.
6. **Rasterization** — Bresenham lines or triangle bounding-box + edge tests.
7. **Perspective Recovery** — undo perspective distortion on interpolated attributes.
8. **Fragment Shader + Depth Test** — write to framebuffer if depth test passes.

- **`FrameBuffer`**: Holds `RGBA* mColorBuffer` and `float* mDepthBuffer`. The color buffer is self-allocated (no external canvas pointer).

- **`Shader`** (abstract): Defines `vertexShader(...)` → `VsOutput` and `fragmentShader(VsOutput, FsOutput&)`.

- **`DefaultShader`**: Reference implementation with `mModelMatrix`, `mViewMatrix`, `mProjectionMatrix` uniforms. Vertex shader applies MVP; fragment shader outputs interpolated color.

### Math Library (`math/`)

- Column-major matrix storage (`m[col * 4 + row]`).
- Vectors: `vec2f`, `vec3f`, `vec4f`.
- Matrices: `mat3f`, `mat4f`.
- Functions: `translate`, `scale`, `rotate`, `perspective`, `orthographic`, `screenMatrix`, `inverse`, `transpose`, `lerp`, `dot`, `cross`, `normalize`.

---

## Key Conventions and Patterns

1. **Singleton Macros**
   ```cpp
   #define sgl GPU::getInstance()
   ```
   The `app` macro was removed along with the Win32 `Application` class.

2. **OpenGL-Style API Naming**
   The GPU class deliberately mirrors OpenGL function names (`genBuffer`, `bindBuffer`, `drawElement`, etc.).

3. **Manual Memory Management**
   The code uses raw `new`/`delete` for singletons, shaders, buffers, and framebuffers. No smart pointers or RAII wrappers are used.

4. **Column-Major Matrices**
   Matrices are stored column-major. Transforms are multiplied right-to-left (e.g., `src * rotate` for local rotation).

5. **RGBA Color Buffer**
   The `RGBA` struct is laid out as `{ mR, mG, mB, mA }` (standard RGBA) to match Slint's `Rgba8Pixel` layout. The constructor takes arguments in `(r, g, b, a)` order. This allows zero-copy blitting from the GPU framebuffer into a `slint::SharedPixelBuffer<slint::Rgba8Pixel>`.

6. **Source File Encoding**
   Source files use **CRLF line endings** and contain **Chinese (GBK) comments**. When editing, preserving the existing encoding is recommended to avoid mojibake.

7. **No Tests / No CI**
   There are no unit tests, integration tests, or CI configuration files in this project.

8. **Real-Time Render Loop**
   `main.cpp` uses a `slint::Timer` firing every 16 ms to render continuously at ~60 FPS. Each frame copies the GPU framebuffer into a `slint::SharedPixelBuffer` and updates the Slint `Image` property.

---

## Main Entry Point (`main.cpp`)

The executable entry point is standard `main()`. The typical flow is:

1. `auto ui = AppWindow::create();` — create Slint window.
2. `sgl->initSurface(WIDTH, HEIGHT);` — GPU allocates internal framebuffer.
3. `prepare()` — allocate shader, VBOs, VAOs, EBO; set GPU state (`CULL_FACE`, `DEPTH_TEST`).
4. `slint::Timer` callback (every 16 ms):
   - `render()` — update MVP matrices, clear, bind VAO/EBO, `drawElement()`.
   - Copy `sgl->getFrameBuffer()->mColorBuffer` into `slint::SharedPixelBuffer`.
   - `ui->set_rendered_image(image);` — present frame via Slint.
5. `slint::run_event_loop();` — block until window closes.

---

## Security Considerations

- The project uses raw `new`/`delete` throughout. Memory leaks or use-after-free bugs are possible if ownership is not carefully tracked.
- Buffer sizes are passed manually to `bufferData()`. There is no bounds checking in the VBO/EBO access paths.
- `stb_image` is used to load arbitrary image files from disk; ensure input files are trusted.

---

## Notes for Agents

- **Slint is the UI framework** for this lesson. Do not re-introduce Win32 API calls.
- The `slint/` directory contains the pre-built SDK. Do not modify its contents.
- **Preserve Chinese comments** when editing existing files; add new comments in English or Chinese as appropriate, but match the existing style.
- **Keep changes minimal**. This is a tutorial codebase; each lesson is meant to be small and focused.
- When adding new shader classes, inherit from `Shader` and implement `vertexShader()` and `fragmentShader()`.
- When modifying the pipeline in `gpu.cpp`, keep the stage order consistent (vertex → clip → divide → cull → map → raster → recover → fragment → depth test → color write).
- If adding `#include`s for standard library headers (e.g., `<cstring>`, `<cstdint>`, `<algorithm>`), prefer adding them to `global/base.h` or directly to the file that needs them.
