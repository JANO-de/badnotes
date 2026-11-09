#!/usr/bin/env bash
# Needs linuxdeploy + linuxdeploy-plugin-qt in PATH.
set -euo pipefail
cd "$(dirname "$0")/.."
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
DESTDIR=AppDir cmake --install build
QML_SOURCES_PATHS=qml linuxdeploy --appdir AppDir --plugin qt --desktop-file packaging/inkwell.desktop --output appimage
