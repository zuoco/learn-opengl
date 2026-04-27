# Linux + Slint 重构计划

## 目标
将 `softRenderer` 从 Windows/Win32 移植到 Linux，使用 Slint C++ SDK 作为 UI 层。

## 关键变更摘要

### 1. 颜色格式统一（RGBA）
- 当前 `RGBA` 结构体内存布局为 `BGRA`（为了匹配 Win32 DIB）
- 修改为标准 `RGBA` 顺序，与 Slint `Rgba8Pixel` 兼容
- 移除 `Image::createImage` 中的 R/B 通道交换逻辑
- 修改 `GPU::clear()` 的默认清除颜色等

### 2. Application 层完全重写
- 移除所有 Win32 API（`Windows.h`, `HINSTANCE`, `HWND`, `DIB`, `BitBlt`）
- 新 `Application` 类职责：
  - 持有 `slint::ComponentHandle<AppWindow>`
  - 持有 `slint::Timer`，以 ~60fps 驱动渲染循环
  - 连接 Slint callbacks（`key-pressed`, `key-released`, `pointer-moved`, `pointer-pressed`, `pointer-released`）到 `Camera`
  - 每帧将 GPU framebuffer 包装为 `slint::Image` 更新到 UI

### 3. Camera 输入适配
- 原接口使用 Win32 虚拟键码（`0x57`/`0x41`等）和 `int x,y` 鼠标坐标
- 修改为接受字符串键码（`"w"`, `"a"`, `"s"`, `"d"`）和 `float x,y`（适配 Slint 坐标）
- 内部移动状态逻辑不变

### 4. 新增 Slint UI 文件 (`ui/app-window.slint`)
```slint
export component AppWindow inherits Window {
    in property <image> render-image;
    
    callback key-pressed(string);
    callback key-released(string);
    callback pointer-moved(length, length);
    callback pointer-pressed(length, length);
    callback pointer-released(length, length);
    
    width: 1200px;
    height: 900px;
    
    Image {
        source: root.render-image;
        width: 100%;
        height: 100%;
    }
    
    TouchArea {
        width: 100%;
        height: 100%;
        moved => { root.pointer-moved(touch.mouse-x, touch.mouse-y); }
        pointer-event(event) => {
            if event.button == PointerEventButton.left && event.kind == PointerEventKind.down {
                root.pointer-pressed(touch.mouse-x, touch.mouse-y);
            } else if event.button == PointerEventButton.left && event.kind == PointerEventKind.up {
                root.pointer-released(touch.mouse-x, touch.mouse-y);
            }
        }
    }
    
    FocusScope {
        key-pressed(event) => { root.key-pressed(event.text); return accept; }
        key-released(event) => { root.key-released(event.text); return accept; }
    }
}
```

### 5. main.cpp 重构
- `wWinMain` → 标准 `main(int argc, char* argv[])`
- 初始化流程：
  1. `sgl->initSurface(WIDTH, HEIGHT)`（GPU 内部自分配 color buffer）
  2. 加载模型、创建 Camera/Shader
  3. `auto ui = AppWindow::create()`
  4. 注册 callbacks
  5. 启动 `slint::Timer` 渲染循环
  6. `ui->show()` + `slint::run_event_loop()`

### 6. CMake 重构
- **根 `CMakeLists.txt`**：
  - 移除 Win32 DLL 复制、`thirdParty` include/link
  - 添加 `set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/slint")`
  - `find_package(Slint REQUIRED)`
  - 使用系统 assimp：`find_package(PkgConfig REQUIRED)` + `pkg_check_modules(ASSIMP REQUIRED assimp)`
  - `slint_target_sources(softRenderer ui/app-window.slint)`
  - `target_link_libraries(softRenderer applicationLib gpuLib Slint::Slint ${ASSIMP_LIBRARIES})`
  - 复制 `assets` 到 build dir
- **`application/CMakeLists.txt`**：基本不变（移除 Win32 源文件后自动只剩 camera/model/image/stb）
- **`gpu/CMakeLists.txt`**：不变

### 7. Assimp 适配
- 使用系统已安装的 `libassimp.so`（版本 6.0.4）
- 头文件路径改为系统 `/usr/include/assimp`
- 移除 `thirdParty/` 依赖

## 文件变更清单

| 文件 | 操作 | 说明 |
|------|------|------|
| `global/base.h` | 修改 | RGBA 结构体改为 RGBA 顺序；可能需要调整 key code 定义 |
| `application/application.h` | 重写 | 移除 Win32，改为 Slint 相关成员 |
| `application/application.cpp` | 重写 | 完全替换为 Slint Timer + callbacks |
| `application/camera.h` | 修改 | 键码接口改为字符串；鼠标坐标改为 float |
| `application/camera.cpp` | 修改 | 键码匹配改为字符串比较 |
| `application/image.cpp` | 修改 | 移除 stbi_load 后的 R/B 交换 |
| `application/model.cpp` | 修改 | 确保 assimp 头文件路径兼容（可能无需改动） |
| `main.cpp` | 重写 | 标准 main()，Slint 事件循环 |
| `CMakeLists.txt` | 重写 | Linux + Slint + 系统 assimp |
| `application/CMakeLists.txt` | 修改 | 基本不变 |
| `gpu/CMakeLists.txt` | 不变 | |
| `ui/app-window.slint` | 新增 | Slint UI 定义 |

## 风险与注意事项
1. **性能**：每帧将 GPU framebuffer 完整复制到 `slint::SharedPixelBuffer` 可能有开销，但对于 1200x900 软渲染器来说可接受。
2. **焦点**：`FocusScope` 需要获得焦点才能接收键盘事件。需要在 window show 后通过某种方式设置焦点（通常 TouchArea 或默认焦点即可）。
3. **鼠标按钮**：原代码使用右键旋转，Slint `TouchArea` 默认左键。计划改为左键，或用户可在 Slint 中自行调整。本次实现使用左键（更符合常规）。
4. **编码**：源文件原有 GBK 中文注释，修改时保持编码不变，避免乱码。

## 验证步骤
1. `cmake -B build && cmake --build build`
2. 运行 `./build/softRenderer`
3. 确认窗口显示、模型渲染、WASD 移动、鼠标旋转正常工作
