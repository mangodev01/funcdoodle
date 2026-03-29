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
cmake -DCMAKE_BUILD_TYPE=$arg1 -DISTILING=$( (( arg2 == "true" )) && echo "ON" || echo "OFF" ) -DBUILD_TESTS=OFF -DBUILD_IMTESTS=OFF "$root_dir" || exit -1
jobs=$(( ($(nproc_compat) + 2) / 2 ))
make -j"$jobs" || exit -1
cp -r "$root_dir/assets" . || exit -1
cp -r "$root_dir/themes" . || exit -1
popd >/dev/null || exit -1
if [[ "$arg4" == "true" ]]; then
	LSAN_OPTIONS="suppressions=leaks.supp:print_suppressions=false" "$bin_dir/FuncDoodle" || exit -1
fi
