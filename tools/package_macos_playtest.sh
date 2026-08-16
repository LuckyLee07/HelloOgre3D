#!/usr/bin/env bash

set -euo pipefail

root_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
output_dir="${1:-${root_dir}/tmp/HelloOgre3D-macos-playtest}"
archive_path="${output_dir}.zip"
app_dir="${output_dir}/HelloOgre3D.app"
binary_path="${root_dir}/bin/HelloOgre3D"

case "${output_dir}" in
	"${root_dir}/tmp/"*) ;;
	*)
		echo "Output directory must be inside ${root_dir}/tmp" >&2
		exit 1
		;;
esac

if [[ ! -x "${binary_path}" ]]; then
	echo "Missing executable: ${binary_path}" >&2
	exit 1
fi

rm -rf "${output_dir}" "${archive_path}"
mkdir -p "${app_dir}/Contents/MacOS" "${output_dir}/bin"

cp "${root_dir}/tools/macos_playtest/Info.plist" "${app_dir}/Contents/Info.plist"
xcrun clang -Os -mmacosx-version-min=15.6 \
	"${root_dir}/tools/macos_playtest/HelloOgre3DLauncher.c" \
	-o "${app_dir}/Contents/MacOS/HelloOgre3DLauncher"

cp "${binary_path}" "${output_dir}/bin/HelloOgre3D"
cp "${root_dir}/bin/Sandbox.cfg" "${root_dir}/bin/Sandbox_d.cfg" "${output_dir}/bin/"
cp "${root_dir}/bin/SandboxResources.cfg" "${root_dir}/bin/SandboxResources_d.cfg" "${output_dir}/bin/"
mkdir -p "${output_dir}/bin/res" "${output_dir}/media"
rsync -rlt --exclude=.DS_Store "${root_dir}/bin/res/" "${output_dir}/bin/res/"
rsync -rlt --exclude=.DS_Store "${root_dir}/media/" "${output_dir}/media/"
cp "${root_dir}/tools/macos_playtest/README.txt" "${output_dir}/README.txt"

codesign --force --deep --sign - "${app_dir}"
ditto -c -k --norsrc --noextattr --noqtn --noacl --keepParent "${output_dir}" "${archive_path}"

echo "Package: ${output_dir}"
echo "Archive: ${archive_path}"
