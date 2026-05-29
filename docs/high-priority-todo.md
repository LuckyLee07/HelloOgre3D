# High Priority TODO

> Source of truth: `docs/project-direction.md` and `docs/foundation-modules-inventory.md`.
> This file tracks the current execution list only. If it conflicts with the north-star direction, update this file.

## Goal

Ship the first vertical slice:

> A creature defined by data enters the world, a region trigger detects the player, the creature attacks through data-driven BT logic, and data changes can be hot-reloaded.

## Iteration Log

- 2026-05-29: Added `SandboxServices` carrier and injected it through `GameManager -> ObjectManager -> BaseObject`; components can now read services from their owner.
- 2026-05-29: Routed `PhysicsComponent` world lookup through `SandboxServices` first, with the old `g_GameManager` path kept as a compatibility fallback.

## P0 - L0 Foundation

- [x] Introduce `SandboxServices` as the dependency-injection carrier for sandbox systems and components.
- [ ] Route new sandbox/runtime code through `SandboxServices` instead of direct `g_*` singleton access.
- [ ] Extend `SandboxEventDispatcherManager` with queued cross-frame dispatch, recursion protection, per-frame throttling, queue limits, filter-as-subscription, parameterized event names, and Local/Team/Global scope routing.
- [ ] Define the first Lua-table creature `Def` format.
- [ ] Add a small creature assembler that builds objects/components from the Lua-table `Def`.

## P0 - First Vertical Slice

- [ ] Add `TriggerVolume` as the first area event source.
- [ ] Add `TriggerRuntime.lua` for hand-written ECA trigger registration and dispatch.
- [ ] Add `BehaviorEventNode` or an equivalent BT event entry point.
- [ ] Add the first slice sample: data-defined creature + player-enter-region trigger + creature attack reaction.
- [ ] Add cheap hot reload for the slice by reloading Def/BT/trigger Lua files.

## P1 - BT Support Needed Before Editor Work

- [ ] Add `Parallel` and `Random` behavior-tree nodes.
- [ ] Add runtime parameter lookup for BT node params, starting with constants and blackboard keys.
- [ ] Add Selector/Sequence interrupt or condition reevaluation.
- [ ] Add object-id and array blackboard value support.

## P1 - Architecture Paydown While Touching Related Areas

- [ ] Stop adding object-level single-component forwarding methods; mark existing ones as legacy when touched.
- [ ] Migrate component ownership toward `std::unique_ptr`.
- [ ] Normalize component owner access to `BaseObject*`.
- [ ] Prefer typed component lookup over scattered string keys.
- [ ] Extract AI perception queries behind a small query interface before adding large-scale multi-creature behavior.

## Deferred

- [ ] Full CSV pipeline.
- [ ] Save/load serialization.
- [ ] Full visual block editor.
- [ ] Trigger object library and editor placement workflow.
- [ ] UGC/mod packaging and sandbox hardening.
- [ ] World streaming.
