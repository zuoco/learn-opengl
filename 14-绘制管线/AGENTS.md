# AGENTS.md

> 本文档面向 AI 编程助手。假设读者对该项目一无所知。

## 项目概览

本项目是一个基于 C++17 的**软件光栅化渲染器（Software Renderer / CPU Rasterizer）**，项目名称 `Graphic`。它从零开始模拟了类似 OpenGL 的 GPU 图形管线，包括顶点着色、透视除法、屏幕映射、光栅化、片元着色等完整阶段。

**重构后**：UI 使用 **Slint** 跨平台 GUI 框架，在 Linux 上运行，通过 Slint 的 `Image` 元素显示 CPU 渲染结果。

该项目是一个教学/学习性质的图形学工程，不依赖任何第三方图形 API（如 OpenGL、DirectX、Vulkan），所有绘制逻辑均在 CPU 端完成。

## 技术栈与平台

- **语言**：C++17
- **构建系统**：CMake（最低版本 3.12）
- **平台**：**Linux**（已重构为 Linux + Slint）
- **UI 框架**：Slint 1.15.1
- **第三方库**：
  - `stb_image.h`（单头文件图像加载库，用于读取纹理图片）
  - Slint C++ SDK（放在 `third_party/slint/`）
- **数学库**：项目内自建，非 GLM/Eigen 等外部库。

## 项目结构

```
.
├── CMakeLists.txt              # 根 CMake：定义工程、复制 assets、链接子目录、配置 Slint
├── main.cpp                    # 程序入口 main()，初始化 Slint UI、场景与渲染循环
├── ui/
│   └── appwindow.slint         # Slint UI 定义：窗口 + Image 元素显示渲染结果
├── application/                # 应用层：图像加载（已移除 Win32 窗口管理）
│   ├── application.h / .cpp    # Application 单例类（已简化，移除所有 Win32 代码）
│   ├── image.h / .cpp          # Image 类：基于 stb_image 的图片加载与释放
│   └── stb_image.h             # stb_image 单头库
├── gpu/                        # 软件 GPU 核心：模拟图形管线
│   ├── gpu.h / .cpp            # GPU 单例类（sgl）：顶点处理、管线调度、状态管理
│   ├── raster.h / .cpp         # Raster 静态类：直线（Bresenham）与三角形光栅化
│   ├── frameBuffer.h / .cpp    # FrameBuffer：托管颜色缓冲区（RGBA 像素数组）
│   ├── bufferObject.h / .cpp   # BufferObject：模拟 OpenGL VBO/EBO
│   ├── vao.h / .cpp            # VertexArrayObject：模拟 OpenGL VAO
│   ├── shader.h / .cpp         # Shader 抽象基类
│   ├── shader/                 # 具体着色器实现
│   │   └── defaultShader.h/.cpp
│   └── dataStructures.h        # 管线核心数据结构
├── math/                       # 自研数学库
│   ├── vector.h                # Vector2 / Vector3 / Vector4
│   ├── matrix.h                # Matrix33 / Matrix44（列主序）
│   ├── mathFunctions.h         # lerp、dot、cross、normalize 等
│   └── math.h                  # 聚合头文件
├── global/
│   └── base.h                  # RGBA、PI、DEG2RAD、缓冲区/绘制模式宏
├── assets/textures/            # 纹理资源目录
└── third_party/slint/          # Slint C++ SDK（lib、include、bin）
```

## 构建与运行

```bash
# 1. 创建构建目录
mkdir build && cd build

# 2. 生成构建系统（使用 Ninja 或默认 Make）
cmake ..

# 3. 编译
cmake --build .

# 4. 运行
./softRenderer
```

- 构建时会自动将 `assets/` 目录复制到构建输出目录。
- 生成的可执行文件名为 `softRenderer`。
- 需要 Linux 图形环境（X11 或 Wayland）。

## 代码组织与模块划分

### 1. application 模块
- `Application` 为**单例模式**，通过宏 `app` 访问。已移除所有 Win32/GDI 代码。
- 目前仅保留基础属性（宽/高），`Image` 类继续负责基于 `stb_image` 的图片加载。

### 2. gpu 模块
- `GPU` 为**单例模式**，通过宏 `sgl` 访问。
- 对外暴露类似 OpenGL ES 的 API：
  - `genBuffer` / `bindBuffer` / `bufferData`
  - `genVertexArray` / `bindVertexArray` / `vertexAttributePointer`
  - `useProgram` / `drawElement` / `clear`
- **渲染管线执行顺序**（在 `GPU::drawElement` 中）：
  1. **Vertex Shader Stage**
  2. **Perspective Division**
  3. **Screen Mapping**
  4. **Rasterization**
  5. **Fragment Shader Stage**
- `Raster` 提供静态方法：直线（Bresenham）与三角形光栅化（重心坐标插值）。

### 3. UI 与显示（Slint）
- `ui/appwindow.slint` 定义了一个包含 `Image` 元素的窗口。
- `main.cpp` 使用 `slint::Timer` 以约 60 FPS 驱动渲染循环。
- 每帧渲染完成后，将 `FrameBuffer` 的像素数据通过 `slint::SharedPixelBuffer<slint::Rgba8Pixel>` 包装为 `slint::Image`，并更新到 UI 属性 `render-image`。

### 4. math 模块
- 列主序（Column-Major）矩阵存储。
- 提供完整的 3D 变换矩阵：`scale`、`translate`、`rotate`、`perspective`、`orthographic`、`screenMatrix`。

### 5. global 模块
- `RGBA` 结构体（字节顺序：**R, G, B, A** —— 已调整为与 Slint 兼容的格式）。

## 着色器系统

- `Shader` 为抽象基类，子类需实现 `vertexShader(...)` 和 `fragmentShader(...)`。
- `DefaultShader` 演示经典的 MVP 变换 + 颜色透传。

## 开发约定与代码风格

- **命名风格**：
  - 类名：大写驼峰（`GPU`、`FrameBuffer`、`DefaultShader`）。
  - 成员变量：以 `m` 为前缀（`mWidth`、`mColorBuffer`、`mModelMatrix`）。
  - 函数名：大写驼峰（`initSurface`、`drawElement`、`vertexShader`）。
  - 类型别名：小写 + 后缀（`vec2f`、`mat4f`）。
- **单例模式**：`GPU` 与 `Application` 均为懒汉单例，通过宏 `sgl` / `app` 全局访问。
- **内存管理**：大量使用裸 `new`/`delete`，无智能指针。
- **错误处理**：使用 `assert(false)` 处理非法状态。

## 从 Windows 迁移到 Linux + Slint 的关键变更

1. **移除了所有 Win32 API 依赖**：删除了 `Windows.h`、GDI、HWND、HDC、消息循环等代码。
2. **入口函数**：由 `wWinMain` 改为标准 `int main(int argc, char** argv)`。
3. **渲染目标**：由 GDI `CreateDIBSection` 改为 `FrameBuffer` 内部自行分配 `RGBA` 数组。
4. **窗口与显示**：由 Win32 窗口 + `BitBlt` 改为 Slint `Window` + `Image` 元素。
5. **渲染循环**：由 `PeekMessage` 消息泵改为 `slint::Timer` 定期触发。
6. **像素格式**：`RGBA` 结构体从 `B, G, R, A`（Windows DIB 格式）改为 `R, G, B, A`（与 Slint `Rgba8Pixel` 兼容）。
7. **CMake 配置**：增加了 Slint SDK 路径、Slint 编译器调用、`slint_target_sources` 宏。

## 测试策略

- **本项目目前没有任何自动化测试**。
- 验证方式以**目视运行**为主：编译后运行 `./softRenderer`，观察 Slint 窗口中是否正确绘制旋转的彩色三角形。

## 安全与注意事项

- **平台相关**：当前目标为 Linux + Slint。若需在其他平台运行，需确保有对应的 Slint C++ SDK 和图形后端。
- **资源泄漏风险**：`GPU` 和 `Application` 析构函数已释放自身管理的 buffer/vao，但 `main.cpp` 中的 `shader` 指针需手动 `delete`。
- **无深度缓冲**：当前 `FrameBuffer` 只有颜色缓冲，**没有 Z-Buffer**。因此存在遮挡关系错误的风险。
- **无裁剪（Clipping）**：顶点在 NDC 空间外时，光栅化阶段没有做视口裁剪，可能导致数组越界或绘制异常。

## 扩展建议（供 AI 助手参考）

- 若用户要求添加**纹理采样**：可在 `DefaultShader::fragmentShader` 中使用 `input.mUV` 对 `Image` 纹理进行采样。项目已有 `Image::createImage` 和 `stb_image` 支持。
- 若用户要求添加**深度测试**：需在 `FrameBuffer` 中增加 `float* mDepthBuffer`，并在片元着色后比较深度值。
- 若用户要求添加**背面剔除**：可在 `Raster::rasterizeTriangle` 中根据叉积符号剔除。
- 若用户要求添加**更多绘制图元**：在 `Raster::rasterize` 中扩展新的 `drawMode` 分支即可。
