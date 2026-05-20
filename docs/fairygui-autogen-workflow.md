# FairyGUI AutoGen Workflow

## Goal

新增 `.fui` 后，优先走工具生成 MVC 骨架和 registry 片段，手写业务只放在 Ctrl / Model 中。

这条链路当前不直接修改 `UIRegistry.lua`，而是生成可 review 的 `*.registry.lua` 片段。确认无误后再手动合并，避免工具误覆盖已有 UI 配置。

## Final Target

FairyGUI UI 接入脚本的最终目标：

- 一个正式入口：`tools/fairygui_generate_ui.py`。
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
python tools\fairygui_generate_ui.py `
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
python tools\fairygui_generate_ui.py `
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
python tools\fairygui_generate_ui.py `
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

`--dry-run` and `--check` do not write files. `--strict` makes `--check` fail when warnings exist. `--force` overwrites View / Model / Ctrl stubs. `--no-registry-aggregate` skips `GeneratedUIRegistry.lua`.

Advanced manifest-only command:

```powershell
python tools\fairygui_asset_manifest.py `
	--asset-dir bin\res\assets\act_38 `
	--package act_38_test `
	--component QingLuanActMain `
	--ui-name Act38Test `
	--output tools\fairygui_manifests\act_38_test.json
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
python tools\fairygui_autogen.py `
	--manifest tools\fairygui_manifests\act_38_test.json `
	--output-dir bin\res\scripts\ui\views `
	--registry-output tools\fairygui_manifests\act_38_test.registry.lua `
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
- 如果确认要重建骨架，再加 `--force`。
- registry 输出为独立 Lua table，不自动合并到 `UIRegistry.lua`。

## Outputs

- `bin/res/scripts/ui/views/<UIName>AutoGen.lua`
- `bin/res/scripts/ui/views/<UIName>View.lua`
- `bin/res/scripts/ui/views/<UIName>Model.lua`
- `bin/res/scripts/ui/views/<UIName>Ctrl.lua`
- `tools/fairygui_manifests/<package>.registry.lua`
- `bin/res/scripts/ui/GeneratedUIRegistry.lua`

生成的 Ctrl 会根据 manifest 识别 `GList`，创建基础 `Refresh*List` 和 `Render*Item` stub，并在注释里标出 item 组件和 item 控件。

## Step 3: Merge Registry

Current generated registry mode:

- `--registry-output` writes a standalone review file under `tools/fairygui_manifests`.
- `--registry-aggregate-output` updates `bin/res/scripts/ui/GeneratedUIRegistry.lua`.
- `UIRegistry.lua` loads `GeneratedUIRegistry.lua` automatically.
- Hand-written entries in `UIRegistry.lua` win over generated entries with the same UI name.
- Once a UI becomes a formal page, copy the reviewed generated entry into `UIRegistry.lua` and keep business edits in Ctrl / Model / View.

把 `tools/fairygui_manifests/<package>.registry.lua` 中的 UI 配置复制到：

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

不要把业务写进 `*AutoGen.lua`。

## Step 5: Validate

Lua 语法检查：

```powershell
luac -p bin\res\scripts\ui\views\<UIName>AutoGen.lua `
	bin\res\scripts\ui\views\<UIName>View.lua `
	bin\res\scripts\ui\views\<UIName>Model.lua `
	bin\res\scripts\ui\views\<UIName>Ctrl.lua `
	tools\fairygui_manifests\<package>.registry.lua
```

工具链检查：

```powershell
python -B -m py_compile tools\fairygui_asset_manifest.py `
	tools\fairygui_autogen.py `
	tools\fairygui_generate_ui.py
```

```powershell
python tools\fairygui_generate_ui.py `
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
