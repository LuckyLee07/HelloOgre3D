---
name: parity-gate-python
description: parity gate 比对脚本默认 python 是坏 shim；用 E:\SoftWare\Python\Python310\python.exe
metadata:
  type: reference
---

`run_chapter9_parity_gate.ps1` 默认 `-Python "python"`，但系统的 `python`/`py`/`C:\Windows\py.exe` 都是指向已卸载的 `...\python39\python.exe` 的坏 shim（连虚拟环境 `~/.virtualenvs/lizhibao-Q25iJeC0` 的 base 也指向它），会报 `No Python at ...python39`。

可用的真解释器（注册表 PythonCore）：
- `E:\SoftWare\Python\Python310\python.exe`（3.10.9，推荐）
- `E:\SoftWare\Python\Python38\python.exe`（3.8 32 位）

跑 gate 时加 `-Python "E:\SoftWare\Python\Python310\python.exe"`，或手动跑比对：
`& E:\SoftWare\Python\Python310\python.exe tools\compare_parity_trace.py <legacy.jsonl> <modern.log> --position-tolerance 1.05 --target-tolerance 1.5 --time-tolerance-ms 250 --cell-tolerance 8`

gate 的 legacy/modern trace 跑在 python 步骤之前，所以即使比对失败，`tmp\chapter9_*_parity_*.{jsonl,log}` 已生成，可直接手动比对。相关构建/运行见 [[build-release-for-runtime]]。
