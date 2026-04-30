#!/usr/bin/env bash
set -euo pipefail

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

tmp_cc="$build_dir/compile_commands.lint.json"

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

echo "[lint] running clang-tidy (no fixes)..."

run-clang-tidy -p "$build_dir" -quiet
