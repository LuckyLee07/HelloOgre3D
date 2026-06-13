---
name: crlf-encoding-gotcha
description: 新建带中文注释的 .h/.cpp 必须 UTF-8 with BOM + CRLF，否则 MSVC 按 GBK 误读爆 C2447
metadata:
  type: feedback
---

本仓用 MSVC 编译，源文件**带大量中文注释**。**新建任何带中文注释的 .h/.cpp，写完必须确保编码是 UTF-8 with BOM + CRLF**。Write/Edit 工具默认产出 UTF-8 无 BOM + LF，MSVC 会按系统 ANSI（中文 Windows = GBK/CP936）解码，中文 UTF-8 字节在 GBK 视角下会"吃掉" `{` `}` `\` 等 ASCII 字节，破坏代码结构。

常见症状：`error C2447: "{"：缺少函数标题` 出现在某 .h 上方（紧跟一段中文注释末尾的 `{`），后续连锁 `未声明的标识符`/`未定义基类`/`C1004 EOF`。**实测**：Phase 1 P0 修复、Phase 3 新建 13 个 behavior/ 文件、本次新建 `PerceptionResultCache.h` 都踩过。

**How to apply:** 报这类错先怀疑编码、别怀疑代码逻辑（99% 是新文件没 BOM）。批量转码：

```powershell
$dir = "<目标目录绝对路径>"
$utf8Bom = New-Object System.Text.UTF8Encoding($true)
Get-ChildItem -Path $dir -Recurse -Include *.h,*.cpp | ForEach-Object {
    $c = [System.IO.File]::ReadAllText($_.FullName)
    $c = ($c -replace "`r`n","`n") -replace "`n","`r`n"
    [System.IO.File]::WriteAllText($_.FullName, $c, $utf8Bom)
}
```
验证 BOM：读前 3 字节应为 `0xEF 0xBB 0xBF`。

不要做的：
- **不要**用"避免在 .h 里写中文注释/改 ASCII"绕过——项目本来就大量中文注释，改英文反而风格不一致。正解是 BOM + 转码。
- Edit 改老文件不破坏已有 BOM（保留原编码），所以只需对**新建**文件转一次。
- `git diff --check` 行尾检查只是**提示不是证据**；要查时逐文件确认 CRLF/LF/CR/BOM，嵌套仓单独审。
