---
name: abi-change-clean-rebuild
description: 改 C++ 类继承/虚函数/字段布局后必须 clean rebuild，否则 ABI 错位致 dynamic_cast 在 RTTI 内崩
metadata:
  type: feedback
---

改 C++ 类的**继承结构（加/去虚基类、第一次引入虚函数、改字段顺序、改虚函数签名）后，必须 clean rebuild**，不能依赖增量编译。

**Why:** 让 `AgentStateController` 继承 `IDecisionDriver`，给原本无虚函数的类加了 vtable 指针，对象布局整体后移 `sizeof(void*)`。增量编译只重编了 `AgentStateController.cpp`，但通过 `AgentStateController*` 访问字段的 `AgentActionContext.cpp` 等没重编，按旧偏移读 `m_agent` 实际读到 `m_fsm`，传进 `dynamic_cast<SoldierObject*>` 在 RTTI 引擎里段错误（`__RTDynamicCast`→`GetCompleteObjectLocatorFromObject`）。

**How to apply:** 改完会动 vtable/字段布局的类后：VS 右键目标项目→"重新生成"（不是"生成"），或删 `build/<Project>/obj/` 再 build；别 `clean sln`（会连第三方静态库重编，慢）。

排查信号：`dynamic_cast` 在 RTTI 函数内部 crash（而非返回 nullptr）、虚调用跳错乱地址、构造完字段就读到奇怪值 → 优先 clean rebuild 验证 ABI，再查逻辑。见 [[build-release-for-runtime]]。
