# Step 01 — Skeleton  | Status: DONE (C++ compiles on Qt 6.4; QML GlassPanel untested: needs Qt>=6.5 QtQuick.Effects)
Contents: CMake (Qt 6.6+), InkCanvas (QQuickPaintedItem, tablet+mouse input, pressure width, pen/highlighter/stroke-eraser, undo/redo), Theme singleton, GlassPanel (ShaderEffectSource + MultiEffect blur), FloatingToolbar, placeholder sidebar, PKGBUILD, AppImage script.
Next chat: first compile and fix errors. Known risks: tablet events reaching QQuickItem::event (verify in Qt 6.x; else use a TapHandler/PointHandler or a QQuickWindow event filter); `maskEnabled` MultiEffect inside GlassPanel; singleton registration of Theme.qml.
Test checklist:
- [ ] cmake -B build -G Ninja && cmake --build build succeeds
- [ ] window opens, glass toolbar + sidebar visible
- [ ] pen draws with pressure variation; mouse draws too
- [ ] highlighter translucent; eraser removes strokes
- [ ] undo/redo buttons enable/disable correctly
- [ ] light/dark follows system
