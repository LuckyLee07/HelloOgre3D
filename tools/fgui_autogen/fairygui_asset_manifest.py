import argparse
import json
import os
import re
import xml.etree.ElementTree as ET


TYPE_MAP = {
	"component": "GComponent",
	"graph": "GGraph",
	"group": "GGroup",
	"image": "GImage",
	"list": "GList",
	"loader": "GLoader",
	"loader3D": "GObject",
	"richtext": "GRichTextField",
	"text": "GTextField",
}


OUTPUT_NEWLINE = "\r\n"


def to_output_newlines(content):
	return content.replace("\r\n", "\n").replace("\r", "\n").replace("\n", OUTPUT_NEWLINE)


def normalize_path(path):
	return path.replace("\\", "/")


def strip_ext(name):
	return os.path.splitext(name)[0]


def sanitize_field_name(name):
	field_name = re.sub(r"[^0-9A-Za-z_]", "_", name)
	if not field_name:
		return "node"
	if field_name[0].isdigit():
		field_name = "_" + field_name
	return field_name


def is_generated_name(name):
	return re.match(r"^n\d+(_[0-9A-Za-z]+)?$", name or "") is not None


def load_xml(path):
	return ET.parse(path).getroot()


def get_package_resources(asset_dir):
	package_xml = os.path.join(asset_dir, "package.xml")
	if not os.path.exists(package_xml):
		raise FileNotFoundError("package.xml not found: " + normalize_path(package_xml))
	root = load_xml(package_xml)
	resources = []
	for item in root.findall("./resources/*"):
		resources.append({
			"kind": item.tag,
			"id": item.get("id"),
			"name": item.get("name"),
			"path": item.get("path") or "/",
			"exported": item.get("exported") == "true",
		})
	return root.get("id"), resources


def resolve_resource_by_url(url, resources):
	if not url or not resources:
		return None

	for item in sorted(resources, key=lambda value: len(value.get("id") or ""), reverse=True):
		resource_id = item.get("id")
		if resource_id and url.endswith(resource_id):
			return item
	return None


def build_list_info(node, resources):
	default_item = node.get("defaultItem")
	default_item_resource = resolve_resource_by_url(default_item, resources)
	list_info = {
		"itemCount": len(node.findall("./item")),
	}
	if default_item:
		list_info["defaultItem"] = default_item
	if default_item_resource is not None:
		list_info["defaultItemResourceId"] = default_item_resource.get("id")
		list_info["defaultItemComponent"] = strip_ext(default_item_resource.get("name") or "")
	if node.get("selectionController"):
		list_info["selectionController"] = node.get("selectionController")
	return list_info


def get_component_controls(component_path, resources=None):
	root = load_xml(component_path)
	controllers = []
	for controller in root.findall("./controller"):
		name = controller.get("name")
		if name:
			controllers.append({
				"name": sanitize_field_name(name),
				"path": name,
				"type": "GController",
			})

	transitions = []
	for transition in root.findall("./transition"):
		name = transition.get("name")
		if name:
			transitions.append({
				"name": sanitize_field_name(name),
				"path": name,
				"type": "Transition",
			})

	controls = []
	display_list = root.find("./displayList")
	if display_list is not None:
		for node in list(display_list):
			name = node.get("name")
			if not name or is_generated_name(name):
				continue
			control = {
				"name": sanitize_field_name(name),
				"path": name,
				"type": TYPE_MAP.get(node.tag, "GObject"),
				"xmlTag": node.tag,
			}
			if node.tag == "list":
				control.update(build_list_info(node, resources))
			controls.append(control)
	return controllers, transitions, controls


def find_runtime_package(asset_dir, package_name):
	if package_name:
		return normalize_path(os.path.join("bin", "res", "fuires", package_name + ".fui"))

	asset_name = os.path.basename(os.path.normpath(asset_dir))
	fuires_dir = os.path.join("bin", "res", "fuires")
	if os.path.isdir(fuires_dir):
		candidates = [
			name for name in os.listdir(fuires_dir)
			if name.startswith(asset_name) and name.endswith(".fui")
		]
		if candidates:
			return normalize_path(os.path.join(fuires_dir, sorted(candidates)[0]))
	return ""


def build_manifest(args):
	asset_dir = os.path.normpath(args.asset_dir)
	package_id, resources = get_package_resources(asset_dir)
	component_resources = [item for item in resources if item["kind"] == "component"]
	if not component_resources:
		raise ValueError("no component resources found in package.xml: " + normalize_path(asset_dir))
	exported_components = [item for item in component_resources if item["exported"]]

	component_name = args.component
	if not component_name:
		source = exported_components[0] if exported_components else component_resources[0]
		component_name = strip_ext(source["name"])

	component_file = component_name if component_name.endswith(".xml") else component_name + ".xml"
	component_path = os.path.join(asset_dir, component_file)
	if not os.path.exists(component_path):
		raise FileNotFoundError("component XML not found: " + normalize_path(component_path))
	controllers, transitions, controls = get_component_controls(component_path, resources)

	component_controls = {}
	for item in component_resources:
		name = strip_ext(item["name"])
		path = os.path.join(asset_dir, item["name"])
		if os.path.exists(path):
			component_controllers, component_transitions, component_nodes = get_component_controls(path, resources)
			component_controls[name] = {
				"controllers": component_controllers,
				"transitions": component_transitions,
				"controls": component_nodes,
			}

	package_name = args.package_name
	if not package_name:
		runtime_package = find_runtime_package(asset_dir, None)
		package_name = strip_ext(os.path.basename(runtime_package)) if runtime_package else os.path.basename(asset_dir)
	else:
		runtime_package = find_runtime_package(asset_dir, package_name)

	return {
		"manifestVersion": 1,
		"generatedBy": "tools/fgui_autogen/fairygui_asset_manifest.py",
		"uiName": args.ui_name or sanitize_field_name(component_name),
		"package": package_name,
		"component": component_name,
		"assetDir": normalize_path(asset_dir),
		"runtimePackage": runtime_package,
		"packageId": package_id,
		"controllers": controllers,
		"transitions": transitions,
		"controls": controls,
		"resources": resources,
		"componentControls": component_controls,
	}


def main():
	parser = argparse.ArgumentParser(description="Build a FairyGUI manifest from exported XML assets.")
	parser.add_argument("--asset-dir", required=True, help="Directory containing package.xml and component XML files.")
	parser.add_argument("--package", dest="package_name", help="Runtime .fui package file name without extension.")
	parser.add_argument("--component", help="Exported component name without .xml.")
	parser.add_argument("--ui-name", help="Lua UI class name.")
	parser.add_argument("--output", required=True, help="Manifest JSON output path.")
	args = parser.parse_args()

	manifest = build_manifest(args)
	output_dir = os.path.dirname(args.output)
	if output_dir and not os.path.isdir(output_dir):
		os.makedirs(output_dir)
	content = json.dumps(manifest, ensure_ascii=False, indent="\t") + "\n"
	with open(args.output, "w", encoding="utf-8", newline="") as file:
		file.write(to_output_newlines(content))


if __name__ == "__main__":
	main()
