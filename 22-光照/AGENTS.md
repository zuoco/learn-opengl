# AGENTS.md — softRenderer 项目指南

> 本文档面向 AI 编程助手。阅读本文档前，默认你对本项目一无所知。

---

## 项目概览

本项目是一个**基于 CPU 的软件光栅化渲染器（Software Renderer）**，使用 C++17 编写，目标是模拟 OpenGL 风格的图形渲染管线。它是《Learn OpenGL》风格教程的课程代码，当前对应第 22 课——光照（Light）。

项目核心能力包括：
- 完整的软件渲染管线：顶点着色 → 裁剪空间 → NDC → 面剔除 → 屏幕映射 → 光栅化 → 片段着色 → 深度测试 → 混合
- 基于 Sutherland-Hodgman 算法的裁剪
- 支持线框和三角形两种绘制模式
- 支持纹理采样（NEAREST / LINEAR、REPEAT / MIRROR）
- 简单的方向光（Directional Light）+ 环境光（Ambient Light）光照模型
- 第一人称相机（WASD + 鼠标视角控制）
- 通过 Slint UI 框架将渲染结果输出到窗口

**当前入口程序（`main.cpp`）渲染一个带贴图的立方体**，使用 `LightShader` 计算光照。

---

## 技术栈

| 层级 | 技术 |
|------|------|
| 语言 | C++17 |
| 构建系统 | CMake（最低版本 3.21） |
| 窗口/UI | Slint C++ SDK v1.15.1（`slint/` 目录下为预编译库） |
| 图像加载 | stb_image（`application/stb_image.h`） |
| 平台 | **Linux x86_64**（依赖 Slint 的 Linux 后端） |

---

## 项目结构

```
22-光照/
├── CMakeLists.txt              # 根 CMake：定义工程、C++17、子目录、可执行文件、Slint 集成
├── main.cpp                    # 程序入口：标准 main、游戏循环（Slint Timer）、场景准备、渲染调用
├── ui/
│   └── appwindow.slint         # Slint UI 定义：Image 显示渲染帧 + TouchArea/FocusScope 输入捕获
├── global/
│   └── base.h                  # 全局基础定义：RGBA、常量宏（BUFFER/DRAW/CULL/DEPTH/TEXTURE）、常用头文件
├── math/
│   ├── math.h                  # math 模块总入口
│   ├── vector.h                # 模板向量类：Vector2/3/4，含运算符重载
│   ├── matrix.h                # 模板矩阵类：Matrix33、Matrix44，列主序存储
│   └── mathFunctions.h         # 数学工具：lerp、dot、cross、normalize、矩阵运算、变换矩阵（translate/rotate/scale/perspective/lookAt/screenMatrix）
├── application/
│   ├── CMakeLists.txt          # 编译为 static library: applicationLib
│   ├── application.h/cpp       # 单例 Application：跨平台窗口抽象、输入事件转发给 Camera
│   ├── camera.h/cpp            # 第一人称相机：view / projection 矩阵、键盘鼠标输入处理
│   ├── image.h/cpp             # 图片封装：基于 stb_image 加载 JPG/PNG
│   └── stb_image.h             # 第三方头文件：图像解码
├── gpu/
│   ├── CMakeLists.txt          # 编译为 static library: gpuLib
│   ├── gpu.h/cpp               # 单例 GPU（宏 sgl）：整个软件渲染管线的核心调度器
│   ├── frameBuffer.h/cpp       # FrameBuffer：RGBA 颜色缓冲 + float 深度缓冲
│   ├── bufferObject.h/cpp      # VBO / EBO 的数据封装
│   ├── vao.h/cpp               # VertexArrayObject：管理顶点属性绑定描述（binding -> vbo/stride/offset）
│   ├── shader.h/cpp            # Shader 抽象基类：定义 vertexShader / fragmentShader 接口 + 工具函数
│   ├── shader/
│   │   ├── defaultShader.h/cpp # 默认纯色着色器
│   │   ├── textureShader.h/cpp # 贴图着色器
│   │   └── lightShader.h/cpp   # 方向光+贴图着色器（当前主场景使用）
│   ├── texture.h/cpp           # Texture：RGBA 贴图数据、采样、wrap/filter 参数
│   ├── raster.h/cpp            # Raster：静态方法，光栅化线/三角形（含属性插值）
│   ├── clipper.h/cpp           # Clipper：Sutherland-Hodgman 裁剪 + 面剔除判断
│   └── dataStructures.h        # 管线数据结构：BindingDescription、VsOutput、FsOutput
└── assets/
    └── textures/               # 贴图资源（goku.jpg、me.png），CMake 构建时自动复制到输出目录
```

---

## 构建与运行

### 构建命令（CMake）

```bash
# 在项目根目录执行
mkdir build && cd build
cmake ..
cmake --build .
```

构建产物：
- `applicationLib`（静态库）
- `gpuLib`（静态库）
- `softRenderer`（主程序，链接上述两个库 + Slint）

### 资源复制

根目录 `CMakeLists.txt` 在配置阶段会自动将 `./assets` 复制到 `${CMAKE_BINARY_DIR}`，确保 exe 运行时能读取到贴图文件。

### 运行要求

需要在 Linux 图形环境下运行（X11 或 Wayland）。Slint 会依赖系统的窗口管理器和图形后端。

---

## 架构详解

### 1. 应用层（Application）

`Application` 为单例（宏 `app`），负责：
- 存储窗口尺寸
- 将键盘/鼠标事件转发给 `Camera`

窗口创建和显示完全由 Slint 管理。`main.cpp` 中创建 `AppWindow` Slint 组件，并通过 `slint::Timer` 驱动渲染循环。

### 2. UI 层（Slint）

`ui/appwindow.slint` 定义了主窗口：
- `Image` 元素占据整个窗口，显示每帧渲染结果
- `TouchArea` 捕获鼠标事件（右键拖拽旋转相机）
- `FocusScope` 捕获键盘事件（WASD 移动）
- 通过 `callback` 将输入事件暴露给 C++ 端

### 3. 渲染层（GPU）

`GPU` 为单例（宏 `sgl`），是软件渲染器的核心。对外暴露的 API **刻意模仿 OpenGL ES 风格**：
- `genBuffer` / `bindBuffer` / `bufferData`
- `genVertexArray` / `bindVertexArray` / `vertexAttributePointer`
- `genTexture` / `bindTexture` / `texImage2D` / `texParameter`
- `useProgram`
- `enable` / `disable`（`CULL_FACE`、`DEPTH_TEST`、`BLENDING`）
- `drawElement`

内部渲染管线（`drawElement` 的执行顺序）：

1. **Vertex Shader Stage**：遍历 EBO 索引，逐顶点调用当前 Shader 的 `vertexShader()`
2. **Clip Space**：调用 `Clipper::doClipSpace`，使用 Sutherland-Hodgman 算法对图元进行 CVV 裁剪
3. **NDC（Perspective Division）**：`perspectiveDivision()` 执行透视除法（除以 `w`），并 trim 到 `[-1, 1]`
4. **Face Culling**：若启用，调用 `Clipper::cullFace` 根据前向面和剔除面设置剔除背面或正面
5. **Screen Mapping**：通过 `screenMatrix` 将 NDC 坐标转换到屏幕像素坐标
6. **Rasterization**：`Raster::rasterize` 将图元离散化为片段（支持线段和三角形扫描线填充，含重心坐标插值）
7. **Perspective Recover**：将光栅化插值后的属性除以 `1/w` 恢复正确透视
8. **Fragment Shader**：逐片段调用 `fragmentShader()`
9. **Depth Test & Blending**：根据 `DEPTH_LESS` / `DEPTH_GREATER` 进行深度测试；若开启混合，执行基于 Alpha 的混合

### 4. 着色器系统（Shader）

`Shader` 为抽象基类，子类通过重写两个纯虚函数实现自定义材质：
- `vertexShader(bindingMap, bufferMap, index) -> VsOutput`
- `fragmentShader(VsOutput, FsOutput&, textureMap)`

当前内置三种着色器：
- `DefaultShader`：纯色输出
- `TextureShader`：仅贴图采样
- `LightShader`：**方向光 + 漫反射贴图 + 环境光**。Uniform 包括 `mModelMatrix`、`mViewMatrix`、`mProjectionMatrix`、`mDiffuseTexture`、`mDirectionalLight`、`mEnvLight`

### 5. 数学库（math）

自定义模板数学库，全部位于 `math` 命名空间：
- **列主序（Column-Major）**矩阵存储：`m[col * 4 + row]`
- 矩阵乘法采用“列向量左乘”逻辑（与 OpenGL 一致）
- 提供的变换矩阵：`translate`、`scale`、`rotate`、`perspective`、`orthographic`、`lookAt`、`screenMatrix`
- 类型别名：`vec2f`/`vec3f`/`vec4f`、`mat3f`/`mat4f`

### 6. 相机（Camera）

- 使用 `perspective` 投影 + `lookAt` 风格的 view 矩阵
- 支持 WASD 移动、鼠标右键拖拽旋转（pitch/yaw）
- `update()` 每帧根据移动状态和朝向重新计算 `mViewMatrix`

---

## 代码风格与约定

### 命名规范
- 类名：`PascalCase`（如 `LightShader`、`BufferObject`）
- 成员变量：以 `m` 为前缀 + `PascalCase`（如 `mModelMatrix`、`mWidth`）
- 函数名：`camelCase`（如 `getViewMatrix`、`drawElement`）
- 宏定义：全大写下划线（如 `ARRAY_BUFFER`、`DEPTH_TEST`）

### 代码注释
- **源文件中的注释以中文为主**（课程讲解性质）。AI 助手在修改代码时，新增注释可继续使用中文或英文，但应保持与周围代码一致。
- 头文件中的接口说明通常较为简洁。

### 关键编程约定
1. **单例模式**：`Application` 和 `GPU` 均为单例，通过宏 `app` 和 `sgl` 全局访问。不要在堆上重复创建实例。
2. **RGB 像素顺序**：`RGBA` 结构体在内存中的顺序为 `B-G-R-A`，但构造参数顺序为 `(R, G, B, A)`。
3. **颜色空间**：
   - Shader 内部计算使用 **0.0~1.0** 的浮点颜色
   - `FsOutput.mColor` 和 `FrameBuffer` 使用 **0~255** 的字节颜色
   - 转换工具：`Shader::vectorToRGBA()`
4. **资源管理**：`GPU` 析构时会清理所有 `BufferObject`、`VertexArrayObject`、`Texture`；`main.cpp` 中也需要手动 `delete shader`、`delete camera` 等。
5. **无异常处理**：代码使用 `assert()` 进行参数合法性校验，不抛异常。

---

## 测试策略

**当前项目没有任何自动化测试。** 验证方式仅为：
- 编译成功
- 运行 `softRenderer` 后窗口正确显示带光照的贴图立方体
- WASD 和鼠标可正常控制相机

如果你要添加测试，建议方向：
- 为 `math` 库添加单元测试（向量/矩阵运算、变换矩阵正确性）
- 为 `Clipper` 和 `Raster` 添加基于已知输入输出的几何测试
- 使用软件帧缓冲截取（不依赖 GUI 窗口）进行像素级回归测试

---

## 安全与注意事项

1. **平台绑定**：代码现在依赖 Slint UI 框架的 Linux 后端。若需移植到其他平台，需要确保 Slint SDK 和目标平台兼容。
2. **内存管理**：项目中存在原始指针和手动 `new/delete`（如 `Camera*`、`LightShader*`、`Image*`），注意避免内存泄漏。
3. **无多线程**：渲染管线和游戏循环均运行在单线程（Slint 主事件循环线程），无需考虑线程安全。
4. **stb_image**：`application/stb_image.h` 为第三方头文件，修改前请确认是否需要同步更新。
5. **颜色格式转换**：GPU 内部使用 BGRA 格式，但 Slint 的 `Image` 需要 RGBA 格式。`main.cpp` 中的 `createSlintImageFromFrameBuffer` 每帧执行此转换。

---

## 常见问题（FAQ）

**Q: `slint/` 目录下的库是干什么的？**
> A: 这是项目使用的 Slint C++ SDK（v1.15.1，Linux x86_64）预编译库。CMake 配置中通过 `CMAKE_PREFIX_PATH` 指向 `slint/lib/cmake/Slint` 来集成。`bin/slint-compiler` 负责将 `.slint` 文件编译为 C++ 头文件。

**Q: 为什么键盘事件需要点击窗口后才能生效？**
> A: Slint 的 `FocusScope` 需要在获得焦点后才能接收键盘事件。程序启动时窗口可能没有自动获得焦点，点击一下窗口即可。后续可在 `.slint` 文件中使用 `forward-focus` 等机制优化。

**Q: 如何添加一个新的 Shader？**
> A: 继承 `Shader` 类，在 `gpu/shader/` 下新增 `.h/.cpp` 文件，重写 `vertexShader` 和 `fragmentShader`，然后在 `main.cpp` 中 `new` 并使用 `sgl->useProgram()` 激活即可。`gpu/CMakeLists.txt` 使用 `GLOB_RECURSE` 自动收集所有 `.cpp`，无需手动添加。

---

## 关键文件速查

| 目标 | 文件 |
|------|------|
| 程序入口 | `main.cpp` |
| Slint UI 定义 | `ui/appwindow.slint` |
| 全局宏/常量 | `global/base.h` |
| 渲染核心调度 | `gpu/gpu.h`、`gpu/gpu.cpp` |
| 着色器基类 | `gpu/shader.h` |
| 当前使用的着色器 | `gpu/shader/lightShader.h`、`gpu/shader/lightShader.cpp` |
| 光栅化实现 | `gpu/raster.cpp` |
| 裁剪实现 | `gpu/clipper.cpp` |
| 相机逻辑 | `application/camera.cpp` |
| 数学变换矩阵 | `math/mathFunctions.h` |
