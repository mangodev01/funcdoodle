#!/usr/bin/env bash

usage() {
	echo "Usage: $0 <Debug/Release> <tiling?> <clean?> <run?>"
	exit -1
}

if [[ $# -ne 0 ]] && [[ $# -ne 4 ]]; then
	usage
fi

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

arg1=$(echo "$1" | tr '[:upper:]' '[:lower:]')
arg2=$(echo "$2" | tr '[:upper:]' '[:lower:]')
arg3=$(echo "$3" | tr '[:upper:]' '[:lower:]')
arg4=$(echo "$4" | tr '[:upper:]' '[:lower:]')

detect_platform() {
	case "$(uname -s)" in
		Linux*)  echo "linux" ;;
		Darwin*) echo "macos" ;;
		CYGWIN*|MINGW*|MSYS*) echo "microslop" ;;
		*)       echo "unknown" ;;
	esac
}

nproc_compat() {
	if command -v nproc >/dev/null 2>&1; then
		nproc
	else
		# macOS (and BSD fallback)
		sysctl -n hw.ncpu 2>/dev/null || echo 1
	fi
}

platform="$(detect_platform)"
bin_dir="$root_dir/bin/$platform"

if [[ "$platform" == "unknown" ]]; then
	echo "Unsupported platform: $(uname -s)"
	exit -1
fi

detect_pa_libdir() {
	if [[ -n "${PORTAUDIO_LIBDIR:-}" ]]; then
		echo "$PORTAUDIO_LIBDIR"
		return
	fi
	for d in /lib64 /usr/lib64 /usr/lib /usr/local/lib; do
		if [[ -f "$d/libportaudio.so" || -f "$d/libportaudio.dylib" || -f "$d/libportaudio.a" ]]; then
			echo "$d"
			return
		fi
	done
	echo "/usr/lib"
}

detect_pa_incldir() {
	if [[ -n "${PORTAUDIO_INCLDIR:-}" ]]; then
		echo "$PORTAUDIO_INCLDIR"
		return
	fi
	if [[ -f "/usr/include/portaudio.h" ]]; then
		echo "/usr/include"
		return
	fi
	if [[ -f "/usr/local/include/portaudio.h" ]]; then
		echo "/usr/local/include"
		return
	fi
	echo ""
}

use_bundled="${FUNCDOODLE_USE_BUNDLED_PORTAUDIO:-}"
if [[ -z "$use_bundled" && -f "$root_dir/lib/portaudio/CMakeLists.txt" ]]; then
	use_bundled="ON"
fi

pa_libdir=""
pa_incldir=""
pa_static="${PORTAUDIO_STATIC:-}"
if [[ "$use_bundled" != "ON" ]]; then
	pa_libdir="$(detect_pa_libdir)"
	pa_incldir="$(detect_pa_incldir)"
	if [[ -z "$pa_static" ]]; then
		pa_static="ON"
	fi
	if [[ "$pa_static" == "ON" && ! -f "$pa_libdir/libportaudio.a" ]]; then
		echo "Static PortAudio not found at $pa_libdir/libportaudio.a"
		echo "Install/build static PortAudio or set PORTAUDIO_LIBDIR to its location."
		exit -1
	fi
fi

if [[ $# -eq 0 ]]; then
	arg1="debug"
	arg2="true"
	arg3="false"
	arg4="true"
fi

if [[ "$arg1" != "debug" && "$arg1" != "release" ]]; then
	echo "Mode must be either debug or release -- $arg1 is invalid"
	exit -1
fi
if [[ "$arg2" != "true" && "$arg2" != "false" ]]; then
	echo "istiling argument must be true or false -- $arg2 is invalid"
	exit -1
fi
if [[ "$arg3" != "true" && "$arg3" != "false" ]]; then
	echo "clean argument must be true or false -- $arg3 is invalid"
	exit -1
fi
if [[ "$arg4" != "true" && "$arg4" != "false" ]]; then
	echo "run argument must be true or false -- $arg4 is invalid"
	exit -1
fi

arg1=$(echo "$arg1" | awk '{print toupper(substr($0,1,1)) substr($0,2)}')

if [ "$arg3" == "true" ]; then
	rm -rf "$bin_dir"
fi

mkdir -p "$bin_dir" || exit -1
pushd "$bin_dir" >/dev/null || exit -1
cmake_args=(
	-DCMAKE_BUILD_TYPE=$arg1
	-DISTILING=$( (( arg2 == "true" )) && echo "ON" || echo "OFF" )
	-DBUILD_TESTS=OFF
	-DBUILD_IMTESTS=OFF
	-DFUNCDOODLE_USE_BUNDLED_PORTAUDIO="${use_bundled:-OFF}"
)
if [[ "${use_bundled:-OFF}" != "ON" ]]; then
	cmake_args+=(-DPORTAUDIO_LIBDIR="$pa_libdir" -DPORTAUDIO_STATIC="$pa_static")
	if [[ -n "$pa_incldir" ]]; then
		cmake_args+=(-DPORTAUDIO_INCLDIR="$pa_incldir")
	fi
fi
cmake "${cmake_args[@]}" "$root_dir" || exit -1
jobs=$(( ($(nproc_compat) + 2) / 2 ))
make -j"$jobs" || exit -1
cp -r "$root_dir/assets" . || exit -1
cp -r "$root_dir/themes" . || exit -1
popd >/dev/null || exit -1
if [[ "$arg4" == "true" ]]; then
	LSAN_OPTIONS="suppressions=leaks.supp:print_suppressions=false" "$bin_dir/FuncDoodle" || exit -1
fi
