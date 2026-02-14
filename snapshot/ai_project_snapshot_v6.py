#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
AI Snapshot Generator (v6, based on v5)

Goals:
- Keep v5 capabilities (dir_map/dir_summary fold/thirdparty_overview/manifest/symbols/edges/candidates/meta/project_snapshot)
- Add stronger summaries without increasing lite output count:
  - PROJECT_SNAPSHOT.md: Top Modules by symbols + Top Edge hotspots + CORE Subgraph Summary
  - entry_candidates.md: annotated score/lines/module2/hints
- Fix ZIP recursion explosion (never include the zip itself)
- Support OUTPUT_PROFILE: lite (6 files) / full (legacy extra files)

Drop this file into: <PROJECT_ROOT>/snapshot/
Outputs go to: <PROJECT_ROOT>/snapshot/_ai_snapshot/
Optional zip goes to: <PROJECT_ROOT>/snapshot/ (NOT inside _ai_snapshot to avoid recursion)
"""

import os
import re
import json
import csv
import fnmatch
import time
import zipfile
from collections import defaultdict, Counter

# =========================
# Default Config (can override by snapshot/config.json)
# NOTE: config.json MUST be strict JSON (no comments, no expressions).
# =========================
DEFAULT_CONFIG = {
    # output
    "OUT_DIR_NAME": "_ai_snapshot",

    # lite|full
    # - lite: output only 6 files for web LLM
    # - full: output legacy files too (compatible with v5)
    "OUTPUT_PROFILE": "lite",

    # zip for full profile
    "MAKE_FULL_ZIP": True,
    "FULL_ZIP_NAME": "snapshot_full.zip",

    # Global ignore (dir_map + indexing both skip traversing these dir names)
    "IGNORE_DIR_NAMES_GLOBAL": [
        ".git", ".svn", ".hg", ".vs",
        ".idea", ".vscode",
        "__pycache__", ".pytest_cache",
        "build", "Build", "out", "Out", "obj", "Obj",
        "Binaries", "Intermediate",
        "snapshot",
    ],

    # Index-only prune (only affects deep indexing)
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

    # Legacy "module" field in manifests/symbols
    "MODULE_LEVELS": 2,

    # Third-party overview output (separate file)
    "THIRDPARTY_PREFIXES": [
        "src/External",
        "src/Engine/ThirdParty",
    ],
    "THIRDPARTY_LIB_DEPTH": 1,
    "THIRDPARTY_LIST_MAX": 200,

    # Fold these prefixes in dir_summary.md (keep details in thirdparty_overview.md)
    "DIR_SUMMARY_FOLD_PREFIXES": [
        "src/External",
        "src/Engine/ThirdParty",
    ],

    # CORE filter for candidates + CORE subgraph summary
    "CORE_PATH_PREFIXES": [
        "src/HelloOgre3D/",
        "bin/res/",
        "premake/",
        "scripts/",
    ],
}

# =========================
# Regex（lightweight）
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

    cfg["THIRDPARTY_PREFIXES"] = list(cfg.get("THIRDPARTY_PREFIXES", ["src/External", "src/Engine/ThirdParty"]))
    cfg["THIRDPARTY_LIB_DEPTH"] = int(cfg.get("THIRDPARTY_LIB_DEPTH", 1))
    cfg["THIRDPARTY_LIST_MAX"] = int(cfg.get("THIRDPARTY_LIST_MAX", 200))
    cfg["DIR_SUMMARY_FOLD_PREFIXES"] = list(cfg.get("DIR_SUMMARY_FOLD_PREFIXES", ["src/External", "src/Engine/ThirdParty"]))
    cfg["CORE_PATH_PREFIXES"] = list(cfg.get("CORE_PATH_PREFIXES", ["src/HelloOgre3D/"]))

    cfg["OUTPUT_PROFILE"] = str(cfg.get("OUTPUT_PROFILE", "lite")).lower()
    if cfg["OUTPUT_PROFILE"] not in ("lite", "full"):
        cfg["OUTPUT_PROFILE"] = "lite"
    cfg["MAKE_FULL_ZIP"] = bool(cfg.get("MAKE_FULL_ZIP", True))
    cfg["FULL_ZIP_NAME"] = str(cfg.get("FULL_ZIP_NAME", "snapshot_full.zip"))

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

def match_prefix(rp, prefixes):
    for p in prefixes:
        p = p.strip("/")
        if rp == p or rp.startswith(p + "/"):
            return p
    return None

def get_lib_path(rp, prefix, depth):
    rest = rp[len(prefix):].lstrip("/")
    if not rest:
        return prefix
    parts = rest.split("/")
    depth = max(1, int(depth))
    head = parts[:depth]
    return prefix + "/" + "/".join(head)

def write_thirdparty_overview(path, thirdparty_lib_counts, cfg):
    tp_prefixes = cfg.get("THIRDPARTY_PREFIXES", ["src/External", "src/Engine/ThirdParty"])
    max_list = int(cfg.get("THIRDPARTY_LIST_MAX", 200))

    with open(path, "w", encoding="utf-8") as f:
        f.write("# Third-Party Overview\n\n")
        f.write("按第三方根目录统计“库清单（到一层/两层目录） + 文件类别计数”。\n")
        f.write("用于让 AI 快速知道项目依赖了哪些第三方库。\n\n")

        for prefix in tp_prefixes:
            libs = thirdparty_lib_counts.get(prefix, {})
            f.write(f"## `{prefix}`\n\n")
            if not libs:
                f.write("- (no files matched)\n\n")
                continue

            items = list(libs.items())
            items.sort(key=lambda kv: kv[1].get("total", 0), reverse=True)

            f.write("| Library | total | code | config | doc | resource | binary | other | noext |\n")
            f.write("|---|---:|---:|---:|---:|---:|---:|---:|---:|\n")
            for lib_path, c in items[:max_list]:
                f.write(
                    f"| `{lib_path}` | {c.get('total',0)} | {c.get('code',0)} | {c.get('config',0)} | "
                    f"{c.get('doc',0)} | {c.get('resource',0)} | {c.get('binary',0)} | {c.get('other',0)} | {c.get('noext',0)} |\n"
                )
            f.write("\n")

def is_under_prefix(path, prefix):
    prefix = prefix.strip("/")
    path = path.strip("/")
    if not prefix:
        return False
    return path != prefix and path.startswith(prefix + "/")

def format_dir_counts_block(f, c):
    for cat in ["total", "code", "config", "doc", "resource", "binary", "other", "noext"]:
        if c.get(cat, 0):
            f.write(f"- {cat}: {c[cat]}\n")

def compute_folded_dir_counts(dir_counts, fold_prefix):
    agg = Counter()
    for d, c in dir_counts.items():
        d_norm = d.strip("`").strip()
        if d_norm == fold_prefix or is_under_prefix(d_norm, fold_prefix):
            agg.update(c)
    return agg

def norm_prefixes(prefixes):
    out = []
    for p in prefixes or []:
        p = (p or "").strip().replace("\\", "/")
        if not p:
            continue
        if not p.endswith("/"):
            p += "/"
        out.append(p)
    return out

def is_core_path_any(rp, core_prefixes_norm):
    rp = rp.replace("\\", "/")
    for p in core_prefixes_norm:
        if rp.startswith(p):
            return True
    return False

def jsonl_write(fp, obj):
    fp.write(json.dumps(obj, ensure_ascii=False) + "\n")

def zip_dir_safe(zip_path, src_dir):
    # Important: do NOT place zip inside src_dir, and always skip itself.
    zip_abs = os.path.abspath(zip_path).replace("\\", "/")
    src_abs = os.path.abspath(src_dir).replace("\\", "/")
    with zipfile.ZipFile(zip_path, "w", compression=zipfile.ZIP_DEFLATED) as z:
        for root, _, files in os.walk(src_dir):
            for fn in files:
                full = os.path.abspath(os.path.join(root, fn)).replace("\\", "/")
                if full == zip_abs:
                    continue
                rel = os.path.relpath(full, src_abs).replace("\\", "/")
                z.write(full, rel)

def read_optional_text(path, max_bytes=512*1024):
    """
    Read a text file if exists. Return '' if missing or failed.
    max_bytes avoids accidentally embedding huge files.
    """
    try:
        if not os.path.isfile(path):
            return ""
        with open(path, "rb") as f:
            data = f.read(max_bytes)
        # keep it simple: assume utf-8; fallback latin-1
        try:
            return data.decode("utf-8", errors="replace").replace("\r\n", "\n")
        except Exception:
            return data.decode("latin-1", errors="replace").replace("\r\n", "\n")
    except Exception:
        return ""

def write_project_snapshot_md(out_dir, meta, top_entries, top_ticks, top_scripts, extra_sections, prompt_block_text=""):
    path = os.path.join(out_dir, "PROJECT_SNAPSHOT.md")
    with open(path, "w", encoding="utf-8") as f:
        f.write("# Project Snapshot\n\n")

        # ✅ v7: prepend AI read guide block if provided
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

        f.write("## Quick Entry Points (Top)\n")
        for p in top_entries[:20]:
            f.write(f"- `{p}`\n")
        f.write("\n## Tick / Update / AI Loop (Top)\n")
        for p in top_ticks[:20]:
            f.write(f"- `{p}`\n")
        f.write("\n## Script / Lua / Binding (Top)\n")
        for p in top_scripts[:20]:
            f.write(f"- `{p}`\n")

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
    profile = cfg["OUTPUT_PROFILE"]
    is_full = (profile == "full")

    OUT_DIR = os.path.join(SNAPSHOT_DIR, cfg["OUT_DIR_NAME"])
    ensure_dir(OUT_DIR)

    # LITE: 6 files only
    lite_files = {
        "dir_map": os.path.join(OUT_DIR, "dir_map.jsonl"),
        "index": os.path.join(OUT_DIR, "index.jsonl"),
        "edges": os.path.join(OUT_DIR, "edges.jsonl"),
        "candidates": os.path.join(OUT_DIR, "entry_candidates.md"),
        "snapshot": os.path.join(OUT_DIR, "PROJECT_SNAPSHOT.md"),
        "meta": os.path.join(OUT_DIR, "snapshot_meta.json"),
    }

    # Legacy (FULL only)
    legacy_paths = {
        "dir_summary": os.path.join(OUT_DIR, "dir_summary.md"),
        "thirdparty_overview": os.path.join(OUT_DIR, "thirdparty_overview.md"),
        "code_manifest": os.path.join(OUT_DIR, "code_manifest.jsonl"),
        "symbols_min": os.path.join(OUT_DIR, "symbols_min.jsonl"),
        "includes_edges": os.path.join(OUT_DIR, "includes_edges.csv"),
        "includes_resolved": os.path.join(OUT_DIR, "includes_resolved.csv"),
        "lua_require_edges": os.path.join(OUT_DIR, "lua_require_edges.csv"),
        "cpp_lua_edges": os.path.join(OUT_DIR, "cpp_lua_edges.csv"),
    }

    # Stats
    dir_counts = defaultdict(Counter)
    files_inventory = []  # {rp, full, ext, size_bytes, mtime, category, dir_rp}

    thirdparty_lib_counts = defaultdict(lambda: defaultdict(Counter))
    tp_prefixes = cfg.get("THIRDPARTY_PREFIXES", ["src/External", "src/Engine/ThirdParty"])
    tp_depth = int(cfg.get("THIRDPARTY_LIB_DEPTH", 1))

    # =========================
    # Pass 1: walk once -> dir_map + dir_counts + inventory (+ thirdparty stats)
    # =========================
    with open(lite_files["dir_map"], "w", encoding="utf-8") as dm:
        for dirpath, dirnames, filenames in os.walk(PROJECT_ROOT):
            rp_dir = relpath_unix(dirpath, PROJECT_ROOT)
            if rp_dir == ".":
                rp_dir = ""

            dirnames[:] = [d for d in dirnames if d not in cfg["IGNORE_DIR_NAMES_GLOBAL"]]
            jsonl_write(dm, {"path": rp_dir or ".", "type": "dir"})

            for fn in filenames:
                full = os.path.join(dirpath, fn)
                rp = relpath_unix(full, PROJECT_ROOT)
                _, ext = os.path.splitext(fn)
                ext = ext.lower()

                cat = classify_ext(ext, cfg)
                jsonl_write(dm, {"path": rp, "type": "file", "ext": ext, "category": cat})

                parent = rp_dir or "."
                dir_counts[parent][cat] += 1
                dir_counts[parent]["total"] += 1

                fp = match_prefix(rp, tp_prefixes)
                if fp:
                    libp = get_lib_path(rp, fp, tp_depth)
                    thirdparty_lib_counts[fp][libp][cat] += 1
                    thirdparty_lib_counts[fp][libp]["total"] += 1

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
                    "dir_rp": parent,
                })

    # =========================
    # Build include resolution index
    # =========================
    by_rel, by_base = build_include_resolution_index(files_inventory, cfg)

    # =========================
    # Pass 2: indexing + symbols + edges + candidates (and build lite index/edges)
    # =========================
    kw_lower = [k.lower() for k in cfg["SYMBOL_KEYWORDS"]]

    include_edges = []
    include_resolved_edges = []
    lua_edges = []          # (from, kind, target)
    cpp_lua_edges = []      # (from, hint, target_guess)

    # candidates
    entry_scores = []
    tick_scores = []
    script_scores = []

    # v6: per-file meta for annotated candidates + summaries
    file_meta = {}  # path -> {lines,module2,es,ts,ss}
    module2_symbol_counts = Counter()  # module2 -> symbol count

    indexed_files = 0

    # lite outputs
    ix = open(lite_files["index"], "w", encoding="utf-8")
    eg = open(lite_files["edges"], "w", encoding="utf-8")

    # full legacy outputs
    cm = None
    sf = None
    if is_full:
        cm = open(legacy_paths["code_manifest"], "w", encoding="utf-8")
        sf = open(legacy_paths["symbols_min"], "w", encoding="utf-8")

    try:
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

            max_read = cfg["MAX_READ_BYTES"]
            w = dir_weight(rp, cfg)
            if w >= 1.12:
                max_read = max(max_read, cfg["MAX_READ_BYTES_ENTRY_BOOST"])

            if size_bytes > max_read:
                # lite index: still write the file record (so LLM knows it exists and is big)
                jsonl_write(ix, {
                    "type": "file",
                    "path": rp, "ext": ext,
                    "category": category,
                    "module": module_legacy,
                    "module2": module2,
                    "module4": module4,
                    "lines": 0,
                    "size_bytes": size_bytes, "mtime": mtime,
                    "note": "too_large_skip_read"
                })
                if cm:
                    jsonl_write(cm, {
                        "path": rp, "ext": ext,
                        "category": category,
                        "module": module_legacy,
                        "module2": module2,
                        "module4": module4,
                        "lines": 0,
                        "size_bytes": size_bytes, "mtime": mtime,
                        "note": "too_large_skip_read"
                    })
                continue

            try:
                text = safe_read_text_guess(full, max_read)
            except Exception:
                jsonl_write(ix, {
                    "type": "file",
                    "path": rp, "ext": ext,
                    "category": category,
                    "module": module_legacy,
                    "module2": module2,
                    "module4": module4,
                    "lines": 0,
                    "size_bytes": size_bytes, "mtime": mtime,
                    "note": "read_failed"
                })
                if cm:
                    jsonl_write(cm, {
                        "path": rp, "ext": ext,
                        "category": category,
                        "module": module_legacy,
                        "module2": module2,
                        "module4": module4,
                        "lines": 0,
                        "size_bytes": size_bytes, "mtime": mtime,
                        "note": "read_failed"
                    })
                continue

            lines = count_lines(text)
            indexed_files += 1

            file_rec = {
                "type": "file",
                "path": rp, "ext": ext,
                "category": category,
                "module": module_legacy,
                "module2": module2,
                "module4": module4,
                "lines": lines,
                "size_bytes": size_bytes, "mtime": mtime
            }
            jsonl_write(ix, file_rec)
            if cm:
                jsonl_write(cm, dict(file_rec, **{"path": rp, "type": None}) if False else {
                    "path": rp, "ext": ext,
                    "category": category,
                    "module": module_legacy,
                    "module2": module2,
                    "module4": module4,
                    "lines": lines,
                    "size_bytes": size_bytes, "mtime": mtime
                })

            stripped = strip_comments_for_scoring(text, ext, cfg)
            tl = stripped.lower()

            es = score_for_hints(tl, cfg["ENTRY_HINTS"])
            ts = score_for_hints(tl, cfg["TICK_HINTS"])
            ss = score_for_hints(tl, cfg["SCRIPT_HINTS"])

            file_meta[rp] = {"lines": lines, "module2": module2, "es": es, "ts": ts, "ss": ss}

            if es:
                entry_scores.append((es * w, rp))
            if ts:
                tick_scores.append((ts * w, rp))
            if ss:
                script_scores.append((ss * w, rp))

            # C++ edges
            if ext in cfg["CPP_EXTS"]:
                for m in RE_INCLUDE.finditer(text):
                    inc = m.group(1)
                    include_edges.append((rp, inc))
                    resolved = resolve_include(inc, by_rel, by_base)
                    include_resolved_edges.append((rp, inc, resolved))

                for h in cfg["CPP_LUA_HINTS"]:
                    if h.lower() in tl:
                        q = re.search(r'["\']([^"\']+\.lua|[^"\']+)["\']', text)
                        target = q.group(1) if q else ""
                        cpp_lua_edges.append((rp, h, target))
                        break

            # Lua edges
            if ext in cfg["LUA_EXTS"]:
                for rre in cfg["LUA_REQUIRE_REGEX"]:
                    for m in rre.finditer(text):
                        lua_edges.append((rp, "require", m.group(1)))
                for dre in cfg["LUA_DOFILE_REGEX"]:
                    for m in dre.finditer(text):
                        lua_edges.append((rp, "dofile", m.group(1)))

            # Symbols (filtered)
            for s in extract_symbols_min(rp, text, ext, kw_lower, cfg):
                sym = dict(s)
                sym["type"] = "symbol"
                jsonl_write(ix, sym)
                module2_symbol_counts[sym.get("module2", ".")] += 1
                if sf:
                    jsonl_write(sf, s)

        # Write unified edges.jsonl (lite)
        for fr, inc in include_edges:
            jsonl_write(eg, {"type": "edge", "kind": "cpp_include_raw", "from": fr, "to": inc})
        for fr, inc, res in include_resolved_edges:
            if res:
                jsonl_write(eg, {"type": "edge", "kind": "cpp_include_resolved", "from": fr, "to": res, "raw": inc})
            else:
                jsonl_write(eg, {"type": "edge", "kind": "cpp_include_unresolved", "from": fr, "to": "", "raw": inc})
        for fr, kind, tgt in lua_edges:
            jsonl_write(eg, {"type": "edge", "kind": "lua_" + kind, "from": fr, "to": tgt})
        for fr, hint, tgt in cpp_lua_edges:
            jsonl_write(eg, {"type": "edge", "kind": "cpp_to_lua_hint", "from": fr, "to": tgt, "hint": hint})

    finally:
        ix.close()
        eg.close()
        if cm:
            cm.close()
        if sf:
            sf.close()

    # =========================
    # FULL legacy outputs: CSV edges + dir_summary + thirdparty_overview
    # =========================
    if is_full:
        # include edges CSV
        with open(legacy_paths["includes_edges"], "w", encoding="utf-8", newline="") as f:
            wcsv = csv.writer(f)
            wcsv.writerow(["from", "include"])
            for fr, inc in include_edges:
                wcsv.writerow([fr, inc])

        with open(legacy_paths["includes_resolved"], "w", encoding="utf-8", newline="") as f:
            wcsv = csv.writer(f)
            wcsv.writerow(["from", "include_raw", "include_resolved"])
            for fr, inc, res in include_resolved_edges:
                wcsv.writerow([fr, inc, res])

        with open(legacy_paths["lua_require_edges"], "w", encoding="utf-8", newline="") as f:
            wcsv = csv.writer(f)
            wcsv.writerow(["from_lua", "kind", "target"])
            for fr, kind, tgt in lua_edges:
                wcsv.writerow([fr, kind, tgt])

        with open(legacy_paths["cpp_lua_edges"], "w", encoding="utf-8", newline="") as f:
            wcsv = csv.writer(f)
            wcsv.writerow(["from_cpp", "hint", "target_guess"])
            for fr, hint, tgt in cpp_lua_edges:
                wcsv.writerow([fr, hint, tgt])

        # dir_summary with folding
        fold_prefixes = [p.strip("/").replace("\\", "/") for p in cfg.get("DIR_SUMMARY_FOLD_PREFIXES", []) if p]
        fold_prefixes = sorted(set(fold_prefixes))

        with open(legacy_paths["dir_summary"], "w", encoding="utf-8") as f:
            f.write("# Directory Summary\n\n")
            f.write("每个目录的文件类别统计（不读文件内容、不算大小）。\n")
            if fold_prefixes:
                f.write("\n> Note: some prefixes are folded to keep this file readable.\n\n")
            else:
                f.write("\n")

            for fp in fold_prefixes:
                agg = compute_folded_dir_counts(dir_counts, fp)
                if not agg:
                    continue
                f.write(f"## `{fp} (folded)`\n")
                format_dir_counts_block(f, agg)
                f.write("\n")

            for d, c in sorted(dir_counts.items(), key=lambda x: x[0]):
                d_norm = d.strip()
                if any(d_norm == fp for fp in fold_prefixes):
                    continue
                if any(is_under_prefix(d_norm, fp) for fp in fold_prefixes):
                    continue

                f.write(f"## `{d}`\n")
                format_dir_counts_block(f, c)
                f.write("\n")

        # thirdparty overview
        write_thirdparty_overview(legacy_paths["thirdparty_overview"], thirdparty_lib_counts, cfg)

    # =========================
    # Candidates (v6 annotated, CORE + ALL in ONE entry_candidates.md)
    # =========================
    def top_items(scores, n=240):
        scores.sort(key=lambda x: x[0], reverse=True)
        seen = set()
        out = []
        for s, rp in scores:
            if rp in seen:
                continue
            seen.add(rp)
            out.append((s, rp))
            if len(out) >= n:
                break
        return out

    top_entry_all = top_items(entry_scores, n=240)
    top_tick_all = top_items(tick_scores, n=240)
    top_script_all = top_items(script_scores, n=240)

    core_prefixes_norm = norm_prefixes(cfg.get("CORE_PATH_PREFIXES", []))

    def filter_core(items, n=120):
        out = []
        for s, p in items:
            if is_core_path_any(p, core_prefixes_norm):
                out.append((s, p))
            if len(out) >= n:
                break
        return out

    top_entry_core = filter_core(top_entry_all, n=120)
    top_tick_core = filter_core(top_tick_all, n=120)
    top_script_core = filter_core(top_script_all, n=120)

    def fmt_item(score, path):
        m = file_meta.get(path, {})
        lines = m.get("lines", 0)
        module2 = m.get("module2", ".")
        es = m.get("es", 0)
        ts = m.get("ts", 0)
        ss = m.get("ss", 0)
        return f"- `{path}` (score={score:.2f}, lines={lines}, module2=`{module2}`, hints: entry={es}, tick={ts}, script={ss})"

    def write_list(f, title, items, annotate_top=40):
        f.write(f"## {title}\n")
        for i, (s, p) in enumerate(items):
            if i < annotate_top:
                f.write(fmt_item(s, p) + "\n")
            else:
                f.write(f"- `{p}`\n")
        f.write("\n")

    with open(lite_files["candidates"], "w", encoding="utf-8") as f:
        f.write("# Entry & Critical Path Candidates (v6)\n\n")
        f.write(f"> CORE_PATH_PREFIXES={cfg.get('CORE_PATH_PREFIXES', [])}\n\n")

        write_list(f, "CORE: Entry points", top_entry_core)
        write_list(f, "CORE: Tick/Update/AI loop", top_tick_core)
        write_list(f, "CORE: Script/Lua/binding", top_script_core)

        f.write("---\n\n")
        write_list(f, "ALL: Entry points", top_entry_all)
        write_list(f, "ALL: Tick/Update/AI loop", top_tick_all)
        write_list(f, "ALL: Script/Lua/binding", top_script_all)

    # =========================
    # Strong summaries (Top Modules / Edge Hotspots / CORE Subgraph)
    # =========================
    out_deg = Counter()
    edge_type_counts = Counter()

    for fr, _ in include_edges:
        out_deg[fr] += 1
        edge_type_counts["cpp_include_raw"] += 1
    for fr, _, _ in include_resolved_edges:
        out_deg[fr] += 1
        edge_type_counts["cpp_include_resolved_or_unresolved"] += 1
    for fr, kind, _ in lua_edges:
        out_deg[fr] += 1
        edge_type_counts["lua_" + kind] += 1
    for fr, _, _ in cpp_lua_edges:
        out_deg[fr] += 1
        edge_type_counts["cpp_to_lua_hint"] += 1

    # CORE subgraph
    core_out_deg = Counter()
    core_edge_type_counts = Counter()
    core_module2_file_counts = Counter()
    core_module2_hint_strength = Counter()

    for rp, m in file_meta.items():
        if is_core_path_any(rp, core_prefixes_norm):
            mod2 = m.get("module2", ".")
            core_module2_file_counts[mod2] += 1
            core_module2_hint_strength[mod2] += int(m.get("es", 0)) + int(m.get("ts", 0)) + int(m.get("ss", 0))

    for fr, inc in include_edges:
        if is_core_path_any(fr, core_prefixes_norm):
            core_out_deg[fr] += 1
            core_edge_type_counts["cpp_include_raw"] += 1
    for fr, inc, res in include_resolved_edges:
        if is_core_path_any(fr, core_prefixes_norm):
            core_out_deg[fr] += 1
            core_edge_type_counts["cpp_include_resolved_or_unresolved"] += 1
    for fr, kind, tgt in lua_edges:
        if is_core_path_any(fr, core_prefixes_norm):
            core_out_deg[fr] += 1
            core_edge_type_counts["lua_" + kind] += 1
    for fr, hint, tgt in cpp_lua_edges:
        if is_core_path_any(fr, core_prefixes_norm):
            core_out_deg[fr] += 1
            core_edge_type_counts["cpp_to_lua_hint"] += 1

    # =========================
    # Meta + PROJECT_SNAPSHOT.md
    # =========================
    outputs_lite = [
        os.path.basename(lite_files["dir_map"]),
        os.path.basename(lite_files["index"]),
        os.path.basename(lite_files["edges"]),
        os.path.basename(lite_files["candidates"]),
        os.path.basename(lite_files["snapshot"]),
        os.path.basename(lite_files["meta"]),
    ]
    outputs_full_extra = []
    if is_full:
        outputs_full_extra = [
            os.path.basename(legacy_paths["dir_summary"]),
            os.path.basename(legacy_paths["thirdparty_overview"]),
            os.path.basename(legacy_paths["code_manifest"]),
            os.path.basename(legacy_paths["symbols_min"]),
            os.path.basename(legacy_paths["includes_edges"]),
            os.path.basename(legacy_paths["includes_resolved"]),
            os.path.basename(legacy_paths["lua_require_edges"]),
            os.path.basename(legacy_paths["cpp_lua_edges"]),
        ]

    meta = {
        "generated_at": time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()),
        "profile": profile,
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
            "dir_summary_fold_prefixes": list(cfg.get("DIR_SUMMARY_FOLD_PREFIXES", [])),
            "thirdparty_prefixes": list(cfg.get("THIRDPARTY_PREFIXES", [])),
            "thirdparty_lib_depth": int(cfg.get("THIRDPARTY_LIB_DEPTH", 1)),
            "thirdparty_list_max": int(cfg.get("THIRDPARTY_LIST_MAX", 200)),
            "core_path_prefixes": list(cfg.get("CORE_PATH_PREFIXES", [])),
            "module_levels_legacy": int(cfg.get("MODULE_LEVELS", 2)),
            "module_levels_extra": [2, 4],
        },

        "outputs": {
            "lite": outputs_lite,
            "full_extra": outputs_full_extra
        }
    }

    with open(lite_files["meta"], "w", encoding="utf-8") as f:
        json.dump(meta, f, ensure_ascii=False, indent=2)

    # Build extra sections
    extra_sections = []

    # Key files for lite
    extra_sections.append(("Key Output Files (Lite 6)", [
        f"- `{os.path.basename(lite_files['dir_map'])}`: full project map (lightweight)",
        f"- `{os.path.basename(lite_files['index'])}`: file manifest + filtered symbols (jsonl)",
        f"- `{os.path.basename(lite_files['edges'])}`: unified edges (include/require/hint) (jsonl)",
        f"- `{os.path.basename(lite_files['candidates'])}`: entry/tick/script candidates (annotated)",
        f"- `{os.path.basename(lite_files['meta'])}`: snapshot meta/config/rules",
    ]))

    # Top third-party libs (from pass1 stats) - keep in PROJECT_SNAPSHOT even in lite
    tp_lines = []
    for prefix in tp_prefixes:
        libs = thirdparty_lib_counts.get(prefix, {})
        if not libs:
            continue
        items = list(libs.items())
        items.sort(key=lambda kv: kv[1].get("total", 0), reverse=True)
        tp_lines.append(f"- `{prefix}` top libs:")
        for lib_path, c in items[:20]:
            tp_lines.append(f"  - `{lib_path}` total={c.get('total',0)} code={c.get('code',0)} config={c.get('config',0)} doc={c.get('doc',0)}")
    if tp_lines:
        extra_sections.append(("Top Third-Party Libraries (summary)", tp_lines))

    # Top Modules by symbols
    mod_lines = []
    for mod, cnt in module2_symbol_counts.most_common(20):
        mod_lines.append(f"- `{mod}` symbols={cnt}")
    if mod_lines:
        extra_sections.append(("Top Modules (by symbol count)", mod_lines))

    # Top Edge hotspots
    hot_lines = []
    for p, cnt in out_deg.most_common(20):
        m = file_meta.get(p, {})
        hot_lines.append(f"- `{p}` out_edges={cnt} lines={m.get('lines',0)} module2=`{m.get('module2','.')}`")
    if hot_lines:
        extra_sections.append(("Top Edge Hotspots (most dependencies)", hot_lines))

    # CORE Subgraph Summary
    core_lines = []
    core_lines.append(f"- CORE_PATH_PREFIXES={cfg.get('CORE_PATH_PREFIXES', [])}")

    if core_edge_type_counts:
        core_lines.append("- Edge types (CORE-only):")
        for k, v in core_edge_type_counts.most_common(10):
            core_lines.append(f"  - {k}: {v}")

    core_lines.append("- Top CORE edge hotspots (from-files with most outgoing edges):")
    for p, cnt in core_out_deg.most_common(20):
        m = file_meta.get(p, {})
        core_lines.append(
            f"  - `{p}` out_edges={cnt} lines={m.get('lines',0)} module2=`{m.get('module2','.')}` "
            f"hints(entry={m.get('es',0)},tick={m.get('ts',0)},script={m.get('ss',0)})"
        )

    core_lines.append("- Top CORE modules (module2) by file count & hint strength:")
    items = list(core_module2_file_counts.items())
    items.sort(key=lambda kv: (kv[1], core_module2_hint_strength.get(kv[0], 0)), reverse=True)
    for mod2, fc in items[:20]:
        hs = core_module2_hint_strength.get(mod2, 0)
        core_lines.append(f"  - `{mod2}` files={fc} hint_strength={hs}")

    extra_sections.append(("CORE Subgraph Summary (quick map)", core_lines))

    # For front page: use CORE candidates paths
    top_entry_core_paths = [p for _, p in top_entry_core]
    top_tick_core_paths = [p for _, p in top_tick_core]
    top_script_core_paths = [p for _, p in top_script_core]

    ai_prompt_path = os.path.join(SNAPSHOT_DIR, "AI_README_PROMPT.md")
    ai_prompt_text = read_optional_text(ai_prompt_path, max_bytes=512*1024)

    write_project_snapshot_md(
        OUT_DIR,
        meta,
        top_entry_core_paths,
        top_tick_core_paths,
        top_script_core_paths,
        extra_sections,
        prompt_block_text=ai_prompt_text
    )

    # =========================
    # FULL: safe zip (outside OUT_DIR to avoid recursion)
    # =========================
    if is_full and cfg.get("MAKE_FULL_ZIP", True):
        zip_path = os.path.join(SNAPSHOT_DIR, cfg.get("FULL_ZIP_NAME", "snapshot_full.zip"))
        # Ensure zip is NOT inside OUT_DIR; if user configured it, still safe due to skip-self, but better to keep outside.
        zip_dir_safe(zip_path, OUT_DIR)
        print("[OK] Full zip:", zip_path)

    print("[OK] Snapshot v6 written to:", OUT_DIR)
    print("Profile:", profile)
    print("Lite outputs:", ", ".join(outputs_lite))
    if is_full:
        print("Full extra outputs:", ", ".join(outputs_full_extra))


if __name__ == "__main__":
    main()
