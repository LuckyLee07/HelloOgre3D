---
name: hello-develop-design
description: Use when the user asks to run hello-develop-design, design or plan a new HelloOgre3D feature, create a spec or implementation plan, or use the repository gated development workflow in Codex.
metadata:
  short-description: HelloOgre3D feature workflow
---

# hello-develop-design for Codex

This is the Codex-native entry for the HelloOgre3D feature-development workflow.
It shares the repository workflow assets with Claude Code, but does not depend on
Claude slash-command runtime.

## Trigger

Use this skill when the user asks for any of these:

- `hello-develop-design`
- `/hello-develop-design`
- a new HelloOgre3D feature design, spec, or plan
- gated implementation using the repository workflow
- module-aware planning based on `docs/registry.json`

## Required Context

At the start of the workflow, read:

1. `AGENTS.md`
2. `docs/project-direction.md`
3. `docs/README.md`
4. `docs/registry.json`
5. `docs/memory/MEMORY.md`

Then match the user request to relevant module entries in `docs/registry.json`.
Read each matched module's `doc_path` and its `related_docs` before proposing or
editing anything.

## Workflow

Follow the repository's six-stage workflow using these shared node documents:

1. Stage 0: preflight and module-context injection from `docs/registry.json`
2. Stage 1: read and follow `docs/skills/brainstorming.md`
3. Stage 2: write specs under `docs/dev-design/specs/`
4. Stage 3: read and follow `docs/skills/writing-plans.md`, writing plans under `docs/dev-design/plans/`
5. Stage 4: read and follow `docs/skills/subagent-driven-development.md` and `docs/skills/verify.md`
6. Stage 5: read and follow `docs/skills/doc-sync.md`
7. Stage 6: summarize outcome and remaining risk

Use `docs/skills/gitcommit.md` only when the user explicitly asks to commit or
the current gated workflow stage requires a commit and the user has approved it.

## Codex-Specific Rules

- Communicate with the user in Chinese.
- Prefer Release verification. Do not run Debug unless the user explicitly asks.
- Use `update_plan` for visible progress tracking.
- Use `apply_patch` for manual file edits.
- If multi-agent tools are available, use them for Stage 4 as described by
  `subagent-driven-development.md`.
- If multi-agent tools are not available, execute the same implement, review,
  and verify sequence in the current thread and say that you are using the
  single-thread fallback.
- Do not treat `.claude/commands/hello-develop-design.md` as the source of truth
  in Codex. It is the Claude Code wrapper only.
- The shared workflow source of truth is `docs/skills/*.md`, plus
  `docs/registry.json`, `AGENTS.md`, and `docs/project-direction.md`.

## Validation

Use `docs/skills/verify.md` and `docs/registry.json` to choose validation.
Default gates are:

- Release x64 build when C++ or build-facing files changed
- `tools/run_sandbox_smoke.ps1` for affected samples
- `tools/run_chapter9_*` for Chapter9 parity or visual behavior work
- `tools/run_fgui_production_gate.ps1` for FairyGUI changes

Report any skipped validation explicitly.
