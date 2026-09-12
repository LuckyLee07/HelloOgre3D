---
name: minigame-control-reference
description: 用户认可的 MiniGame 操作体验参考项目
type: reference
---

2026-09-12 用户明确以 Workspace 下的 MiniGame 作为优秀操作体验参考。本机路径为 `/Users/lizi/Desktop/Workspace/MiniGame`；macOS 当前主线是 `Miniw-Client`，`MacStandalone` 仅为技术原型。优先沿 `macOS/MiniwMacMain.mm → iworld/PCControl.cpp → iworld/camera/GameCamera.cpp → iworld/player/PlayerControl.cpp → iworld/actors/ActorBody.cpp` 核对实际链路，勿只看名称像主控制器的 TPSCamera.cpp。入口文档为该项目的 `docs/macos-mouse-input.md`。这是外部参考目录，普通 HelloOgre3D 任务只读。
