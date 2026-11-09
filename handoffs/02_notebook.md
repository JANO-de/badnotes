# Step 02 — Notebook model, pages, zoom/pan | Status: TODO
Scope: C++ `Notebook`/`Page` model (QAbstractListModel), stroke data per page, page templates (blank, lined, grid, dotted), vertical scroll of pages, pinch-zoom + two-finger pan (touch only), palm rejection (ignore touch while pen in proximity), save/load (.inkwell = zip of JSON/binary strokes, versioned).
Files: src/Notebook.*, src/Page.*, src/Serializer.*, qml/NotebookView.qml (replace placeholder).
Move Stroke storage out of InkCanvas into Page; InkCanvas edits a Page.
Checklist:
- [ ] add/delete/reorder pages
- [ ] templates render and switch per page
- [ ] pinch zoom 25–500%, pan smooth, pen never pans
- [ ] save, close, reopen: strokes identical
- [ ] 100 pages scroll at 60fps
