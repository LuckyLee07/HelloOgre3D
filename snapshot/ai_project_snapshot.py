#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import re
import json
import csv
import fnmatch
from collections import defaultdict, Counter

# =========================
# 配置区（直接运行版）
# =========================
# 当前脚本在 snapshot/ 下
SNAPSHOT_DIR = os.path.abspath(os.path.dirname(__file__))

# 项目根目录 = snapshot 的上一级
PROJECT_ROOT = os.path.abspath(os.path.join(SNAPSHOT_DIR, ".."))

# 输出目录仍然在 snapshot 里
OUT_DIR = os.path.join(SNAPSHOT_DIR, "_ai_snapshot")

IGNORE_DIR_NAMES_GLOBAL = {".git", ".svn", ".hg", "snapshot"}

# 目录名剪枝（不影响“目录地图”的统计，只影响“深度索引”）
IGNORE_DIR_NAMES_FOR_INDEX = {
    ".git", ".svn", ".hg", ".idea", ".vscode",
    "__pycache__", ".pytest_cache",
    "build", "Build", "obj", "Obj", "out", "Out",
    "Binaries", "Intermediate", "snapshot"
}

# 深度索引忽略（第三方/资源/二进制大概率没必要读内容）
IGNORE_PATH_GLOBS_FOR_INDEX = [
    "*/ThirdParty/*", "*/third_party/*", "*/external/*",
    "*/vendor/*", "*/deps/*", "*/Dependencies/*",
    "*/SDK/*", "*/Plugins/*",
    "*/libs/*", "*/Libs/*",
    "*/media/*", "*/Media/*", "*/assets/*", "*/Assets/*",
    "*/bin/*.pdb", "*/bin/*.exe", "*/bin/*.dll", "*/bin/*.lib",
]

# 深度索引（读取内容/抽符号）的白名单
INDEX_EXTS = {
    ".h", ".hpp", ".hh", ".inl",
    ".c", ".cc", ".cpp", ".cxx", ".mm", ".m",
    ".lua",
    ".json", ".xml", ".ini", ".cfg", ".yaml", ".yml",
    ".md", ".txt",
}

MAX_READ_BYTES = 2 * 1024 * 1024  # 2MB

SYMBOL_KEYWORDS = [
    "main", "WinMain", "AppDelegate",
    "Init", "Initialize", "Startup", "Run", "Loop",
    "Tick", "OnTick", "Update", "FixedUpdate", "LateUpdate",
    "Render", "Draw", "Frame",
    "AI", "Agent", "Controller",
    "FSM", "State", "StateMachine",
    "BehaviorTree", "BTree", "BT", "Task", "Evaluator",
    "Event", "Dispatcher", "Message",
    "Load", "Reload",
    "Lua", "Script", "tolua", "binding", "Require",
]

ENTRY_HINTS = ["main(", "WinMain(", "SDL_main(", "AppDelegate", "Init", "Startup", "Run(", "Loop"]
TICK_HINTS  = ["Tick(", "OnTick(", "Update(", "FixedUpdate(", "Render(", "Draw(", "BehaviorTree", "FSM", "StateMachine", "AI"]
SCRIPT_HINTS = ["lua", "script", "tolua", "binding", "require(", "dofile(", "__init__", "__tick__"]

# 文件类型分类（用于目录地图）
RESOURCE_EXTS = {".png", ".jpg", ".jpeg", ".webp", ".tga", ".dds", ".mesh", ".material", ".fontdef", ".ttf", ".otf"}
BINARY_EXTS   = {".pdb", ".exe", ".dll", ".lib", ".a", ".so", ".dylib"}
CODE_EXTS     = {".h", ".hpp", ".hh", ".inl", ".c", ".cc", ".cpp", ".cxx", ".mm", ".m", ".lua", ".py"}
CONFIG_EXTS   = {".json", ".xml", ".ini", ".cfg", ".yaml", ".yml", ".toml"}
DOC_EXTS      = {".md", ".txt", ".rst"}

CPP_EXTS = {".h", ".hpp", ".hh", ".inl", ".c", ".cc", ".cpp", ".cxx", ".mm", ".m"}
LUA_EXTS = {".lua"}

# =========================
# Regex（轻量）
# =========================
RE_INCLUDE   = re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]', re.MULTILINE)
RE_CLASS     = re.compile(r'^\s*(class|struct)\s+([A-Za-z_]\w*)\b', re.MULTILINE)
RE_NAMESPACE = re.compile(r'^\s*namespace\s+([A-Za-z_]\w*)\b', re.MULTILINE)
RE_MACRO     = re.compile(r'^\s*#\s*define\s+([A-Za-z_]\w*)\b', re.MULTILINE)
RE_FUNC_DEF  = re.compile(
    r'^\s*(?:[\w:\<\>\~\*\&]+\s+)+'     
    r'([A-Za-z_]\w*(?:::\w+)*)'
    r'\s*\([^;]*\)\s*'
    r'(?:const\s*)?(?:noexcept\s*)?\{',
    re.MULTILINE
)
RE_LUA_FUNC = re.compile(r'^\s*function\s+([A-Za-z_]\w*(?:\.[A-Za-z_]\w*)*)\s*\(', re.MULTILINE)
RE_LUA_LOCAL_FUNC = re.compile(r'^\s*local\s+function\s+([A-Za-z_]\w*)\s*\(', re.MULTILINE)

# =========================
# Utils
# =========================
def ensure_dir(p): os.makedirs(p, exist_ok=True)

def relpath_unix(p):
    return os.path.relpath(p, PROJECT_ROOT).replace("\\", "/")

def matches_any_glob(rp, globs):
    for g in globs:
        if fnmatch.fnmatch(rp, g):
            return True
    return False

def safe_read_text(path, max_bytes):
    with open(path, "rb") as f:
        data = f.read(max_bytes)
    return data.decode("utf-8", errors="replace")

def count_lines(text):
    if not text:
        return 0
    return text.count("\n") + (0 if text.endswith("\n") else 1)

def score_for_hints(text_lower, hints):
    return sum(1 for h in hints if h.lower() in text_lower)

def classify_ext(ext):
    ext = ext.lower()
    if ext in CODE_EXTS: return "code"
    if ext in CONFIG_EXTS: return "config"
    if ext in DOC_EXTS: return "doc"
    if ext in RESOURCE_EXTS: return "resource"
    if ext in BINARY_EXTS: return "binary"
    if ext == "": return "noext"
    return "other"

def index_should_skip(rp, dirnames_chain):
    # 目录名剪枝（索引时）
    for d in dirnames_chain:
        if d in IGNORE_DIR_NAMES_FOR_INDEX:
            return True
    # 路径 glob 剪枝（索引时）
    if matches_any_glob(rp, IGNORE_PATH_GLOBS_FOR_INDEX):
        return True
    return False

def extract_symbols_min(rp, text, ext, kw_lower):
    out = []

    def keep(name):
        n = name.lower()
        return any(k in n for k in kw_lower)

    def add(kind, name, line):
        if keep(name):
            out.append({"path": rp, "kind": kind, "name": name, "line": line})

    if ext in CPP_EXTS:
        for m in RE_CLASS.finditer(text):
            add(m.group(1), m.group(2), text[:m.start()].count("\n") + 1)
        for m in RE_NAMESPACE.finditer(text):
            add("namespace", m.group(1), text[:m.start()].count("\n") + 1)
        for m in RE_MACRO.finditer(text):
            add("macro", m.group(1), text[:m.start()].count("\n") + 1)
        for m in RE_FUNC_DEF.finditer(text):
            add("func_def", m.group(1), text[:m.start()].count("\n") + 1)

    elif ext in LUA_EXTS:
        for m in RE_LUA_FUNC.finditer(text):
            add("lua_function", m.group(1), text[:m.start()].count("\n") + 1)
        for m in RE_LUA_LOCAL_FUNC.finditer(text):
            add("lua_local_function", m.group(1), text[:m.start()].count("\n") + 1)

    return out

# =========================
# Main
# =========================
def main():
    ensure_dir(OUT_DIR)

    dir_map_path = os.path.join(OUT_DIR, "dir_map.jsonl")
    dir_summary_path = os.path.join(OUT_DIR, "dir_summary.md")
    code_manifest_path = os.path.join(OUT_DIR, "code_manifest.jsonl")
    symbols_path = os.path.join(OUT_DIR, "symbols_min.jsonl")
    edges_path = os.path.join(OUT_DIR, "includes_edges.csv")
    entry_path = os.path.join(OUT_DIR, "entry_candidates.md")

    # 目录统计：每个目录 -> 分类计数
    dir_counts = defaultdict(Counter)

    # 全量目录地图输出（轻量）
    with open(dir_map_path, "w", encoding="utf-8") as dm:
        for dirpath, dirnames, filenames in os.walk(PROJECT_ROOT):
            rp_dir = relpath_unix(dirpath)
            if rp_dir == ".": rp_dir = ""
            
            dirnames[:] = [d for d in dirnames if d not in IGNORE_DIR_NAMES_GLOBAL]
            # 目录节点
            dm.write(json.dumps({"path": rp_dir or ".", "type": "dir"}, ensure_ascii=False) + "\n")

            # 文件节点（只写路径+分类，不读内容，不算大小）
            for fn in filenames:
                full = os.path.join(dirpath, fn)
                rp = relpath_unix(full)
                _, ext = os.path.splitext(fn)
                ext = ext.lower()
                cat = classify_ext(ext)

                dm.write(json.dumps({"path": rp, "type": "file", "ext": ext, "category": cat}, ensure_ascii=False) + "\n")

                # 统计到父目录
                parent = rp_dir or "."
                dir_counts[parent][cat] += 1
                dir_counts[parent]["total"] += 1

    # 深度索引输出
    kw_lower = [k.lower() for k in SYMBOL_KEYWORDS]
    include_edges = []
    entry_scores, tick_scores, script_scores = [], [], []

    with open(code_manifest_path, "w", encoding="utf-8") as cm, \
         open(symbols_path, "w", encoding="utf-8") as sf:

        for dirpath, dirnames, filenames in os.walk(PROJECT_ROOT):
            rp_dir = relpath_unix(dirpath)
            if rp_dir == ".": rp_dir = ""
            chain = [d for d in rp_dir.split("/") if d]  # 用于目录名剪枝（索引）

            for fn in filenames:
                full = os.path.join(dirpath, fn)
                rp = relpath_unix(full)
                _, ext = os.path.splitext(fn)
                ext = ext.lower()

                # 只索引白名单
                if ext not in INDEX_EXTS:
                    continue

                # 索引剪枝（第三方/资源/二进制/构建产物）
                if index_should_skip(rp, chain):
                    continue

                try:
                    size_bytes = os.stat(full).st_size
                except OSError:
                    continue

                # 读取内容（限制大小）
                text = ""
                lines = 0
                if size_bytes <= MAX_READ_BYTES:
                    text = safe_read_text(full, MAX_READ_BYTES)
                    lines = count_lines(text)
                else:
                    # 太大就不读，只记录清单
                    cm.write(json.dumps({"path": rp, "ext": ext, "lines": 0, "note": "too_large_skip_read"}, ensure_ascii=False) + "\n")
                    continue

                cm.write(json.dumps({"path": rp, "ext": ext, "lines": lines}, ensure_ascii=False) + "\n")

                tl = text.lower()
                es = score_for_hints(tl, ENTRY_HINTS)
                ts = score_for_hints(tl, TICK_HINTS)
                ss = score_for_hints(tl, SCRIPT_HINTS)
                if es: entry_scores.append((es, rp))
                if ts: tick_scores.append((ts, rp))
                if ss: script_scores.append((ss, rp))

                # include edges
                if ext in CPP_EXTS:
                    for m in RE_INCLUDE.finditer(text):
                        include_edges.append((rp, m.group(1)))

                # symbols (filtered)
                for s in extract_symbols_min(rp, text, ext, kw_lower):
                    sf.write(json.dumps(s, ensure_ascii=False) + "\n")

    # 写 include edges
    with open(edges_path, "w", encoding="utf-8", newline="") as f:
        w = csv.writer(f)
        w.writerow(["from", "include"])
        for fr, inc in include_edges:
            w.writerow([fr, inc])

    # 写 entry candidates
    def top_paths(scores, n=120):
        scores.sort(reverse=True)
        return [rp for s, rp in scores[:n]]

    with open(entry_path, "w", encoding="utf-8") as f:
        f.write("# Entry & Critical Path Candidates\n\n")
        f.write("## Entry points\n")
        for p in top_paths(entry_scores):
            f.write(f"- `{p}`\n")
        f.write("\n## Tick/Update/AI loop\n")
        for p in top_paths(tick_scores):
            f.write(f"- `{p}`\n")
        f.write("\n## Script/Lua/binding\n")
        for p in top_paths(script_scores):
            f.write(f"- `{p}`\n")

    # 写目录摘要
    with open(dir_summary_path, "w", encoding="utf-8") as f:
        f.write("# Directory Summary\n\n")
        f.write("每个目录的文件类别统计（不读文件内容、不算大小）。\n\n")
        for d, c in sorted(dir_counts.items(), key=lambda x: x[0]):
            f.write(f"## `{d}`\n")
            # 按重要类别顺序输出
            for cat in ["total", "code", "config", "doc", "resource", "binary", "other", "noext"]:
                if c.get(cat, 0):
                    f.write(f"- {cat}: {c[cat]}\n")
            f.write("\n")

    print("[OK] Snapshot written to:", OUT_DIR)
    print(" - dir_map.jsonl (full project map, lightweight)")
    print(" - dir_summary.md (directory category summary)")
    print(" - code_manifest.jsonl / symbols_min.jsonl / includes_edges.csv / entry_candidates.md")


if __name__ == "__main__":
    main()
