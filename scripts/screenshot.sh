#!/usr/bin/env bash
set -e

APP="./bin/linux/FuncDoodle"
IMGUI_INI="imgui.ini"

declare -A THEMES=(
  # catppuccin
  [mocha]="581758bc-91f3-40e1-9d9f-5f2640f52f7a"
  [macchiato]="df170c68-3946-4a44-9dbb-35fa8b3e2475"
  [latte]="69d6624a-b5a4-4a58-b5a4-6774408fe2a0"
  [frappe]="855dcd06-11e6-4349-a71d-9757577101c9"

  # normal
  [light]="375cd4bb-c1ab-4604-8200-28e86b2d34d2"
  [dark]="5c68ea4d-e432-435e-b1b9-14fe8be40852"
  [classic]="baf1acaf-333a-4dfa-a3ab-1e27cd7157dc"
  [funcdoodle]="d0c1a009-d09c-4fe6-84f8-eddcb2da38f9"
)

set_theme() {
  local uuid="$1"
  sed -i -E "s/^Theme=\".*\"/Theme=\"$uuid\"/" "$IMGUI_INI"
}

get_geom() {
  hyprctl activewindow -j | jq -r '"\(.at[0]),\(.at[1]) \(.size[0])x\(.size[1])"'
}

launch_app() {
  $APP &
  APP_PID=$!
  sleep 1
}

kill_app() {
  kill $APP_PID 2>/dev/null || true
  wait $APP_PID 2>/dev/null || true
  sleep 0.5
}

screenshot() {
  grim -g "$(get_geom)" "$1"
}

press_ctrl_n_enter() {
  ydotool key 29:1 49:1 49:0 29:0
  sleep 0.2
  ydotool key 28:1 28:0
  sleep 0.5
}

disable_decorations() {
  hyprctl keyword decoration:rounding 0
  hyprctl keyword decoration:drop_shadow false
  hyprctl keyword decoration:blur:enabled false
}

restore_decorations() {
  hyprctl reload
}

# -------- fixed screenshot (IMPORTANT PART) --------
screenshot_clean() {
  local path="$1"

  disable_decorations

  # wait for compositor to actually render the no-decor frame
  sleep 0.15

  screenshot "$path"

  # wait so grim doesn't race the restore frame
  sleep 0.15

  restore_decorations
  sleep 0.2
}

# -------- main loop --------
for theme in "${!THEMES[@]}"; do
  uuid="${THEMES[$theme]}"
  echo "Theme: $theme"

  mkdir -p "screenshots/$theme"

  # welcome
  set_theme "$uuid"
  launch_app
  screenshot_clean "screenshots/$theme/welcome.png"
  kill_app

  # main
  set_theme "$uuid"
  launch_app
  press_ctrl_n_enter
  screenshot_clean "screenshots/$theme/main.png"
  kill_app
done

# -------- catwalk previews --------

catwalk screenshots/latte/main.png screenshots/frappe/main.png screenshots/macchiato/main.png screenshots/mocha/main.png
mv preview.webp screenshots/catppuccin_main.webp

catwalk screenshots/latte/welcome.png screenshots/frappe/welcome.png screenshots/macchiato/welcome.png screenshots/mocha/welcome.png
mv preview.webp screenshots/catppuccin_welcome.webp

catwalk screenshots/light/main.png screenshots/dark/main.png screenshots/funcdoodle/main.png screenshots/classic/main.png
mv preview.webp screenshots/normal_main.webp

catwalk screenshots/light/welcome.png screenshots/dark/welcome.png screenshots/funcdoodle/welcome.png screenshots/classic/welcome.png
mv preview.webp screenshots/normal_welcome.webp

# -------- convert webp -> png --------
for f in screenshots/*.webp; do
  magick "$f" "${f%.webp}.png"
  rm "$f"
done

echo "All done."
