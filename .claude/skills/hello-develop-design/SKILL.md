---
name: hello-develop-design
description: "Use when the user requests /hello-develop-design, hello-develop-design, HelloOgre3D feature design, implementation planning, gated development workflow, or new-feature work in the HelloOgre3D repository."
---

# hello-develop-design

Read `.claude/commands/hello-develop-design.md` and follow it as the source of truth.

Required behavior:
- Preserve all HelloOgre3D repository rules from `AGENTS.md` and the north star `docs/project-direction.md`.
- Use Chinese for user-facing communication.
- Treat `.claude/commands/hello-develop-design.md` as the only command flow source; do not duplicate or reinterpret it here.
- The six-stage flow delegates to node skills under `docs/skills/` (brainstorming / writing-plans / subagent-driven-development / verify / gitcommit / doc-sync). Read each when the command flow reaches its stage.
- Verification means Release x64 build + `tools/run_sandbox_smoke.ps1` (and `run_chapter9_*` / `run_fgui_production_gate.ps1` as applicable), per `docs/skills/verify.md`.
- If `.claude/commands/hello-develop-design.md` is not found in the current workspace, tell the user the command file is missing and stop.
