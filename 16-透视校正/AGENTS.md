# AGENTS.md

本文件为 AI 编程助手提供项目背景、构建方式、代码风格与开发注意事项。阅读者应对本项目没有任何先验知识。

## 项目概述

本项目是一个基于 C++17 的**软件渲染器（Software Renderer）**，工程名为 `Graphic`。它从零开始模拟现代 GPU 的图形管线，完全依靠 CPU 计算完成顶点变换、视锥裁剪、光栅化和片段着色，最终通过 Win32 窗口显示结果。

当前目录 `16-透视校正` 是系列教程的第 16 课，核心目标是实现**透视校正插值（Perspective-Correct Interpolation）**：
- 在顶点着色后执行透视除法（Perspective Division），将颜色和 UV 等属性同步乘以 `1/w`。
- 光栅化阶段对 `1/w`、颜色、UV 进行插值。
- 片段着色前执行透视恢复（Perspective Recover），将属性除以插值后的 `1/w`，得到正确的透视校正结果。

**技术栈**
- 语言：C++17
- 构建工具：CMake 3.12+
- 窗口与显示：Win32 API（Windows 独占）
- 图像加载：stb_image（单头文件库，`application/stb_image.h`）
- 数学库：自研模板数学库（`math/` 目录）
- UI 框架：`slint/` 目录下预置了 Slint C++ SDK v1.15.1，但**当前工程并未在 CMake 中集成或使用**

## 项目结构

```
├── CMakeLists.txt              # 根 CMake：定义工程、C++ 标准、子目录、可执行目标
├── main.cpp                    # 程序入口（wWinMain），负责窗口初始化、渲染循环、场景数据准备
├── application/                # 应用层：Win32 窗口管理与图像加载
│   ├── CMakeLists.txt          # 编译 applicationLib
│   ├── application.h/.cpp      # Application 单例类，封装窗口注册、消息循环、双缓冲位图
│   ├── image.h/.cpp            # Image 类，基于 stb_image 加载 PNG/JPG 纹理
│   └── stb_image.h             # stb_image 单头文件库
├── gpu/                        # 软件渲染器核心（模拟 GPU）
│   ├── CMakeLists.txt          # 编译 gpuLib
│   ├── gpu.h/.cpp              # GPU 单例类，提供类似 OpenGL 的 API，编排完整渲染管线
│   ├── frameBuffer.h/.cpp      # 帧缓冲：管理颜色缓冲区（RGBA 数组）
│   ├── bufferObject.h/.cpp     # 缓冲区对象（VBO / EBO）
│   ├── vao.h/.cpp              # 顶点数组对象（VAO），管理 attribute 与 VBO 的绑定关系
│   ├── shader.h/.cpp           # Shader 抽象基类，提供按 attribute location 读取顶点数据的工具
│   ├── shader/
│   │   └── defaultShader.h/.cpp # 默认着色器：实现 MVP 变换，透传颜色与 UV
│   ├── raster.h/.cpp           # 光栅化器：Bresenham 线段、三角形包围盒扫描线填充
│   ├── clipper.h/.cpp          # 裁剪器：Sutherland-Hodgman 多边形裁剪算法
│   └── dataStructures.h        # 核心数据结构：VsOutput、FsOutput、BindingDescription、RGBA
├── math/                       # 自研模板数学库
│   ├── vector.h                # Vector2<T> / Vector3<T> / Vector4<T>
│   ├── matrix.h                # Matrix33<T> / Matrix44<T>
│   └── mathFunctions.h         # lerp、dot、cross、normalize、inverse、translate、rotate、scale、perspective、ortho、screenMatrix
├── global/
│   └── base.h                  # 全局头文件：基础类型别名、常量、宏（PI、DEG2RAD、ARRAY_BUFFER 等）
├── assets/textures/            # 纹理资源（goku.jpg、me.png），构建时自动复制到输出目录
└── slint/                      # Slint C++ SDK 预编译库（当前未接入构建）
```

## 构建与运行

### 构建命令

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### 构建说明

- 根 `CMakeLists.txt` 使用 `file(COPY ...)` 将 `assets` 目录复制到构建目录，确保运行时能加载纹理。
- 构建产物：
  - `applicationLib`：应用层静态库
  - `gpuLib`：渲染核心静态库
  - `softRenderer`：最终可执行文件
- **平台限制**：入口函数为 `wWinMain`，源码直接调用 Win32 API（`Windows.h`），目前只能在 Windows 平台（MSVC 或 MinGW）编译运行。

### 运行

直接运行 `softRenderer.exe`。程序会创建 800×600 的窗口，显示一个绕 Y 轴旋转的三角形，用于验证透视校正插值的正确性。

## 渲染管线流程

`GPU::drawElement` 是管线总入口，按以下顺序执行：

1. **顶点着色阶段（Vertex Shader Stage）**
   根据当前绑定的 EBO 索引，逐顶点调用 `Shader::vertexShader`，输出 `VsOutput`（含裁剪空间坐标、颜色、UV）。

2. **裁剪空间（Clip Space）**
   `Clipper::doClipSpace` 使用 Sutherland-Hodgman 算法，按视锥体平面（w>0、near、far、left、right、top、bottom）对图元进行裁剪。三角形被裁剪后可能产生新的多边形，再被扇形拆解（fan triangulation）为多个三角形。

3. **NDC 变换（Perspective Division）**
   `GPU::perspectiveDivision` 将顶点坐标 `xyz` 除以 `w`，同时将颜色、UV 乘以 `1/w`，为后续插值做准备。

4. **屏幕映射（Screen Mapping）**
   通过 `math::screenMatrix` 将 `[-1, 1]` 的 NDC 坐标转换到屏幕像素坐标 `[0, width-1]`、`[0, height-1]`。

5. **光栅化（Rasterization）**
   - 线段：`Raster::rasterizeLine` 使用 Bresenham 算法。
   - 三角形：`Raster::rasterizeTriangle` 使用包围盒扫描，通过二维叉积判断像素是否在三角形内，再用面积法（重心坐标）插值 `1/w`、深度、颜色、UV。

6. **透视恢复（Perspective Recover）**
   `GPU::perspectiveRecover` 将颜色和 UV 除以插值后的 `1/w`，恢复透视校正后的属性值。

7. **片段着色（Fragment Shader）**
   调用 `Shader::fragmentShader` 计算最终像素颜色与位置，直接写入 `FrameBuffer` 的颜色缓冲区。

8. **窗口呈现**
   `Application::show()` 通过 `BitBlt` 将内存位图拷贝到窗口 DC，完成一帧显示。

## 代码风格与开发约定

### 编码与注释
- 大量源文件使用 **GBK / GB2312** 编码，注释以中文为主。AI 助手在修改时应注意避免引入乱码；若新增中文注释，建议保持与现有文件一致的编码。

### 命名规范
- **类名**：大驼峰（`Application`、`DefaultShader`、`FrameBuffer`）
- **成员变量**：`m` 前缀 + 大驼峰（`mModelMatrix`、`mColorBuffer`、`mCurrentVAO`）
- **函数名**：小驼峰（`drawElement`、`perspectiveDivision`、`rasterizeTriangle`）
- **宏 / 全局常量**：全大写下划线（`ARRAY_BUFFER`、`ELEMENT_ARRAY_BUFFER`、`DRAW_TRIANGLES`）
- **类型别名**：模板类型使用 `f` / `i` 后缀（`vec3f`、`mat4f`、`vec2i`）

### 设计模式
- **单例模式**：`Application` 与 `GPU` 均为单例，分别通过宏 `app`、`sgl` 全局访问。
- **模板数学库**：向量与矩阵均为模板类，目前主要实例化为 `float` 类型。

### 矩阵与坐标系
- **列主序（column-major）**存储，与 OpenGL 一致。
- `math::perspective` 的 FOV 基于 Y 轴， near/far 为正值。
- `screenMatrix` 将 NDC `(-1,-1)` 映射到屏幕左下角像素 `(0,0)`。

### API 设计
`GPU` 类刻意模仿 OpenGL ES 风格：
- `genBuffer` / `deleteBuffer`
- `bindBuffer(ARRAY_BUFFER, id)` / `bindBuffer(ELEMENT_ARRAY_BUFFER, id)`
- `bufferData`
- `genVertexArray` / `bindVertexArray`
- `vertexAttributePointer(binding, itemSize, stride, offset)`
- `useProgram(shader)`
- `drawElement(drawMode, first, count)`

### 颜色格式
- `RGBA` 结构体在 C++ 代码中的字段顺序为 `mB, mG, mR, mA`，但实际逻辑读写按 `mR, mG, mB, mA` 处理。
- Win32 `BITMAPINFO` 使用 32bit `BI_RGB`，因此位图在 Windows 内存中的真实字节顺序为 `BGRA`。
- `Image::createImage` 在加载图片后会交换 R 与 B 通道，以匹配后续渲染管线的逻辑。

## 测试策略

- 本项目**没有自动化单元测试框架**（未引入 GoogleTest、Catch2 等）。
- 当前验证方式以**视觉输出**为主：运行程序后观察窗口中的渲染结果（三角形旋转、颜色渐变、UV 映射等）是否符合预期。
- 若后续需要添加测试，建议优先对 `math/` 库（向量运算、矩阵乘法、逆矩阵）和 `Raster`（光栅化插值）引入单元测试，因为这两部分最容易通过数值断言验证。

## 安全与内存管理注意事项

- 项目中普遍使用**裸指针**（`new` / `delete`）管理 `BufferObject`、`FrameBuffer`、`Shader`、`Image`、`Application`、`GPU` 等对象，尚未使用智能指针。
- `BufferObject::setBufferData` 在数据变大会释放旧内存并重新分配，但实现中先判断 `mBufferSize < dataSize` 再 `delete[]`，逻辑上安全。
- `FrameBuffer` 支持外部传入缓冲区指针（`mExternBuffer` 标记），析构时不会误删外部内存。
- 裁剪器 `Clipper` 使用 `assert(outputs.size() == 0)` 作为前置条件，Release 模式下若未清空输出向量可能导致未定义行为。

## 修改建议与常见陷阱

1. **不要直接修改 `slint/` 目录**：该目录为第三方预编译库，且当前构建系统未引用。如需集成 UI，应在根 `CMakeLists.txt` 中新增 `find_package(Slint)` 逻辑，而非修改库文件本身。
2. **保持管线阶段性完整**：本工程是系列教程的一课，每一课在上一个基础上增加新特性。修改渲染管线时，应确保 Vertex Shader → Clip → NDC → Screen Map → Raster → Perspective Recover → Fragment Shader 的链路仍然通顺。
3. **注意 `1/w` 的一致性**：新增需要在光栅化阶段插值的属性（如法线、切线）时，务必在 `perspectiveDivision` 中乘以 `mOneOverW`，在 `perspectiveRecover` 中除以 `mOneOverW`，否则会出现透视错误。
4. **Win32 平台限制**：若需跨平台，需将 `application/` 层替换为 SDL/GLFW 等跨平台窗口库，并修改 `main.cpp` 的入口函数。
