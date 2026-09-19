# Crossfire UI 字库

运行资源为 `crossfire.gorilla` 与 `crossfire.png`，独立于原 `fonts/dejavu/dejavu`。Windows/macOS 读取相同的 PNG，无系统字体依赖。原 atlas 的 ASCII 字体 9/14/24/91/141/241 及白像素完整保留。

中文与统一界面拉丁字符来自 **Noto Sans SC Regular 2.004**，版权 Adobe，SIL Open Font License 1.1，见 LICENSE-Noto-OFL.txt。上游发布：https://github.com/notofonts/noto-cjk/tree/Sans2.004/Sans 。初次生成使用本机已安装的该版本 TTF，字体 name table 的版本和许可证已核对；没有使用 Apple 系统字体。DejaVu 原 atlas 像素继续遵循 LICENSE-DejaVu.txt。

`CrossfireUI-Subset.ttf` 是改名后的 OFL 字体子集，仅用于离线重建。它包含本次 Lua 界面文案及 ASCII；游戏不加载该 TTF。生成 PNG 与派生字体同属 OFL 授权资源。不要以此名称覆盖上游字体。

## 重建

需 Python 3、Pillow 和 fonttools。在项目根目录执行：

```
python3 tools/generate_crossfire_font.py
```

脚本扫描 `bin/res/scripts/samples/Sandbox20.lua` 与 `crossfire_*.lua`（排除生成映射本身），校验每个非 ASCII 字符均可绘制。新文案若引入子集中不存在的字，生成会失败并列出缺字；此时传完整 Noto Sans SC 2.004 字体：

```
python3 tools/generate_crossfire_font.py --font /path/to/NotoSansSC-Regular.ttf
```

三个输出必须一起更新：本目录的 `.gorilla` / `.png` 与 `bin/res/scripts/samples/crossfire_hud_glyphs.lua`。生成器从字体实际 advance 生成 ASCII 测量表。每个中文字号使用若干 printable-ASCII banks；Lua text helper 按 UTF-8 码点转换成 Gorilla `%@font%` 标记并转义百分号，所以 Gorilla 不会遇到有符号 UTF-8 字节。所有 bank 都完整定义 33–126，确保切换字体所需的 `%` / `@` 在当前 bank 范围内。

字号 9/14/24 对应新的界面像素字号 16/20/32、行高 20/26/40。HUD 保存原始文案与排版缓存，内容变化才重写 markup。未知字在运行中显示问号；发布前必须运行生成器验证覆盖，不能把问号作为译文。

测试应包括真实窗口中文字形、长句换行、960×720布局、modal遮罩和下方3D点击阻断。离线 mock/图集检查不能替代实际 GPU 渲染。
