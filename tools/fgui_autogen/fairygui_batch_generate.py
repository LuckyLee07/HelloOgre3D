import argparse
import json
import os
import re
import subprocess
import sys

import fairygui_asset_manifest


def normalize_path(path):
	return path.replace("\\", "/")


def abs_norm(path):
	return os.path.normcase(os.path.abspath(path))


def find_asset_dirs(assets_root):
	if not os.path.isdir(assets_root):
		return []
	result = []
	for name in sorted(os.listdir(assets_root)):
		path = os.path.join(assets_root, name)
		if not os.path.isdir(path):
			continue
		if name == "fairygui_manifests":
			continue
		if os.path.exists(os.path.join(path, "package.xml")):
			result.append(path)
	return result


def load_json(path):
	with open(path, "r", encoding="utf-8-sig") as file:
		return json.load(file)


def find_existing_manifest(asset_dir, manifest_dir):
	if not os.path.isdir(manifest_dir):
		return None
	for name in sorted(os.listdir(manifest_dir)):
		if not name.endswith(".json"):
			continue
		path = os.path.join(manifest_dir, name)
		manifest = load_json(path)
		manifest_asset = manifest.get("assetDir")
		if manifest_asset and abs_norm(manifest_asset) == abs_norm(asset_dir):
			return manifest
	return None


def build_default_manifest(asset_dir):
	class Args:
		pass
	args = Args()
	args.asset_dir = asset_dir
	args.package_name = None
	args.component = None
	args.ui_name = None
	return fairygui_asset_manifest.build_manifest(args)


def read_text(path):
	with open(path, "r", encoding="utf-8-sig") as file:
		return file.read()


def match_string(body, field, default):
	matched = re.search(r"\b{0}\s*=\s*\"([^\"]*)\"".format(re.escape(field)), body)
	return matched.group(1) if matched else default


def match_bool(body, field, default):
	matched = re.search(r"\b{0}\s*=\s*(true|false)".format(re.escape(field)), body)
	if not matched:
		return default
	return matched.group(1) == "true"


def match_number(body, field, default):
	matched = re.search(r"\b{0}\s*=\s*([0-9.]+)".format(re.escape(field)), body)
	return matched.group(1) if matched else default


def extract_registry_body(content, ui_name):
	pattern = r"\b{0}\s*=\s*\{{(?P<body>.*?)\n\t\}},".format(re.escape(ui_name))
	matched = re.search(pattern, content, re.S)
	return matched.group("body") if matched else ""


def read_registry_options(manifest, manifest_dir):
	ui_name = manifest.get("uiName")
	package_name = manifest.get("package")
	registry_path = os.path.join(manifest_dir, (package_name or ui_name) + ".registry.lua")
	body = ""
	if os.path.exists(registry_path):
		body = extract_registry_body(read_text(registry_path), ui_name)
	return {
		"layer": match_string(body, "layer", "Normal"),
		"group": match_string(body, "group", ""),
		"scene": match_string(body, "scene", "Default"),
		"cache": match_bool(body, "cache", True),
		"full_screen": match_bool(body, "fullScreen", False),
		"modal": match_bool(body, "modal", False),
		"modal_alpha": match_number(body, "modalAlpha", "0.35"),
		"close_on_mask_click": match_bool(body, "closeOnMaskClick", False),
		"close_on_escape": match_bool(body, "closeOnEscape", False),
		"popup_mode": match_string(body, "popupMode", ""),
	}


def append_bool_flag(command, enabled, flag):
	if enabled:
		command.append(flag)


def build_generate_command(script_path, manifest, options, mode_args):
	command = [
		sys.executable,
		"-B",
		script_path,
		"--asset-dir", manifest.get("assetDir"),
		"--package", manifest.get("package"),
		"--component", manifest.get("component"),
		"--ui-name", manifest.get("uiName"),
		"--layer", options["layer"],
		"--group", options["group"],
		"--scene", options["scene"],
		"--cache", "true" if options["cache"] else "false",
	]
	append_bool_flag(command, options["full_screen"], "--full-screen")
	append_bool_flag(command, options["modal"], "--modal")
	if options["modal"]:
		command.extend(["--modal-alpha", options["modal_alpha"]])
	append_bool_flag(command, options["close_on_mask_click"], "--close-on-mask-click")
	append_bool_flag(command, options["close_on_escape"], "--close-on-escape")
	if options["popup_mode"]:
		command.extend(["--popup-mode", options["popup_mode"]])
	command.extend(mode_args)
	return command


def main():
	parser = argparse.ArgumentParser(description="Batch generate FairyGUI manifests, MVC stubs, and registries.")
	parser.add_argument("--assets-root", default=os.path.join("bin", "res", "assets"))
	parser.add_argument("--manifest-dir", default=os.path.join("bin", "res", "assets", "fairygui_manifests"))
	parser.add_argument("--check", action="store_true")
	parser.add_argument("--strict", action="store_true")
	parser.add_argument("--dry-run", action="store_true")
	args = parser.parse_args()

	script_dir = os.path.dirname(os.path.abspath(__file__))
	repo_root = os.path.abspath(os.path.join(script_dir, "..", ".."))
	os.chdir(repo_root)

	asset_dirs = find_asset_dirs(args.assets_root)
	if not asset_dirs:
		print("No FairyGUI asset directories found under " + normalize_path(args.assets_root))
		return 1

	mode_args = []
	if args.check:
		mode_args.append("--check")
	if args.strict:
		mode_args.append("--strict")
	if args.dry_run:
		mode_args.append("--dry-run")

	generate_script = os.path.join(script_dir, "fairygui_generate_ui.py")
	failures = 0
	for asset_dir in asset_dirs:
		manifest = find_existing_manifest(asset_dir, args.manifest_dir) or build_default_manifest(asset_dir)
		options = read_registry_options(manifest, args.manifest_dir)
		print("[FGUI-BATCH] {0} -> {1}".format(normalize_path(asset_dir), manifest.get("uiName")))
		result = subprocess.call(build_generate_command(generate_script, manifest, options, mode_args))
		if result != 0:
			failures += 1

	if failures:
		print("[FGUI-BATCH] failed: {0}".format(failures))
		return 1
	print("[FGUI-BATCH] completed: {0}".format(len(asset_dirs)))
	return 0


if __name__ == "__main__":
	raise SystemExit(main())
