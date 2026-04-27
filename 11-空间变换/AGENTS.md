# AGENTS.md

本文件为 AI 编程助手提供项目背景、构建说明和开发指南。 expectation: 读者对本项目一无所知。

## 项目概述

本项目是一个**软件光栅化器（Software Rasterizer）**，使用 C++20 从零实现 3D 渲染管线。这是 learn-opengl 系列的第 11 课（Transform），重点讲授和实现三维变换：

- 模型变换（缩放、平移、旋转）
- 视图变换（摄像机矩阵）
- 投影变换（透视投影、正交投影）
- 屏幕空间变换

**当前演示**：一个绕 Y 轴旋转的彩色三角形，展示 MVP（Model-View-Projection）矩阵变换的完整流程。

## 技术栈

- **语言**：C++20
- **构建系统**：CMake 3.21+
- **编译器**：GCC/Clang（配置中指定 gcc/g++）
- **UI 框架**：Slint 1.15.0（本地捆绑在 `slint/` 目录）
- **图像加载**：stb_image（头文件库，位于 `application/stb_image.h`）
- **系统依赖**：Freetype, Fontconfig, libinput, xkbcommon, libudev, gbm, libseat

## 构建和运行

### 配置项目

```bash
cmake -B build -S .
```

### 构建主程序

```bash
cmake --build build
```

构建产物：
- `build/softRenderer` - 主渲染程序
- `build/testMatrix` - 矩阵运算测试程序

### 运行程序

```bash
# 运行渲染器
./build/softRenderer

# 运行矩阵测试
./build/testMatrix
```

### 清理构建

```bash
rm -rf build/
```

## 项目结构

```
.
├── CMakeLists.txt          # 主构建配置
├── main.cpp                # 程序入口和渲染循环
├── testMatrix.cpp          # 矩阵测试程序
├── CLAUDE.md               # 项目速查文档
├── AGENTS.md               # 本文件
│
├── global/                 # 基础类型定义
│   └── base.h              # RGBA 结构体、宏定义
│
├── math/                   # 自定义线性代数库
│   ├── math.h              # 统一头文件包含
│   ├── vector.h            # Vector2/3/4 模板类
│   ├── matrix.h            # Matrix33/44 模板类（列主序）
│   └── mathFunctions.h     # 变换函数：translate/rotate/scale/perspective 等
│
├── gpu/                    # 软件 GPU 实现
│   ├── gpu.h/cpp           # GPU 单例，绘图接口
│   ├── frameBuffer.h/cpp   # 帧缓冲区管理
│   ├── raster.h/cpp        # 光栅化算法（线、三角形）
│   └── dataStructures.h    # Point 结构体
│
├── application/            # 应用程序层
│   ├── application.h/cpp   # Application 单例，窗口生命周期
│   ├── image.h/cpp         # 图像加载（stb_image 包装）
│   └── stb_image.h         # 第三方图像加载库
│
├── ui/                     # UI 定义
│   └── main.slint          # Slint UI 窗口定义
│
├── assets/                 # 资源文件
│   └── textures/           # 纹理图片
│
└── slint/                  # 本地捆绑的 Slint UI 库
    ├── bin/                # Slint 编译器
    ├── include/            # 头文件
    ├── lib/                # 库文件
    └── licenses/           # 许可证
```

## 核心架构

### 渲染管线流程

```
main.cpp (渲染循环)
    ↓
transform() - 计算 MVP 矩阵，将 3D 顶点变换到屏幕坐标
    ↓
sgl->drawTriangle() - GPU 绘制三角形
    ↓
Raster::rasterizeTriangle() - 光栅化：生成像素
    ↓
sampleNearest/sampleBilinear - 纹理采样（如果有纹理）
    ↓
drawPoint() - 写入帧缓冲区
    ↓
拷贝到 Slint SharedPixelBuffer - Y 轴翻转显示
```

### 关键模块说明

#### 1. math 命名空间（`math/`）

自定义线性代数库，**列主序矩阵**（OpenGL 风格）：

- **向量**：`Vector2<T>`, `Vector3<T>`, `Vector4<T>`，支持基本运算（+、-、*、/）
- **矩阵**：`Matrix33<T>`, `Matrix44<T>`，元素访问 `m[col * 4 + row]`
- **类型别名**：`vec2f`, `vec3f`, `vec4f`, `mat3f`, `mat4f`

**变换函数**（`mathFunctions.h`）：
- `translate(mat4f, vec3f)` - 平移矩阵
- `rotate(mat4f, angle, axis)` - 绕任意轴旋转
- `scale(mat4f, x, y, z)` - 缩放矩阵
- `perspective(fovy, aspect, near, far)` - 透视投影
- `orthographic(left, right, bottom, top, near, far)` - 正交投影
- `screenMatrix(width, height)` - NDC 转屏幕坐标
- `inverse(mat4f)` - 矩阵求逆（伴随矩阵法）
- `dot`, `cross`, `normalize`, `length` - 向量运算

#### 2. GPU 单例（`gpu/`）

通过 `sgl` 宏访问：`#define sgl (&GPU::getInstance())`

- **帧缓冲管理**：`initSurface()`, `getFrameBuffer()`, `clear()`
- **绘制接口**：`drawPoint()`, `drawLine()`, `drawTriangle()`, `drawImage()`
- **渲染状态**：`setBlending()`, `setBilinear()`, `setTexture()`, `setTextureWrap()`
- **纹理采样**：`sampleNearest()`, `sampleBilinear()`

#### 3. Raster 类（`gpu/raster.h`）

静态光栅化算法：

- `rasterizeLine()` - Bresenham 直线算法
- `rasterizeTriangle()` - 包围盒扫描线三角形填充
- `interpolantLine/Triangle()` - 颜色/UV 插值
- `lerpRGBA()`, `lerpUV()` - 线性插值

#### 4. Application 单例（`application/`）

通过 `sglApp` 宏访问：`#define sglApp (&Application::getInstance())`

- 窗口尺寸管理（默认 800x600）
- 生命周期控制：`quit()`, `isAlive()`

#### 5. Image 类（`application/`）

使用 stb_image 加载图片：

- `createImage(path)` - 加载图片文件（自动翻转 Y 轴，BGRA→RGBA 转换）
- `destroyImage()` - 释放资源

## 坐标约定

### 矩阵存储

- **列主序**（Column-major），遵循 OpenGL 约定
- `Matrix44` 内部数组布局：
  ```
  m0  m4  m8  m12
  m1  m5  m9  m13
  m2  m6  m10 m14
  m3  m7  m11 m15
  ```
- 元素访问：`m[col * 4 + row]`

### RGBA 结构

```cpp
struct RGBA {
    byte mB;  // 注意：实际存储顺序是 BGRA
    byte mG;
    byte mR;
    byte mA;
    
    RGBA(byte r, byte g, byte b, byte a);  // 构造函数接收 RGBA 顺序
};
```

### 坐标系

- **3D 坐标**：右手坐标系
- **屏幕坐标**：原点在左下角，X 向右，Y 向上
- **纹理坐标**：UV 范围 [0, 1]，原点在左下角
- **Y 轴翻转**：拷贝到 Slint 缓冲区时需要翻转（`srcY = height - 1 - y`）

## MVP 变换流程（main.cpp）

```cpp
// 1. 模型变换 - 绕 Y 轴旋转
modelMatrix = rotate(mat4f(1.0f), angle, vec3f{0.0f, 1.0f, 0.0f});

// 2. 视图变换 - 摄像机后退
cameraModelMatrix = translate(mat4f(1.0f), vec3f{0.0f, 0.0f, cameraPos});
viewMatrix = inverse(cameraModelMatrix);

// 3. 投影变换 - 透视投影
perspectiveMatrix = perspective(60.0f, WIDTH/HEIGHT, 0.1f, 100.0f);

// 4. 相乘得到 MVP 矩阵并变换顶点
sp1 = perspectiveMatrix * viewMatrix * modelMatrix * pos1;

// 5. 透视除法
sp1 /= sp1.w;

// 6. 屏幕空间变换
screenMatrix = screenMatrix<float>(WIDTH, HEIGHT);
sp1 = screenMatrix * sp1;
```

## 编码规范

### 命名风格

- **类名**：大驼峰（`GPU`, `FrameBuffer`, `Raster`）
- **函数名**：小驼峰（`drawTriangle`, `rasterizeLine`）
- **成员变量**：前缀 `m`（`mWidth`, `mColorBuffer`）
- **宏定义**：全大写（`PI`, `DEG2RAD`, `TEXTURE_WRAP_REPEAT`）

### 单例模式

项目中大量使用单例模式：

```cpp
// 获取实例
static ClassName& getInstance();

// 宏定义快捷访问
#define sgl (&GPU::getInstance())
#define sglApp (&Application::getInstance())
```

### 注释语言

项目主要使用**中文注释**，关键算法有详细的中文解释。

### 头文件包含

- 使用 `#pragma once` 防止重复包含
- 模板类实现直接写在头文件中
- `math/math.h` 统一包含向量、矩阵、函数头文件

## 测试

### 矩阵测试程序（testMatrix.cpp）

```bash
cmake --build build --target testMatrix
./build/testMatrix
```

测试内容：
- Matrix33 基本运算
- 矩阵乘法
- scale 变换函数

## 扩展指南

### 添加新的 3D 物体

1. 在 `main.cpp` 中定义顶点位置（`pos1`, `pos2`, `pos3`...）
2. 设置顶点颜色（`p1.color`）和 UV（`p1.uv`）
3. 在 `transform()` 中应用 MVP 变换
4. 在 `render()` 中调用 `sgl->drawTriangle()`

### 添加新的变换函数

在 `math/mathFunctions.h` 中添加：

```cpp
template <typename T>
Matrix44<T> myTransform(const Matrix44<T>& src, ...) {
    // 实现变换逻辑
}
```

### 添加纹理

1. 将图片放入 `assets/textures/`
2. CMake 会自动拷贝资源到构建目录
3. 使用 `Image::createImage("assets/textures/mytexture.jpg")` 加载

## 注意事项

1. **Slint 生成的头文件**：`build/main.h` 是由 Slint 编译器从 `ui/main.slint` 生成的，包含 `MainWindow` 类定义
2. **资源拷贝**：CMake 配置中自动将 `assets/` 拷贝到构建目录
3. **Y 轴翻转**：帧缓冲区和纹理加载时都处理了 Y 轴翻转，确保显示正确
4. **内存管理**：`FrameBuffer` 和 `Image` 使用原始指针，注意手动释放
