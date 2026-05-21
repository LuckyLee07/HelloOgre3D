import argparse
import json
import os

import fairygui_asset_manifest
import fairygui_autogen


def ensure_dir(path):
	if path and not os.path.isdir(path):
		os.makedirs(path)


def normalize_path(path):
	return path.replace("\\", "/")


def write_manifest(path, manifest):
	output_dir = os.path.dirname(path)
	ensure_dir(output_dir)
	with open(path, "w", encoding="utf-8", newline="\n") as file:
		json.dump(manifest, file, ensure_ascii=False, indent="\t")
		file.write("\n")


def default_manifest_output(package_name, ui_name):
	name = package_name or ui_name
	return os.path.join("bin", "res", "assets", "fairygui_manifests", name + ".json")


def default_registry_output(package_name, ui_name):
	name = package_name or ui_name
	return os.path.join("bin", "res", "assets", "fairygui_manifests", name + ".registry.lua")


def class_file(output_dir, class_name):
	return os.path.join(output_dir, class_name + ".lua")


def get_manifest_component_path(asset_dir, component):
	component_file = component if component.endswith(".xml") else component + ".xml"
	return os.path.join(asset_dir, component_file)


def expected_runtime_package_path(package_name):
	if not package_name:
		return ""
	return os.path.join("bin", "res", "fuires", package_name + ".fui")


def collect_source_errors(args):
	errors = []
	warnings = []
	asset_dir = os.path.normpath(args.asset_dir)
	package_xml = os.path.join(asset_dir, "package.xml")

	if not os.path.isdir(asset_dir):
		errors.append("asset directory not found: " + normalize_path(asset_dir))
	elif not os.path.exists(package_xml):
		errors.append("package.xml not found: " + normalize_path(package_xml))

	if args.component:
		component_path = get_manifest_component_path(asset_dir, args.component)
		if not os.path.exists(component_path):
			errors.append("component XML not found: " + normalize_path(component_path))

	if args.package_name:
		runtime_package = expected_runtime_package_path(args.package_name)
		if not os.path.exists(runtime_package):
			warnings.append("runtime .fui not found yet: " + normalize_path(runtime_package))
	return errors, warnings


def collect_manifest_warnings(manifest):
	warnings = []
	if not manifest.get("runtimePackage"):
		warnings.append("runtime package path could not be inferred")
	elif not os.path.exists(manifest.get("runtimePackage")):
		warnings.append("runtime package path does not exist: " + manifest.get("runtimePackage"))
	if not manifest.get("controls"):
		warnings.append("manifest has no named controls")
	if not manifest.get("component"):
		warnings.append("manifest has no component name")
	return warnings


def build_lua_output_plan(manifest, args):
	ui_name = args.ui_name or manifest.get("uiName")
	package_name = args.package_name or manifest.get("package")
	component = args.component or manifest.get("component") or ui_name
	controls = fairygui_autogen.get_controls(manifest, [])
	controllers = fairygui_autogen.get_controllers(manifest)
	component_controls = manifest.get("componentControls") or {}

	if not ui_name or not package_name:
		raise SystemExit("--ui-name and --package are required when manifest does not provide them")

	outputs = []
	for class_name, content, overwrite in [
		(ui_name + "AutoGen", fairygui_autogen.render_autogen(ui_name), True),
		(ui_name + "View", fairygui_autogen.render_view(ui_name, controls, controllers, args.field_style), args.force),
		(ui_name + "Model", fairygui_autogen.render_model(ui_name), args.force),
		(ui_name + "Ctrl", fairygui_autogen.render_ctrl(ui_name, controls, component_controls, args.field_style), args.force),
	]:
		outputs.append({
			"kind": "lua",
			"path": class_file(args.output_dir, class_name),
			"content": content,
			"overwrite": overwrite,
		})

	registry_entry = fairygui_autogen.render_registry_entry(ui_name, package_name, component, args.module_prefix, args)
	registry = fairygui_autogen.render_registry(ui_name, package_name, component, args.module_prefix, args)
	if args.registry_output:
		outputs.append({
			"kind": "registry",
			"path": args.registry_output,
			"content": registry,
			"overwrite": True,
		})
	if args.registry_aggregate_output:
		outputs.append({
			"kind": "registry_aggregate",
			"path": args.registry_aggregate_output,
			"ui_name": ui_name,
			"registry_entry": registry_entry,
			"overwrite": True,
		})

	return outputs, registry_entry


def describe_output_action(output):
	path = output["path"]
	if output["kind"] == "registry_aggregate":
		return "update" if os.path.exists(path) else "create"
	if os.path.exists(path):
		return "overwrite" if output["overwrite"] else "skip"
	return "create"


def collect_output_warnings(outputs):
	warnings = []
	for output in outputs:
		if describe_output_action(output) == "skip":
			warnings.append("will keep existing hand-written file: " + normalize_path(output["path"]))
	return warnings


def write_lua_outputs(outputs):
	written = []
	for output in outputs:
		path = output["path"]
		if output["kind"] == "registry_aggregate":
			fairygui_autogen.ensure_dir(os.path.dirname(path))
			if fairygui_autogen.update_aggregate_registry(path, output["ui_name"], output["registry_entry"]):
				written.append(path)
			continue

		output_dir = os.path.dirname(path)
		if output_dir:
			fairygui_autogen.ensure_dir(output_dir)
		if fairygui_autogen.write_file(path, output["content"], force=output["overwrite"]):
			written.append(path)
	return written


def print_messages(title, messages):
	if not messages:
		return
	print(title + ":")
	for message in messages:
		print("  " + message)


def print_plan(manifest_output, outputs):
	print("Planned manifest:")
	print("  " + normalize_path(manifest_output))
	print("Planned files:")
	for output in outputs:
		print("  [{0}] {1}".format(describe_output_action(output), normalize_path(output["path"])))


def main():
	parser = argparse.ArgumentParser(description="Generate FairyGUI manifest, Lua MVC stubs, and registry entries.")
	parser.add_argument("--asset-dir", required=True, help="Directory containing package.xml and component XML files.")
	parser.add_argument("--package", dest="package_name", help="Runtime .fui package file name without extension.")
	parser.add_argument("--component", help="Exported component name without .xml.")
	parser.add_argument("--ui-name", help="UI class prefix, for example Act38Test.")
	parser.add_argument("--manifest-output", help="Manifest JSON output path.")
	parser.add_argument("--module-prefix", default="res.scripts.ui.views")
	parser.add_argument("--output-dir", default=os.path.join("bin", "res", "scripts", "ui", "views"))
	parser.add_argument("--registry-output", help="Standalone registry Lua table output path.")
	parser.add_argument("--registry-aggregate-output", default=os.path.join("bin", "res", "scripts", "ui", "GeneratedUIRegistry.lua"))
	parser.add_argument("--field-style", choices=("camel", "manifest"), default="camel")
	parser.add_argument("--layer", default="Normal")
	parser.add_argument("--group", default="")
	parser.add_argument("--scene", default="Default")
	parser.add_argument("--cache", type=fairygui_autogen.parse_bool, default=True)
	parser.add_argument("--full-screen", action="store_true")
	parser.add_argument("--modal", action="store_true")
	parser.add_argument("--modal-alpha", default="0.35")
	parser.add_argument("--close-on-mask-click", action="store_true")
	parser.add_argument("--close-on-escape", action="store_true")
	parser.add_argument("--popup-mode")
	parser.add_argument("--dry-run", action="store_true", help="Print planned outputs without writing files.")
	parser.add_argument("--check", action="store_true", help="Validate inputs and planned outputs without writing files.")
	parser.add_argument("--strict", action="store_true", help="Treat warnings as errors in --check mode.")
	parser.add_argument("--no-registry-aggregate", action="store_true", help="Do not update GeneratedUIRegistry.lua.")
	parser.add_argument("--force", action="store_true", help="Overwrite View / Model / Ctrl stubs.")
	args = parser.parse_args()

	if args.no_registry_aggregate:
		args.registry_aggregate_output = None

	errors, warnings = collect_source_errors(args)
	if errors:
		print_messages("Errors", errors)
		return 1

	manifest = fairygui_asset_manifest.build_manifest(args)
	ui_name = args.ui_name or manifest.get("uiName")
	package_name = args.package_name or manifest.get("package")
	manifest_output = args.manifest_output or default_manifest_output(package_name, ui_name)
	args.registry_output = args.registry_output or default_registry_output(package_name, ui_name)
	warnings.extend(collect_manifest_warnings(manifest))
	outputs, registry_entry = build_lua_output_plan(manifest, args)
	warnings.extend(collect_output_warnings(outputs))

	print_messages("Warnings", warnings)
	if args.dry_run or args.check:
		print_plan(manifest_output, outputs)
		print("")
		print("Registry entry:")
		print(registry_entry)
		if args.check and args.strict and warnings:
			return 1
		return 0

	write_manifest(manifest_output, manifest)
	written = write_lua_outputs(outputs)
	print("Generated manifest:")
	print("  " + normalize_path(manifest_output))
	print("Generated files:")
	for path in written:
		print("  " + normalize_path(path))
	print("")
	print("Registry entry:")
	print(registry_entry)
	return 0


if __name__ == "__main__":
	raise SystemExit(main())
