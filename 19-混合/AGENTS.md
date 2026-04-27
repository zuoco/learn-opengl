# AGENTS.md — 软渲染器图形学习项目

> 本文档面向 AI 编程助手。如果你对这个项目一无所知，请先阅读本文档。

---

## 项目概述

本项目是一个**基于 C++17 的软件渲染器（Software Renderer / Soft Renderer）**，用于计算机图形学学习。它从零开始手写了一个简化版的 GPU 渲染管线，API 设计风格模仿 OpenGL。

当前目录 `19-混合` 对应课程第 19 节：**颜色混合（Alpha Blending）**。`main.cpp` 中演示了三个不同深度（Z 值）的三角形，其中两个带有半透明颜色（alpha < 1.0），展示了开启 `BLENDING` 后的颜色混合效果。

**技术栈：**
- 语言：C++17
- 构建工具：CMake（最低版本 3.12）
- UI 框架：Slint C++ SDK v1.15.1（Linux x86_64）
- 数学库：项目自研（`math/` 目录），未使用 GLM / Eigen 等第三方数学库
- 图像加载：`stb_image.h`（`application/` 中）

**平台：**
- **Linux x86_64**。使用 Slint 作为窗口系统，软渲染器的帧缓冲内容通过 `slint::Image` 显示在 Slint 的 `Image` 元素中。

---

## 项目结构

```
├── CMakeLists.txt              # 根 CMake：定义工程名 Graphic、C++17、子目录、最终可执行文件 softRenderer
├── main.cpp                    # 程序入口（标准 main）。初始化 GPU + Slint UI，准备 VAO/VBO/EBO/Shader，通过 slint::Timer 驱动渲染
├── ui.slint                    # Slint UI 定义：一个 800x600 的窗口，内含全屏 Image 用于显示软渲染器输出
├── global/
│   └── base.h                  # 全局基础头文件：类型别名、常量定义（RGBA、BUFFER 类型、DRAW 模式、CULL/DEPTH/BLEND 开关等）
├── application/
│   ├── CMakeLists.txt          # 编译为静态库 applicationLib（仅剩 image 模块）
│   ├── image.h / .cpp          # 图像封装类，基于 stb_image 加载图片（支持 jpg/png 等）
│   └── stb_image.h             # stb_image 单头文件库
├── gpu/
│   ├── CMakeLists.txt          # 编译为静态库 gpuLib
│   ├── gpu.h / .cpp            # 核心软渲染器（单例 GPU / sgl），实现完整渲染管线
│   ├── frameBuffer.h / .cpp    # 帧缓冲区：ColorBuffer（RGBA） + DepthBuffer（float）
│   ├── bufferObject.h / .cpp   # VBO / EBO 的底层数据存储
│   ├── vao.h / .cpp            # 顶点数组对象（VAO），管理 attribute 到 VBO 的绑定描述
│   ├── shader.h / .cpp         # Shader 抽象基类 + 工具函数（从 VBO 读取 attribute、vec4 转 RGBA）
│   ├── shader/
│   │   ├── defaultShader.h     # 默认着色器声明：实现 MVP 变换的顶点着色器 + 简单颜色输出的片元着色器
│   │   └── defaultShader.cpp   # 默认着色器实现
│   ├── dataStructures.h        # 渲染管线核心数据结构：BindingDescription、VsOutput、FsOutput
│   ├── raster.h / .cpp         # 光栅化器：Bresenham 直线、三角形包围盒 + 重心坐标插值
│   └── clipper.h / .cpp        # 裁剪器：Sutherland-Hodgman 裁剪 + 面剔除（Cull Face）
├── math/
│   ├── math.h                  # 数学库总头文件（包含 vector / matrix / mathFunctions）
│   ├── vector.h                # 模板化 Vector2 / Vector3 / Vector4，含常用运算、下标访问、打印
│   ├── matrix.h                # 模板化 Matrix33 / Matrix44，列主序存储，含矩阵乘法、逆矩阵、转置
│   └── mathFunctions.h         # 数学工具：lerp、dot、cross、length、normalize、scale/translate/rotate/perspective/orthographic/screenMatrix
├── assets/
│   └── textures/               # 纹理资源（goku.jpg、me.png），CMake 构建时会自动复制到输出目录
└── slint/
    ├── bin/slint-compiler      # Slint 编译器
    ├── include/slint/          # Slint C++ 头文件
    ├── lib/libslint_cpp.so     # Slint 共享库（Linux）
    ├── lib/cmake/Slint/        # CMake 包配置文件
    └── CLAUDE.md               # Slint 预编译库的说明文档
```

---

## 构建与运行

### 构建命令

项目使用 CMake 构建，建议在项目根目录执行：

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

构建产物：
- `softRenderer`（Linux 可执行文件）
- `applicationLib.a`（Application 层静态库）
- `gpuLib.a`（GPU 渲染核心静态库）

### 运行依赖

`softRenderer` 运行时依赖 `libslint_cpp.so`。CMake 构建时已通过 `RPATH` 设置使其能自动找到 `slint/lib/` 下的共享库，因此可直接运行：

```bash
./softRenderer
```

### 资源文件

根 `CMakeLists.txt` 会在构建时将 `./assets` 目录复制到 `${CMAKE_BINARY_DIR}`，因此运行时可执行文件需与 `assets` 同级目录，否则纹理加载可能失败。

---

## 渲染管线架构

`GPU::drawElement()` 实现了完整的固定功能 + 可编程着色器混合管线，执行顺序如下：

1. **顶点着色阶段（Vertex Shader Stage）**
   - 根据 EBO 索引，从 VAO/VBO 读取顶点属性
   - 调用用户自定义 `Shader::vertexShader()` 得到 `VsOutput`
2. **裁剪空间裁剪（Clip Space Clipping）**
   - `Clipper::doClipSpace()`：Sutherland-Hodgman 算法对三角形进行视锥体裁剪
3. **透视除法（Perspective Division）**
   - 将 clip space 坐标除以 `w`，转换到 NDC
4. **面剔除（Face Culling）**
   - 根据 `frontFace`（CW/CCW）和 `cullFace`（Front/Back）配置剔除背面或正面
5. **屏幕映射（Screen Mapping）**
   - 通过 `math::screenMatrix` 将 NDC `[-1,1]` 映射到屏幕像素坐标
6. **光栅化（Rasterization）**
   - `Raster::rasterize()`：对三角形使用包围盒 + 叉积判断像素是否在内部
   - 使用重心坐标对颜色、UV、`1/w`、深度进行插值
7. **透视恢复（Perspective Recover）**
   - 光栅化插值在屏幕空间进行，需将颜色、UV 除以 `1/w` 恢复到透视前的正确值
8. **片元着色阶段（Fragment Shader Stage）**
   - 调用 `Shader::fragmentShader()` 得到 `FsOutput`（像素坐标、深度、颜色）
9. **深度测试（Depth Test）**
   - 可选 `DEPTH_LESS` 或 `DEPTH_GREATER`，决定是否写入深度缓冲
10. **颜色混合（Blending）**
    - 若开启 `BLENDING`，执行 `GPU::blend()`：基于源颜色 alpha 的线性混合（`srcAlpha * src + (1-srcAlpha) * dst`）
11. **帧缓冲写入**
    - 最终颜色写入 `FrameBuffer::mColorBuffer`

### 显示到屏幕

与早期章节不同，本版本**不再使用 Win32 GDI**。渲染完成后，CPU 执行以下步骤：
- `main.cpp` 中的 `slint::Timer` 回调每 16ms 触发一次
- 调用 `render()` 完成上述完整管线
- 从 `sgl->getFrameBuffer()->getColorBuffer()` 读取像素
- 将 `RGBA`（内存布局 BGRA）的字段值逐个复制到 `slint::SharedPixelBuffer<slint::Rgba8Pixel>`
- 构造 `slint::Image` 并通过 `ui->set_framebuffer_image(image)` 更新 Slint UI
- Slint 的 `Image` 元素将最新帧缓冲显示在窗口中

### 单例访问宏

- `#define sgl GPU::getInstance()` — 全局 GPU 实例

---

## 代码组织与模块划分

| 模块 | 职责 | 关键类/文件 |
|------|------|-------------|
| `global` | 基础类型与常量 | `base.h`：RGBA、`byte`、管线状态宏 |
| `math` | 自研数学库 | `Vector2/3/4`、`Matrix33/44`、各种空间变换函数 |
| `application` | 图像加载 | `Image`（基于 stb_image 加载图片） |
| `gpu` | 渲染核心 | `GPU`、`FrameBuffer`、`BufferObject`、`VAO`、`Shader`、`Raster`、`Clipper` |
| `gpu/shader` | 着色器实现 | `DefaultShader`（MVP + 颜色传递） |

### 矩阵存储约定

- **列主序（Column-Major）**。`Matrix44` 内部数组 `m[16]` 的排列为：
  ```
  m0 m4 m8  m12
  m1 m5 m9  m13
  m2 m6 m10 m14
  m3 m7 m11 m15
  ```
- 矩阵乘法顺序与 OpenGL 一致：向量在右侧，`matrix * vector`。

### 颜色格式

- `RGBA` 结构体在内存中的顺序为 `mB, mG, mR, mA`（BGRA）。这是历史遗留（早期章节匹配 Win32 DIBSection）。
- 在向 Slint 的 `Rgba8Pixel` 转换时，按字段名读取（`mR`→`r`、`mG`→`g`、`mB`→`b`、`mA`→`a`），因此逻辑颜色通道正确，不受内存布局影响。

---

## 开发约定与代码风格

1. **注释语言**：代码注释以**中文**为主，夹杂少量英文术语（如 MVP、VAO、VBO）。新增代码建议保持中文注释风格，与现有代码一致。
2. **头文件保护**：使用 `#pragma once`。
3. **命名风格**：
   - 类名：`PascalCase`（如 `FrameBuffer`、`BufferObject`）
   - 成员变量：`m` 前缀 + `PascalCase`（如 `mColorBuffer`、`mModelMatrix`）
   - 局部变量 / 函数参数：`camelCase`
   - 宏定义 / 常量：全大写下划线（如 `ARRAY_BUFFER`、`DEPTH_LESS`）
   - 命名空间：`math`（全小写）
4. **模板化数学类型**：`Vector2<T>`、`Matrix44<T>` 等，项目中主要使用 `float` 特化（`vec2f`、`mat4f`）。
5. **内存管理**：使用原始 `new` / `delete`，未使用智能指针。单例对象在程序结束时不主动释放（依赖 OS 回收）。
6. **断言**：大量使用 `assert(false)` 处理非法状态或缺失的 attribute / buffer。

---

## 如何添加新功能

### 添加新的 Shader

1. 在 `gpu/shader/` 下新建继承自 `Shader` 的类。
2. 实现纯虚函数：
   - `VsOutput vertexShader(...)` — 顶点处理，必须输出 clip space 坐标到 `mPosition`。
   - `void fragmentShader(const VsOutput& input, FsOutput& output)` — 片元处理，必须填写 `mPixelPos`、`mDepth`、`mColor`。
3. 在 `main.cpp` 中实例化并使用 `sgl->useProgram(yourShader)`。

### 添加新的绘制模式或状态

- 若需扩展 `drawElement` 的 `drawMode`（目前仅支持 `DRAW_LINES` 和 `DRAW_TRIANGLES`），需在 `Raster::rasterize()` 中增加分支。
- 若需新增管线状态（如新的 depth func），在 `GPU` 中增加成员变量，并在 `enable()` / `disable()` / `depthFunc()` 等接口中处理。

### 添加纹理采样

当前 `DefaultShader` 已读取 UV 坐标并传入 `VsOutput::mUV`，但 `fragmentShader` 中尚未采样纹理。如需实现：
1. 在 `Shader` 子类中添加纹理 uniform（如 `Image*` 指针）。
2. 在 `fragmentShader` 中根据 UV 坐标从 `Image::mData` 读取颜色，与顶点颜色混合。

---

## 测试策略

**本项目目前没有单元测试、集成测试或自动化测试框架。**

验证渲染结果的主要方式是：
- **目视检查**：运行 `softRenderer`，观察窗口中三角形的颜色、深度顺序、混合效果是否正确。
- **对比预期**：课程每一节通常有明确的学习目标（如本节为 blending），通过调整 alpha 值、深度、绘制顺序来验证管线行为。

如果你需要引入测试，建议的方向：
- 对 `math/` 中的向量、矩阵运算添加单元测试（可使用 Catch2 / GoogleTest）。
- 对 `Raster`、`Clipper` 等纯算法模块，可编写离线测试，输入固定顶点数据，断言输出像素或裁剪结果。

---

## 安全与注意事项

1. **Slint 后端**：Slint 在 Linux 上默认可能使用 Skia/OpenGL 或软件渲染后端。如果在无显示服务器环境（如纯 SSH/容器）中运行，可能需要设置环境变量（如 `SLINT_BACKEND`）或使用 Xvfb。
2. **内存安全**：项目使用原始指针和手动内存管理。`BufferObject::setBufferData()` 仅在需要更大内存时才释放旧内存，否则直接覆盖；`FrameBuffer` 析构时需区分内部分配与外部传入的 buffer。
3. **深度精度**：深度缓冲使用 `float`，精度问题在远近平面距离较大时可能出现。
4. **混合公式**：当前混合使用简单的 `srcAlpha` 线性混合，未实现 OpenGL 中 `glBlendFunc` / `glBlendEquation` 的完整灵活性。如需更复杂的混合模式，需扩展 `GPU::blend()`。
5. **无多线程**：渲染管线、Slint 事件循环均为单线程，不存在并发安全问题。

---

## 常见问题

**Q: 为什么之前是 Windows 项目，现在变成 Linux 项目？**
A: 原项目源码为 Windows 设计（`learn-opengl-slint` 系列课程在 Windows 下录制/开发）。本目录已完成重构，移除了所有 Win32 API（`wWinMain`、`HWND`、`BitBlt` 等），改用标准 `main()` + Slint 跨平台 UI 框架，使其能在 Linux 上编译运行。

**Q: 如何调试渲染管线？**
A: 可在 `GPU::drawElement()` 的各个阶段（如裁剪后、光栅化后）插入断点或打印日志。`GPU::printVAO()` 可用于打印当前 VAO 的 attribute 绑定信息。

**Q: `main.cpp` 中的中文注释显示为乱码？**
A: 源代码文件使用 GBK / GB2312 编码存储中文注释。在支持 UTF-8 的编辑器中打开可能出现乱码，建议用支持 GBK 的编辑器查看，或统一转换为 UTF-8（但不影响编译，因为注释对编译器无意义）。
