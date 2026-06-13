# /hello-develop-design — 新功能开发统一入口

`/hello-develop-design <功能描述>` 是 HelloOgre3D **新功能开发的统一入口**。
把 brainstorm → write spec → write plan → 子代理执行 → 验证 → 文档同步 流程内嵌为仓库自带 skill，叠加中文交互、强制读相关模块文档、每任务验证（Release 编译 + sample smoke）、文档同步四项约定。

---

## 调用形式

```
/hello-develop-design <功能描述>
```

`<功能描述>` 至少一句话；为空则报错退出：`/hello-develop-design 需要功能描述参数`。

---

## 执行边界

| 操作 | 允许范围 |
|---|---|
| 读源码 / 文档 | 仓库内任意 |
| 写 spec | 仅 `docs/dev-design/specs/` |
| 写 plan | 仅 `docs/dev-design/plans/` |
| 改源码 | 通过阶段 4 子代理派发，主会话不直接改 |
| 改项目文档 | 仅阶段 5 doc-sync 触发 |

基线规则：全程遵循 `AGENTS.md`（依赖流、位置真源、目录职责）与 `docs/project-direction.md`（北极星）。严禁修改 `src/Engine`、`src/External` vendored 代码，除非子代理 plan task 显式要求。

---

## 六阶段流水线

```
[阶段0] 预检 + 按 docs/README.md 索引读相关文档
   ↓
[阶段1] 设计探索（docs/skills/brainstorming.md）
   ↓
[阶段2] 写 spec → 🚦Gate1 用户确认
   ↓
[阶段3] 写 plan（docs/skills/writing-plans.md）→ 🚦Gate2 用户确认
   ↓
[阶段4] 子代理派发（docs/skills/subagent-driven-development.md）
   └ 每任务结束 → docs/skills/verify.md 验证门（Release 编译 + sample smoke）
   ↓
[阶段5] 🚦Gate3 文档同步（docs/skills/doc-sync.md）
   ↓
[阶段6] 收尾报告
```

---

## 阶段 0：预检 + 文档注入

### 0.1 预检（任一失败即报错退出，无写入）

1. `$ARGUMENTS` 非空
2. `docs/dev-design/specs/` 与 `docs/dev-design/plans/` 存在（缺则 mkdir，不报错）
3. `docs/registry.json` 存在且合法 JSON
4. **节点 skill 文档齐备**（缺一报错）：`docs/skills/` 下 `brainstorming.md`、`writing-plans.md`、`subagent-driven-development.md`、`verify.md`、`gitcommit.md`、`doc-sync.md`
5. 续跑探测：扫 `docs/dev-design/specs|plans/` 今日草案（`状态`=`草案*` 或 `已批准，执行中`），若有进入 §续跑分支

### 0.2 模块匹配 + 文档注入（强制读）

Read `docs/registry.json`。按 `<功能描述>` 以 L1(name) → L2(tags) → L3(constraints_summary) → L4(source_paths) 优先级匹配模块 alias，去重。

| 匹配数 | 行为 |
|---|---|
| 0 | 问用户：手动指定 alias / 继续（无模块上下文，读 project-direction.md 兜底）/ 取消 |
| 1–3 | 直接 Read 命中模块的 `doc_path` 全文 + 其 `related_docs`；输出 `已读：[alias 列表]` |
| ≥4 | 列匹配项让用户勾选 1–3 个 |

必然先读 `docs/project-direction.md`（北极星）。模块文档缺失（doc_path 不存在）时回退读 registry 该条 `constraints_summary` + `related_docs`，并把"补写该模块文档"记入阶段 5 doc-sync 待办。

### 0.3 产物

无文件写入。输出中文摘要 + `进入阶段 1`。

---

## 阶段 1：设计探索

**节点 skill**：`docs/skills/brainstorming.md`

Read 全文并严格执行：探索上下文 → 单问题多选澄清 → 提 2-3 方案 → 分节呈现逐节确认。全程中文。完成后进入阶段 2。

---

## 阶段 2：写 spec

设计节全部确认后：

1. 路径 `docs/dev-design/specs/<YYYY-MM-DD>-<topic-kebab>-design.md`
2. 按 brainstorming.md 模板写，含 `状态：草案，待 Review`
3. 自审清单四项就地修
4. 经 `docs/skills/gitcommit.md` 落地 spec **创建** commit（中文 `[dev]` message）

---

## Gate 1：用户确认 spec

```
[Gate 1] spec 已写入并提交：<路径>  commit: <短hash> <message>
请 review，回复：
  ✅ ok → 进入阶段 3
  📝 改 <章节号>: <意见> → 修改后 amend commit + 再 Gate
  ❌ 取消 → 终止（spec 保留为草案）
```

硬约束：等待用户响应；`ok` 时把 `状态` 改 `已批准` 并 amend，进阶段 3。

---

## 阶段 3：写 plan

**节点 skill**：`docs/skills/writing-plans.md`

Read 全文。输入=spec 路径。按"文件结构→Task 结构→自审"产出，路径 `docs/dev-design/plans/<YYYY-MM-DD>-<topic-kebab>.md`，含 `状态：草案，待 Review`。每 task 必标层归属 + 验证策略 + tolua 步骤（如需）。自审后经 gitcommit.md 落地 plan commit。

---

## Gate 2：用户确认 plan

```
[Gate 2] plan 已写入并提交：<路径>  commit: <短hash> <message>
  任务总数：N（并行 X / 串行 Y）  改动文件预估：[...]
请 review，回复：✅ ok → 阶段 4 / 📝 改 <任务号>: <意见> / ❌ 取消
```

硬约束：`ok` 时把 `状态` 改 `已批准，执行中` 并 amend，进阶段 4。

---

## 阶段 4：子代理派发 + 每任务验证

**节点 skill**：`docs/skills/subagent-driven-development.md` + `docs/skills/verify.md`

Read 两文档。逐 task 派发 implementer → spec review → quality review → **verify 验证门（Release 编译 + 目标 sample smoke 不退化）** → TodoWrite 标完成。

- commit 路由：实现子代理**不自己 commit**，列改动文件 + 起草 message 回报；每 task 过两段 review + 验证门后由**主会话**经 gitcommit.md 落地。
- 累计：改动面 M、文档同步待办 D。
- 任意 task BLOCKED 不阻断同批其他 task；收尾报告标红。
- **tolua 强约束**：改导出给 Lua 的 C++ API 走手术式改绑定，禁止 `tolua.bat` 全量重生成。

---

## 阶段 5：Gate 3 + 文档同步

**节点 skill**：`docs/skills/doc-sync.md`

Read 全文。输入=阶段 4 的 M 与 D。按 Gate 3 问用户 u/m/s，按选择更新相关 `docs/` + 可选 `MEMORY`，单独 commit。

---

## 阶段 6：收尾报告

```
=== /hello-develop-design 执行报告 ===
功能描述：<原始 $ARGUMENTS>
[阶段0] 已读文档：<列表>
[阶段1-2] spec：<路径> @ <hash>
[阶段3] plan：<路径> @ <hash>
[阶段4] 子代理执行：
  - 任务1：✅ <短描述>
  - 任务N：❌ BLOCKED（<失败摘要>）/ ⚠ skipped verify
[阶段5] 文档同步：<u/m/s>，更新 <docs 列表>
[阶段6] git 改动：git diff --stat HEAD~<commits> HEAD
⚠ 待人工处理：<BLOCKED / skip 项>
下一步建议：<...>
```

收尾：全部非 BLOCKED → plan `状态` 翻 `已完成` 并 amend；有 BLOCKED → 保留 `已批准，执行中`。

---

## 续跑探测

阶段 0 扫今日草案：解析 `状态` 字段，若存在则问 `[c] continue（按状态续到对应 Gate/阶段）/ [r] restart（git restore + rm 草案）/ [n] new（追加 -2 后缀新建）`。

---

## 错误处理

| 场景 | 处理 |
|---|---|
| 阶段 0 预检失败 / 节点文档缺失 | 报错退出，无写入 |
| 阶段 1 用户取消 | 退出，无文件写入 |
| Gate 1/2/3 中断 | 已写文件保留；下次跑命令续跑探测 |
| 阶段 4 子代理 BLOCKED | 单 task 失败不阻断同批；进收尾报告 |
| 验证门连续 skip | 允许；收尾报告红字 |

---

## 设计要点

- 阶段 0 按 `docs/registry.json` 模块匹配，自动注入命中模块 `docs/modules/<alias>.md` + related_docs。
- verify 策略库：**Release 编译 + `run_sandbox_smoke.ps1` + `run_chapter9_*` / `run_fgui_production_gate.ps1`**（见 `docs/skills/verify.md`），按 registry source_paths 反查回归目标。
- tolua 强约束：改导出给 Lua 的 C++ API 走手术式改绑定，禁 `tolua.bat` 全量重生成。
- 规则基线：`AGENTS.md` + `docs/project-direction.md`（北极星）。
- 阶段 5 doc-sync 维护 `docs/modules/` + `docs/registry.json`，保持文档↔代码一致。
