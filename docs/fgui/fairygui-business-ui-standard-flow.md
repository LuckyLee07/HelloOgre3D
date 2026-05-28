# FairyGUI 业务 UI 标准接入流程

## 目标

新增业务 UI 默认走固定链路：FairyGUI 导出资源、AutoGen 生成、Registry 注册、Ctrl/Model/View 填业务、selftest 验证。业务代码不直接调用 native 后端，也不把逻辑写进 `*AutoGen.lua`。

## 标准流程

1. 放置资源。

   - FairyGUI 导出 XML：`bin/res/assets/<module>/package.xml` 和组件 XML。
   - 运行时包：`bin/res/fuires/<package>.fui` 和 atlas。

2. 生成 manifest、MVC scaffold 和 registry。

   ```powershell
   python -B tools\fgui_autogen\fairygui_generate_ui.py `
   	--asset-dir bin\res\assets\act_38 `
   	--package act_38_test `
   	--component QingLuanActMain `
   	--ui-name BusinessBenchmark `
   	--manifest-output bin\res\assets\fairygui_manifests\business_benchmark.json `
   	--registry-output bin\res\assets\fairygui_manifests\business_benchmark.registry.lua `
   	--registry-aggregate-output bin\res\scripts\ui\GeneratedUIRegistry.lua `
   	--layer Popup `
   	--group Benchmark `
   	--scene BusinessBenchmark `
   	--cache false `
   	--full-screen `
   	--close-on-escape
   ```

3. 填业务代码。

   - `*Model.lua`：数据结构、打开参数、状态变更。
   - `*Ctrl.lua`：事件绑定、刷新逻辑、列表 item renderer。
   - `*View.lua`：控件别名和少量视图封装。
   - `*AutoGen.lua`：只读生成文件，禁止写业务逻辑。

4. 使用 AutoGen 子结构 API。

   - 根控件：`self:GetControlPath("m2DayTaskList")`
   - 列表 item 定义：`self:GetListItemDefine("m2_dayTaskList")`
   - 列表 item 子控件：`self:GetListItemControlPath("m2_dayTaskList", "btnGo")`
   - 子组件定义：`self:GetComponentDefine("task_item")`
   - 子组件控件：`self:GetComponentControlPath("task_item", "desc")`
   - item 绑定快照：`self:BindListItemControls("m2_dayTaskList", item)`

5. 注册与打开。

   `GeneratedUIRegistry.lua` 会被 `UIRegistry.lua` 自动加载，手写 `UIRegistry.lua` 条目优先。正式业务页可以先留在生成 registry 中，策略稳定后再复制到 `UIRegistry.lua` 做长期维护。

   ```lua
   local ctrl = FairyGuiManager:Open("BusinessBenchmark", {
   	key = "BusinessBenchmarkPage",
   	scene = "BusinessBenchmark",
   	group = "BusinessBenchmark",
   })
   ```

6. 补 selftest。

   新业务 UI 至少覆盖打开、列表渲染、事件响应、关闭回收。复杂页面还要覆盖弹窗、Toast/Loading、文本输入、DebugPanel 或资源统计。

## 当前基准样例

`BusinessBenchmark` 是当前业务 UI 标准流程样例：

- 复用 `act_38_test` 资源，独立生成 `BusinessBenchmarkAutoGen/View/Model/Ctrl`。
- 通过 `business_benchmark.json` 和 `business_benchmark.registry.lua` 固化生成输入。
- 覆盖页面、三个列表、列表 item 子结构绑定、虚拟列表、DebugPanel 和 TextInput policy。
- 可通过 `HELLO_FGUI_BUSINESS_BENCHMARK_SELF_TEST=1` 或 selftest 脚本回归。

## 验证命令

```powershell
python -B tools\fgui_autogen\fairygui_generate_ui.py `
	--asset-dir bin\res\assets\act_38 `
	--package act_38_test `
	--component QingLuanActMain `
	--ui-name BusinessBenchmark `
	--manifest-output bin\res\assets\fairygui_manifests\business_benchmark.json `
	--registry-output bin\res\assets\fairygui_manifests\business_benchmark.registry.lua `
	--registry-aggregate-output bin\res\scripts\ui\GeneratedUIRegistry.lua `
	--layer Popup `
	--group Benchmark `
	--scene BusinessBenchmark `
	--cache false `
	--full-screen `
	--close-on-escape `
	--check `
	--strict
```

```powershell
powershell -ExecutionPolicy Bypass -File tools\check_fgui_static.ps1
powershell -ExecutionPolicy Bypass -File tools\run_fgui_selftest.ps1 -Mode BusinessBenchmark -StopExisting
```
