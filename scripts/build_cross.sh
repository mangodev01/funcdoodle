if [[ $# -ne 8 ]]; then
	echo "Usage: $0 <Debug/Release> <clean?> <run?> <win portaudio.h path> <win portaudio lib path> <mac portaudio.h path> <mac x86_64 portaudio lib/dylib path> <mac arm64 portaudio lib/dylib path>"
	exit -1
fi

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
use_bundled="${FUNCDOODLE_USE_BUNDLED_PORTAUDIO:-}"
if [[ -z "$use_bundled" && -f "$root_dir/lib/portaudio/CMakeLists.txt" ]]; then
	use_bundled="ON"
fi

arg1=$(echo "$1" | tr '[:upper:]' '[:lower:]')
arg2=$(echo "$2" | tr '[:upper:]' '[:lower:]')
arg3=$(echo "$3" | tr '[:upper:]' '[:lower:]')
arg4=$(echo "$4")
arg5=$(echo "$5")
arg6=$(echo "$6")
arg7=$(echo "$7")
arg8=$(echo "$8")

win_pa_path="$arg5"
mac_pa_lib_x86_path="$arg7"
mac_pa_lib_arm_path="$arg8"
pa_static_override="${PORTAUDIO_STATIC:-}"

if [[ "$arg1" != "debug" && "$arg1" != "release" ]]; then
	echo "Mode must be either debug or release -- $arg1 is invalid"
	exit -1
fi
if [[ "$arg2" != "true" && "$arg2" != "false" ]]; then
	echo "clean argument must be true or false -- $arg2 is invalid"
	exit -1
fi
if [[ "$arg3" != "true" && "$arg3" != "false" ]]; then
	echo "run argument must be true or false -- $arg3 is invalid"
	exit -1
fi

arg1=$(echo "$arg1" | awk '{print toupper(substr($0,1,1)) substr($0,2)}')

if [[ "$arg2" == "true" ]]; then
	rm -rf "$root_dir/bin/microslop" "$root_dir/bin/macos"
fi

mkdir -p "$root_dir/bin/microslop" || exit -1
pushd "$root_dir/bin/microslop" >/dev/null || exit -1
mkdir -p incl
if [[ "$use_bundled" != "ON" && -n "$arg4" ]]; then
	cp "$arg4" incl/ || exit -1
fi

cmake_args=(
	-DCMAKE_TOOLCHAIN_FILE="$root_dir/mingw.cmake"
	-DCMAKE_BUILD_TYPE="$arg1"
	-DFUNCDOODLE_USE_BUNDLED_PORTAUDIO="$use_bundled"
	-DPA_USE_JACK=OFF
)
if [[ "$use_bundled" != "ON" && -n "$win_pa_path" ]]; then
	win_pa_base="$(basename "$win_pa_path")"
	win_pa_name="${win_pa_base%.*}"
	win_pa_name="${win_pa_name#lib}"
	win_pa_static="ON"
	if [[ -n "$pa_static_override" ]]; then
		win_pa_static="$pa_static_override"
	elif [[ "$win_pa_base" != *static* && "$win_pa_base" != *.a ]]; then
		win_pa_static="OFF"
	fi
	if [[ "$win_pa_static" != "ON" ]]; then
		echo "Static PortAudio is required. Provide a static Windows library."
		exit -1
	fi
	cp "$win_pa_path" "$win_pa_base" || exit -1
	cmake_args+=(
		-DPORTAUDIO_INCLDIR="$root_dir/bin/microslop/incl/"
		-DPORTAUDIO_LIBDIR="$root_dir/bin/microslop/"
		-DPORTAUDIO_LIBNAME="$win_pa_name"
		-DPORTAUDIO_STATIC="$win_pa_static"
	)
fi
cmake "${cmake_args[@]}" "$root_dir" || exit -1
mkdir -p CMakeFiles/FuncDoodle.dir/src \
	CMakeFiles/FuncDoodle.dir/lib/glad/src \
	CMakeFiles/FuncDoodle.dir/lib/imgui \
	CMakeFiles/FuncDoodle.dir/lib/imgui/backends \
	CMakeFiles/FuncDoodle.dir/lib/nfd/src || exit -1
jobs=$(( ($(nproc) + 2) / 2 ))
cmake --build . -j"$jobs" || exit -1
cp -r "$root_dir/assets" . || exit -1
cp -r "$root_dir/themes" . || exit -1
if [[ "$arg3" == "true" ]]; then
	wine ./FuncDoodle.exe || exit -1
fi
popd >/dev/null || exit -1

mac_pa_incl="$arg6"
mac_pa_lib_x86="$arg7"
mac_pa_lib_arm="$arg8"

if [[ -f "$mac_pa_incl" ]]; then
	mac_pa_incl="$(dirname "$mac_pa_incl")"
fi
if [[ -f "$mac_pa_lib_x86" ]]; then
	mac_pa_lib_x86="$(dirname "$mac_pa_lib_x86")"
fi
if [[ -f "$mac_pa_lib_arm" ]]; then
	mac_pa_lib_arm="$(dirname "$mac_pa_lib_arm")"
fi
if [[ -n "$mac_pa_lib_x86" ]]; then
	mac_pa_lib_x86="${mac_pa_lib_x86%/}/"
fi
if [[ -n "$mac_pa_lib_arm" ]]; then
	mac_pa_lib_arm="${mac_pa_lib_arm%/}/"
fi

if [[ "$use_bundled" == "ON" || ( -n "$mac_pa_incl" && -n "$mac_pa_lib_x86" && -n "$mac_pa_lib_arm" ) ]]; then
	mac_x86_name=""
	mac_arm_name=""
	mac_x86_static="ON"
	mac_arm_static="ON"
	if [[ -f "$mac_pa_lib_x86_path" ]]; then
		mac_x86_base="$(basename "$mac_pa_lib_x86_path")"
		mac_x86_name="${mac_x86_base%.*}"
		mac_x86_name="${mac_x86_name#lib}"
		if [[ "$mac_x86_base" == *.dylib || "$mac_x86_base" == *.so ]]; then
			mac_x86_static="OFF"
		fi
	fi
	if [[ -f "$mac_pa_lib_arm_path" ]]; then
		mac_arm_base="$(basename "$mac_pa_lib_arm_path")"
		mac_arm_name="${mac_arm_base%.*}"
		mac_arm_name="${mac_arm_name#lib}"
		if [[ "$mac_arm_base" == *.dylib || "$mac_arm_base" == *.so ]]; then
			mac_arm_static="OFF"
		fi
	fi
	if [[ -n "$pa_static_override" ]]; then
		mac_x86_static="$pa_static_override"
		mac_arm_static="$pa_static_override"
	fi
	if [[ "$use_bundled" != "ON" ]]; then
		if [[ "$mac_x86_static" != "ON" || "$mac_arm_static" != "ON" ]]; then
			echo "Static PortAudio is required for macOS builds. Provide .a libs."
			exit -1
		fi
	fi
	build_macos() {
		local arch="$1"
		local outdir="$2"
		local libdir="$3"
		local libname="$4"
		local static="$5"

		mkdir -p "$outdir" || exit -1
		pushd "$outdir" >/dev/null || exit -1
		cmake_args=(
			-DCMAKE_TOOLCHAIN_FILE="$root_dir/macos.cmake"
			-DCMAKE_BUILD_TYPE="$arg1"
			-DCMAKE_OSX_ARCHITECTURES="$arch"
			-DFUNCDOODLE_USE_BUNDLED_PORTAUDIO="$use_bundled"
			-DPA_USE_JACK=OFF
		)
		if [[ "$use_bundled" != "ON" ]]; then
			cmake_args+=(
				-DPORTAUDIO_INCLDIR="$mac_pa_incl"
				-DPORTAUDIO_LIBDIR="$libdir"
				-DPORTAUDIO_LIBNAME="$libname"
				-DPORTAUDIO_STATIC="$static"
			)
		fi
		cmake "${cmake_args[@]}" "$root_dir" || exit -1
		mkdir -p CMakeFiles/FuncDoodle.dir/src \
			CMakeFiles/FuncDoodle.dir/lib/glad/src \
			CMakeFiles/FuncDoodle.dir/lib/imgui \
			CMakeFiles/FuncDoodle.dir/lib/imgui/backends \
			CMakeFiles/FuncDoodle.dir/lib/nfd/src || exit -1
		jobs=$(( ($(nproc) + 2) / 3 ))
		cmake --build . -j"$jobs" || exit -1
		cp -r "$root_dir/assets" . || exit -1
		cp -r "$root_dir/themes" . || exit -1
		if [[ "$arg3" == "true" && "$(uname -s)" == "Darwin" ]]; then
			LSAN_OPTIONS="suppressions=leaks.supp:print_suppressions=false" ./FuncDoodle || exit -1
		fi
		popd >/dev/null || exit -1
	}

	build_macos "x86_64" "$root_dir/bin/macos" "$mac_pa_lib_x86" \
		"$mac_x86_name" "$mac_x86_static"
	build_macos "arm64" "$root_dir/bin/macos-arm64" "$mac_pa_lib_arm" \
		"$mac_arm_name" "$mac_arm_static"
else
	echo "macOS PortAudio paths not provided. Skipping macOS build."
fi

"$root_dir/scripts/build.sh" "$arg1" true "$arg2" "$arg3" || exit -1
rm -rf "$root_dir/bin/linux-tiling"
cp -r "$root_dir/bin/linux" "$root_dir/bin/linux-tiling"
"$root_dir/scripts/build.sh" "$arg1" false false "$arg3" || exit -1
