# AGENTS.md

本文件为 AI 编程助手提供项目背景、构建方式和代码组织信息。

---

## 项目概述

本项目是一个**软件光栅化渲染器（Software Renderer）**，使用 C++17 编写，通过 CMake 构建。项目模拟了 OpenGL 风格的 API，从零实现了完整的图形渲染管线，用于计算机图形学学习。

当前目录（`17-面剔除`）对应课程的第 17 节，核心内容是 **面剔除（Face Culling）**：
- 在 GPU 类中新增了 `enable(CULL_FACE)`、`frontFace()`、`cullFace()` 等状态设置接口；
- Clipper 中实现了基于 NDC 空间法向量的背面/正面剔除逻辑；
- Draw 流程中在裁剪后、屏幕映射前执行剔除。

### 运行平台

- **仅支持 Windows**：`application` 层直接使用 Win32 API（`WNDCLASSEXW`、`CreateWindowW`、`BitBlt` 等）创建窗口和双缓冲绘图；
- 入口函数为 `wWinMain`，子系统为 Console；
- 非跨平台项目，无法在 Linux / macOS 上直接编译运行。

### 第三方依赖

| 依赖 | 用途 | 说明 |
|------|------|------|
| stb_image | 图片加载 | 以源码形式放在 `application/stb_image.h`，支持 JPG/PNG 等 |
| Slint C++ SDK | UI 工具包 | 预编译库位于 `slint/`，**当前主项目未使用**，仅作为预留依赖 |

---

## 技术栈与构建方式

- **语言**：C++17
- **构建工具**：CMake（最低版本 3.12）
- **编译器**：MSVC（推荐，因使用 `Windows.h` 及 `#pragma comment`）

### 构建命令

```bash
# 在项目根目录执行
mkdir build && cd build
cmake ..
cmake --build .
```

构建产出：
- `softRenderer.exe`：主可执行文件
- `applicationLib.lib`：应用层静态库
- `gpuLib.lib`：渲染核心静态库

CMake 会自动将 `assets/` 目录复制到构建输出目录。

---

## 代码组织与模块划分

```
.
├── CMakeLists.txt          # 根 CMake：定义工程、C++标准、子目录、可执行文件
├── main.cpp                # 程序入口：WinMain、消息循环、prepare()、render()
│
├── application/            # 应用层：窗口系统 + 图像 IO
│   ├── application.h/cpp   # Win32 窗口封装（单例 Application）
│   ├── image.h/cpp         # 基于 stb_image 的图片加载/释放
│   └── stb_image.h         # stb_image 单头文件库
│
├── gpu/                    # 渲染核心：模拟 OpenGL 的软件 GPU
│   ├── gpu.h/cpp           # GPU 单例：状态机、绘制命令、渲染管线调度
│   ├── shader.h/cpp        # Shader 抽象基类，提供顶点属性读取工具
│   ├── shader/             # 具体 Shader 实现
│   │   ├── defaultShader.h/cpp   # 默认 Shader：MVP 变换 + 透传颜色
│   ├── dataStructures.h    # 核心数据结构：RGBA、VsOutput、FsOutput、BindingDescription
│   ├── frameBuffer.h/cpp   # 帧缓冲：包装 BGRA 像素内存
│   ├── bufferObject.h/cpp  # VBO / EBO 缓冲对象
│   ├── vao.h/cpp           # VAO：顶点属性绑定描述
│   ├── clipper.h/cpp       # 裁剪与剔除：Sutherland-Hodgman + 背面剔除
│   ├── raster.h/cpp        # 光栅化：Bresenham 画线、三角形重心/叉积填充
│   └── CMakeLists.txt      # gpu 子目录编译为 gpuLib
│
├── math/                   # 数学库： handmade，无外部依赖
│   ├── math.h              # 总入口，包含 vector/matrix/mathFunctions
│   ├── vector.h            # Vector2/3/4 模板类及常用运算
│   ├── matrix.h            # Matrix33/44 模板类及常用运算
│   └── mathFunctions.h     # lerp、dot、cross、normalize、inverse、scale/translate/rotate/perspective/screenMatrix 等
│
├── global/                 # 全局基础定义
│   └── base.h              # 常用宏、类型别名、常量（ARRAY_BUFFER、DRAW_TRIANGLES、CULL_FACE 等）
│
├── assets/                 # 资源文件
│   └── textures/           # 纹理图片（如 goku.jpg、me.png）
│
└── slint/                  # Slint UI C++ SDK 预编译库（当前未参与主构建）
    ├── bin/
    ├── include/slint/
    ├── lib/
    └── CLAUDE.md           # Slint 使用说明
```

---

## 渲染管线流程

`GPU::drawElement()` 内部按以下顺序执行：

1. **Vertex Shader Stage**：按 EBO 索引遍历顶点，调用 Shader 的 `vertexShader()`，输出裁剪空间坐标；
2. **Clip Space（裁剪空间裁剪）**：`Clipper::doClipSpace()` 使用 Sutherland-Hodgman 算法对三角形/线段进行 CVV 裁剪；
3. **NDC（透视除法）**：`GPU::perspectiveDivision()` 执行 `xyz /= w`，并做 `trim` 限制到 `[-1, 1]`；
4. **Cull Face（面剔除）**：若启用 `CULL_FACE`，`Clipper::cullFace()` 根据 frontFace（CCW/CW）和 cullFace（Back/Front）在 NDC 空间用法向量判断并剔除；
5. **Screen Mapping**：`GPU::screenMapping()` 使用 `screenMatrix` 将 NDC 转换到屏幕像素坐标；
6. **Rasterization**：`Raster::rasterize()` 对三角形做包围盒 + 叉积测试填充，对线做 Bresenham；
7. **Perspective Recover**：恢复透视正确的颜色/UV（除以 `1/w`）；
8. **Fragment Shader**：逐像素调用 `Shader::fragmentShader()`，写入 `FrameBuffer`。

---

## 开发约定与代码风格

- **注释语言**：核心算法和类说明使用**中文注释**，阅读代码时应注意中文编码；
- **命名风格**：
  - 类名：`PascalCase`（如 `FrameBuffer`、`DefaultShader`）
  - 成员变量：`m` 前缀 + `PascalCase`（如 `mFrameBuffer`、`mColorBuffer`）
  - 函数名：`camelCase`（如 `drawElement`、`perspectiveDivision`）
  - 宏/常量：全大写下划线（如 `DRAW_TRIANGLES`、`CULL_FACE`）
- **单例模式**：`Application` 和 `GPU` 均为单例，通过宏 `app` 和 `sgl` 全局访问；
- **内存管理**：手动 `new/delete`，无智能指针，注意在 `~GPU()` 中释放 Buffer/VAO；
- **OpenGL 风格 API**：有意模仿 OpenGL 状态机，如 `genBuffer`、`bindBuffer`、`bufferData`、`genVertexArray`、`useProgram`、`enable`/`disable`、`drawElement` 等；
- **矩阵存储**：列主序（Column-Major），与 OpenGL 一致；`math::mat4f` 通过 `setColum`/`getColum` 操作；
- **颜色空间**：
  - Shader 中颜色使用 `0.0~1.0` 浮点；
  - 最终输出 `FsOutput.mColor` 使用 `0~255` 的 `RGBA` 结构；
  - `FrameBuffer` 实际内存格式为 **BGRA**（与 Win32 `BI_RGB` DIB Section 一致）。

---

## 测试策略

- **当前项目无单元测试框架**，也无自动测试脚本；
- 验证方式以**视觉输出**为主：运行 `softRenderer.exe` 观察窗口中三角形的渲染结果；
- 若修改裁剪、光栅化或剔除逻辑，建议通过改变顶点顺序、相机角度、启用/禁用 `CULL_FACE` 来肉眼验证行为。

---

## 安全与注意事项

- **Windows 专用**：包含 `<Windows.h>` 并使用 `#pragma comment(linker, ...)`，不可直接移植；
- **手动内存管理**：多处使用裸指针 `new/delete`，修改时注意防止内存泄漏或重复释放；
- **assert 广泛使用**：`math` 库和 `GPU` 中对越界、非法状态使用 `assert(false)`，Release 构建可能直接崩溃；
- **无深度缓冲**：当前 FrameBuffer 仅含颜色缓冲，没有 Z-Buffer，存在像素覆盖问题；
- **DIB Section 格式**：Win32 位图实际为 BGRA，但 `RGBA` 结构体字段顺序为 `B, G, R, A`，与 Windows 一致，不要误改。

---

## 如何扩展

- **新增 Shader**：继承 `Shader`，实现 `vertexShader()` 和 `fragmentShader()`，在 `main.cpp` 的 `prepare()` 中 `useProgram()`；
- **新增渲染状态**：在 `global/base.h` 定义状态常量，在 `GPU` 中新增布尔/枚举成员，`enable/disable` 中切换；
- **新增图元模式**：目前支持 `DRAW_LINES` 和 `DRAW_TRIANGLES`，扩展需在 `Raster` 和 `Clipper` 中增加对应处理；
- **使用 Slint UI**：如需使用 `slint/` 中的 UI 库，参考 `slint/CLAUDE.md`，在根 `CMakeLists.txt` 中 `find_package(Slint)` 并链接。
