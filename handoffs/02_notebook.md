# Step 02 — Notebook model, pages, zoom/pan | Status: DONE (C++/QML load + save/load roundtrip verified headless; touch pinch/pan/palm-rejection NOT verified on hardware)
Implemented: InkPage (strokes, templates blank/lined/grid/dotted, per-page undo ops), Notebook (QAbstractListModel, global undo/redo across pages, add/remove/move page, save/load, autosave, title), Tools singleton (tool/color/width, penNear, fingerDraws), InkCanvas (draws one page, scaled by width/pageWidth so ink stays sharp), NotebookView (Flickable, pages instantiated only near viewport, zoom 25-400%, pinch via PinchHandler, Ctrl+wheel, fit-width), PageBar (zoom, page nav, add/delete/template/move, open/save), shortcuts (Ctrl+Z/Shift+Z/S/O/N).
Palm rejection: touch-synthesized mouse never draws (unless Tools.fingerDraws); Flickable disabled while pen in proximity (TabletEnterProximity/Leave via app event filter).
Known gaps / next: page add/delete/template/move are not undoable; no thumbnails (step 03); pages use QQuickPaintedItem (step 04 replaces); eraser = whole-stroke only; zoom >4x capped by FBO size; file format in handoffs/FORMAT.md.
Risks to verify on device: TabletEnterProximity actually delivered on Wayland (else Flickable pans under palm); PinchHandler vs Flickable conflict; FileDialog under GNOME portal.
Test checklist:
- [ ] build ok; window shows toolbar + bottom page bar
- [ ] + Page / Delete / template button / ↑ ↓ work
- [ ] two-finger pinch zooms (anchored), one finger pans, pen never pans, palm ignored with pen near
- [ ] Ctrl+wheel zoom, Fit (click %), 25-400%
- [ ] Save, close, Open: strokes + templates identical; Ctrl+S autosaves after first save
- [ ] undo/redo works across pages; erase then undo restores stroke
- [ ] 100 pages scroll smoothly
