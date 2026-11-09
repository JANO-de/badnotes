# Step 04 — Ink engine (performance) | Status: TODO
Scope: replace QQuickPaintedItem with a scene-graph/RHI renderer (QSGRenderNode or QSGGeometryNode): triangle-strip stroke meshes with variable width, Catmull-Rom/Chaikin smoothing, incremental tessellation (only new segments per frame), prediction (extrapolate 1–2 pts), tile cache for finished strokes, spatial index for eraser/lasso, velocity+pressure+tilt width, per-tool profiles (fountain, ballpoint, brush, highlighter w/ multiply), pixel eraser.
Target: <10ms pen-to-photon feel, 120Hz stable, 50k strokes.
Checklist:
- [ ] no visible polyline corners at any zoom
- [ ] frame time graph flat while writing fast
- [ ] highlighter doesn't darken on self-overlap
- [ ] 50k-stroke page pans at 60fps
