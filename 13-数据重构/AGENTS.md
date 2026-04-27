# AGENTS.md — 项目指南

本文档供 AI 编程助手阅读，帮助其快速理解本项目结构、构建方式与开发约定。

---

## 1. 项目概述

本项目是一个**软件光栅化渲染器（Soft Renderer）**的 C++ 学习工程，属于"Learn OpenGL"系列教程的第 13 课（`13-数据重构`）。

当前课程的核心目标是：
- 通过**状态机模式**模拟 OpenGL 的 VBO（顶点缓冲对象）、VAO（顶点数组对象）和 EBO（索引缓冲对象）的**创建、绑定与数据上传**流程。
- 在 CPU 端实现一个简化版 GPU 状态管理与图元光栅化管线。

项目入口为 `main.cpp`，使用 Windows 原生 Win32 API 创建窗口，并通过内存位图（DIBSection）作为帧缓冲，最终通过 `BitBlt` 将渲染结果呈现到窗口。

> **注意**：项目根目录下附带了一份预编译的 **Slint C++ SDK**（`slint/`），但当前课程的源码（`main.cpp`、`application/`）**尚未集成 Slint**，仍基于原生 Win32 窗口系统。Slint 可能用于后续课程或跨平台分支。

---

## 2. 技术栈

| 层级 | 技术 |
|------|------|
| 语言 | C++17 |
| 构建系统 | CMake（最低版本 3.12） |
| 平台 API | Windows Win32 API（`wWinMain`、`WNDCLASSEXW`、`PeekMessage` 等） |
| 数学库 | 自研模板数学库（`math/`），含向量、矩阵、变换、插值 |
| 图像加载 | stb_image（单头文件库，`application/stb_image.h`） |
| UI 工具包（备用） | Slint C++ SDK v1.15.1（Linux x86_64，预编译二进制） |

---

## 3. 项目结构

```
13-数据重构/
├── CMakeLists.txt              # 根 CMake：定义工程、C++17、复制 assets、链接子模块
├── main.cpp                    # 程序入口：WinMain 消息循环 + VBO/VAO/EBO 准备逻辑
├── AGENTS.md                   # 本文件
│
├── application/                # 应用层：窗口与事件管理
│   ├── CMakeLists.txt          # 编译为静态库 applicationLib
│   ├── application.h / .cpp    # Application 单例：Win32 窗口创建、消息循环、DIBSection 画布
│   ├── image.h / .cpp          # Image 类，基于 stb_image 加载纹理（JPG/PNG 等）
│   └── stb_image.h             # stb_image 单头文件库
│
├── gpu/                        # 软件 GPU 层：模拟图形 API 与光栅化
│   ├── CMakeLists.txt          # 编译为静态库 gpuLib
│   ├── gpu.h / .cpp            # GPU 单例：genBuffer、bindBuffer、bufferData、genVertexArray 等 OpenGL 风格 API
│   ├── bufferObject.h / .cpp   # BufferObject：VBO/EBO 的底层字节缓冲区
│   ├── vao.h / .cpp            # VertexArrayObject：管理 binding -> (vboID, itemSize, stride, offset) 映射
│   ├── frameBuffer.h / .cpp    # FrameBuffer：封装 RGBA 颜色缓冲区
│   ├── raster.h / .cpp         # Raster：光栅化器（Bresenham 线段、三角形重心坐标插值）
│   └── dataStructures.h        # 核心数据结构：BindingDescription、VsOutput、FsOutput
│
├── math/                       # 自研数学库（头文件为主）
│   ├── math.h                  # 统一引入 vector、matrix、mathFunctions
│   ├── vector.h                # 模板 Vector2 / Vector3 / Vector4，含各类运算符重载
│   ├── matrix.h                # 模板 Matrix33 / Matrix44，列主序存储
│   └── mathFunctions.h         # lerp、dot、cross、normalize、transpose、inverse、scale、translate、rotate、perspective、screenMatrix 等
│
├── global/                     # 全局基础定义
│   └── base.h                  # 公共头文件、宏定义（PI、DEG2RAD、FRACTION）、RGBA 结构体、Buffer 类型常量
│
├── assets/textures/            # 纹理资源（goku.jpg、me.png）
│   └── ...
│
└── slint/                      # Slint C++ SDK（预编译第三方库，当前课程未使用）
    ├── bin/slint-compiler
    ├── include/slint/
    ├── lib/libslint_cpp.so
    └── CLAUDE.md               # Slint 使用说明
```

---

## 4. 构建与运行

### 4.1 构建命令

本项目使用标准 CMake 构建流程：

```bash
# 1. 创建并进入构建目录
mkdir build && cd build

# 2. 生成构建系统（Linux 下若使用 Slint 分支，可能需要指定 CMAKE_PREFIX_PATH）
cmake ..

# 3. 编译
make -j$(nproc)
```

编译成功后，生成可执行文件 `softRenderer`（Windows 下为 `softRenderer.exe`）。

### 4.2 资源复制

根 `CMakeLists.txt` 会在配置阶段自动将 `assets/` 目录复制到 `${CMAKE_BINARY_DIR}`，确保运行时能够加载纹理等资源。

### 4.3 运行环境

- **当前课程代码基于 Win32 API**，在 Windows 环境下可直接编译运行。
- `main.cpp` 的入口函数为 `wWinMain`，并使用了 `#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup")`。
- 若需在非 Windows 平台运行当前课程代码，需要将 `application/` 和 `main.cpp` 中的 Win32 相关逻辑替换为跨平台窗口框架（如 Slint、GLFW、SDL 等）。

---

## 5. 代码风格与约定

### 5.1 命名规范

- **类名**：大驼峰（PascalCase），如 `Application`、`VertexArrayObject`、`FrameBuffer`
- **成员变量**：以 `m` 为前缀的小驼峰，如 `mWidth`、`mColorBuffer`、`mBindingMap`
- **局部变量/参数**：小驼峰（camelCase），如 `bufferID`、`dataSize`
- **宏定义 / 常量**：全大写 + 下划线，如 `ARRAY_BUFFER`、`ELEMENT_ARRAY_BUFFER`、`PI`
- **类型别名**：使用 `using`，如 `vec2f`、`mat4f`

### 5.2 编码与换行

- **源文件编码**：项目历史源码（尤其是含中文注释的 `.cpp`、`.h`）使用 **GBK/GB2312** 编码（部分文件被识别为 ISO-8859），换行符为 **CRLF**。
- **新建文件建议**：为避免乱码问题，建议后续新增文件统一使用 **UTF-8 with BOM** 或 **UTF-8** 编码，并视情况转为 CRLF 以保持一致性。

### 5.3 架构约定

- **单例模式**：`Application` 和 `GPU` 均使用懒汉式单例，通过宏 `app` 和 `sgl` 提供全局访问：
  ```cpp
  #define app Application::getInstance()
  #define sgl GPU::getInstance()
  ```
- **OpenGL 风格 API**：`gpu` 模块刻意模仿 OpenGL 的状态机接口，如 `genBuffer()`、`bindBuffer(...)`、`bufferData(...)`、`vertexAttributePointer(...)`。
- **矩阵存储**：`math` 库中的矩阵采用**列主序**（column-major）存储，与 OpenGL 保持一致。
- **颜色空间**：
  - `VsOutput.mColor` 中的颜色分量范围为 `0.0 ~ 1.0`（float）。
  - `FsOutput.mColor`（`RGBA` 结构体）中的颜色分量范围为 `0 ~ 255`（byte），内存实际排列为 **BGRA**（与 Win32 DIBSection 一致）。

---

## 6. 测试策略

**当前项目没有单元测试或自动化测试框架。**

验证方式以**手动运行 + 视觉观察**为主：
1. 编译并运行 `softRenderer`。
2. 观察窗口输出是否符合预期（如三角形是否正确渲染、颜色插值是否正常）。
3. 通过 `sgl->printVAO(vao)` 等调试接口在控制台打印 GPU 状态。

如需引入测试，建议：
- 对 `math` 库中的向量/矩阵运算使用轻量级断言测试。
- 对 `gpu` 的状态管理逻辑可编写独立于窗口的单元测试。

---

## 7. 安全与注意事项

- **内存管理**：`BufferObject`、`FrameBuffer`、`VertexArrayObject` 等类内部使用裸指针（`new[] / delete[]`），未使用智能指针。修改时需注意内存泄漏与重复释放问题。
- **平台绑定**：`application` 层深度绑定 Win32 API（`Windows.h`、`HDC`、`HWND`、`HBITMAP` 等），修改渲染窗口逻辑时需注意平台隔离。
- **断言**：代码中大量使用 `assert(false)` 处理非法状态（如未绑定的 buffer、错误参数），在 Release 构建中可能被优化掉，需注意边界校验。
- **单例生命周期**：`Application` 和 `GPU` 单例目前**没有显式释放**，程序退出时依赖操作系统回收。

---

## 8. 关键数据流

1. `main.cpp` 在 `prepare()` 中创建 VBO、EBO、VAO，并上传顶点数据（位置、颜色、UV）。
2. `Application` 创建 Win32 窗口与 DIBSection，提供 `mCanvasBuffer` 作为外部帧缓冲。
3. `GPU::initSurface()` 接收该缓冲区，封装为 `FrameBuffer`。
4. 每帧循环中，`render()` 调用 `sgl->clear()` 清屏，未来课程将在此加入 `sgl->drawElements()` 等绘制调用。
5. `Application::show()` 通过 `BitBlt` 将 `mCanvasDC` 位图拷贝到窗口 DC，完成呈现。

---

## 9. 给 AI 助手的提示

- 修改 `gpu/` 或 `math/` 时，保持**列主序矩阵**与**OpenGL 风格 API** 的一致性。
- 新增文件若包含中文注释，请注意与现有文件编码兼容；推荐 UTF-8。
- 当前代码**无测试覆盖**，任何对核心类（`GPU`、`Raster`、`FrameBuffer`）的修改都应通过实际编译运行验证。
- 若需将项目移植到 Linux/macOS，优先替换 `application/` 层，保留 `gpu/` 和 `math/` 不变。
