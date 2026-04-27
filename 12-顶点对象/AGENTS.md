# AGENTS.md

> This file provides context for AI coding agents working on this repository.
> The agent reader is assumed to know nothing about the project.

---

## Project Overview

This is a **software renderer (softRenderer)** written in C++17. It is a tutorial-style project (part of a "Learn OpenGL" series, as indicated by the directory name `12-顶点对象`) that implements a software GPU from scratch to teach graphics pipeline fundamentals.

The project mimics OpenGL-style APIs:
- **VBO** (Vertex Buffer Objects)
- **VAO** (Vertex Array Objects)
- **Framebuffer**
- **Software rasterization** (line and triangle)

The current implementation stage focuses on:
- Refactoring `dataStructures`
- Adding math `lerp` (linear interpolation) and coordinate systems
- Implementing VBO
- Implementing VAO
- Refactoring the GPU module

---

## Technology Stack

| Component | Technology |
|-----------|------------|
| Language | C++17 |
| Build System | CMake (minimum 3.12) |
| Windowing | Win32 API (Windows GDI) |
| Math | Custom templated math library (vectors, matrices) |
| Image Loading | stb_image (header-only, bundled) |
| UI Toolkit (available, not yet integrated) | Slint C++ SDK v1.15.1 (Linux x86_64, pre-built) |

**Important:** The source code (`main.cpp`, `application/`) uses Win32 APIs (`Windows.h`, `wWinMain`, `HWND`, `HDC`, `DIBSection`) and MSVC-specific linker pragmas (`#pragma comment`). However, the working environment is Linux and a pre-built Linux Slint SDK is included. The code is currently Windows-targeted but being worked on in a Linux environment.

---

## Project Structure

```
.
├── CMakeLists.txt              # Root CMake: defines executable softRenderer, copies assets
├── main.cpp                    # Entry point: wWinMain, game loop
├── AGENTS.md                   # This file
│
├── application/                # Windowing & platform layer
│   ├── CMakeLists.txt          # Builds applicationLib (GLOB all .cpp)
│   ├── application.h/cpp       # Win32 window, message loop, bitmap canvas (singleton)
│   ├── image.h/cpp             # Image wrapper using stb_image
│   └── stb_image.h             # Third-party image loader
│
├── gpu/                        # Software GPU / rendering core
│   ├── CMakeLists.txt          # Builds gpuLib (GLOB all .cpp)
│   ├── gpu.h/cpp               # GPU singleton: surface init, clear, VBO/VAO management
│   ├── frameBuffer.h/cpp       # Color buffer wrapper (RGBA)
│   ├── bufferObject.h/cpp      # VBO / EBO raw memory buffer
│   ├── vao.h/cpp               # VertexArrayObject: binding descriptions
│   ├── dataStructures.h        # BindingDescription, VsOutput, FsOutput
│   └── raster.h/cpp            # Software rasterizer: Bresenham line, triangle fill
│
├── math/                       # Custom math library
│   ├── math.h                  # Aggregates vector.h, matrix.h, mathFunctions.h
│   ├── vector.h                # Templated Vector2/3/4 with full operator overloads
│   ├── matrix.h                # Templated Matrix33/44 (column-major)
│   └── mathFunctions.h         # lerp, dot, cross, normalize, transpose, inverse, transforms
│
├── global/                     # Shared base definitions
│   └── base.h                  # Common includes, RGBA struct, PI, macros, texture wrap modes
│
├── slint/                      # Pre-built Slint C++ SDK (Linux x86_64)
│   ├── bin/slint-compiler
│   ├── include/slint/
│   ├── lib/libslint_cpp.so
│   └── licenses/
│
└── assets/
    └── textures/               # Sample images (goku.jpg, me.png)
```

---

## Build Instructions

### Prerequisites
- CMake >= 3.12
- C++17 compiler
- **Windows SDK** (for the Win32/GDI code in `application/` and `main.cpp`)

### Build Commands
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

The root `CMakeLists.txt` will:
1. Copy the `assets/` directory into the build folder.
2. Compile `application/` into `applicationLib`.
3. Compile `gpu/` into `gpuLib`.
4. Compile `main.cpp` into executable `softRenderer` and link both libraries.

---

## Runtime Architecture

1. **Entry Point** (`main.cpp`):
   - Uses `wWinMain` (Unicode Windows entry).
   - Creates an `Application` singleton (`app`) and initializes a Win32 window.
   - Creates a `GPU` singleton (`sgl`) and binds it to the window's bitmap canvas.
   - Runs a frame loop: `peekMessage()` → `render()` → `show()`.

2. **Application Layer** (`application/`):
   - `Application` is a singleton that manages the Win32 window, device contexts (`HDC`), and a `DIBSection` bitmap.
   - The bitmap buffer (`mCanvasBuffer`) is shared with the GPU as the render target.
   - `show()` uses `BitBlt` to blit the canvas to the window.

3. **GPU Layer** (`gpu/`):
   - `GPU` is a singleton managing the rendering state.
   - `FrameBuffer` wraps the color buffer (RGBA array).
   - `BufferObject` is a raw byte array (VBO/EBO storage).
   - `VertexArrayObject` stores `BindingDescription` maps (how to interpret VBO data).
   - `Raster` provides static methods for line (Bresenham) and triangle rasterization with barycentric interpolation.

4. **Math Layer** (`math/`):
   - Custom templated vectors (`vec2f`, `vec3f`, `vec4f`, `vec2i`, `vec3i`, `vec4i`).
   - Custom templated matrices (`mat3f`, `mat4f`) in **column-major** order.
   - Transformation functions: `scale`, `translate`, `rotate`, `orthographic`, `perspective`, `screenMatrix`.

---

## Code Style and Conventions

- **Language:** Source comments are predominantly in **Chinese** (GBK/GB2312 encoding). Some may appear garbled in UTF-8 environments.
- **Naming:**
  - Classes: `PascalCase` (`Application`, `BufferObject`, `VertexArrayObject`)
  - Member variables: `m` prefix + `PascalCase/camelCase` (`mWidth`, `mColorBuffer`, `mBindingMap`)
  - Macros: `UPPER_SNAKE_CASE` (`PI`, `DEG2RAD`, `TEXTURE_WRAP_REPEAT`)
  - Type aliases: `lowercase` (`vec2f`, `mat4f`)
- **Singleton Pattern:** Both `Application` and `GPU` are singletons accessed via macros:
  - `app` → `Application::getInstance()`
  - `sgl` → `GPU::getInstance()`
- **Memory Management:** Raw `new`/`delete` is used throughout. No smart pointers.
- **Headers:** Use `#pragma once` for include guards.
- **Matrices:** Stored in **column-major** layout.
  - `Matrix44` indices: `m[col * 4 + row]`.
- **Color Format:** `RGBA` struct is defined as `{ byte mB, mG, mR, mA }` in memory to match the Win32 DIBSection BGRA format, but logically treated as RGBA.
- **Template Code:** The math library is fully header-based templated code.

---

## Testing

There is **no automated test suite** in this project. It is a graphics tutorial project where correctness is verified visually by running the executable and observing the rendered output.

To verify changes:
1. Build the project.
2. Run `softRenderer.exe` (on Windows).
3. Visually inspect the window output.

---

## Assets and Resources

- **Textures:** Located in `assets/textures/`.
  - `goku.jpg`
  - `me.png`
- The build system copies `assets/` into the CMake binary directory automatically.
- `stb_image` is used to load images. It flips images vertically on load (`stbi_set_flip_vertically_on_load(true)`) and swaps R/B channels to match the internal format.

---

## Notes for Agents

1. **Windows vs. Linux:** The core rendering code is platform-agnostic C++, but `main.cpp` and `application/` are tightly coupled to Win32. If asked to port or run on Linux, the `application/` module and `main.cpp` will need a cross-platform windowing replacement (e.g., GLFW, SDL, or the bundled Slint SDK).

2. **Encoding:** Source files contain Chinese comments in GBK encoding. Editing these files in a UTF-8-only editor may corrupt the comments. Prefer preserving existing comments or adding new ones in English to avoid encoding issues.

3. **No Package Manager:** Dependencies are either bundled (`stb_image.h`, `slint/`) or assumed to be system-provided (Win32 SDK). There is no `vcpkg.json`, `conanfile.txt`, etc.

4. **Slint SDK:** The `slint/` directory contains a pre-built Linux distribution of the Slint UI toolkit. It is **not currently linked** in the CMake build. To use it, you would need to:
   ```cmake
   set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/slint")
   find_package(Slint REQUIRED)
   target_link_libraries(softRenderer PRIVATE Slint::Slint)
   ```

5. **Math Library Completeness:** The math library is functional but minimal. It does not use SIMD or external libraries like GLM. All matrix/vector operations are implemented from scratch for educational purposes.

6. **Rasterizer:** The current rasterizer implements:
   - Bresenham's line algorithm (`rasterizeLine`)
   - Triangle rasterization via cross-product edge tests (`rasterizeTriangle`)
   - Barycentric interpolation for color and UV attributes

---
