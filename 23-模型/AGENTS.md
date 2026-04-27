# AGENTS.md — Project Guide for AI Coding Agents

> This file documents the architecture, build process, and conventions of the `softRenderer` project so that AI coding agents can work on it effectively.

---

## Project Overview

This is a **software rasterizer** (a CPU-based renderer) written in C++17. It is a learning project that implements a miniature GPU pipeline from scratch, mimicking the OpenGL-style API:

- Custom `GPU` class with VAO / VBO / EBO abstractions
- Programmable vertex and fragment shaders (C++ classes)
- Perspective projection, clipping, face culling, depth testing, alpha blending
- Texture mapping with filtering and wrap modes
- Model loading via Assimp (`.obj`, `.fbx`, `.glb`, etc.)
- **Slint UI** for cross-platform windowing and display

The current lesson (`23-模型`) focuses on loading hierarchical 3D models (meshes with node transforms, diffuse textures) and rendering them with a simple directional-light shader.

---

## Technology Stack

| Layer | Technology |
|-------|------------|
| Language | C++17 |
| Build System | CMake (minimum 3.21) |
| OS Target | **Linux** (x86_64) |
| Math | Custom header-only math library (`math/`) |
| Model Loading | System Assimp (`libassimp.so`) |
| Image Loading | stb_image (`application/stb_image.h`) |
| UI Toolkit | **Slint C++ SDK v1.15.1** (`slint/`) |

---

## Project Structure

```
.
├── CMakeLists.txt           # Root CMake: creates executable, links libs & Slint
├── CMakeSettings.json       # Visual Studio CMake presets (legacy, unused on Linux)
├── main.cpp                 # Entry point (main), game loop, scene setup
│
├── application/             # "Application" static library (applicationLib)
│   ├── application.{h,cpp}  # Singleton: input forwarding to Camera
│   ├── camera.{h,cpp}       # FPS-style camera (WASD + mouse look)
│   ├── model.{h,cpp}        # Assimp model loader → Mesh/VAO/Texture
│   ├── image.{h,cpp}        # Image wrapper around stb_image (RGBA)
│   └── stb_image.h          # Single-header image decoder
│
├── gpu/                     # "GPU" static library (gpuLib)
│   ├── gpu.{h,cpp}          # Singleton GPU: full software rendering pipeline
│   ├── shader.{h,cpp}       # Abstract Shader base class
│   ├── shader/              # Concrete shader programs
│   │   ├── defaultShader.h
│   │   ├── lightShader.{h,cpp}   # Directional light + diffuse texture
│   │   └── textureShader.h
│   ├── raster.{h,cpp}       # Triangle / line rasterization + interpolation
│   ├── clipper.{h,cpp}      # Clip-space clipping & face culling
│   ├── frameBuffer.{h,cpp}  # Color + depth buffers
│   ├── texture.{h,cpp}      # 2D texture sampler (nearest/linear, repeat/mirror)
│   ├── bufferObject.{h,cpp} # VBO / EBO raw memory storage
│   ├── vao.{h,cpp}          # Vertex Array Object (attribute bindings)
│   └── dataStructures.h     # VsOutput, FsOutput, BindingDescription, RGBA
│
├── math/                    # Custom math library (header-only)
│   ├── math.h               # Aggregates vector, matrix, mathFunctions
│   ├── vector.h             # Vector2/3/4 templates + type aliases
│   ├── matrix.h             # Matrix33/44 templates (column-major storage)
│   └── mathFunctions.h      # lerp, dot, cross, normalize, scale, translate, rotate, perspective, lookAt, inverse, screenMatrix
│
├── global/
│   └── base.h               # Common constants, macros, RGBA struct, key codes
│
├── ui/
│   └── app-window.slint     # Slint UI definition (Image + TouchArea + FocusScope)
│
├── assets/                  # Runtime assets (copied to build dir by CMake)
│   ├── model/               # .obj, .fbx, .glb models
│   └── textures/            # .jpg, .png textures
│
└── slint/                   # Pre-built Slint C++ SDK (Linux x86_64)
    ├── bin/slint-compiler
    ├── include/slint/
    ├── lib/libslint_cpp.so
    └── CLAUDE.md            # Slint usage notes (not project-specific)
```

---

## Build and Run

### Requirements
- Linux OS (x86_64)
- CMake ≥ 3.21
- Ninja
- GCC or Clang with C++17 support
- System Assimp (`libassimp.so`, usually via `dnf install assimp-devel` or `apt install libassimp-dev`)
- pkg-config

### Build Commands
```bash
cmake -B build -G Ninja
cmake --build build
```

The root `CMakeLists.txt` automatically copies `./assets` into the binary directory.

### Output
- Executable: `softRenderer`
- Linked libraries: `applicationLib`, `gpuLib`, `Slint::Slint`, `libassimp.so`

---

## Runtime Architecture

1. **main** (`main.cpp`) creates the `AppWindow` Slint UI and initializes the `GPU` singleton.
2. `sgl->initSurface(WIDTH, HEIGHT)` allocates the internal framebuffer (RGBA).
3. `prepare()` loads the 3D model via Assimp, creates `Camera`, and instantiates `LightShader`.
4. A `slint::Timer` drives the render loop at ~60fps:
   - Update camera (keyboard + mouse state)
   - Call `render()`:
     - Update model transform (rotate + translate + scale)
     - Set shader uniforms (MVP matrices, lights)
     - `sgl->clear()`
     - `model->draw(shader)` → binds VAO/EBO and calls `sgl->drawElement(...)`
   - Copy the GPU color buffer into a `slint::SharedPixelBuffer<slint::Rgba8Pixel>`
   - Update the Slint `render-image` property
5. `slint::run_event_loop()` processes window events and UI updates.

---

## Rendering Pipeline (GPU Class)

The `GPU::drawElement()` method executes the full software pipeline in this order:

1. **Vertex Shader Stage** — run user-provided `Shader::vertexShader()` for each indexed vertex
2. **Clip Space** — `Clipper::doClipSpace()` clips primitives against the view frustum
3. **Perspective Division** — divide by `w` to get NDC
4. **Face Culling** — optional back/front face culling
5. **Screen Mapping** — NDC → pixel coordinates via `screenMatrix`
6. **Rasterization** — `Raster::rasterize()` generates per-fragment `VsOutput`s (lines or triangles)
7. **Perspective Recover** — restore attributes divided by `w`
8. **Fragment Shader** — run `Shader::fragmentShader()` for each pixel
9. **Depth Test** — compare against depth buffer (`DEPTH_LESS` by default)
10. **Blending** — simple alpha blend if enabled
11. **Write to FrameBuffer** — store final `RGBA` into the color buffer

---

## Shader System

Shaders are **C++ classes** inheriting from `Shader`:

```cpp
class MyShader : public Shader {
public:
    VsOutput vertexShader(
        const std::map<uint32_t, BindingDescription>& bindingMap,
        const std::map<uint32_t, BufferObject*>& bufferMap,
        const uint32_t& index) override;

    void fragmentShader(
        const VsOutput& input,
        FsOutput& output,
        const std::map<uint32_t, Texture*>& textures) override;
};
```

Uniforms (model/view/projection matrices, lights, textures) are stored as **public member variables** on the shader object. The `GPU` class reads from / writes to these directly.

Existing shaders:
- `DefaultShader` — basic flat color
- `TextureShader` — textured fragments
- `LightShader` — directional light + ambient + diffuse texture

---

## Code Style and Conventions

- **Encoding:** Source files use **GB2312/GBK** encoding for Chinese comments. When reading files, expect `ISO-8859` / `ANSI` encoding rather than UTF-8.
- **Line endings:** CRLF (`\r\n`).
- **Naming:**
  - Member variables: `mCamelCase` (e.g., `mModelMatrix`, `mFrameBuffer`)
  - Classes: `PascalCase`
  - Functions: `camelCase`
  - Macros / constants: `UPPER_SNAKE_CASE` (e.g., `ARRAY_BUFFER`, `CULL_FACE`)
- **Singletons:**
  - `GPU::getInstance()` → macro `sgl`
  - `Application::getInstance()` → macro `APP`
- **Math storage:** Matrices are stored **column-major** (consistent with OpenGL conventions).
- **Color formats:**
  - Shaders use `0.0–1.0` float colors (`math::vec4f`)
  - Framebuffer uses `0–255` byte colors (`RGBA` struct, layout `RGBA` in memory)

---

## Asset Loading

- **Models:** Loaded via `Model::read(path)` using Assimp with flags `aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals`.
- **Textures:** Loaded via `stb_image` (flipped vertically, standard `RGBA` format).
- **Supported formats:** `.obj`, `.fbx`, `.glb` (anything Assimp supports).
- **Embedded textures:** Supported (e.g., textures packed inside `.fbx` or `.glb`).

---

## Input Handling

Input is captured through Slint UI callbacks and forwarded to the `Camera` via `Application`:

- **Keyboard:** `FocusScope` in `app-window.slint` captures `key-pressed` / `key-released`, mapped to WASD movement.
- **Mouse:** `TouchArea` captures `pointer-pressed` / `pointer-released` / `moved` for camera rotation (left button).

---

## Testing and CI/CD

- **No automated test suite** is present.
- **No CI/CD configuration** (no GitHub Actions, no `.gitlab-ci.yml`, etc.).
- Verification is manual: build and run the executable to inspect the rendered output.

---

## Security Considerations

- `stb_image` is used to decode user-supplied image files; ensure only trusted assets are loaded.
- Assimp parses complex 3D model files; keep the library updated to avoid parsing vulnerabilities.
- The project uses raw `new`/`delete` without smart pointers; memory-leak and use-after-free risks exist during refactoring.
- `assert()` is used for error handling in the math library and GPU state machine; builds with `NDEBUG` will silently skip these checks.

---

## Notes for AI Agents

1. **Do not assume OpenGL / GPU acceleration.** This is a pure CPU renderer. There are no GL handles, no VAOs in the driver sense, and no shader compilation.
2. **When adding features** (e.g., new shaders, new GPU capabilities), mirror the existing OpenGL-style API in `gpu/gpu.h` and update the pipeline stages in `gpu/gpu.cpp`.
3. **When modifying math**, remember the library is column-major and stores matrices as flat arrays indexed `m[col * 4 + row]`.
4. **When adding models / textures**, place them under `assets/`; CMake will copy them to the build directory automatically.
5. **The project is now Linux-only.** Do not re-introduce Win32 API or Windows-specific constructs.
6. **Slint UI changes** should be made in `ui/app-window.slint`. The C++ side receives callbacks via `ui->on_*` methods.
