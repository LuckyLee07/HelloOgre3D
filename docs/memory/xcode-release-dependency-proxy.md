# macOS 生成工程的 Release 依赖代理

类型：project

当前 Premake 生成的 HelloOgre3D Xcode 主工程即使用 Release configuration，也可能把静态依赖作为带 _d 名称的 PBXReferenceProxy / 链接项（已实测 ogre3d_gorilla）。单独构建依赖的 Release configuration 只会更新无 _d 的 archive，主程序随后可能继续链接旧的 _d archive，让源码修复看似已编译却没有进入可执行文件。

修改被主程序以 _d 代理引用的 vendored 静态库后，先在生成的主 project.pbxproj 核对实际 proxy / linker 名称；若是 _d，显式重建对应依赖的 Debug configuration，再触发主程序 Release 重链接。最后用运行行为、符号/反汇编或其它可执行文件级证据确认修复已进入二进制，不能只看依赖 Release BUILD SUCCEEDED。
