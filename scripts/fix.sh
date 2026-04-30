#!/usr/bin/env bash
set -euo pipefail

rm -rf fixes
mkdir fixes

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

platform="$(uname -s | tr '[:upper:]' '[:lower:]')"
case "$platform" in
    linux*) platform="linux" ;;
    darwin*) platform="macos" ;;
    msys*|mingw*|cygwin*) platform="microslop" ;;
    *) echo "Unknown platform"; exit 1 ;;
esac

build_dir="$root_dir/bin/$platform"
cc_file="$build_dir/compile_commands.json"
ignore_file="$root_dir/.clang-tidy-ignore"

if [[ ! -f "$cc_file" ]]; then
    echo "Missing compile_commands.json at $cc_file"
    exit 1
fi

tmp_cc="$build_dir/compile_commands.fix.json"

cp "$cc_file" "$tmp_cc"

if [[ -f "$ignore_file" ]]; then
    patterns=$(cat "$ignore_file" | grep -vE '^#|^$' | tr '\n' '|' | sed 's/|$//')

    if [[ -n "$patterns" ]]; then
        jq -r "map(select(.file | test(\"$patterns\") | not))" \
            "$tmp_cc" > "$tmp_cc.tmp"
        mv "$tmp_cc.tmp" "$tmp_cc"
    fi
fi

mv "$cc_file" "$build_dir/compile_commands_tmp.json"
cp "$tmp_cc" "$cc_file"

echo "[fix] running clang-tidy (no fixes)..."

run-clang-tidy -p "$build_dir" -header-filter='^'"$root_dir"'/src/' -fix
# -j "$(nproc 2>/dev/null || sysctl -n hw.ncpu)" 

# clang-apply-replacements fixes/

mv "$build_dir/compile_commands_tmp.json" "$cc_file"

# Restore any clang-tidy modifications to third-party libs
git checkout -- lib/ 2>/dev/null || true
