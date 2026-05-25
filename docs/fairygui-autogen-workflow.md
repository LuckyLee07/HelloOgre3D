# FairyGUI AutoGen Workflow

补充说明：新增业务 UI 的完整接入流程已经单独沉淀到 `docs/fairygui-business-ui-standard-flow.md`。当前生产级样例是 `BusinessBenchmark`，用于验证 AutoGen 子结构、列表 item 绑定、Benchmark 自测和 registry 生成一致性。

## Goal

新增 `.fui` 后，优先走工具生成 MVC 骨架和 registry 片段，手写业务只放在 Ctrl / Model 中。

这条链路当前不直接修改 `UIRegistry.lua`，而是生成可 review 的 `*.registry.lua` 片段。确认无误后再手动合并，避免工具误覆盖已有 UI 配置。

## Final Target

FairyGUI UI 接入脚本的最终目标：

- 一个正式入口：`tools/fgui_autogen/fairygui_generate_ui.py`。
- 一条命令完成 manifest、MVC 骨架、registry 片段、`GeneratedUIRegistry.lua`。
- 默认只覆盖 `*AutoGen.lua` 和生成 registry，不覆盖手写 `View / Model / Ctrl`。
- 写入前可用 `--dry-run` 查看计划，用 `--check` 做 CI/本地预检。
- registry 生成可重复执行，按 `FairyGUIAutoGen:<UIName>` 标记块更新，不重复追加。
- 正式页面最终仍建议把 review 后的配置复制到 `UIRegistry.lua`，让人工配置成为长期维护源。

## Iteration Plan

1. Manifest 生成：从 FairyGUI 导出 XML 读取 package、component、controls、controllers、list item 元信息。
2. Lua 骨架生成：生成 AutoGen/View/Model/Ctrl，List 自动生成基础 `Refresh*List / Render*Item` stub。
3. Registry 生成：生成独立 review 文件，并更新 `GeneratedUIRegistry.lua`。
4. 安全校验：补 `--dry-run / --check / --strict`，明确 create / overwrite / skip / update。
5. 运行时接入：`UIRegistry.lua` 自动加载 `GeneratedUIRegistry.lua`，手写配置优先。
6. 验证闭环：用 Act38 作为标准样例，检查 Python、Lua 语法和运行时自测。

## Inputs

- 运行时资源：`bin/res/fuires/<package>.fui`
- FairyGUI 导出 XML：`bin/res/assets/<module>/package.xml` 和组件 XML
- 主组件名：例如 `QingLuanActMain`
- UI 逻辑名：例如 `Act38Test`

## Step 1: Generate Manifest

Recommended one-shot command:

```powershell
python tools\fgui_autogen\fairygui_generate_ui.py `
	--asset-dir bin\res\assets\act_38 `
	--package act_38_test `
	--component QingLuanActMain `
	--ui-name Act38Test `
	--layer Popup `
	--group Sample `
	--scene Default `
	--cache false `
	--full-screen
```

This command generates the manifest, MVC stubs, standalone registry snippet, and `GeneratedUIRegistry.lua`.

Useful safety modes:

```powershell
python tools\fgui_autogen\fairygui_generate_ui.py `
	--asset-dir bin\res\assets\act_38 `
	--package act_38_test `
	--component QingLuanActMain `
	--ui-name Act38Test `
	--layer Popup `
	--group Sample `
	--scene Default `
	--cache false `
	--full-screen `
	--dry-run
```

```powershell
python tools\fgui_autogen\fairygui_generate_ui.py `
	--asset-dir bin\res\assets\act_38 `
	--package act_38_test `
	--component QingLuanActMain `
	--ui-name Act38Test `
	--layer Popup `
	--group Sample `
	--scene Default `
	--cache false `
	--full-screen `
	--check
```

批量扫描入口：

```powershell
python -B tools\fgui_autogen\fairygui_batch_generate.py --check --strict
```

该脚本默认扫描 `bin/res/assets/*/package.xml`，跳过 `fairygui_manifests`，优先复用已有 manifest 的 `uiName/package/component` 和独立 registry 中的 layer/cache/fullScreen 等策略。去掉 `--check` 即可批量更新受管生成物。

`--dry-run` 不写文件，只打印计划。`--check` 不写文件，但会比对 manifest、`*AutoGen.lua`、独立 registry 和 `GeneratedUIRegistry.lua` 的实际内容；发现缺失或过期生成物时返回失败。`--strict` 会让 `--check` 在存在 warning 时也返回失败。`--force` 会刷新 View / Model / Ctrl stubs，并保留 `FairyGUIUserCode` 标记区。`--no-registry-aggregate` 会跳过 `GeneratedUIRegistry.lua`。

Advanced manifest-only command:

```powershell
python tools\fgui_autogen\fairygui_asset_manifest.py `
	--asset-dir bin\res\assets\act_38 `
	--package act_38_test `
	--component QingLuanActMain `
	--ui-name Act38Test `
	--output bin\res\assets\fairygui_manifests\act_38_test.json
```

manifest 会记录：

- package / component / packageId
- 主组件 controls / controllers
- list 的 defaultItem / itemCount / selectionController
- 子组件 controls，例如 `task_item`、`shop_item`
- 资源清单

## Step 2: Generate Lua MVC Stubs

Advanced Lua-only command:

```powershell
python tools\fgui_autogen\fairygui_autogen.py `
	--manifest bin\res\assets\fairygui_manifests\act_38_test.json `
	--output-dir bin\res\scripts\ui\views `
	--registry-output bin\res\assets\fairygui_manifests\act_38_test.registry.lua `
	--registry-aggregate-output bin\res\scripts\ui\GeneratedUIRegistry.lua `
	--layer Popup `
	--group Sample `
	--scene Default `
	--cache false `
	--full-screen
```

默认策略：

- `*AutoGen.lua` 会覆盖，因为它是生成文件。
- `*View.lua / *Model.lua / *Ctrl.lua` 默认不覆盖，避免误删手写业务。
- 如果确认要刷新骨架，再加 `--force`；工具会保留 `FairyGUIUserCode` 标记区中的业务代码。
- registry 输出为独立 Lua table，不自动合并到 `UIRegistry.lua`。

## Outputs

- `bin/res/scripts/ui/views/<UIName>AutoGen.lua`
- `bin/res/scripts/ui/views/<UIName>View.lua`
- `bin/res/scripts/ui/views/<UIName>Model.lua`
- `bin/res/scripts/ui/views/<UIName>Ctrl.lua`
- `bin/res/assets/fairygui_manifests/<package>.registry.lua`
- `bin/res/scripts/ui/GeneratedUIRegistry.lua`

生成的 `*AutoGen.lua` 会输出 `ControlPath`、`ControlType`、`ControllerPath`、`TransitionName`、`ListItem`，并提供 `GetControlPath/GetControlType/GetControllerPath/GetTransitionName/GetListItemDefine` 查询函数。

生成的 Ctrl 会根据 manifest 识别 `GList`，创建基础 `Refresh*List` 和 `Render*Item` stub，并在注释里标出 item 组件和 item 控件。

`View / Model / Ctrl` scaffold 会生成类似下面的保留区：

```lua
-- <FairyGUIUserCode:ShopMainCtrl:RegisterUIEvents>
	self:AddClick("btn_close", function()
		self:Close(true)
	end)
-- </FairyGUIUserCode:ShopMainCtrl:RegisterUIEvents>
```

业务代码优先写进这些区域。后续控件结构变化时，可以重新执行生成命令并附加 `--force`，工具会刷新 scaffold，同时把旧文件中相同 key 的保留区内容合并回新文件。旧的无标记手写文件在 `--force` 下仍会被整体替换，工具会在计划输出里提示。

## Step 3: Merge Registry

Current generated registry mode:

- `--registry-output` writes a standalone review file under `bin/res/assets/fairygui_manifests`.
- `--registry-aggregate-output` updates `bin/res/scripts/ui/GeneratedUIRegistry.lua`.
- `UIRegistry.lua` loads `GeneratedUIRegistry.lua` automatically.
- Hand-written entries in `UIRegistry.lua` win over generated entries with the same UI name.
- Once a UI becomes a formal page, copy the reviewed generated entry into `UIRegistry.lua` and keep business edits in Ctrl / Model / View.

把 `bin/res/assets/fairygui_manifests/<package>.registry.lua` 中的 UI 配置复制到：

```text
bin/res/scripts/ui/UIRegistry.lua
```

正式 UI 建议都走 registry，这样后续才能统一支持：

- `FairyGuiManager:Open("UIName", param)`
- MVC 自动关联
- layer / popup / modal / fullScreen
- cache / scene cleanup
- resource dump
- debug trace

## Step 4: Fill Business Code

业务只改：

- `<UIName>Model.lua`：数据结构、打开参数、状态变更。
- `<UIName>Ctrl.lua`：事件绑定、刷新逻辑、List item renderer。
- `<UIName>View.lua`：必要时补充控件访问封装。

建议把事件绑定写进 `Ctrl:RegisterUIEvents` 保留区，把模型初始化写进 `Model:Init` 保留区，把额外方法写进文件尾部 `custom` 保留区。

不要把业务写进 `*AutoGen.lua`。

## Step 5: Validate

一键静态检查：

```powershell
powershell -ExecutionPolicy Bypass -File tools\check_fgui_static.ps1
```

该脚本会覆盖：

- `bin/res/scripts` 下 Lua 语法检查。
- FGUI AutoGen Python 工具编译检查。
- 批量扫描 Act37 / Act38 的 AutoGen `--check --strict`，包括生成文件一致性检查。

Lua 语法检查：

```powershell
luac -p bin\res\scripts\ui\views\<UIName>AutoGen.lua `
	bin\res\scripts\ui\views\<UIName>View.lua `
	bin\res\scripts\ui\views\<UIName>Model.lua `
	bin\res\scripts\ui\views\<UIName>Ctrl.lua `
	bin\res\assets\fairygui_manifests\<package>.registry.lua
```

工具链检查：

```powershell
python -B -m py_compile tools\fgui_autogen\fairygui_asset_manifest.py `
	tools\fgui_autogen\fairygui_autogen.py `
	tools\fgui_autogen\fairygui_generate_ui.py
```

```powershell
python tools\fgui_autogen\fairygui_generate_ui.py `
	--asset-dir bin\res\assets\act_38 `
	--package act_38_test `
	--component QingLuanActMain `
	--ui-name Act38Test `
	--layer Popup `
	--group Sample `
	--scene Default `
	--cache false `
	--full-screen `
	--check
```

运行时验证：

- 推荐使用统一脚本唤起 exe；脚本默认会检查本次新增日志中的 `OGRE EXCEPTION`、`call_func error` 和 `self test result: false`，命中时返回失败。

```powershell
powershell -ExecutionPolicy Bypass -File tools\run_fgui_selftest.ps1 -Mode DebugPanel -Visible
powershell -ExecutionPolicy Bypass -File tools\run_fgui_selftest.ps1 -Mode BusinessFlow -Visible
```

- 合并 registry 后调用 `FairyGuiManager:Open("UIName", param)`。
- 复杂 UI 应补一个 `HELLO_FGUI_<NAME>_SELF_TEST=1` 入口，至少覆盖打开、List 渲染、关闭。

当前 Act38 基准验收：

```powershell
cd E:\Workspace\HelloOgre3D\bin
$env:HELLO_FGUI_ACT38_SELF_TEST="1"
.\HelloOgre3D.exe
```

预期日志：

```text
[FGUI] open act_38_test: ... dayCount= 3 spcCount= 2 shopCount= 2
[FGUI] act38 list api self test result: true
[FGUI] act38 dump: ... act_42_qingluanact QingLuanActMain ...
[FGUI] act38 day count: 4
```
