# Sandbox19 鼠标、镜头与角色控制

日期：2026-09-12。状态：macOS Release 与合成输入回放通过；最终版物理鼠标手感、失焦重捕和 Windows 原生路径待实测。

## 问题与参考

Sandbox19 原先让 FOLLOW 镜头和指挥官共同决定前进与射击方向，但普通鼠标移动不转镜。第一次尝试把屏幕光标投射到地面来改变身体朝向，造成镜头、身体和移动方向分离；即使 W 后来改为沿身体朝向，这条绝对光标链路仍不符合 MiniGame。参考 MiniGame 的 `macOS/MiniwMacMain.mm → iworld/PCControl.cpp → iworld/camera/GameCamera.cpp → iworld/player/PlayerControl.cpp`：进入视角模式时光标居中并捕获，相对鼠标位移旋转镜头，角色朝镜头水平视线转身，前进取当前身体朝向。

## 最终控制链路

活跃 FOLLOW 玩法先将指针移到窗口中心，再捕获并隐藏；相对位移经 InputManager / GameManager 进入 `CameraService::RotateFollowView`。PlayerController 每个仿真步读取 `GetFollowForward`，用既有 `AgentLocomotion::FaceDirection` 平滑转动刚体；W/S 沿刚体当前 `GetForward()` 前后移动，A/D 相对身体侧移。左键或 Space 开火、滚轮缩放、Q/E 辅助转镜。快速转镜时身体和移动方向可短暂落后视线，但 W 与可见身体方向一致。默认 tank sample 不启用此路径。

Sandbox19 仍需 HUD、框选和世界指令，因此按住 Alt 临时释放捕获并恢复绝对指针；松开 Alt 再居中捕获。暂停、失焦、关闭窗口也释放捕获，失焦恢复的首个点击仅重新捕获，不触发射击或下令。macOS 使用 `CGAssociateMouseAndMouseCursorPosition`、`CGWarpMouseCursorPosition` 和 NSEvent delta；Windows 用客户区 `ClipCursor`、`SetCursorPos` 与 OIS 相对位移。后台窗口及内部回放不创建物理输入设备。此改动不增加 Lua 绑定；Sandbox19 十字准星、帮助文字和试玩包说明已同步。

## 验证与边界

macOS arm64 Release 完整隔离构建通过，最终二进制 SHA-256 为 `c4d32d5059889b4bc9cb24065ce43969b2ffacdd1180a590edb6c2687f7b8684`。真实 1280×800 GL 后台窗口的 `mouse-look.txt` 内部回放记录：镜头右转后 `cameraForward.x=-0.201`、`playerForward.x=-0.202`；W 释放时角色水平朝向 `(-0.208, 0, 0.978)`，速度 `(-0.619, 0, 2.910)`，两者归一化同向；Space 触发真实枪口发弹。按 Alt 移动光标时镜头不动，右键 rally 被两名队友接受并在 66 ms 内执行；松开 Alt 后再移动，镜头和角色继续转向。回放 `completed reason=quit`，日志在本地 `tmp/mouse-look-20260912/mouse-look-centered.log`。

同一最终二进制的完整自然对局门禁 PASS，77.022 秒自然胜利，并按序重开、暂停、退出；摘要在本地 `tmp/relay-natural-20260912-232621-m7avf26r/summary.json`。产品夹具、Sandbox6/7/8 smoke、720p HUD/下令回放、Lua 5.1 语法和 diff 检查通过。macOS 试玩包签名、ZIP 完整性和包内二进制哈希一致，最终包前台产品自测 `PASS all=true`。居中前版本曾记录前台原生捕获开启/释放；最终包的前台自测在窗口激活前执行，故不能以此证明最终版居中后的实际鼠标移动。真人连续键鼠手感、失焦后首击重捕和 Windows D3D9 原生输入仍待实测，P3 保持未验收。
