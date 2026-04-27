# AGENTS.md — 软渲染器（Soft Renderer）项目指南

本文档面向 AI 编程助手，帮助其快速理解本项目结构、技术栈与开发约定。

---

## 项目概述

本项目是一个**基于 CPU 的软渲染器（Software Renderer）**，使用 C++17 编写，CMake 构建。项目通过纯软件方式模拟 GPU 图形管线，包括顶点变换、裁剪、背面剔除、光栅化、片段着色、深度测试与混合等阶段。

项目名称为 `Graphic`，当前章节为**第 20 课：纹理（Texture）**，演示如何在软渲染器中实现纹理采样、过滤（Nearest / Linear）、环绕模式（Repeat / Mirror），以及片段着色器中的纹理颜色混合。

**注意**：项目已重构为**跨平台 Linux 版本**，UI 使用 **Slint C++ SDK** 显示渲染结果，不再依赖 Windows Win32 API。

---

## 技术栈

| 层级 | 技术 |
|------|------|
| 语言 | C++17 |
| 构建系统 | CMake（最低版本 3.12）|
| UI 框架 | Slint C++ SDK（v1.15.1，Linux x86_64，位于 `slint/`）|
| 图像加载 | stb_image（单头文件库，位于 `application/stb_image.h`）|
| 数学库 | 自研模板数学库（`math/` 目录）|

---

## 目录结构

```
.
├── CMakeLists.txt          # 根 CMake：定义项目、复制 assets、链接 Slint
├── main.cpp                # 程序入口：初始化 GPU/Slint、准备资源、Timer 渲染循环
├── ui/
│   └── app.slint           # Slint UI 定义：窗口 + Image 元素显示 framebuffer
├── assets/
│   └── textures/           # 纹理资源（goku.jpg、me.png 等）
├── global/
│   └── base.h              # 基础类型定义：RGBA（RGBA 顺序）、常量宏、buffer/绘制模式枚举
├── application/
│   ├── CMakeLists.txt      # 编译为 applicationLib 静态库
│   ├── application.h/.cpp  # Application 单例（简化版，保留接口兼容）
│   ├── image.h/.cpp        # Image 类：基于 stb_image 加载图片，自动翻转 Y 轴
│   └── stb_image.h         # stb_image 单头文件图像解码库
├── gpu/
│   ├── CMakeLists.txt      # 编译为 gpuLib 静态库
│   ├── gpu.h/.cpp          # GPU 单例：模拟 OpenGL 风格的软渲染 API 与完整管线
│   ├── dataStructures.h    # VsOutput、FsOutput、BindingDescription 结构体
│   ├── frameBuffer.h/.cpp  # FrameBuffer：颜色缓冲与深度缓冲管理
│   ├── bufferObject.h/.cpp # BufferObject：VBO/EBO 底层字节存储
│   ├── vao.h/.cpp          # VertexArrayObject：顶点属性绑定描述（VAO）
│   ├── texture.h/.cpp      # Texture：2D 纹理存储、采样、过滤与环绕
│   ├── shader.h/.cpp       # Shader 抽象基类，提供 getVector / vectorToRGBA 工具
│   ├── shader/
│   │   ├── defaultShader.h/.cpp   # 默认着色器：仅顶点色，无纹理
│   │   └── textureShader.h/.cpp   # 纹理着色器：支持 MVP 矩阵与 diffuse 纹理
│   ├── clipper.h/.cpp      # 裁剪器：Sutherland-Hodgman 算法 + 背面剔除
│   └── raster.h/.cpp       # 光栅器：Bresenham 画线 + 三角形扫描线光栅化
└── math/
    ├── math.h              # 统一引入 vector、matrix、mathFunctions
    ├── vector.h            # 模板 Vector2 / Vector3 / Vector4
    ├── matrix.h            # 模板 Matrix33 / Matrix44
    └── mathFunctions.h     # lerp、dot、cross、perspective、rotate、translate 等
```

---

## 构建与运行

### 构建命令

```bash
# 在项目根目录执行
mkdir build && cd build
cmake ..
cmake --build .
```

CMake 会自动将 `assets/` 目录复制到构建输出目录，并使用 `slint_target_sources` 将 `ui/app.slint` 编译为 C++ 头文件。

### 运行

构建产物为 `softRenderer`（Linux ELF 可执行文件），直接运行即可。默认窗口大小为 800×600，渲染一个带纹理的旋转三角形。

```bash
./softRenderer
```

---

## 软渲染管线流程

`GPU::drawElement()` 是核心绘制函数，内部按以下顺序执行：

1. **Vertex Shader Stage**：根据 EBO 索引遍历顶点，调用 Shader 的 `vertexShader()`。
2. **Clip Space**：使用 `Clipper::doClipSpace()` 对图元进行 Sutherland-Hodgman 裁剪。
3. **Perspective Division**：将裁剪坐标除以 W，转换到 NDC。
4. **Face Culling**：根据设置进行背面/正面剔除。
5. **Screen Mapping**：通过 `screenMatrix` 将 NDC 映射到屏幕像素坐标。
6. **Rasterization**：`Raster::rasterize()` 对三角形/线段进行光栅化，插值顶点属性。
7. **Perspective Recover**：将插值后的颜色、UV 等属性除以 `1/w` 恢复透视正确性。
8. **Fragment Shader & Output**：对每个像素调用 `fragmentShader()`，随后执行深度测试与 Alpha 混合，写入 FrameBuffer。

---

## Slint 集成与渲染循环

与原始 Win32 版本不同，本项目使用 **Slint UI 框架** 显示渲染结果：

1. **UI 定义**：`ui/app.slint` 定义了一个 `AppWindow`，包含一个全屏 `Image` 元素，其 `source` 绑定到 `framebuffer` 属性。
2. **帧缓冲传输**：在 `main.cpp` 中，每 16ms 的 `slint::Timer` 回调内：
   - 调用 `render()` 让 GPU 渲染到内部 FrameBuffer。
   - 将 FrameBuffer 的像素数据 **Y 轴翻转**（渲染器为 bottom-up，Slint 为 top-down）拷贝到 `slint::SharedPixelBuffer<slint::Rgba8Pixel>`。
   - 构造 `slint::Image` 并设置到 UI 的 `framebuffer` 属性。
3. **事件循环**：通过 `slint::run_event_loop()` 驱动窗口消息循环。

---

## 着色器系统

着色器采用继承自 `Shader` 基类的 C++ 类实现，而非运行时编译的 GPU Shader 代码。

- **Uniform 变量**：直接作为着色器类的 `public` 成员暴露（如 `mModelMatrix`、`mDiffuseTexture`）。
- **Attribute 读取**：基类提供 `getVector(bindingMap, bufferMap, location, index)`，按 VAO 绑定信息从 VBO 中提取顶点数据。
- **纹理采样**：`TextureShader` 在片段着色器中通过 `Texture::getColor(u, v)` 读取纹理颜色，支持 **Nearest** 与 **Linear** 过滤。

---

## 编码与代码风格

### 文件编码

- **源文件使用 GBK / ISO-8859 编码 + CRLF 换行符**，注释以中文为主。
- 若使用现代编辑器打开出现乱码，需手动切换编码为 GB18030 / GBK。

### 命名约定

- 类名：大驼峰（`Application`、`GPU`、`TextureShader`）。
- 成员变量：以 `m` 为前缀的小驼峰（`mWidth`、`mColorBuffer`、`mModelMatrix`）。
- 局部变量/参数：小驼峰（`width`、`vsOutputs`、`bindingMap`）。
- 宏常量：全大写下划线（`ARRAY_BUFFER`、`DRAW_TRIANGLES`、`TEXTURE_FILTER_LINEAR`）。
- 单例访问宏：`app`（Application）、`sgl`（GPU）。

### 内存管理

- 项目使用**原始指针 + 手动 `new/delete`**，未使用智能指针。
- `GPU` 析构函数负责清理 `FrameBuffer`、`BufferObject`、`VertexArrayObject`、`Texture` 等资源。
- 若新增资源类型，需在 `GPU` 析构函数中添加对应的清理逻辑。

---

## 数学库

位于 `math/` 目录，完全自研，均为模板实现：

| 类型 | 说明 |
|------|------|
| `math::vec2f / vec3f / vec4f` | 2D/3D/4D 浮点向量 |
| `math::mat3f / mat4f` | 3×3 / 4×4 浮点矩阵，列主序存储 |
| `math::lerp` | 重载的线性插值（标量、向量、RGBA、三顶点重心插值）|
| `math::perspective` | 透视投影矩阵 |
| `math::orthographic` | 正交投影矩阵 |
| `math::rotate / translate / scale` | 模型变换矩阵 |
| `math::inverse` | 4×4 矩阵求逆（伴随矩阵法）|
| `math::screenMatrix` | NDC → 屏幕空间映射矩阵 |

矩阵内部存储为 `T m[16]`，注释中给出列主序布局：
```
m0 m4 m8  m12
m1 m5 m9  m13
m2 m6 m10 m14
m3 m7 m11 m15
```

---

## 测试策略

- **本项目目前没有单元测试框架**（无 GoogleTest、Catch2 等）。
- 验证方式以**视觉输出**为主：运行程序观察窗口中三角形是否正确渲染、纹理是否正确映射、旋转是否流畅。
- 修改渲染管线后，建议通过以下方式验证：
  1. 编译并运行，确认无崩溃。
  2. 观察输出图像是否符合预期（颜色、深度、纹理坐标）。
  3. 检查控制台是否有 `Error: current vao is invalid!` 等错误输出。

---

## 添加新着色器的步骤

若需新增自定义 Shader（如法线贴图、多纹理混合）：

1. 在 `gpu/shader/` 下新建 `.h` / `.cpp` 文件，继承 `Shader`。
2. 重写 `vertexShader()` 与 `fragmentShader()`。
3. 将 Uniform 变量声明为 `public` 成员。
4. 在 `main.cpp`（或调用方）中 `new` 该 Shader 实例，设置 Uniform，调用 `sgl->useProgram()` 绑定。
5. 若新增纹理，使用 `sgl->genTexture()` / `sgl->bindTexture()` / `sgl->texImage2D()` 上传数据。

---

## 资源与资产

- 纹理图片存放于 `assets/textures/`，CMake 构建时会复制到输出目录。
- `Image::createImage(path)` 默认使用 `stbi_set_flip_vertically_on_load(true)` 将图片 Y 轴翻转，以配合渲染器左下角为原点的坐标系。
- 像素格式为 **RGBA**（与 Slint 的 `slint::Rgba8Pixel` 布局一致），不再使用 Windows DIB 的 BGRA 格式。

---

## 安全与注意事项

1. **Slint 依赖**：运行时需要能加载 `libslint_cpp.so` 以及 Qt6/X11/Wayland 等相关库。确保 `LD_LIBRARY_PATH` 包含 `slint/lib/` 或系统已安装对应依赖。
2. **编码问题**：中文注释使用 GBK 编码，在 UTF-8 默认环境下可能显示为乱码；修改时请保持编码一致，或考虑统一迁移到 UTF-8。
3. **无智能指针**：存在内存泄漏风险（如 `main.cpp` 中部分资源未显式释放）。新增代码时建议保持 `new/delete` 风格以统一项目风格，或引入 `std::unique_ptr` 等现代 C++ 机制。
4. **assert 使用**：多处使用 `assert(false)` 处理错误分支，Release 模式下断言被忽略，可能导致未定义行为。生产环境需增加错误码返回机制。
