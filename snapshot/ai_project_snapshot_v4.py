#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
AI Snapshot Generator (improved, v4)
- One-pass file inventory + second-pass indexing
- Optional config via snapshot/config.json (STRICT JSON only)
- Strip comments for hint scoring (reduce false positives)
- Include edges + resolved includes
- Lua require/dofile edges + C++->Lua hint edges
- Richer manifests (size/mtime/category/module)
- Project snapshot meta + PROJECT_SNAPSHOT.md aggregator
- thirdparty_overview.md (list External/ThirdParty libraries)
- NEW(1): entry_candidates_core.md + entry_candidates_all.md
- NEW(2): dir_summary folds ThirdParty/External prefixes (keeps overview elsewhere)
- NEW(3): manifest/symbols include module2 + module4 (in addition to legacy module)

Drop this file into: <PROJECT_ROOT>/snapshot/
Outputs go to: <PROJECT_ROOT>/snapshot/_ai_snapshot/
"""

import os
import re
import json
import csv
import fnmatch
import time
from collections import defaultdict, Counter


# =========================
# Default Config (can override by snapshot/config.json)
# NOTE: config.json MUST be strict JSON (no comments, no expressions).
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

    # NEW(2): fold these prefixes in dir_summary.md (keep details in thirdparty_overview.md)
    "DIR_SUMMARY_FOLD_PREFIXES": [
        "src/External",
        "src/Engine/ThirdParty",
    ],

    # NEW(1): core candidate filters (for entry_candidates_core.md)
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

    cfg["THIRDPARTY_PREFIXES"] = list(cfg.get("THIRDPARTY_PREFIXES", ["src/External", "src/Engine/ThirdParty"]))
    cfg["THIRDPARTY_LIB_DEPTH"] = int(cfg.get("THIRDPARTY_LIB_DEPTH", 1))
    cfg["THIRDPARTY_LIST_MAX"] = int(cfg.get("THIRDPARTY_LIST_MAX", 200))

    cfg["DIR_SUMMARY_FOLD_PREFIXES"] = list(cfg.get("DIR_SUMMARY_FOLD_PREFIXES", ["src/External", "src/Engine/ThirdParty"]))
    cfg["CORE_PATH_PREFIXES"] = list(cfg.get("CORE_PATH_PREFIXES", ["src/HelloOgre3D/"]))

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


def write_project_snapshot_md(out_dir, meta, top_entries, top_ticks, top_scripts, extra_sections):
    path = os.path.join(out_dir, "PROJECT_SNAPSHOT.md")
    with open(path, "w", encoding="utf-8") as f:
        f.write("# Project Snapshot\n\n")
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
    """
    Fold all directories that are under fold_prefix into one aggregated Counter.
    Also include fold_prefix itself if present.
    """
    agg = Counter()
    for d, c in dir_counts.items():
        d_norm = d.strip("`").strip()
        d_norm = d_norm.strip()
        if d_norm == fold_prefix or is_under_prefix(d_norm, fold_prefix):
            agg.update(c)
    return agg


def is_core_path(rp, core_prefixes):
    for p in core_prefixes:
        if not p:
            continue
        if rp.startswith(p):
            return True
    return False


# =========================
# Main
# =========================
def main():
    SNAPSHOT_DIR = os.path.abspath(os.path.dirname(__file__))
    PROJECT_ROOT = os.path.abspath(os.path.join(SNAPSHOT_DIR, ".."))

    cfg = load_config(SNAPSHOT_DIR)
    OUT_DIR = os.path.join(SNAPSHOT_DIR, cfg["OUT_DIR_NAME"])
    ensure_dir(OUT_DIR)

    # Outputs
    dir_map_path = os.path.join(OUT_DIR, "dir_map.jsonl")
    dir_summary_path = os.path.join(OUT_DIR, "dir_summary.md")
    code_manifest_path = os.path.join(OUT_DIR, "code_manifest.jsonl")
    symbols_path = os.path.join(OUT_DIR, "symbols_min.jsonl")
    edges_path = os.path.join(OUT_DIR, "includes_edges.csv")
    edges_resolved_path = os.path.join(OUT_DIR, "includes_resolved.csv")
    lua_require_edges_path = os.path.join(OUT_DIR, "lua_require_edges.csv")
    cpp_lua_edges_path = os.path.join(OUT_DIR, "cpp_lua_edges.csv")
    snapshot_meta_path = os.path.join(OUT_DIR, "snapshot_meta.json")
    thirdparty_overview_path = os.path.join(OUT_DIR, "thirdparty_overview.md")

    # NEW(1): two candidate lists
    entry_candidates_all_path = os.path.join(OUT_DIR, "entry_candidates_all.md")
    entry_candidates_core_path = os.path.join(OUT_DIR, "entry_candidates_core.md")
    # Backward compatible name (points to ALL)
    entry_candidates_compat_path = os.path.join(OUT_DIR, "entry_candidates.md")

    dir_counts = defaultdict(Counter)
    files_inventory = []  # {rp, full, ext, size_bytes, mtime, category, dir_rp}

    # third-party library counts
    thirdparty_lib_counts = defaultdict(lambda: defaultdict(Counter))
    tp_prefixes = cfg.get("THIRDPARTY_PREFIXES", ["src/External", "src/Engine/ThirdParty"])
    tp_depth = int(cfg.get("THIRDPARTY_LIB_DEPTH", 1))

    # -------------------------
    # Pass 1: walk once -> dir_map + dir_counts + inventory (+ thirdparty stats)
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

    # -------------------------
    # Build include resolution index
    # -------------------------
    by_rel, by_base = build_include_resolution_index(files_inventory, cfg)

    # -------------------------
    # Pass 2: indexing + symbols + edges + candidates
    # -------------------------
    kw_lower = [k.lower() for k in cfg["SYMBOL_KEYWORDS"]]
    include_edges = []
    include_resolved_edges = []
    lua_require_edges = []
    cpp_lua_edges = []

    entry_scores, tick_scores, script_scores = [], [], []
    indexed_files = 0

    with open(code_manifest_path, "w", encoding="utf-8") as cm, \
         open(symbols_path, "w", encoding="utf-8") as sf:

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
                cm.write(json.dumps({
                    "path": rp, "ext": ext,
                    "category": category,
                    "module": module_legacy,
                    "module2": module2,
                    "module4": module4,
                    "lines": 0,
                    "size_bytes": size_bytes, "mtime": mtime,
                    "note": "too_large_skip_read"
                }, ensure_ascii=False) + "\n")
                continue

            try:
                text = safe_read_text_guess(full, max_read)
            except Exception:
                cm.write(json.dumps({
                    "path": rp, "ext": ext,
                    "category": category,
                    "module": module_legacy,
                    "module2": module2,
                    "module4": module4,
                    "lines": 0,
                    "size_bytes": size_bytes, "mtime": mtime,
                    "note": "read_failed"
                }, ensure_ascii=False) + "\n")
                continue

            lines = count_lines(text)
            indexed_files += 1

            cm.write(json.dumps({
                "path": rp, "ext": ext,
                "category": category,
                "module": module_legacy,
                "module2": module2,
                "module4": module4,
                "lines": lines,
                "size_bytes": size_bytes, "mtime": mtime
            }, ensure_ascii=False) + "\n")

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

            if ext in cfg["LUA_EXTS"]:
                for rre in cfg["LUA_REQUIRE_REGEX"]:
                    for m in rre.finditer(text):
                        lua_require_edges.append((rp, "require", m.group(1)))
                for dre in cfg["LUA_DOFILE_REGEX"]:
                    for m in dre.finditer(text):
                        lua_require_edges.append((rp, "dofile", m.group(1)))

            for s in extract_symbols_min(rp, text, ext, kw_lower, cfg):
                sf.write(json.dumps(s, ensure_ascii=False) + "\n")

    # -------------------------
    # Write include edges
    # -------------------------
    with open(edges_path, "w", encoding="utf-8", newline="") as f:
        wcsv = csv.writer(f)
        wcsv.writerow(["from", "include"])
        for fr, inc in include_edges:
            wcsv.writerow([fr, inc])

    with open(edges_resolved_path, "w", encoding="utf-8", newline="") as f:
        wcsv = csv.writer(f)
        wcsv.writerow(["from", "include_raw", "include_resolved"])
        for fr, inc, res in include_resolved_edges:
            wcsv.writerow([fr, inc, res])

    # -------------------------
    # Write Lua / C++->Lua edges
    # -------------------------
    with open(lua_require_edges_path, "w", encoding="utf-8", newline="") as f:
        wcsv = csv.writer(f)
        wcsv.writerow(["from_lua", "kind", "target"])
        for fr, kind, tgt in lua_require_edges:
            wcsv.writerow([fr, kind, tgt])

    with open(cpp_lua_edges_path, "w", encoding="utf-8", newline="") as f:
        wcsv = csv.writer(f)
        wcsv.writerow(["from_cpp", "hint", "target_guess"])
        for fr, hint, tgt in cpp_lua_edges:
            wcsv.writerow([fr, hint, tgt])

    # -------------------------
    # Candidate outputs (NEW(1))
    # -------------------------
    def top_paths(scores, n=120):
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

    top_entry_all = top_paths(entry_scores, n=240)
    top_tick_all = top_paths(tick_scores, n=240)
    top_script_all = top_paths(script_scores, n=240)

    core_prefixes = cfg.get("CORE_PATH_PREFIXES", ["src/HelloOgre3D/"])

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

    def write_candidates_md(path, top_entry, top_tick, top_script, title_suffix=""):
        with open(path, "w", encoding="utf-8") as f:
            f.write("# Entry & Critical Path Candidates\n\n")
            if title_suffix:
                f.write(f"> {title_suffix}\n\n")
            f.write("## Entry points\n")
            for p in top_entry:
                f.write(f"- `{p}`\n")
            f.write("\n## Tick/Update/AI loop\n")
            for p in top_tick:
                f.write(f"- `{p}`\n")
            f.write("\n## Script/Lua/binding\n")
            for p in top_script:
                f.write(f"- `{p}`\n")

    write_candidates_md(
        entry_candidates_all_path,
        top_entry_all, top_tick_all, top_script_all,
        title_suffix="ALL: includes engine/external if they rank high"
    )
    write_candidates_md(
        entry_candidates_core_path,
        top_entry_core, top_tick_core, top_script_core,
        title_suffix=f"CORE: filtered by CORE_PATH_PREFIXES={core_prefixes}"
    )
    # Backward compatible: entry_candidates.md -> ALL
    try:
        # On Windows you may not want symlink; just write a copy.
        # We'll write a copy for maximum compatibility.
        write_candidates_md(
            entry_candidates_compat_path,
            top_entry_all, top_tick_all, top_script_all,
            title_suffix="(compat) same as entry_candidates_all.md"
        )
    except Exception:
        pass

    # -------------------------
    # Directory summary with folding (NEW(2))
    # -------------------------
    fold_prefixes = [p.strip("/").replace("\\", "/") for p in cfg.get("DIR_SUMMARY_FOLD_PREFIXES", []) if p]
    fold_prefixes = sorted(set(fold_prefixes))

    folded_written = set()

    with open(dir_summary_path, "w", encoding="utf-8") as f:
        f.write("# Directory Summary\n\n")
        f.write("每个目录的文件类别统计（不读文件内容、不算大小）。\n")
        if fold_prefixes:
            f.write("\n> Note: some prefixes are folded to keep this file readable.\n\n")
        else:
            f.write("\n")

        # First, write folded blocks (stable order), then write non-folded dirs.
        for fp in fold_prefixes:
            agg = compute_folded_dir_counts(dir_counts, fp)
            if not agg:
                continue
            f.write(f"## `{fp} (folded)`\n")
            format_dir_counts_block(f, agg)
            f.write("\n")
            folded_written.add(fp)

        # Now write all other directories, skipping anything under folded prefixes (and skipping the prefixes themselves)
        for d, c in sorted(dir_counts.items(), key=lambda x: x[0]):
            d_norm = d.strip()
            if any(d_norm == fp for fp in fold_prefixes):
                continue
            if any(is_under_prefix(d_norm, fp) for fp in fold_prefixes):
                continue

            f.write(f"## `{d}`\n")
            format_dir_counts_block(f, c)
            f.write("\n")

    # -------------------------
    # Third-party overview (separate file)
    # -------------------------
    write_thirdparty_overview(thirdparty_overview_path, thirdparty_lib_counts, cfg)

    # -------------------------
    # Meta + PROJECT_SNAPSHOT.md aggregator (includes NEW outputs)
    # -------------------------
    meta = {
        "generated_at": time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()),
        "project_root": PROJECT_ROOT.replace("\\", "/"),
        "snapshot_dir": SNAPSHOT_DIR.replace("\\", "/"),
        "out_dir": OUT_DIR.replace("\\", "/"),
        "total_files": len(files_inventory),
        "indexed_files": indexed_files,

        # ✅ 关键：把“规则/过滤/折叠/核心候选输出”完整写回去
        "rules": {
            # 1) 遍历层面的过滤
            "ignore_dir_names_global": sorted(list(cfg["IGNORE_DIR_NAMES_GLOBAL"])),

            # 2) 索引层面的过滤
            "ignore_dir_names_for_index": sorted(list(cfg["IGNORE_DIR_NAMES_FOR_INDEX"])),
            "ignore_path_globs_for_index": list(cfg["IGNORE_PATH_GLOBS_FOR_INDEX"]),
            "index_exts": sorted(list(cfg["INDEX_EXTS"])),
            "max_read_bytes": int(cfg["MAX_READ_BYTES"]),
            "max_read_bytes_entry_boost": int(cfg["MAX_READ_BYTES_ENTRY_BOOST"]),

            # 3) 候选打分（权重规则 + 关键词）
            "dir_weight_rules": list(cfg.get("DIR_WEIGHT_RULES", [])),
            "symbol_keywords": list(cfg.get("SYMBOL_KEYWORDS", [])),
            "entry_hints": list(cfg.get("ENTRY_HINTS", [])),
            "tick_hints": list(cfg.get("TICK_HINTS", [])),
            "script_hints": list(cfg.get("SCRIPT_HINTS", [])),

            # 4) 目录摘要折叠规则（你这次最想写回 meta 的部分）
            "dir_summary_fold_prefixes": list(cfg.get("DIR_SUMMARY_FOLD_PREFIXES", [])),

            # 5) 第三方概览规则
            "thirdparty_prefixes": list(cfg.get("THIRDPARTY_PREFIXES", [])),
            "thirdparty_lib_depth": int(cfg.get("THIRDPARTY_LIB_DEPTH", 1)),
            "thirdparty_list_max": int(cfg.get("THIRDPARTY_LIST_MAX", 200)),

            # 6) 核心候选过滤规则（如果你有 core/all 两份候选）
            "core_path_prefixes": list(cfg.get("CORE_PATH_PREFIXES", [])),

            # 7) module 粒度（你已经生成 module2/module4，就把这个写回去）
            "module_levels_legacy": int(cfg.get("MODULE_LEVELS", 2)),
            "module_levels_extra": [2, 4],
        },

        # ✅ 让读者知道“这份快照实际产出了哪些文件”
        "outputs": {
            "dir_map": "dir_map.jsonl",
            "dir_summary": "dir_summary.md",
            "thirdparty_overview": "thirdparty_overview.md",
            "code_manifest": "code_manifest.jsonl",
            "symbols_min": "symbols_min.jsonl",
            "includes_edges": "includes_edges.csv",
            "includes_resolved": "includes_resolved.csv",
            "lua_require_edges": "lua_require_edges.csv",
            "cpp_lua_edges": "cpp_lua_edges.csv",

            # 候选文件：如果你当前只产出 entry_candidates.md，也先写这个；
            # 如果你产出 core/all，就把它们都列出来（见下一个改动点）
            "entry_candidates": [
                "entry_candidates.md",
                # "entry_candidates_core.md",
                # "entry_candidates_all.md",
            ],

            "project_snapshot": "PROJECT_SNAPSHOT.md",
            "snapshot_meta": "snapshot_meta.json",
        }
    }

    with open(snapshot_meta_path, "w", encoding="utf-8") as f:
        json.dump(meta, f, ensure_ascii=False, indent=2)

    # For Project Snapshot front page: use CORE candidates (better first impression)
    extra_sections = []
    extra_sections.append(("Key Output Files", [
        f"- `{os.path.basename(dir_map_path)}`: full project map (lightweight)",
        f"- `{os.path.basename(dir_summary_path)}`: directory category summary (folded)",
        f"- `{os.path.basename(thirdparty_overview_path)}`: third-party libraries overview",
        f"- `{os.path.basename(code_manifest_path)}`: indexed file manifest (lines/size/mtime/module,module2,module4)",
        f"- `{os.path.basename(symbols_path)}`: filtered symbols (module,module2,module4)",
        f"- `{os.path.basename(edges_path)}`: include edges (raw)",
        f"- `{os.path.basename(edges_resolved_path)}`: include edges (resolved)",
        f"- `{os.path.basename(lua_require_edges_path)}`: Lua require/dofile edges",
        f"- `{os.path.basename(cpp_lua_edges_path)}`: C++ -> Lua hint edges",
        f"- `{os.path.basename(entry_candidates_core_path)}`: entry candidates (CORE)",
        f"- `{os.path.basename(entry_candidates_all_path)}`: entry candidates (ALL)",
        f"- `{os.path.basename(snapshot_meta_path)}`: snapshot meta/config",
    ]))

    write_project_snapshot_md(
        OUT_DIR,
        meta,
        top_entry_core,
        top_tick_core,
        top_script_core,
        extra_sections
    )

    print("[OK] Snapshot written to:", OUT_DIR)
    print(" - dir_map.jsonl")
    print(" - dir_summary.md (folded)")
    print(" - thirdparty_overview.md")
    print(" - code_manifest.jsonl / symbols_min.jsonl")
    print(" - includes_edges.csv / includes_resolved.csv")
    print(" - lua_require_edges.csv / cpp_lua_edges.csv")
    print(" - entry_candidates_core.md / entry_candidates_all.md / entry_candidates.md")
    print(" - snapshot_meta.json / PROJECT_SNAPSHOT.md")


if __name__ == "__main__":
    main()
