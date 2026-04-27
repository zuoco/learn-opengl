# AGENTS.md

本文档供 AI 编程助手阅读，帮助其快速理解本项目的结构、构建方式与开发惯例。

---

## 项目概览

本项目是一个基于 C++17 的**软件光栅化渲染器（Software Renderer）**，模拟了类似 OpenGL 的 GPU 图形管线。项目名称为 `Graphic`，当前阶段（`15-裁剪`）的核心目标是实现**裁剪（Clip）**功能，具体采用 **Sutherland-Hodgman 算法**对图元进行视锥体裁剪。

渲染器完整的管线阶段如下：
1. **顶点着色器阶段（Vertex Shader Stage）**
2. **裁剪空间阶段（Clip Space）** — Sutherland-Hodgman 算法，对 7 个裁剪平面（w>0, near, far, left, right, top, bottom）进行裁剪
3. **NDC 变换（Perspective Division）**
4. **屏幕映射（Screen Mapping）**
5. **光栅化（Rasterization）** — Bresenham 直线算法 + 三角形包围盒扫描线算法
6. **片元着色器阶段（Fragment Shader Stage）**

项目使用 **Win32 API** 创建窗口，并通过内存位图（DIBSection）将渲染结果输出到屏幕。图片加载依赖 `stb_image` 库。

> ⚠️ **平台限制**：本项目仅能在 **Windows** 平台上编译运行，因为入口函数为 `wWinMain`，并大量使用了 `Windows.h` 中的 API。

---

## 技术栈

- **语言**：C++17
- **构建工具**：CMake（最低版本 3.12）
- **窗口系统**：Win32 API
- **图片加载**：stb_image（单头文件库，位于 `application/stb_image.h`）
- **数学库**：自研模板数学库（Vector2/3/4、Matrix33/44、变换函数）

---

## 项目结构

```
15-裁剪/
├── CMakeLists.txt              # 根 CMake 配置，定义可执行文件 softRenderer
├── main.cpp                    # 程序入口（wWinMain），初始化应用与 GPU，主渲染循环
├── AGENTS.md                   # 本文档
│
├── application/                # 应用层：窗口管理、位图画布、图片加载
│   ├── CMakeLists.txt          # 编译为静态库 applicationLib
│   ├── application.h / .cpp    # Win32 窗口封装（单例），消息循环，DIBSection 画布管理
│   ├── image.h / .cpp          # 图片对象封装，基于 stb_image 加载文件
│   └── stb_image.h             # stb_image 单头文件库
│
├── gpu/                        # GPU 层：软件渲染管线核心实现
│   ├── CMakeLists.txt          # 编译为静态库 gpuLib
│   ├── gpu.h / .cpp            # GPU 单例，模拟 OpenGL 风格的 API（VBO/VAO/Shader/Draw）
│   ├── dataStructures.h        # 核心数据结构：VsOutput、FsOutput、BindingDescription、RGBA
│   ├── frameBuffer.h / .cpp    # 帧缓冲，管理像素颜色缓冲区
│   ├── bufferObject.h / .cpp   # 缓冲对象（VBO/EBO）
│   ├── vao.h / .cpp            # 顶点数组对象（VAO），管理顶点属性绑定描述
│   ├── shader.h / .cpp         # Shader 抽象基类，提供顶点/片元着色器接口与工具函数
│   ├── shader/                 # 具体着色器实现
│   │   ├── defaultShader.h / .cpp   # 默认着色器：实现 MVP 矩阵变换与基础颜色输出
│   ├── clipper.h / .cpp        # 裁剪器：Sutherland-Hodgman 算法实现
│   └── raster.h / .cpp         # 光栅器：直线与三角形光栅化，含颜色/UV 插值
│
├── math/                       # 数学库
│   ├── math.h                  # 统一头文件，包含 vector、matrix、mathFunctions
│   ├── vector.h                # 模板类 Vector2 / Vector3 / Vector4 及类型别名
│   ├── matrix.h                # 模板类 Matrix33 / Matrix44（列主序存储）
│   └── mathFunctions.h         # lerp、dot、cross、normalize、矩阵乘法/逆/转置、空间变换函数
│
├── global/                     # 全局基础定义
│   └── base.h                  # 常用头文件、宏、类型别名（byte、RGBA、BUFFER/DRAW 常量）
│
└── assets/
    └── textures/               # 纹理资源目录（构建时自动复制到输出目录）
```

---

## 构建与运行

### 构建命令

```bash
# 在项目根目录下创建构建目录并编译
mkdir build && cd build
cmake ..
cmake --build .
```

或（Release 模式）：

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### 构建产物

- `softRenderer.exe`：主可执行文件
- `applicationLib.lib`：应用层静态库
- `gpuLib.lib`：GPU 层静态库

### 资源复制

根目录 `CMakeLists.txt` 会在构建时自动将 `./assets` 目录复制到 `${CMAKE_BINARY_DIR}`，确保程序运行时能正确加载纹理等资源。

### 运行

直接在 Windows 环境下运行生成的 `softRenderer.exe` 即可。程序会创建一个 800×600 的窗口，显示一个旋转并移动的彩色三角形。

---

## 代码组织与核心模块

### 1. Application（应用层）

- **单例模式**，通过宏 `app` 访问。
- 负责注册/创建 Win32 窗口、消息泵（`peekMessage`）、DIBSection 位图画布管理。
- `getCanvas()` 返回位图内存缓冲区指针，供 GPU 直接写入像素。
- `show()` 通过 `BitBlt` 将画布内容贴到窗口 DC 上。

### 2. GPU（渲染核心）

- **单例模式**，通过宏 `sgl` 访问。
- 提供一套仿 OpenGL 的 API：
  - `genBuffer` / `bindBuffer` / `bufferData`
  - `genVertexArray` / `bindVertexArray` / `vertexAttributePointer`
  - `useProgram` / `drawElement`
- `drawElement` 内部按序执行：顶点着色 → 裁剪 → NDC → 屏幕映射 → 光栅化 → 片元着色。

### 3. Shader（着色器系统）

- `Shader` 为抽象基类，定义 `vertexShader` 与 `fragmentShader` 纯虚接口。
- `DefaultShader` 继承自 `Shader`，实现了经典的 **MVP（Model-View-Projection）** 变换。
- 顶点属性通过 `getVector` 工具函数按 binding 从 VBO 中读取。

### 4. Clipper（裁剪器）

- 入口 `doClipSpace` 根据 `drawMode`（`DRAW_TRIANGLES` 或 `DRAW_LINES`）将图元拆分为基本单元。
- 对每个图元调用 `sutherlandHodgman`，依次用 7 个裁剪平面进行裁剪。
- 三角形裁剪后通过**扇形分解（fan triangulation）**重新三角化。
- `intersect` 函数对裁剪交点进行**位置、颜色、UV 的线性插值**。

### 5. Raster（光栅器）

- **直线**：Bresenham 算法，支持 x/y 翻转与斜率修正，并对颜色/UV 进行线性插值。
- **三角形**：包围盒扫描 + 叉积判断点是否在三角形内，使用**重心坐标（面积比）**对颜色/UV 进行插值。

### 6. Math（数学库）

- 全部实现为**头文件模板库**，无编译单元。
- **列主序（Column-Major）**存储矩阵，下标布局如下：
  - Matrix33: `m0 m3 m6 / m1 m4 m7 / m2 m5 m8`
  - Matrix44: `m0 m4 m8 m12 / m1 m5 m9 m13 / m2 m6 m10 m14 / m3 m7 m11 m15`
- 提供常用变换：translate、rotate、scale、orthographic、perspective、screenMatrix。
- 提供矩阵乘法、转置、逆（4×4 伴随矩阵法）。

---

## 开发惯例与代码风格

- **注释语言**：源代码中的注释以**中文**为主。
- **命名风格**：
  - 类名：大驼峰（`BufferObject`、`DefaultShader`）
  - 成员变量：以 `m` 前缀 + 大驼峰（`mModelMatrix`、`mColorBuffer`）
  - 函数名：大驼峰（`genBuffer`、`drawElement`）
  - 类型别名：`vec2f`、`mat4f` 等
- **单例访问宏**：`#define app Application::getInstance()`、`#define sgl GPU::getInstance()`
- **颜色格式**：
  - `RGBA` 结构体在内存中的顺序为 B-G-R-A（因 Win32 `CreateDIBSection` 默认使用 `BI_RGB` 32bpp，实际存储为 BGRA）。
  - `stb_image` 加载后通过代码手动交换 R/B 通道以匹配。
  - 着色器中颜色以 0.0~1.0 的 float 表示，`vectorToRGBA` 负责乘以 255 转换。
- **矩阵乘法顺序**：在 `DefaultShader` 中，顶点变换顺序为 `Projection * View * Model * Position`。

---

## 测试策略

本项目**未集成任何自动化测试框架**，也没有单元测试或集成测试文件。验证渲染正确性的主要方式是：

1. **视觉验证**：运行程序，观察窗口中三角形的旋转、移动、颜色与裁剪行为是否符合预期。
2. **断点调试**：在 `GPU::drawElement` 及各管线阶段（`Clipper`、`Raster`）中设置断点，检查顶点数据与插值结果。

若需添加测试，建议引入轻量级 C++ 测试框架（如 doctest、Catch2），并对 `math` 库中的向量/矩阵运算、`Clipper` 的裁剪算法、`Raster` 的插值逻辑进行单元测试。

---

## 安全与注意事项

- **平台绑定**：代码深度依赖 Win32 API，不具备跨平台能力。如需移植到 Linux/macOS，需替换 `application` 模块中的窗口与位图实现（可考虑 SDL、GLFW 等跨平台库）。
- **内存管理**：使用原始 `new`/`delete` 管理单例及缓冲对象内存，无智能指针。`GPU` 析构时会释放 `FrameBuffer`、`BufferObject`、`VertexArrayObject`。
- **无深度缓冲**：当前 `FrameBuffer` 仅包含颜色缓冲，没有 Z-Buffer。因此存在**深度冲突（Z-fighting）**与**遮挡错误**的风险，后续阶段可能需要扩展。
- **硬编码参数**：窗口尺寸（800×600）、相机位置、旋转速度等参数在 `main.cpp` 中硬编码，未提供外部配置文件或命令行参数接口。
- **assert 使用**：多处使用 `assert(false)` 处理非法状态（如错误的 bufferType、找不到 binding/vbo），在 Release 模式下可能导致未定义行为，建议补充错误码或异常处理。

---

## 如何修改与扩展

### 添加新的着色器

1. 继承 `Shader` 基类，在 `gpu/shader/` 下新建头文件/实现文件。
2. 重写 `vertexShader` 与 `fragmentShader`。
3. 在 `main.cpp` 中实例化并使用 `sgl->useProgram(yourShader)`。

### 修改裁剪逻辑

- 编辑 `gpu/clipper.cpp`，重点关注 `sutherlandHodgman` 与 `inside`/`intersect` 函数。
- 裁剪平面定义在 `clipPlanes` 数组中，可按需增删。

### 修改光栅化行为

- 编辑 `gpu/raster.cpp`。
- `rasterizeTriangle` 使用叉积符号判断像素是否在三角形内，插值逻辑在 `interpolantTriangle` 中。

### 添加纹理采样

当前 `DefaultShader` 的 `fragmentShader` 仅输出纯色。`VsOutput` 已包含 `mUV` 字段，`Raster` 也会对 UV 进行插值。要启用纹理采样：
1. 在 `DefaultShader` 中传入 `Image` 纹理数据。
2. 在 `fragmentShader` 中根据插值后的 UV 采样纹理颜色并输出。
