# Inkwell — handoff index
Goal: GoodNotes-equivalent handwriting app for Linux tablets (Arch, Wayland/GNOME). Qt6/QML + C++20, GPU ink, themeable, Apple-glass UI.
Constraints: performance first; glass = in-app blur (GNOME has no window blur); touch = pan/zoom, pen = draw (palm rejection); user wants minimal chat output and, per update, ONLY a test checklist.
Repo layout: src/ (C++), qml/ (UI, module `Inkwell`), packaging/ (PKGBUILD, AppImage), handoffs/.
Steps: 01 skeleton (DONE, tested) · 02 notebook (DONE, see file for gaps) · 03 library+sidebar (DONE) · 04 ink engine · 05 lasso/shapes/media · 06 PDF · 07 glass/theme polish · 08 packaging · 09 later.
How to use: paste the target step file into a new chat with the tar.gz attached. Each step lists scope, files, acceptance checklist. Update the "Status" line when finished.
