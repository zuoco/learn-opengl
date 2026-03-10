# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A software rasterizer (软件光栅化器) written in C++20 that implements a 3D rendering pipeline from scratch. This lesson (11-Transform) focuses on 3D transformations: model/view/projection matrices and a rotating triangle demo.

## Build Commands

```bash
# Configure (from project root)
cmake -B build -S .

# Build
cmake --build build

# Run the renderer
./build/softRenderer

# Build and run the matrix test
cmake --build build --target testMatrix
./build/testMatrix
```

Requires: clang/clang++, CMake 3.21+, Freetype, Fontconfig, libinput, xkbcommon, libudev, gbm, libseat. Slint UI toolkit is bundled locally in `slint/`.

## Architecture

**Rendering pipeline flow:** `main.cpp` drives the render loop at ~60fps via a Slint timer. Each frame calls `transform()` (applies MVP + screen matrix) then `sgl->drawTriangle()`. The rendered framebuffer is copied into a Slint `SharedPixelBuffer` for display.

**Key modules:**

- **`gpu/`** — The software GPU. `GPU` is a singleton (accessed via `sgl` macro) that manages the framebuffer and drawing operations. `Raster` provides static rasterization algorithms (line via Bresenham-style, triangle via bounding-box scan). `FrameBuffer` holds the RGBA color buffer.
- **`math/`** — Custom linear algebra library in `math` namespace. Column-major matrices (`Matrix33`, `Matrix44`) and vectors (`Vector2`-`Vector4`). `mathFunctions.h` contains transform functions: `translate`, `rotate`, `scale`, `perspective`, `orthographic`, `screenMatrix`, `inverse`, and vector ops (dot, cross, normalize).
- **`application/`** — `Application` singleton wraps window dimensions and lifecycle. `Image` handles texture loading via stb_image.
- **`global/base.h`** — Foundational types: `RGBA` struct (BGRA byte order), `byte` typedef, texture wrap modes.
- **`ui/main.slint`** — Slint UI definition for the display window.

**Coordinate conventions:**
- Matrices are column-major (OpenGL convention). Element access: `m[col * 4 + row]`.
- `RGBA` struct stores bytes in BGRA order (mB, mG, mR, mA) but constructor takes (r, g, b, a).
- Framebuffer is Y-flipped when copying to Slint display buffer.

**Type aliases:** `math::vec2f`, `math::vec3f`, `math::vec4f`, `math::mat3f`, `math::mat4f`.
