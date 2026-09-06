---
name: macos-arm64-build
description: 本机默认通用 Release 构建在 Bullet x86_64 intrinsic 失败，arm64 显式构建可通过
type: project
---

2026-09-06 本机 Xcode 默认同时构建 arm64/x86_64 时，vendored Bullet 的 btVector3.h / btMatrix3x3.h 报 intrinsic immediate 参数 10880 超出 0..255。当前 Apple Silicon 运行验证可用：

```bash
xcodebuild -project build/HelloOgre3D/HelloOgre3D.xcodeproj -target HelloOgre3D -configuration Release ARCHS=arm64 ONLY_ACTIVE_ARCH=YES build
```

这只证明 arm64 构建，不能宣称通用架构通过。不要为普通玩法任务顺手修改第三方。受限沙箱还可能阻止 Xcode module cache 或 OpenGL 图形会话，需区分环境错误与编译/运行错误。证据见 `docs/stability-2026-09-06.md`。
