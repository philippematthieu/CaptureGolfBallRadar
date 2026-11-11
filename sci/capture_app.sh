#!/bin/bash
# capture_app.sh <fichier.csv|wav> [club] — génère le HTML 3D via simu_headless --capture,
# l'ouvre dans Safari, attend le rendu WebGL, et capture la fenêtre dans sci/img/<nom>.png
# usage : ./capture_app.sh 2019-02-24-F7-124m-156kmh-5948trm-597trm-cs119.csv [F7]
set -e
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC="$ROOT/Cpp/src"
CAP="$1"
CLUB="${2:-F7}"
BASE="$(basename "$CAP")"; BASE="${BASE%.*}"
OUT="$ROOT/sci/img/${BASE}.png"

cd "$SRC"
./simu_headless --capture "$ROOT/sci/$CAP" "$CLUB" 1.0 > /dev/null
HTML="$SRC/trajectoire_capture_3d.html"

osascript <<EOF
tell application "Safari"
    activate
    if (count of windows) > 0 then close front window
    open POSIX file "$HTML"
    set bounds of front window to {0, 24, 1600, 1000}
end tell
EOF
# attendre le rendu three.js + spectrogramme (WebGL)
sleep 6
# capture de l'écran entier (Safari au premier plan) — -l ne prend pas l'id AppleScript
screencapture -x "$OUT"
echo "image écrite : $OUT"
