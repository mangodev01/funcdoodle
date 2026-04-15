#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
release_dir="$root_dir/release"
mkdir -p "$release_dir"

export FUNCDOODLE_USE_BUNDLED_PORTAUDIO=ON

"$root_dir/scripts/build_cross.sh" Release true false "" "" "" "" ""

package_dir() {
	local name="$1"
	local src="$2"

	if [[ ! -d "$src" ]]; then
		return 0
	fi

	local tmp
	tmp="$(mktemp -d)"
	mkdir -p "$tmp/$name"
	cp -r "$src/." "$tmp/$name/"

	rm -rf "$tmp/$name/CMakeFiles" \
		"$tmp/$name/CMakeCache.txt" \
		"$tmp/$name/Makefile" \
		"$tmp/$name/cmake_install.cmake" \
		"$tmp/$name/compile_commands.json" \
		"$tmp/$name/lib"

	tar -caf "$release_dir/${name}.tar.xz" -C "$tmp" "$name"
	rm -rf "$tmp"
}

package_dir "funcdoodle-windows" "$root_dir/bin/microslop"
package_dir "funcdoodle-macos-x86_64" "$root_dir/bin/macos"
package_dir "funcdoodle-macos-arm64" "$root_dir/bin/macos-arm64"
package_dir "funcdoodle-linux" "$root_dir/bin/linux"
package_dir "funcdoodle-linux-tiling" "$root_dir/bin/linux-tiling"

if [[ -d "$root_dir/bin/macos" ]]; then
	"$root_dir/scripts/build_app.sh" \
		"$root_dir/bin/macos" \
		"$release_dir" \
		"FuncDoodle" \
		"" \
		"x86_64"
fi
if [[ -d "$root_dir/bin/macos-arm64" ]]; then
	"$root_dir/scripts/build_app.sh" \
		"$root_dir/bin/macos-arm64" \
		"$release_dir" \
		"FuncDoodle-arm64" \
		"" \
		"arm64"
fi

echo "Generated:"
ls -1 "$release_dir"/*.tar.xz "$release_dir"/*.app 2>/dev/null || true
