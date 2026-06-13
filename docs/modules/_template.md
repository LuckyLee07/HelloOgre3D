# <模块名>（alias: <alias>）

> 模块参考文档。由 `/hello-develop-design` 阶段 0 按 `docs/registry.json` 匹配并自动注入。
> 维护：改了本模块源码后，经 `docs/skills/doc-sync.md` 阶段 5 更新本文。

## 1. 职责

<一句话 + 2-3 句展开。这个模块在大图里承担什么。>

## 2. 源码位置（source_paths）

- `<相对路径>`

## 3. 关键类 / 文件

| 文件/类 | 角色 | 说明 |
|---|---|---|
| `path` | 基类/驱动/工厂/系统 | … |

## 4. 公开能力要点

- <对外提供的能力；节点类型/state/字段/查询 API 等>

## 5. 约束与红线

- <依赖流、位置真源、tolua（手术式改绑定勿全量重生成）、所有权、不可动的点>

## 6. 数据流 / 与其他模块关系

- <被谁驱动、写什么、读什么、关联模块 alias>

## 7. 验证策略（接 docs/skills/verify.md）

- **回归 sample**：`Sandbox<X>`（`tools/run_sandbox_smoke.ps1 -Sample/-Preset ...`）
- **编译**：`MSBuild build\HelloOgre3D.sln /p:Configuration=Release /p:Platform=x64 /m`
- **其它 gate**：parity_gate / visual_capture / fgui_production_gate / ai_perf（按需）

## 8. 已知 gap / 相关文档

- <现有 docs 链接（roadmap/gap/perf 等）+ 开放项>
