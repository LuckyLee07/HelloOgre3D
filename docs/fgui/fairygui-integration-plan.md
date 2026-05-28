# FairyGUI 接入计划

## 背景

当前项目已有 Gorilla UI，适合 sample 里的基础文字、调试和面板显示；如果后续要承载更完整的游戏 UI，仍需要 FairyGUI 这类编辑器驱动的组件体系。

 MiniUI 证实 FairyGUI 可以通过一层 Cocos2d-x 风格运行时接入 Ogre，但 MiniUI 源码本身深度绑定 env1 的渲染器、材质、动态 VB/IB、输入和资源系统。因此本项目不直接引入 MiniUI 源码，只把它作为实现参考。

本方案使用官方 `FairyGUI-cocos2dx` 作为 UI 逻辑基础，在当前项目内实现最小 `cocos-lite` 兼容层和 Ogre 渲染后端。

## 最终目标

- 不引入完整 Cocos2d-x 引擎。
- 保留 Gorilla，短期内与 FairyGUI 共存。
- 支持 FairyGUI 编辑器导出的 `.fui`、atlas 和资源包。
- UI 可以在 Ogre 1.10 渲染管线中显示、响应输入、处理 resize。
- 提供 Lua API，让 sample/gameplay 脚本可以加载和驱动 FairyGUI。
- FairyGUI 默认可关闭，关闭时不影响现有 sample 行为。

## 目录规划

- `src/external/fairygui_cocos2dx`
  - 官方 `FairyGUI-cocos2dx/libfairygui` 源码。
  - 尽量保持官方源码原貌，降低后续升级成本。
- `src/HelloOgre3D/runtime/ui/fairygui`
  - 当前项目自己的 FairyGUI 适配层。
  - 包含 `FairyGuiSystem`、`cocos-lite`、后续 Ogre renderer、输入桥和资源桥。
- `bin/res/ui`
  - 后续放 FairyGUI 编辑器导出的运行时资源。

## 当前状态

- 已增加 `--with-fairygui` / `HELLO_ENABLE_FGUI` / `ENABLE_FGUI` / `FGUI_ENABLE` 开关。
- 已 vendor 官方 `FairyGUI-cocos2dx/libfairygui`。
- 已增加 `fairygui` 静态库工程。
- Debug 输出 `fairygui_d.lib`，Release 输出 `fairygui.lib`，避免配置之间互相覆盖。
- 已增加 `cocos-lite` 兼容层，使官方 FairyGUI 源码可以在不引入完整 Cocos2d-x 的前提下编译。
- 已增加 `FairyGuiSystem`，并接入 `ClientManager` 生命周期。
- `FairyGuiSystem` 已提供最小运行时入口：
  - `LoadPackage(path)`
  - `CreateObject(packageName, objectName)`
  - `AddToRoot(object)`
- `cocos-lite` 已补上渲染命令提交链路：
  - `Renderer::addCommand(RenderCommand*)`
  - `TrianglesCommand` 保存 texture、blend、vertices、indices、transform
  - `Sprite::draw` 和 `FUISprite::draw` 均可提交 triangle command
  - `FairyGuiSystem` 可接收并统计本帧 UI triangle command
- 已增加第一版 Ogre 渲染后端：
  - 通过 `ManualObject` 生成 overlay triangle list
  - 将 Cocos screen-space 坐标转换到 Ogre identity projection
  - 从 `Texture2D` 保存的图片数据创建 Ogre texture/material
  - 保留 FairyGUI command 提交顺序，先保证显示闭环

## 阶段计划

### Phase 0: 工程入口

目标：
- 增加 FairyGUI 构建开关。
- 新增 `FairyGuiSystem`。
- 挂接初始化、更新、渲染、resize、销毁入口。
- 默认不开启，不改变现有 sample 行为。

验收：
- 未开启 FairyGUI 时现有工程行为不变。
- 开启 FairyGUI 后工程出现独立生命周期入口。

状态：已完成。

### Phase 1: 官方源码进入工程

目标：
- vendor 官方 `FairyGUI-cocos2dx/libfairygui`。
- 新增 Premake 静态库工程。
- 主工程按配置链接 FairyGUI 静态库。

验收：
- VS2017 工程可生成。
- `fairygui` Debug/Release 静态库可单独构建。
- 主工程 Debug 可链接通过。

状态：已完成。

### Phase 2: cocos-lite 兼容层

目标：
- 只实现 FairyGUI 实际用到的 Cocos2d-x API。
- 跑通基础数学、引用计数、节点树、调度器、事件、文件读取、图片/纹理对象、渲染命令对象。

验收：
- `GRoot::create(Scene*)` 可以创建并挂到 Scene。
- `Director::mainLoop(delta)` 可以驱动最小运行时。
- 基础节点树可以遍历。

状态：基础版本已完成，后续会随着渲染/输入补齐缺失接口。

### Phase 3: Ogre 渲染后端

目标：
- 将 Cocos/FairyGUI 的 `Sprite::draw`、`FUISprite::draw`、`TrianglesCommand` 转成 Ogre UI batch。
- 支持 position、color、uv 顶点格式。
- 通过 Ogre 材质关闭深度测试，开启 alpha blend，走 UI render queue。
- 第一版先保证图片和基础组件稳定显示，不急着做复杂合批。

验收：
- 能绘制一张 atlas 图片。
- resize 后坐标不漂移。
- Tracy 能看到 FairyGUI render 消耗。

状态：进行中。当前已完成 Cocos/FairyGUI draw 到 `TrianglesCommand` 的命令提交、`FairyGuiSystem` 捕获，以及第一版 `ManualObject` 后端。下一步需要用一个真实 FairyGUI 包做显示验证，并继续补资源路径、裁剪和输入。

### Phase 4: 资源加载闭环

目标：
- 支持 `UIPackage::addPackage("res/ui/xxx")`。
- `.fui` 通过当前资源路径读取。
- atlas 图片通过 Ogre `TextureManager` 或兼容 `Texture2D` 包装加载。

验收：
- 能加载一个自制 FairyGUI 包。
- 能创建一个普通组件和按钮。

### Phase 5: 输入接入

目标：
- 新增 FairyGUI 输入桥。
- 将 OIS mouse/keyboard 转成 FairyGUI touch/mouse/key event。
- UI 命中后可阻断游戏输入。

验收：
- 按钮点击可触发 C++ 回调。
- 未命中 UI 时，原相机/角色输入不受影响。

### Phase 6: Lua API

目标：
- 提供最小脚本接口：
  - `FairyGui.loadPackage(path)`
  - `FairyGui.createObject(pkg, name)`
  - `FairyGui.addToRoot(obj)`
  - `FairyGui.setText(obj, value)`
  - `FairyGui.onClick(obj, func)`

验收：
- sample Lua 可以创建一个 UI 面板。
- 点击按钮能回调 Lua。

### Phase 7: 能力补齐与优化

优先级：
1. 图片、按钮、组件
2. 文本、bitmap font
3. 九宫格
4. 滚动列表
5. 裁剪
6. mask/stencil
7. 灰度/颜色变换
8. 输入框
9. transition
10. 合批和资源释放

验收：
- 常用 UI 组件可正常显示和交互。
- UI batch 数、CPU 消耗、纹理加载在 Tracy 中可观测。
