#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
AI Snapshot Generator (lite, v8)
Goal: keep output to 6 files while improving "structure quality".

Outputs (6 files) into: <PROJECT_ROOT>/snapshot/_ai_snapshot/
  1) PROJECT_SNAPSHOT.md   (front page + embedded AI_README_PROMPT.md + outgoing/incoming hotspots + control-flow hypotheses)
  2) entry_candidates.md   (core-first candidates)
  3) index.jsonl           (type=file + type=symbol records)
  4) edges.jsonl           (type=include/include_resolved/lua_require/cpp_lua_hint)
  5) dir_map.jsonl         (full directory map lightweight)
  6) snapshot_meta.json    (rules + outputs + stats)

Place this file in: <PROJECT_ROOT>/snapshot/
Optional: snapshot/config.json (STRICT JSON only)
Optional: snapshot/AI_README_PROMPT.md (will be embedded into PROJECT_SNAPSHOT.md)
"""

import os
import re
import json
import fnmatch
import time
from collections import defaultdict, Counter

# =========================
# Default Config (override by snapshot/config.json)
# =========================
DEFAULT_CONFIG = {
    "OUT_DIR_NAME": "_ai_snapshot",

    # Global ignore (dir_map + indexing both skip traversing these dir names)
    "IGNORE_DIR_NAMES_GLOBAL": [
        ".git", ".svn", ".hg", ".vs",
        ".idea", ".vscode",
        "__pycache__", ".pytest_cache",
        "build", "Build", "out", "Out", "obj", "Obj",
        "Binaries", "Intermediate",
        "snapshot",  # IMPORTANT: ignore snapshot itself to avoid self-including
    ],

    # Index-only prune (deep indexing)
    "IGNORE_DIR_NAMES_FOR_INDEX": [
        ".git", ".svn", ".hg", ".vs", ".idea", ".vscode",
        "__pycache__", ".pytest_cache",
        "build", "Build", "obj", "Obj", "out", "Out",
        "Binaries", "Intermediate", "snapshot",
    ],

    # Index-only path glob prune
    "IGNORE_PATH_GLOBS_FOR_INDEX": [
        "*/ThirdParty/*", "*/third_party/*",
        "*/External/*", "*/external/*",
        "*/vendor/*", "*/deps/*", "*/Dependencies/*",
        "*/SDK/*", "*/Plugins/*",
        "*/libs/*", "*/Libs/*",
        "*/media/*", "*/Media/*", "*/assets/*", "*/Assets/*",
        "*/bin/*.pdb", "*/bin/*.exe", "*/bin/*.dll", "*/bin/*.lib",
    ],

    # Which extensions are eligible for reading/indexing
    "INDEX_EXTS": [
        ".h", ".hpp", ".hh", ".inl",
        ".c", ".cc", ".cpp", ".cxx", ".mm", ".m",
        ".lua",
        ".json", ".xml", ".ini", ".cfg", ".yaml", ".yml",
        ".md", ".txt",
    ],

    "MAX_READ_BYTES": 2 * 1024 * 1024,              # 2MB
    "MAX_READ_BYTES_ENTRY_BOOST": 8 * 1024 * 1024,  # 8MB for likely entry/tick/script files

    # Candidate scoring directory weights
    "DIR_WEIGHT_RULES": [
        {"glob": "src/HelloOgre3D/*", "w": 1.80},
        {"glob": "src/HelloOgre3D/sandbox/*", "w": 1.80},

        {"glob": "src/Engine/*", "w": 0.30},
        {"glob": "src/External/*", "w": 0.30},
        {"glob": "ThirdParty/*", "w": 0.30},
        {"glob": "third_party/*", "w": 0.30},

        {"glob": "src/*", "w": 1.10},
        {"glob": "Source/*", "w": 1.10},
        {"glob": "sandbox/*", "w": 1.10},
        {"glob": "app/*", "w": 1.10},
        {"glob": "game/*", "w": 1.10},

        {"glob": "test/*", "w": 0.60},
        {"glob": "tests/*", "w": 0.60},
        {"glob": "tools/*", "w": 0.70},
        {"glob": "tool/*", "w": 0.70},
    ],

    # Keywords for symbol filtering
    "SYMBOL_KEYWORDS": [
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
    ],

    "ENTRY_HINTS": ["main(", "WinMain(", "SDL_main(", "AppDelegate", "Init", "Startup", "Run(", "Loop"],
    "TICK_HINTS": ["Tick(", "OnTick(", "Update(", "FixedUpdate(", "Render(", "Draw(", "BehaviorTree", "FSM", "StateMachine", "AI"],
    "SCRIPT_HINTS": ["lua", "script", "tolua", "binding", "require(", "dofile(", "__init__", "__tick__"],

    "RESOURCE_EXTS": [".png", ".jpg", ".jpeg", ".webp", ".tga", ".dds", ".mesh", ".material", ".fontdef", ".ttf", ".otf"],
    "BINARY_EXTS": [".pdb", ".exe", ".dll", ".lib", ".a", ".so", ".dylib"],
    "CODE_EXTS": [".h", ".hpp", ".hh", ".inl", ".c", ".cc", ".cpp", ".cxx", ".mm", ".m", ".lua", ".py"],
    "CONFIG_EXTS": [".json", ".xml", ".ini", ".cfg", ".yaml", ".yml", ".toml"],
    "DOC_EXTS": [".md", ".txt", ".rst"],

    "CPP_EXTS": [".h", ".hpp", ".hh", ".inl", ".c", ".cc", ".cpp", ".cxx", ".mm", ".m"],
    "LUA_EXTS": [".lua"],

    "LUA_REQUIRE_REGEX": [
        r'require\s*\(\s*["\']([^"\']+)["\']\s*\)',
        r'require\s+["\']([^"\']+)["\']',
    ],
    "LUA_DOFILE_REGEX": [
        r'dofile\s*\(\s*["\']([^"\']+)["\']\s*\)',
        r'loadfile\s*\(\s*["\']([^"\']+)["\']\s*\)',
    ],

    "CPP_LUA_HINTS": [
        "luaL_dofile", "luaL_loadfile", "dofile", "loadfile",
        "Require", "require", "LoadScript", "RunScript", "DoFile",
        "tolua", "luaopen", "Register", "Bind",
    ],

    # Legacy module levels, plus extra fixed module2/module4
    "MODULE_LEVELS": 2,

    # CORE focus prefixes (used for candidates & frontpage)
    "CORE_PATH_PREFIXES": [
        "src/HelloOgre3D/",
        "bin/res/",
        "premake/",
        "scripts/",
    ],

    # For control-flow hypotheses: prefer names containing these hints (in order)
    "HYP_ENTRY_NAME_HINTS": ["main.cpp", "main(", "WinMain(", "AppDelegate"],
    "HYP_TICK_NAME_HINTS": ["Tick", "OnTick", "Update", "Loop", "Run", "ClientManager", "Game", "World"],
    "HYP_SCRIPT_NAME_HINTS": ["Lua", "Script", "tolua", "binding", "SandboxToLua", "require(", "dofile("],

    # How many hotspots/hypotheses to print
    "TOP_N_OUTGOING": 10,
    "TOP_N_INCOMING": 10,
    "TOP_N_MODULE_INCOMING": 8,
    "HYPOTHESES_COUNT": 3,
}

# =========================
# Regex (lightweight)
# =========================
RE_INCLUDE = re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]', re.MULTILINE)
RE_CLASS = re.compile(r'^\s*(class|struct)\s+([A-Za-z_]\w*)\b', re.MULTILINE)
RE_NAMESPACE = re.compile(r'^\s*namespace\s+([A-Za-z_]\w*)\b', re.MULTILINE)
RE_MACRO = re.compile(r'^\s*#\s*define\s+([A-Za-z_]\w*)\b', re.MULTILINE)

RE_FUNC_DEF_STRICT = re.compile(
    r'^\s*(?:[\w:\<\>\~\*\&]+\s+)+'      # return type-ish
    r'([A-Za-z_]\w*(?:::\w+)*)'          # function name (with scope)
    r'\s*\([^;]*\)\s*'                   # params
    r'(?:const\s*)?(?:noexcept\s*)?\{',  # qualifiers + {
    re.MULTILINE
)

RE_FUNC_DEF_FALLBACK = re.compile(
    r'^\s*([A-Za-z_]\w*(?:::\w+)*)\s*\([^;]*\)\s*\{',
    re.MULTILINE
)

RE_LUA_FUNC = re.compile(r'^\s*function\s+([A-Za-z_]\w*(?:\.[A-Za-z_]\w*)*)\s*\(', re.MULTILINE)
RE_LUA_LOCAL_FUNC = re.compile(r'^\s*local\s+function\s+([A-Za-z_]\w*)\s*\(', re.MULTILINE)

# =========================
# Utils
# =========================
def ensure_dir(p):
    os.makedirs(p, exist_ok=True)

def load_config(snapshot_dir):
    cfg = dict(DEFAULT_CONFIG)
    cfg_path = os.path.join(snapshot_dir, "config.json")
    if os.path.isfile(cfg_path):
        try:
            with open(cfg_path, "r", encoding="utf-8") as f:
                user_cfg = json.load(f)
            for k, v in user_cfg.items():
                cfg[k] = v
        except Exception:
            pass

    # normalize types
    cfg["IGNORE_DIR_NAMES_GLOBAL"] = set(cfg.get("IGNORE_DIR_NAMES_GLOBAL", []))
    cfg["IGNORE_DIR_NAMES_FOR_INDEX"] = set(cfg.get("IGNORE_DIR_NAMES_FOR_INDEX", []))
    cfg["IGNORE_PATH_GLOBS_FOR_INDEX"] = list(cfg.get("IGNORE_PATH_GLOBS_FOR_INDEX", []))

    cfg["INDEX_EXTS"] = set(cfg.get("INDEX_EXTS", []))
    cfg["SYMBOL_KEYWORDS"] = list(cfg.get("SYMBOL_KEYWORDS", []))
    cfg["ENTRY_HINTS"] = list(cfg.get("ENTRY_HINTS", []))
    cfg["TICK_HINTS"] = list(cfg.get("TICK_HINTS", []))
    cfg["SCRIPT_HINTS"] = list(cfg.get("SCRIPT_HINTS", []))

    cfg["RESOURCE_EXTS"] = set(cfg.get("RESOURCE_EXTS", []))
    cfg["BINARY_EXTS"] = set(cfg.get("BINARY_EXTS", []))
    cfg["CODE_EXTS"] = set(cfg.get("CODE_EXTS", []))
    cfg["CONFIG_EXTS"] = set(cfg.get("CONFIG_EXTS", []))
    cfg["DOC_EXTS"] = set(cfg.get("DOC_EXTS", []))
    cfg["CPP_EXTS"] = set(cfg.get("CPP_EXTS", []))
    cfg["LUA_EXTS"] = set(cfg.get("LUA_EXTS", []))

    cfg["LUA_REQUIRE_REGEX"] = [re.compile(x) for x in cfg.get("LUA_REQUIRE_REGEX", [])]
    cfg["LUA_DOFILE_REGEX"] = [re.compile(x) for x in cfg.get("LUA_DOFILE_REGEX", [])]

    cfg["CPP_LUA_HINTS"] = list(cfg.get("CPP_LUA_HINTS", []))
    cfg["DIR_WEIGHT_RULES"] = list(cfg.get("DIR_WEIGHT_RULES", []))

    cfg["MODULE_LEVELS"] = int(cfg.get("MODULE_LEVELS", 2))
    cfg["MAX_READ_BYTES"] = int(cfg.get("MAX_READ_BYTES", 2 * 1024 * 1024))
    cfg["MAX_READ_BYTES_ENTRY_BOOST"] = int(cfg.get("MAX_READ_BYTES_ENTRY_BOOST", 8 * 1024 * 1024))

    cfg["CORE_PATH_PREFIXES"] = list(cfg.get("CORE_PATH_PREFIXES", ["src/HelloOgre3D/"]))
    cfg["HYP_ENTRY_NAME_HINTS"] = list(cfg.get("HYP_ENTRY_NAME_HINTS", []))
    cfg["HYP_TICK_NAME_HINTS"] = list(cfg.get("HYP_TICK_NAME_HINTS", []))
    cfg["HYP_SCRIPT_NAME_HINTS"] = list(cfg.get("HYP_SCRIPT_NAME_HINTS", []))

    cfg["TOP_N_OUTGOING"] = int(cfg.get("TOP_N_OUTGOING", 10))
    cfg["TOP_N_INCOMING"] = int(cfg.get("TOP_N_INCOMING", 10))
    cfg["TOP_N_MODULE_INCOMING"] = int(cfg.get("TOP_N_MODULE_INCOMING", 8))
    cfg["HYPOTHESES_COUNT"] = int(cfg.get("HYPOTHESES_COUNT", 3))

    return cfg

def relpath_unix(p, project_root):
    return os.path.relpath(p, project_root).replace("\\", "/")

def matches_any_glob(rp, globs):
    for g in globs:
        if fnmatch.fnmatch(rp, g):
            return True
    return False

def safe_read_text_guess(path, max_bytes):
    with open(path, "rb") as f:
        data = f.read(max_bytes)
    try:
        text = data.decode("utf-8", errors="replace")
        if text:
            bad = text.count("\ufffd")
            if bad > 0 and (bad / max(1, len(text))) > 0.01:
                text2 = data.decode("latin-1", errors="replace")
                return text2.replace("\r\n", "\n")
        return text.replace("\r\n", "\n")
    except Exception:
        return data.decode("latin-1", errors="replace").replace("\r\n", "\n")

def read_optional_text(path, max_bytes=512*1024):
    try:
        if not os.path.isfile(path):
            return ""
        with open(path, "rb") as f:
            data = f.read(max_bytes)
        try:
            return data.decode("utf-8", errors="replace").replace("\r\n", "\n")
        except Exception:
            return data.decode("latin-1", errors="replace").replace("\r\n", "\n")
    except Exception:
        return ""

def count_lines(text):
    if not text:
        return 0
    return text.count("\n") + (0 if text.endswith("\n") else 1)

def classify_ext(ext, cfg):
    ext = ext.lower()
    if ext in cfg["CODE_EXTS"]:
        return "code"
    if ext in cfg["CONFIG_EXTS"]:
        return "config"
    if ext in cfg["DOC_EXTS"]:
        return "doc"
    if ext in cfg["RESOURCE_EXTS"]:
        return "resource"
    if ext in cfg["BINARY_EXTS"]:
        return "binary"
    if ext == "":
        return "noext"
    return "other"

def module_from_path(rp, levels):
    parts = [p for p in rp.split("/") if p]
    if not parts:
        return "."
    return "/".join(parts[:max(1, int(levels))])

def index_should_skip(rp, dirnames_chain, cfg):
    for d in dirnames_chain:
        if d in cfg["IGNORE_DIR_NAMES_FOR_INDEX"]:
            return True
    if matches_any_glob(rp, cfg["IGNORE_PATH_GLOBS_FOR_INDEX"]):
        return True
    return False

def strip_comments_for_scoring(text, ext, cfg):
    if not text:
        return ""
    if ext in cfg["CPP_EXTS"]:
        text = re.sub(r"/\*.*?\*/", " ", text, flags=re.DOTALL)
        text = re.sub(r"//.*?$", " ", text, flags=re.MULTILINE)
        return text
    if ext in cfg["LUA_EXTS"]:
        text = re.sub(r"--\[\[.*?\]\]", " ", text, flags=re.DOTALL)
        text = re.sub(r"--.*?$", " ", text, flags=re.MULTILINE)
        return text
    return text

def score_for_hints(text_lower, hints):
    return sum(1 for h in hints if h.lower() in text_lower)

def dir_weight(rp, cfg):
    w = 1.0
    for rule in cfg["DIR_WEIGHT_RULES"]:
        g = rule.get("glob", "")
        rw = float(rule.get("w", 1.0))
        if g and fnmatch.fnmatch(rp, g):
            w *= rw
    return w

def is_core_path(rp, core_prefixes):
    for p in core_prefixes:
        if p and rp.startswith(p):
            return True
    return False

def build_include_resolution_index(files_inventory, cfg):
    by_rel = set()
    by_base = defaultdict(list)
    for it in files_inventory:
        rp = it["rp"]
        _, ext = os.path.splitext(rp)
        ext = ext.lower()
        if ext in cfg["CPP_EXTS"]:
            by_rel.add(rp)
            by_base[os.path.basename(rp)].append(rp)
    return by_rel, by_base

def resolve_include(include_raw, by_rel, by_base):
    inc = include_raw.replace("\\", "/").strip()
    if not inc:
        return ""
    if inc in by_rel:
        return inc
    base = os.path.basename(inc)
    cands = by_base.get(base, [])
    if len(cands) == 1:
        return cands[0]
    return ""

def extract_symbols_min(rp, text, ext, kw_lower, cfg):
    out = []

    module_legacy = module_from_path(rp, cfg["MODULE_LEVELS"])
    module2 = module_from_path(rp, 2)
    module4 = module_from_path(rp, 4)

    def keep(name):
        n = name.lower()
        return any(k in n for k in kw_lower)

    def add(kind, name, line):
        if keep(name):
            out.append({
                "type": "symbol",
                "path": rp,
                "kind": kind,
                "name": name,
                "line": line,
                "module": module_legacy,
                "module2": module2,
                "module4": module4,
            })

    if ext in cfg["CPP_EXTS"]:
        for m in RE_CLASS.finditer(text):
            add(m.group(1), m.group(2), text[:m.start()].count("\n") + 1)
        for m in RE_NAMESPACE.finditer(text):
            add("namespace", m.group(1), text[:m.start()].count("\n") + 1)
        for m in RE_MACRO.finditer(text):
            add("macro", m.group(1), text[:m.start()].count("\n") + 1)

        hit_any = False
        for m in RE_FUNC_DEF_STRICT.finditer(text):
            hit_any = True
            add("func_def", m.group(1), text[:m.start()].count("\n") + 1)

        if not hit_any:
            for m in RE_FUNC_DEF_FALLBACK.finditer(text):
                name = m.group(1)
                if name in ("if", "for", "while", "switch", "catch"):
                    continue
                add("func_candidate", name, text[:m.start()].count("\n") + 1)

    elif ext in cfg["LUA_EXTS"]:
        for m in RE_LUA_FUNC.finditer(text):
            add("lua_function", m.group(1), text[:m.start()].count("\n") + 1)
        for m in RE_LUA_LOCAL_FUNC.finditer(text):
            add("lua_local_function", m.group(1), text[:m.start()].count("\n") + 1)

    return out

def choose_by_name_hints(paths, hints):
    """
    Choose a path from list by checking hints in order (case-insensitive).
    Returns (path, matched_hint) or ("","")
    """
    lower_map = [(p, p.lower()) for p in paths]
    for h in hints:
        hl = h.lower()
        for p, pl in lower_map:
            if hl in pl:
                return p, h
    # fallback: first
    return (paths[0], "(fallback_first)") if paths else ("", "")

def top_unique(scores, n):
    scores.sort(key=lambda x: x[0], reverse=True)
    seen = set()
    out = []
    for s, rp in scores:
        if rp in seen:
            continue
        seen.add(rp)
        out.append(rp)
        if len(out) >= n:
            break
    return out

def write_entry_candidates_md(path, top_entry, top_tick, top_script, note):
    with open(path, "w", encoding="utf-8") as f:
        f.write("# Entry & Critical Path Candidates\n\n")
        f.write(f"> {note}\n\n")

        f.write("## Entry points\n")
        for p in top_entry:
            f.write(f"- `{p}`\n")

        f.write("\n## Tick/Update/AI loop\n")
        for p in top_tick:
            f.write(f"- `{p}`\n")

        f.write("\n## Script/Lua/binding\n")
        for p in top_script:
            f.write(f"- `{p}`\n")

def write_project_snapshot_md(
    out_dir,
    meta,
    top_entries,
    top_ticks,
    top_scripts,
    module_symbol_counts,
    core_module_symbol_counts,
    outgoing_hotspots,
    incoming_hotspots,
    incoming_module2_hotspots,
    hypotheses,
    extra_sections,
    prompt_block_text=""
):
    path = os.path.join(out_dir, "PROJECT_SNAPSHOT.md")
    with open(path, "w", encoding="utf-8") as f:
        f.write("# Project Snapshot\n\n")

        # v8: embed prompt block at top if provided
        if prompt_block_text.strip():
            f.write("<!-- AI_README_PROMPT: BEGIN -->\n\n")
            f.write(prompt_block_text.rstrip() + "\n")
            f.write("\n<!-- AI_README_PROMPT: END -->\n\n")
            f.write("---\n\n")

        f.write(f"- Generated at: `{meta.get('generated_at')}`\n")
        f.write(f"- Project root: `{meta.get('project_root')}`\n")
        f.write(f"- Total files: `{meta.get('total_files')}`\n")
        f.write(f"- Indexed files: `{meta.get('indexed_files')}`\n")
        f.write("\n")

        # v8: a gentle focus note
        f.write("> Note: Engine layer is infrastructure; for gameplay logic, focus on `src/HelloOgre3D/` (CORE).\n\n")

        f.write("## Quick Entry Points (Top)\n")
        for p in top_entries[:20]:
            f.write(f"- `{p}`\n")
        f.write("\n## Tick / Update / AI Loop (Top)\n")
        for p in top_ticks[:20]:
            f.write(f"- `{p}`\n")
        f.write("\n## Script / Lua / Binding (Top)\n")
        for p in top_scripts[:20]:
            f.write(f"- `{p}`\n")

        # modules by symbol count
        f.write("\n## Top Modules (by symbol count)\n")
        for m, c in module_symbol_counts:
            f.write(f"- `{m}` symbols={c}\n")

        f.write("\n## CORE Subgraph Summary\n")
        if core_module_symbol_counts:
            f.write("### CORE Modules (by symbol count)\n")
            for m, c in core_module_symbol_counts:
                f.write(f"- `{m}` symbols={c}\n")
        else:
            f.write("- (no CORE modules found)\n")

        # v8: outgoing hotspots
        f.write("\n## Top Edge Hotspots (Outgoing)\n")
        for item in outgoing_hotspots:
            f.write(f"- `{item['path']}` out_edges={item['out_edges']}\n")

        # v8: incoming hotspots (files)
        f.write("\n## Top Incoming Dependency Targets\n")
        for item in incoming_hotspots:
            f.write(f"- `{item['target']}` in_edges={item['in_edges']}\n")

        # v8: incoming hotspots (module2)
        f.write("\n## Top Incoming Modules (module2)\n")
        for item in incoming_module2_hotspots:
            f.write(f"- `{item['module2']}` in_edges={item['in_edges']}\n")

        # v8: control-flow hypotheses
        f.write("\n## Example Control-Flow Hypotheses (heuristic)\n")
        if not hypotheses:
            f.write("- (not enough evidence)\n")
        else:
            for h in hypotheses:
                f.write(f"- {h}\n")

        # extra sections (output files etc.)
        for title, lines in extra_sections:
            f.write(f"\n## {title}\n")
            for line in lines:
                f.write(line + "\n")

# =========================
# Main
# =========================
def main():
    SNAPSHOT_DIR = os.path.abspath(os.path.dirname(__file__))
    PROJECT_ROOT = os.path.abspath(os.path.join(SNAPSHOT_DIR, ".."))

    cfg = load_config(SNAPSHOT_DIR)
    OUT_DIR = os.path.join(SNAPSHOT_DIR, cfg["OUT_DIR_NAME"])
    ensure_dir(OUT_DIR)

    # 6 outputs
    dir_map_path = os.path.join(OUT_DIR, "dir_map.jsonl")
    entry_candidates_path = os.path.join(OUT_DIR, "entry_candidates.md")
    index_path = os.path.join(OUT_DIR, "index.jsonl")
    edges_path = os.path.join(OUT_DIR, "edges.jsonl")
    snapshot_meta_path = os.path.join(OUT_DIR, "snapshot_meta.json")
    project_snapshot_path = os.path.join(OUT_DIR, "PROJECT_SNAPSHOT.md")

    # optional prompt file (in snapshot dir)
    ai_prompt_path = os.path.join(SNAPSHOT_DIR, "AI_README_PROMPT.md")
    ai_prompt_text = read_optional_text(ai_prompt_path, max_bytes=512 * 1024)

    # inventory
    dir_counts = defaultdict(Counter)
    files_inventory = []  # {rp, full, ext, size_bytes, mtime, category}

    # -------------------------
    # Pass 1: walk once -> dir_map + inventory + dir_counts
    # -------------------------
    with open(dir_map_path, "w", encoding="utf-8") as dm:
        for dirpath, dirnames, filenames in os.walk(PROJECT_ROOT):
            rp_dir = relpath_unix(dirpath, PROJECT_ROOT)
            if rp_dir == ".":
                rp_dir = ""

            dirnames[:] = [d for d in dirnames if d not in cfg["IGNORE_DIR_NAMES_GLOBAL"]]
            dm.write(json.dumps({"path": rp_dir or ".", "type": "dir"}, ensure_ascii=False) + "\n")

            for fn in filenames:
                full = os.path.join(dirpath, fn)
                rp = relpath_unix(full, PROJECT_ROOT)
                _, ext = os.path.splitext(fn)
                ext = ext.lower()

                cat = classify_ext(ext, cfg)
                dm.write(json.dumps({"path": rp, "type": "file", "ext": ext, "category": cat}, ensure_ascii=False) + "\n")

                parent = rp_dir or "."
                dir_counts[parent][cat] += 1
                dir_counts[parent]["total"] += 1

                try:
                    st = os.stat(full)
                    size_bytes = int(st.st_size)
                    mtime = int(st.st_mtime)
                except OSError:
                    size_bytes = 0
                    mtime = 0

                files_inventory.append({
                    "rp": rp,
                    "full": full,
                    "ext": ext,
                    "size_bytes": size_bytes,
                    "mtime": mtime,
                    "category": cat,
                })

    # include resolution index
    by_rel, by_base = build_include_resolution_index(files_inventory, cfg)

    # -------------------------
    # Pass 2: indexing -> index.jsonl + edges.jsonl + candidates stats
    # -------------------------
    kw_lower = [k.lower() for k in cfg["SYMBOL_KEYWORDS"]]
    core_prefixes = cfg.get("CORE_PATH_PREFIXES", ["src/HelloOgre3D/"])

    entry_scores, tick_scores, script_scores = [], [], []
    indexed_files = 0

    # stats for v8
    module_symbol_counter = Counter()
    core_module_symbol_counter = Counter()

    outgoing_edge_counter = Counter()          # file -> out edges
    incoming_edge_counter = Counter()          # resolved target -> in edges
    incoming_module2_counter = Counter()       # module2(target) -> in edges

    with open(index_path, "w", encoding="utf-8") as index_f, \
         open(edges_path, "w", encoding="utf-8") as edges_f:

        for it in files_inventory:
            rp = it["rp"]
            full = it["full"]
            ext = it["ext"]

            if ext not in cfg["INDEX_EXTS"]:
                continue

            rp_dir = os.path.dirname(rp).replace("\\", "/")
            chain = [d for d in rp_dir.split("/") if d]
            if index_should_skip(rp, chain, cfg):
                continue

            size_bytes = it["size_bytes"]
            mtime = it["mtime"]
            category = it["category"]

            module_legacy = module_from_path(rp, cfg["MODULE_LEVELS"])
            module2 = module_from_path(rp, 2)
            module4 = module_from_path(rp, 4)

            # decide read size (boost likely important dirs)
            max_read = cfg["MAX_READ_BYTES"]
            w = dir_weight(rp, cfg)
            if w >= 1.12:
                max_read = max(max_read, cfg["MAX_READ_BYTES_ENTRY_BOOST"])

            # record file metadata in index.jsonl
            if size_bytes > max_read:
                index_f.write(json.dumps({
                    "type": "file",
                    "path": rp,
                    "ext": ext,
                    "category": category,
                    "module": module_legacy,
                    "module2": module2,
                    "module4": module4,
                    "lines": 0,
                    "size_bytes": size_bytes,
                    "mtime": mtime,
                    "note": "too_large_skip_read"
                }, ensure_ascii=False) + "\n")
                continue

            try:
                text = safe_read_text_guess(full, max_read)
            except Exception:
                index_f.write(json.dumps({
                    "type": "file",
                    "path": rp,
                    "ext": ext,
                    "category": category,
                    "module": module_legacy,
                    "module2": module2,
                    "module4": module4,
                    "lines": 0,
                    "size_bytes": size_bytes,
                    "mtime": mtime,
                    "note": "read_failed"
                }, ensure_ascii=False) + "\n")
                continue

            indexed_files += 1
            lines = count_lines(text)

            index_f.write(json.dumps({
                "type": "file",
                "path": rp,
                "ext": ext,
                "category": category,
                "module": module_legacy,
                "module2": module2,
                "module4": module4,
                "lines": lines,
                "size_bytes": size_bytes,
                "mtime": mtime
            }, ensure_ascii=False) + "\n")

            # scoring candidates (comment-stripped)
            stripped = strip_comments_for_scoring(text, ext, cfg)
            tl = stripped.lower()

            es = score_for_hints(tl, cfg["ENTRY_HINTS"])
            ts = score_for_hints(tl, cfg["TICK_HINTS"])
            ss = score_for_hints(tl, cfg["SCRIPT_HINTS"])

            if es:
                entry_scores.append((es * w, rp))
            if ts:
                tick_scores.append((ts * w, rp))
            if ss:
                script_scores.append((ss * w, rp))

            # edges
            if ext in cfg["CPP_EXTS"]:
                for m in RE_INCLUDE.finditer(text):
                    inc_raw = m.group(1)
                    resolved = resolve_include(inc_raw, by_rel, by_base)

                    # outgoing counts for this file
                    outgoing_edge_counter[rp] += 1

                    # incoming counts for resolved targets only (avoid noise)
                    if resolved:
                        incoming_edge_counter[resolved] += 1
                        incoming_module2_counter[module_from_path(resolved, 2)] += 1

                    edges_f.write(json.dumps({
                        "type": "include_resolved" if resolved else "include",
                        "from": rp,
                        "include_raw": inc_raw,
                        "include_resolved": resolved
                    }, ensure_ascii=False) + "\n")

                # C++ -> Lua hint (light heuristic)
                for h in cfg["CPP_LUA_HINTS"]:
                    if h.lower() in tl:
                        q = re.search(r'["\']([^"\']+\.lua|[^"\']+)["\']', text)
                        target = q.group(1) if q else ""
                        edges_f.write(json.dumps({
                            "type": "cpp_lua_hint",
                            "from": rp,
                            "hint": h,
                            "target_guess": target
                        }, ensure_ascii=False) + "\n")
                        break

            elif ext in cfg["LUA_EXTS"]:
                for rre in cfg["LUA_REQUIRE_REGEX"]:
                    for m in rre.finditer(text):
                        edges_f.write(json.dumps({
                            "type": "lua_require",
                            "from": rp,
                            "kind": "require",
                            "target": m.group(1)
                        }, ensure_ascii=False) + "\n")
                for dre in cfg["LUA_DOFILE_REGEX"]:
                    for m in dre.finditer(text):
                        edges_f.write(json.dumps({
                            "type": "lua_require",
                            "from": rp,
                            "kind": "dofile",
                            "target": m.group(1)
                        }, ensure_ascii=False) + "\n")

            # symbols -> index.jsonl
            symbols = extract_symbols_min(rp, text, ext, kw_lower, cfg)
            for s in symbols:
                index_f.write(json.dumps(s, ensure_ascii=False) + "\n")
                # update module symbol stats
                module_symbol_counter[s["module2"]] += 1
                if is_core_path(s["path"], core_prefixes):
                    core_module_symbol_counter[s["module2"]] += 1

    # -------------------------
    # Candidates: write entry_candidates.md (core-first)
    # -------------------------
    top_entry_all = top_unique(entry_scores, n=240)
    top_tick_all = top_unique(tick_scores, n=240)
    top_script_all = top_unique(script_scores, n=240)

    def filter_core(paths, n=120):
        out = []
        for p in paths:
            if is_core_path(p, core_prefixes):
                out.append(p)
            if len(out) >= n:
                break
        return out

    top_entry_core = filter_core(top_entry_all, n=120)
    top_tick_core = filter_core(top_tick_all, n=120)
    top_script_core = filter_core(top_script_all, n=120)

    # if core is empty (for some repos), fallback
    top_entries_for_front = top_entry_core if top_entry_core else top_entry_all[:120]
    top_ticks_for_front = top_tick_core if top_tick_core else top_tick_all[:120]
    top_scripts_for_front = top_script_core if top_script_core else top_script_all[:120]

    write_entry_candidates_md(
        entry_candidates_path,
        top_entries_for_front,
        top_ticks_for_front,
        top_scripts_for_front,
        note=f"v8 lite: CORE-first (CORE_PATH_PREFIXES={core_prefixes})"
    )

    # -------------------------
    # v8: outgoing/incoming hotspots
    # -------------------------
    top_outgoing = outgoing_edge_counter.most_common(cfg["TOP_N_OUTGOING"])
    outgoing_hotspots = [{"path": p, "out_edges": c} for p, c in top_outgoing]

    top_incoming = incoming_edge_counter.most_common(cfg["TOP_N_INCOMING"])
    incoming_hotspots = [{"target": p, "in_edges": c} for p, c in top_incoming]

    top_incoming_module2 = incoming_module2_counter.most_common(cfg["TOP_N_MODULE_INCOMING"])
    incoming_module2_hotspots = [{"module2": m, "in_edges": c} for m, c in top_incoming_module2]

    # -------------------------
    # v8: control-flow hypotheses (heuristic + evidence)
    # -------------------------
    hypotheses = []
    # Build pools for picking
    pool_entry = top_entries_for_front[:60]
    pool_tick = top_ticks_for_front[:60]
    pool_script = top_scripts_for_front[:60]

    # choose canonical ones
    entry_pick, entry_hit = choose_by_name_hints(pool_entry, cfg["HYP_ENTRY_NAME_HINTS"])
    tick_pick, tick_hit = choose_by_name_hints(pool_tick, cfg["HYP_TICK_NAME_HINTS"])
    script_pick, script_hit = choose_by_name_hints(pool_script, cfg["HYP_SCRIPT_NAME_HINTS"])

    if entry_pick and tick_pick and script_pick:
        hypotheses.append(
            f"`{entry_pick}` -> `{tick_pick}` -> `{script_pick}` "
            f"(evidence: entry_hint={entry_hit}, tick_hint={tick_hit}, script_hint={script_hit})"
        )

    # produce up to N hypotheses using slight variations (by shifting pick index)
    def nth_or_empty(lst, idx):
        return lst[idx] if 0 <= idx < len(lst) else ""

    for i in range(1, cfg["HYPOTHESES_COUNT"]):
        e = nth_or_empty(pool_entry, i)
        t = nth_or_empty(pool_tick, i)
        s = nth_or_empty(pool_script, i)
        if e and t and s:
            hypotheses.append(f"`{e}` -> `{t}` -> `{s}` (evidence: candidates rank≈{i+1})")

    # -------------------------
    # Meta + PROJECT_SNAPSHOT.md
    # -------------------------
    meta = {
        "generated_at": time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()),
        "project_root": PROJECT_ROOT.replace("\\", "/"),
        "snapshot_dir": SNAPSHOT_DIR.replace("\\", "/"),
        "out_dir": OUT_DIR.replace("\\", "/"),
        "total_files": len(files_inventory),
        "indexed_files": indexed_files,
        "rules": {
            "ignore_dir_names_global": sorted(list(cfg["IGNORE_DIR_NAMES_GLOBAL"])),
            "ignore_dir_names_for_index": sorted(list(cfg["IGNORE_DIR_NAMES_FOR_INDEX"])),
            "ignore_path_globs_for_index": list(cfg["IGNORE_PATH_GLOBS_FOR_INDEX"]),
            "index_exts": sorted(list(cfg["INDEX_EXTS"])),
            "max_read_bytes": int(cfg["MAX_READ_BYTES"]),
            "max_read_bytes_entry_boost": int(cfg["MAX_READ_BYTES_ENTRY_BOOST"]),
            "dir_weight_rules": list(cfg.get("DIR_WEIGHT_RULES", [])),
            "symbol_keywords": list(cfg.get("SYMBOL_KEYWORDS", [])),
            "entry_hints": list(cfg.get("ENTRY_HINTS", [])),
            "tick_hints": list(cfg.get("TICK_HINTS", [])),
            "script_hints": list(cfg.get("SCRIPT_HINTS", [])),
            "core_path_prefixes": list(cfg.get("CORE_PATH_PREFIXES", [])),
            "module_levels_legacy": int(cfg.get("MODULE_LEVELS", 2)),
            "module_levels_extra": [2, 4],
            "v8_additions": [
                "edges.jsonl merged (include/include_resolved/lua_require/cpp_lua_hint)",
                "index.jsonl merged (file+symbol records)",
                "PROJECT_SNAPSHOT.md adds incoming hotspots + module2 incoming + control-flow hypotheses",
                "PROJECT_SNAPSHOT.md embeds AI_README_PROMPT.md if exists"
            ]
        },
        "outputs": {
            "PROJECT_SNAPSHOT.md": "PROJECT_SNAPSHOT.md",
            "entry_candidates.md": "entry_candidates.md",
            "index.jsonl": "index.jsonl",
            "edges.jsonl": "edges.jsonl",
            "dir_map.jsonl": "dir_map.jsonl",
            "snapshot_meta.json": "snapshot_meta.json"
        }
    }

    with open(snapshot_meta_path, "w", encoding="utf-8") as f:
        json.dump(meta, f, ensure_ascii=False, indent=2)

    # module lists (top by symbol count)
    module_symbol_counts = module_symbol_counter.most_common(12)
    core_module_symbol_counts = core_module_symbol_counter.most_common(12)

    extra_sections = []
    extra_sections.append(("Key Output Files", [
        "- `PROJECT_SNAPSHOT.md`: front page (with AI prompt + hotspots + hypotheses)",
        "- `entry_candidates.md`: CORE-first entry/tick/script candidates",
        "- `index.jsonl`: file records + filtered symbols (module2/module4)",
        "- `edges.jsonl`: include/include_resolved + Lua edges + C++->Lua hints",
        "- `dir_map.jsonl`: full directory map (lightweight)",
        "- `snapshot_meta.json`: config/rules/stats",
        "",
        "Tip: If you need deeper analysis on a submodule, generate an extra focused snapshot on that sub-tree only."
    ]))

    write_project_snapshot_md(
        OUT_DIR,
        meta,
        top_entries_for_front,
        top_ticks_for_front,
        top_scripts_for_front,
        module_symbol_counts,
        core_module_symbol_counts,
        outgoing_hotspots,
        incoming_hotspots,
        incoming_module2_hotspots,
        hypotheses,
        extra_sections,
        prompt_block_text=ai_prompt_text
    )

    print("[OK] v8 snapshot written to:", OUT_DIR)
    print(" - PROJECT_SNAPSHOT.md")
    print(" - entry_candidates.md")
    print(" - index.jsonl")
    print(" - edges.jsonl")
    print(" - dir_map.jsonl")
    print(" - snapshot_meta.json")

if __name__ == "__main__":
    main()
