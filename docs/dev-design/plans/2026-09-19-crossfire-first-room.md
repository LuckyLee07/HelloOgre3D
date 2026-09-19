# 双机交叉行动：首个联合样段

日期：2026-09-19。状态：首个联合样段已实现并完成本机验证；独立包三种门禁均通过，作者主观体验待反馈。
用户已授权按[重设计草案](../specs/2026-09-19-relay-microgame-design.md)开始升级；本轮完成 A/B 的一个房间，玩法、场景、角色与视听同步实施。三关扩展在首个样段体验反馈后推进，不自动扩张。

| 工作 | 落点与预期行为 | 验证 |
|---|---|---|
| 原创资产 | crossfire 模型、材质与生成器；统一机器人/哨兵/模块化庭院 | 确定性重建、真实 GL 构图与动态帧 |
| 真实战斗 | 普通 Agent profile、无骨骼枪口、方向护盾、子弹安全消费 | 正面阻挡/侧后伤害/墙体阻挡/死亡清理；6/7/8/19 回归 |
| 行为与指令 | 独立 Sandbox20 与 BT；点击下令、暂停暂存、共同提交、移动到达、前摇交火 | 真实导航/物理、内部输入自然胜负及重开，明确合成输入边界 |
| 产品表现 | 斜俯视固定镜头、庭院/灯光、路径与护盾意图、紧凑 HUD | 720p/900p 实机检查 |
| 音频 | RuntimeUiSound 有限多路短音、局部绑定、原创音源 | 解析/生命周期/平台构建，听感另记 |
| 交付 | 独立样段启动与本地试玩包；文档同步 | Release arm64、包内启动、引用与 diff 检查 |

共享 build/bin、绑定整合和运行窗口由主会话串行执行；按共享实施节点将资产、战斗支持和音频三个独立文件域委派。保持既有未提交文档，未经授权不提交。

当前平台为 macOS arm64；Windows 仅检查条件分支与接口，未运行不得标 PASS。自动窗口后台、输入回放禁硬件、验证静音，作者主观画质与听感单独保留。

## 已落地的行为与资产

独立入口 `HELLO_SANDBOX_SAMPLE=Sandbox20`，开发时可双击 `tools/launch_crossfire.command`。原 Sandbox19 仍是无环境覆盖时的默认入口。新增样段名 CROSS / FIRE，使用固定斜俯视镜头，一间 20×24 m 维修庭院、两台悬浮机器人和一台方向盾哨卫。

- 两个普通 Agent profile 使用盒体、AI/属性/武器组件，不挂人体骨骼动画。专用 BT 运行在既有 C++ driver；sample scheduler 为 33 ms / 每帧 3 单位，物理、感知、导航复用原设施。
- 左键或 1/2 选择，右键地面移动/可见敌人攻击，Space/Esc 暂停规划/恢复执行，X 停留，R 重开，M 静音。暂停计划按稳定 ID/位置暂存，两台在同一仿真时刻提交；移动时不开火，停下自主瞄准、三连发。
- 哨卫有限转向，1.1 s 蓄力并锁定射向。前 120° 盾抵消实弹，侧后命中扣血；真实墙体挡弹，友军无伤。子弹快照持来源 ID，不保存射手指针；同一弹只消费一次，4 s 寿命，重开主动清理。
- 原创 15 个网格、14 种统一材质、68 个静态模块实例：双标记机器人、装甲哨卫、地砖、台底、核心设备、边墙、门厅、低掩体、维修台、管线、终端与工业导向标识。保留 submesh 材质；无需外部购买或下载资产。总计 15,532 三角形（不同模块实例化），来源与碰撞说明见 `media/models/crossfire/SOURCE.md`。
- 图元复用的 HUD、世界血条、路线预览、护盾弧/蓄力线、真实枪火与碰撞反馈；显示层微幅悬浮、移动倾斜、130 ms 后座和死亡侧倾不改物理真源。小窗口镜头按可用高度适配。
- RuntimeUiSound 保持 Play 独占旧语义，新增最多 8 路 PlayLayer、声像/增益/优先级；9 个原创短音在发射与碰撞事实后播放。自动验证的 HELLO_AUDIO_SILENT 为独立硬短路，M 不会解除。
- 方向阴影新增可选 farDistance 参数，默认 32 m 兼容旧调用，新远景镜头使用 65 m。头文件、pkg 引用与局部绑定同步；未全量生成 tolua。

## 验证结果与边界

| 项目 | 证据 | 结论 |
|---|---|---|
| macOS Release arm64 | Xcode target HelloOgre3D；最终 build-final.log `BUILD SUCCEEDED` | PASS |
| Lua | Lua 5.1.4，9 个改动/新增入口与动作文件 `luac -p` | PASS |
| 原创资产 | 两个生成器 `--check`；网格法线/切线/绕序/材质引用、音频 PCM16/端点/峰值、确定性重建 | PASS |
| 物理夹具 | friendly/front/rear/single-consumption/core-wall/clear/TTL；7 passed / 0 failed，真实 Bullet，明确 synthetic=true | PASS |
| 自然双翼路线 | 内部输入走正常分发；两台同刻提交，途中暂停时间不推进，替换暂停命令，约 10.065 s 仿真战斗后胜利，连续两次重开并正常退出 | PASS |
| 自然正面路线 | 两台正面攻击均被盾挡，哨卫未受伤，两台先后损失，约 23.859 s 仿真战斗后失败；失败后重开 | PASS |
| 长按离散键 | 内部回放重复 key_down Space/R，只有一次切换/重开；失焦清键复用 InputManager 的 key-up 链 | 合成输入 PASS；物理失焦键盘未实测 |
| 既有入口 | Sandbox6/7/8/19，各 30 s 后台 smoke；Sandbox19 产品 fixture/导航/碰撞标记齐全 | PASS |
| 实机画面 | OpenGL 4.1；1280×800 与 960×720，最终 FSAA=4；观察准备、路线、交战、胜负、重开 | 技术视觉检查 PASS，不代表作者审美认可 |
| 音频后端 | 真头文件语法检查、ASan/UBSan 隔离后端边界/8 路/淘汰/析构；整体 Release 编译 | 静态/隔离 PASS；扬声器听感 NOT RUN |
| Windows | 检查条件分支与兼容调用；本轮无 Windows 桌面运行 | NOT RUN |

实机过程中修掉了地砖近共面闪烁、出生盒体与维修台穿插、远景阴影距离不足、普通 Agent 的调度配置、绑定参数不匹配、初始化前 Update、计划被替换后的残留路径，以及重开保留夹具引用、离散按键自动重复等问题。运行日志仅保留原有 OpenGL `gldCopyBufferSubData: NEEDS IMPLEMENTATION` 单次提示；最终门禁不允许 Lua/OGRE 错误或用超时代替成功。

本地证据（不入库）：`tmp/crossfire-upgrade/`；最终自然回放 `tmp/crossfire-gate-20260919-115258-82auornf/`；物理断言 `tmp/crossfire-gate-20260919-115043-wtsrrotm/physics/`；旧入口 `tmp/m1-smoke-20260919-114734/`。首次 Xcode 模块缓存写入和沙箱 OpenGL 上下文失败均按环境失败记录，已在获准的本机环境重跑成功。

可重复验证：

```bash
python3 tools/run_crossfire_gate.py
python3 tools/run_m1_smoke.py --samples Sandbox6 Sandbox7 Sandbox8 Sandbox19 --seconds 30
python3 tools/generate_crossfire_assets.py --check
python3 tools/generate_crossfire_audio.py --check
bash tools/package_macos_playtest.sh "$PWD/tmp/Crossfire-first-room-20260919-v1" Sandbox20
```

最终结算 HUD 修正后只重跑受影响的包内自然闭环，结果 PASS，截图与摘要位于 `tmp/crossfire-gate-20260919-120337-lbozquj0/`；包内 Lua/模型/声音与当前源文件逐字节一致。

## 交付与后续边界

本地试玩目录 `tmp/Crossfire-first-room-20260919-v1/`，双击其中 `Crossfire.app`；完整 zip 同名。包保留 bin/media 目录结构，附中文操作说明，启动器默认选择 Sandbox20、4× FSAA。macOS 15.6+ / Apple Silicon，临时签名。包内启动器未传 HELLO_SANDBOX_SAMPLE 覆盖，默认 Sandbox20 已实测；自然通关、正面失败与物理夹具三种门禁均 PASS，并验证夹具重开清理。证据：`tmp/crossfire-gate-20260919-120024-5ewi8luu/summary.json`；包内二进制 SHA-256 与本次 Release 完全一致。

这轮交付 A/B 的一个联合房间。未扩展三关、成长系统、商店、关卡编辑器；尚未验证真人键鼠、扬声器听感与 Windows 新路径。作者此前对旧版整体体验不满意的结论仍成立，新样段需要新的实际反馈，P3 不据自动验证勾选。所有改动保留在当前工作区，未经授权未提交或推送。

后续：作者要求继续升级，三场遭遇、选关/设置/记录与场景补充另见[第二轮实施记录](2026-09-19-crossfire-three-encounters.md)。本文保留首包的历史结果。
