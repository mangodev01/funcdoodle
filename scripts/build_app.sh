#!/usr/bin/env bash
set -e

# ========================
# Configuration
# ========================
APP_NAME="${3:-FuncDoodle}"
BIN_DIR="${1:-bin/macos}"
PORTAUDIO_DYLIB="${4:-}"
MACOS_ARCH="${5:-x86_64}"
ASSETS_DIR="assets"
THEMES_DIR="themes"
OUTPUT_DIR="${2:-appbin}"
BINARY="$BIN_DIR/FuncDoodle"
APP_DIR="$OUTPUT_DIR/$APP_NAME.app"
MACOS_DIR="$APP_DIR/Contents/MacOS"
RESOURCES_DIR="$APP_DIR/Contents/Resources"
FRAMEWORKS_DIR="$APP_DIR/Contents/Frameworks"

# ========================
# Clean previous build
# ========================
rm -rf "$APP_DIR"
mkdir -p "$MACOS_DIR"
mkdir -p "$RESOURCES_DIR"
mkdir -p "$FRAMEWORKS_DIR"

# ========================
# Copy binary
# ========================
if [ ! -f "$BINARY" ]; then
	echo "Binary not found: $BINARY"
	exit 1
fi
cp "$BINARY" "$MACOS_DIR/$APP_NAME"
chmod +x "$MACOS_DIR/$APP_NAME"

if [[ -n "$PORTAUDIO_DYLIB" && "$PORTAUDIO_DYLIB" != *.dylib ]]; then
	echo "Static PortAudio is required; don't pass a dylib to build_app.sh."
	exit 1
fi

if [ -n "$PORTAUDIO_DYLIB" ]; then
    if [ ! -f "$PORTAUDIO_DYLIB" ]; then
        echo "PortAudio dylib not found: $PORTAUDIO_DYLIB"
        exit 1
    fi

    pa_dir="$(dirname "$PORTAUDIO_DYLIB")"
    pa_base="$(basename "$PORTAUDIO_DYLIB")"
    if [[ "$pa_base" == "libportaudio.dylib" ]] && [ -f "$pa_dir/libportaudio.2.dylib" ]; then
        PORTAUDIO_DYLIB="$pa_dir/libportaudio.2.dylib"
    fi

    dylib_name="$(basename "$PORTAUDIO_DYLIB")"
    cp "$PORTAUDIO_DYLIB" "$FRAMEWORKS_DIR/$dylib_name"

    osxcross_bin="${OSXCROSS_BIN:-/home/illia/proj/third-party/osxcross/target/bin}"
    install_name_tool="$osxcross_bin/${MACOS_ARCH}-apple-darwin25.1-install_name_tool"
    if [ ! -x "$install_name_tool" ]; then
        echo "install_name_tool not found for arch $MACOS_ARCH: $install_name_tool"
        exit 1
    fi

    "$install_name_tool" -id "@rpath/$dylib_name" "$FRAMEWORKS_DIR/$dylib_name"
    "$install_name_tool" -change "$PORTAUDIO_DYLIB" "@rpath/$dylib_name" "$MACOS_DIR/$APP_NAME"
    "$install_name_tool" -change "$pa_dir/libportaudio.dylib" "@rpath/$dylib_name" "$MACOS_DIR/$APP_NAME" || true
    "$install_name_tool" -change "$pa_dir/libportaudio.2.dylib" "@rpath/$dylib_name" "$MACOS_DIR/$APP_NAME" || true
    "$install_name_tool" -add_rpath "@executable_path/../Frameworks" "$MACOS_DIR/$APP_NAME" || true
fi

# ==============================
# Copy assets into MacOS folder
# ==============================
mkdir -p "$MACOS_DIR/assets"
cp -r "$ASSETS_DIR/"* "$MACOS_DIR/assets/"

# ==============================
# Copy themes into MacOS folder
# ==============================
mkdir -p "$MACOS_DIR/themes"
cp -r "$THEMES_DIR/"* "$MACOS_DIR/themes/"

# ========================
# Copy icon (if exists)
# ========================
ICON_SRC="$ASSETS_DIR/icon.icns"
if [ -f "$ICON_SRC" ]; then
    cp "$ICON_SRC" "$RESOURCES_DIR/"
fi

# ========================
# Create Info.plist
# ========================
cat > "$APP_DIR/Contents/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN"
 "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>CFBundleName</key><string>$APP_NAME</string>
  <key>CFBundleExecutable</key><string>$APP_NAME</string>
  <key>CFBundleIdentifier</key><string>com.yourcompany.$APP_NAME</string>
  <key>CFBundleVersion</key><string>1.0</string>
  <key>CFBundlePackageType</key><string>APPL</string>
  <key>CFBundleIconFile</key><string>icon</string>
</dict>
</plist>
EOF

echo "App bundle created at $APP_DIR"
