# AGENTS.md — learn-opengl-slint 项目指南

> 本文档面向 AI 编程助手。假设读者对该项目一无所知。
> 本项目包含 23 个循序渐进的章节目录，每个章节都是一个可独立构建的 CMake 工程。

---

## 项目概览

本项目是一个基于 C++ 的**软件光栅化渲染器（Software Renderer / CPU Rasterizer）**，从零开始模拟类似 OpenGL ES 的 GPU 图形管线。项目名称来源于目录名 `learn-opengl-slint`，是一个教学/学习性质的图形学工程。

**核心特点：**
- 纯 CPU 渲染，不依赖 OpenGL、DirectX、Vulkan 等任何硬件图形 API
- 模拟 OpenGL 风格的 API：VAO、VBO、EBO、Shader、Texture、FrameBuffer
- 完整的图形管线实现：顶点着色 → 裁剪 → 透视除法 → 背面剔除 → 屏幕映射 → 光栅化 → 片元着色 → 深度测试 → 混合
- 使用 **Slint** 跨平台 UI 框架作为窗口和显示层（Linux 环境）
- 自研数学库（向量、矩阵、变换函数）

项目包含 **23 个章节**（`01-窗口` 到 `23-模型`），每章演示一个递增的图形学概念，从最基本的窗口创建一直推进到模型加载、光照计算和相机系统。

---

## 技术栈

| 层级 | 技术 |
|------|------|
| 语言 | C++17（早期章节如 01 使用 C++20） |
| 构建系统 | CMake（最低 3.12~3.21） |
| 目标平台 | **Linux**（x86_64） |
| UI 框架 | **Slint C++ SDK v1.15.1**（预编译，放在 `slint/` 或 `third_party/slint/`） |
| 数学库 | 项目内自建 header-only 库（`math/`） |
| 图像加载 | `stb_image.h`（单头文件，内置于 `application/`） |
| 模型加载 | 系统 Assimp（`libassimp.so`，仅第 23 章） |
| 编译器 | Clang/Clang++ 优先 |

**系统依赖（部分早期章节需要）：**
- `cmake`, `ninja`, `pkg-config`
- `freetype-devel`, `fontconfig-devel`
- `libinput-devel`, `libxkbcommon-devel`, `mesa-libgbm-devel`, `libseat-devel`
- `assimp-devel`（第 23 章）

---

## 项目结构

项目根目录下共有 23 个章节目录，命名如下：

```
01-窗口/                 # 基础窗口与 Slint 集成
02-视口/                 # 视口与帧缓冲概念
03-直线光栅化/           # Bresenham 直线算法
04-向量计算/             # 向量数学库
05-三角形光栅化-颜色插值/ # 三角形光栅化与颜色插值
06-混合-透明度/          # Alpha 混合
07-纹理/                 # 纹理映射
08-最临近插值-双线性插值/ # 纹理过滤
09-纹理寻址/             # 纹理环绕模式
10-矩阵/                 # 矩阵数学
11-空间变换/             # 3D 空间变换
12-顶点对象/              # 顶点数组/缓冲区对象抽象
13-数据重构/        # 数据结构重构
14-绘制管线/                 # 绘制管线
15-裁剪/                 # 视锥体裁剪
16-透视校正/ # 透视校正插值
17-面剔除/             # 背面剔除
18-深度测试/            # 深度测试（Z-Buffer）
19-混合/             # 混合管线完善
20-高级纹理/              # 高级纹理
21-相机/               # 相机系统
22-光照/                # 光照计算
23-模型/                # 模型加载（Assimp）
```

**每章内部目录结构（中后期章节，12~23）：**

```
<章节目录>/
├── CMakeLists.txt          # 根 CMake：定义可执行文件、链接子目录
├── main.cpp                # 程序入口：初始化 Slint UI、场景、渲染循环
├── ui/
│   └── app-window.slint    # Slint UI 定义（Image + TouchArea + FocusScope）
├── application/            # 应用层静态库（applicationLib）
│   ├── CMakeLists.txt
│   ├── application.h/cpp   # Application 单例：输入转发、相机绑定
│   ├── camera.h/cpp        # FPS 相机（WASD + 鼠标）— 21 章后
│   ├── model.h/cpp         # Assimp 模型加载 — 23 章
│   ├── image.h/cpp         # 基于 stb_image 的图片加载
│   └── stb_image.h         # 单头文件图像解码库
├── gpu/                    # 软件 GPU 静态库（gpuLib）
│   ├── CMakeLists.txt
│   ├── gpu.h/cpp           # GPU 单例：完整软件渲染管线
│   ├── shader.h/cpp        # Shader 抽象基类
│   ├── shader/             # 具体着色器实现
│   │   ├── defaultShader.h
│   │   ├── textureShader.h
│   │   └── lightShader.h/cpp
│   ├── raster.h/cpp        # 直线/三角形光栅化 + 插值
│   ├── clipper.h/cpp       # 裁剪与面剔除
│   ├── frameBuffer.h/cpp   # 颜色缓冲 + 深度缓冲
│   ├── texture.h/cpp       # 2D 纹理采样器
│   ├── bufferObject.h/cpp  # VBO / EBO 原始字节存储
│   ├── vao.h/cpp           # 顶点数组对象（属性绑定描述）
│   └── dataStructures.h    # VsOutput, FsOutput, BindingDescription
├── math/                   # 自研数学库（header-only）
│   ├── math.h              # 聚合头文件
│   ├── vector.h            # Vector2/3/4 模板
│   ├── matrix.h            # Matrix33/44 模板（列主序）
│   └── mathFunctions.h     # lerp, dot, cross, normalize, transform 等
├── global/
│   └── base.h              # RGBA、宏常量、缓冲区类型、按键码
├── assets/                 # 运行时资源（CMake 自动复制到构建目录）
│   ├── textures/           # 纹理图片（.jpg, .png）
│   └── model/              # 3D 模型（.obj, .fbx, .glb）— 23 章
└── slint/ 或 third_party/slint/  # 预编译 Slint C++ SDK
```

**早期章节（01~02）结构更简单：**
- 使用 `src/` 目录代替 `application/` 和 `gpu/`
- 没有子目录 CMake，所有源文件直接在根 `CMakeLists.txt` 中编译
- 依赖 Freetype、Fontconfig 及 Linux KMS 输入库

---

## 构建与运行

### 依赖安装（Fedora/RHEL 示例）

参考根目录下的 `依赖安装.md`：

```bash
sudo dnf install cmake clang pkg-config ninja-build
sudo dnf install freetype-devel fontconfig-devel
sudo dnf install libinput-devel libxkbcommon-devel systemd-devel gbm-devel libseat-devel
sudo dnf install assimp-devel  # 仅第 23 章需要
```

### 构建命令

每章都是独立的 CMake 工程，需进入对应章节目录后构建：

```bash
cd 23-模型  # 或任意章节

# 生成构建系统
cmake -B build -G Ninja

# 编译
cmake --build build

# 运行
./build/softRenderer
```

**注意：**
- 早期章节（01~02）可能需要显式指定编译器为 Clang，并在 CMake 中开启 `SLINT_FEATURE_BACKEND_LINUXKMS` 等选项（参考 `编译.md` 中的完整配置）
- 中后期章节（12~23）通常只需 `cmake -B build && cmake --build build`
- 构建时会自动将 `assets/` 目录复制到构建输出目录

### Slint SDK 配置

若章节目录中包含 `slint/` 子目录，CMake 会自动通过以下方式之一查找 Slint：

```cmake
set(Slint_DIR "${CMAKE_CURRENT_SOURCE_DIR}/slint/lib/cmake/Slint")
find_package(Slint REQUIRED)
# 或
set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/slint")
find_package(Slint REQUIRED)
```

---

## 运行时架构

1. **`main.cpp`** 创建 Slint UI 窗口（`AppWindow`），初始化 `GPU` 单例
2. `sgl->initSurface(WIDTH, HEIGHT)` 分配内部帧缓冲区（RGBA 数组）
3. `prepare()` 加载模型/纹理、创建相机、实例化 Shader
4. `slint::Timer` 以约 60 FPS 驱动渲染循环：
   - 更新相机（键盘 + 鼠标状态）
   - 调用 `render()`：设置 Shader Uniform → `sgl->clear()` → `sgl->drawElement(...)`
   - 将 `FrameBuffer` 的像素数据通过 `slint::SharedPixelBuffer<slint::Rgba8Pixel>` 包装为 `slint::Image`
   - 更新 Slint UI 的 `render-image` 属性
5. `slint::run_event_loop()` 处理窗口事件

---

## 渲染管线（GPU 类）

`GPU::drawElement()` 按以下顺序执行软件图形管线：

1. **顶点着色阶段** — 运行用户提供的 `Shader::vertexShader()`
2. **裁剪空间** — `Clipper::doClipSpace()` 对图元进行视锥体裁剪
3. **透视除法** — 除以 `w` 得到 NDC
4. **背面剔除** — 根据叉积符号剔除背面/正面（若开启）
5. **屏幕映射** — 通过 `screenMatrix` 将 NDC 转换为像素坐标
6. **光栅化** — `Raster::rasterize()` 生成逐片元数据
7. **透视恢复** — 恢复被 `w` 除过的属性
8. **片元着色** — 运行 `Shader::fragmentShader()`
9. **深度测试** — 与深度缓冲比较（默认 `DEPTH_LESS`）
10. **混合** — 若开启则进行 Alpha 混合
11. **写入帧缓冲** — 将最终 `RGBA` 写入颜色缓冲

---

## 代码风格与开发约定

- **注释语言**：源代码中的注释主要使用 **中文**（GB2312/GBK 编码），在 UTF-8 环境下可能显示为乱码
- **换行符**：CRLF（`\r\n`）
- **类名**：大写驼峰（`GPU`, `FrameBuffer`, `LightShader`）
- **成员变量**：以 `m` 为前缀（`mModelMatrix`, `mColorBuffer`, `mFrameBuffer`）
- **函数名**：大写驼峰（`initSurface`, `drawElement`, `vertexShader`）
- **宏 / 常量**：`UPPER_SNAKE_CASE`（`PI`, `DEG2RAD`, `ARRAY_BUFFER`, `CULL_FACE`）
- **类型别名**：小写 + 后缀（`vec2f`, `vec3f`, `mat4f`, `mat3f`）
- **单例模式**：
  - `GPU::getInstance()` → 宏 `sgl`
  - `Application::getInstance()` → 宏 `app` 或 `APP`
- **内存管理**：大量使用裸 `new` / `delete`，**没有使用智能指针**
- **错误处理**：使用 `assert(false)` 处理非法状态
- **头文件保护**：使用 `#pragma once`
- **矩阵存储**：**列主序**（Column-Major），与 OpenGL 一致，索引方式为 `m[col * 4 + row]`
- **颜色格式**：
  - Shader 中使用 `0.0~1.0` 的浮点颜色（`math::vec4f`）
  - 帧缓冲使用 `0~255` 字节颜色（`RGBA` 结构体，内存布局为 R, G, B, A）

---

## 测试策略

- **本项目没有任何自动化测试套件**。
- 没有使用 GoogleTest、Catch2 等测试框架。
- 没有 CI/CD 配置（无 `.github/workflows/`、`.gitlab-ci.yml` 等）。
- 验证方式为**目视运行**：编译后运行 `./softRenderer`，观察 Slint 窗口中的渲染输出是否正确。

---

## 安全与注意事项

- **平台相关**：当前目标平台为 **Linux + Slint**。后期章节已完全移除 Win32 API 代码，不要重新引入 Windows 专属依赖（如 `Windows.h`、GDI、`wWinMain` 等）。
- **内存泄漏风险**：`GPU` 和 `Application` 的析构函数会释放自身管理的 buffer/vao，但 `main.cpp` 中的 `shader`、`camera`、`model` 等指针需要手动 `delete`。
- **编码问题**：编辑源代码时，若编辑器仅支持 UTF-8，中文注释可能被破坏。建议保留现有注释，新增注释可用英文以避免编码问题。
- **资源安全**：`stb_image` 用于解码用户提供的图片文件；Assimp 用于解析复杂的 3D 模型文件。请确保只加载可信资源。
- **断言安全**：项目中使用 `assert()` 进行状态检查，在 `NDEBUG` 构建模式下这些检查会被静默跳过。

---

## 给 AI 助手的注意事项

1. **不要假设有 GPU 加速**：这是一个纯 CPU 渲染器，没有真正的 GL 句柄、驱动级 VAO 或着色器编译。
2. **添加新功能时**（如新 Shader、新 GPU 能力），请在 `gpu/gpu.h` 中模仿现有的 OpenGL 风格 API，并在 `gpu/gpu.cpp` 中更新对应的管线阶段。
3. **修改数学库时**：请记住矩阵是列主序的，且以扁平数组存储，索引为 `m[col * 4 + row]`。
4. **添加资源时**：将模型/纹理放入 `assets/` 目录下，CMake 会自动将其复制到构建目录。
5. **Slint UI 修改**：应在 `ui/app-window.slint` 中进行，C++ 侧通过 `ui->on_*` 方法接收回调。
6. **每章独立**：23 个章节目录是相互独立的 CMake 工程，修改某一章的代码不会影响其他章节。若需跨章节同步重构，需要手动应用到所有相关章节。
7. **章节演进**：后期章节（如 23-模型）的代码架构最为完善，可作为理解项目整体设计的最佳参考。
