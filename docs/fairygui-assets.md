# FairyGUI 资源整理

## 目录职责

当前 FairyGUI 资源分两层保存：

- `bin/res/assets/<module>`：FairyGUI 导出的 XML 源数据和原始图片资源，用于检查控件结构、生成 AutoGen manifest、定位控件名。
- `bin/res/fuires`：运行时加载用的 `.fui` 和 atlas 图片，`FairyGuiManager:LoadPackage` 只从这里加载运行时 package。

不要只保留其中一层：

- 缺少 `assets` 时，后续无法稳定自动生成 View/AutoGen 控件绑定。
- 缺少 `fuires` 时，运行时无法通过 `UIPackage::addPackage` 加载界面。

## 当前资源

| 模块 | XML 源目录 | 运行时 package | 主组件 | 说明 |
| --- | --- | --- | --- | --- |
| act_37 | `bin/res/assets/act_37` | `bin/res/fuires/act_37_test.fui` | `main_pifushoumai` | 当前 MVC 样例界面 |
| act_38 | `bin/res/assets/act_38` | `bin/res/fuires/act_38_test.fui` | `QingLuanActMain` | 带 `List` 的青鸾活动样例 |

`act_38_test.fui` 的运行时内部 package 名是 `act_42_qingluanact`，这由 FairyGUI 导出决定。Lua 注册表里仍使用 `.fui` 文件名 `act_38_test` 即可，加载后 C++ 会返回真实 package 名。

## act_38 控件结构

`bin/res/assets/act_38/QingLuanActMain.xml` 中当前可直接绑定的主控件：

| 名称 | 类型 | 用途 |
| --- | --- | --- |
| `m2_menuCtrl` | `GController` | 页签控制器 |
| `btn_close` | `GComponent` | 关闭按钮 |
| `btn_help` | `GComponent` | 帮助按钮 |
| `MenuTab` | `GList` | 顶部页签列表 |
| `m2_dayTaskList` | `GList` | 每日任务列表 |
| `m2_spcTaskList` | `GList` | 特殊任务列表 |
| `m2_excShopList` | `GList` | 活动商城列表 |
| `loader_img_bg` | `GLoader` | 背景加载器 |
| `t_date` | `GTextField` | 活动时间文本 |

列表 item 组件：

- `task_item.xml`：包含 `c1 / icon / num / desc / btn_go / btn_get / btn_alrget`。
- `shop_item.xml`：包含 `c1 / itemicon1 / num1 / itemicon2 / num2 / desc / btn_exchange / btn_alrowned / btn_alrfinish`。

## Manifest

XML 源数据已整理为：

- `tools/fairygui_manifests/act_37_test.json`
- `tools/fairygui_manifests/act_38_test.json`

`act_38_test.json` 由工具生成，包含：

- `package / component / packageId`
- 主组件 `controllers / controls`
- `resources` 资源清单
- `componentControls`，用于后续生成子组件或 item 绑定

重新生成命令：

```powershell
python tools\fairygui_asset_manifest.py `
	--asset-dir bin\res\assets\act_38 `
	--package act_38_test `
	--component QingLuanActMain `
	--ui-name Act38Test `
	--output tools\fairygui_manifests\act_38_test.json
```

## 验证入口

`act_38` 当前已经注册为 `Act38Test`，可通过 Lua 调用：

```lua
FGUI_OpenAct38Sample()
FGUI_DumpAct38Sample()
FGUI_CloseAct38Sample()
```

当前 `Act38Test` 已经是 MVC 示例：

- `bin/res/scripts/ui/views/Act38TestAutoGen.lua`
- `bin/res/scripts/ui/views/Act38TestView.lua`
- `bin/res/scripts/ui/views/Act38TestModel.lua`
- `bin/res/scripts/ui/views/Act38TestCtrl.lua`

`game_init.lua` 只保留打开、关闭、dump 包装函数；任务列表、商城列表的数据和 item 渲染逻辑在 `Act38TestModel / Act38TestCtrl` 中维护。

自动验证：

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
