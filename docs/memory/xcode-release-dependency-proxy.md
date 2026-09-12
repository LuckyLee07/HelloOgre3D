# macOS 生成工程的 Release 依赖代理

类型：project

Premake 5.0.0-beta8 的 Xcode 生成器用第一个 configuration 的产品名建立 PBXReferenceProxy。项目保留 Debug 的 `_d` 静态库后缀时，代理会显示 `libogre3d_d.a` 等名称；不能仅凭主 target 的 Release 名字或依赖 Release 的 BUILD SUCCEEDED 判定最后链接的归档。

此前这些 proxy 还直接进入 Frameworks 链接阶段，造成主程序 Release 链接 Debug 归档。当前 `premake/samples.lua` 的 Xcode 分支保留 `dependson` 构建顺序，并把静态归档完整路径放入各 configuration 的 `OTHER_LDFLAGS`。proxy 上的 `_d` 现在只用于依赖元数据，不应再有对应的 PBXBuildFile/Frameworks 链接项；Windows 仍用原有库名配置。

核验时先重新运行 `bash xcode.sh`，检查 Debug 的 `OTHER_LDFLAGS` 为 `libs/lib*_d.a`、Release 为 `libs/lib*.a`，并读取实际链接命令。Release 构建必须让依赖也以 Release 编译；不要再用“重建 Debug 依赖给 Release 主程序链接”作为常规修复，这会污染性能基线。修改静态依赖后仍要用运行行为、符号/反汇编或其它可执行文件级证据确认进入二进制。

隔离生成验证已覆盖 HelloOgre3D 的 29 个静态依赖与 tolua++ 的 2 个依赖：每项链接路径均匹配该依赖相同配置的产品路径，静态 proxy 链接数为 0；这些配置的 Release 优化为 O3、Debug 为 O0。Windows VS2017 生成的 61 个 solution/project/filter 文件与修复前逐字节一致。生成检查不等于当前平台构建或运行验收。

`xcodebuild clean` 可能尝试清理 `CONFIGURATION_BUILD_DIR=bin`；本项目 bin 同时含资源，不能给整个目录添加“允许 Xcode 删除”的标记。需要主程序 ABI clean rebuild 时，先核对并只清理生成的 `build/HelloOgre3D/obj/x64/<configuration>` 中间目录，再构建相应 target；保留资源目录和其他配置产物，并记录此次 clean 的范围。
