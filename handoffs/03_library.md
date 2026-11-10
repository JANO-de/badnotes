# Step 03 — Library + page sidebar | Status: DONE (headless-verified: model ops, covers, thumbnails; touch interactions NOT verified on hardware)
Implemented: Library (QAbstractListModel over ~/.local/share/inkwell/notebooks; folders = dirs, notebooks = *.inkwell; create/rename/duplicate/delete, recursive title search, breadcrumb/up, QFileSystemWatcher refresh), image://cover/<path>?<mtime> provider (first page render), PageRenderer (shared by canvas/thumbs/covers), PageThumb (cached, 350ms debounce), LibraryView (glass header, grid, long-press/right-click menu, AskPopup dialogs), PageSidebar (thumbs, tap=go, long-press+drag=reorder), Main now has library/editor modes ("‹ Library" saves and returns, Esc too, Ctrl+N = new notebook in current folder, ☰ toggles sidebar).
Known gaps: no cover customisation/notebook titles separate from filename; folder duplicate/move-into-folder (drag) missing; menus/dialog use Basic style (polish in step 07); sidebar drag-reorder is not undoable; folder item count is direct children only.
Test checklist:
- [ ] app opens on Library; "+ Notebook" creates and opens one
- [ ] "‹ Library" returns; cover shows your drawing
- [ ] "+ Folder", open it, create notebook inside, "‹" goes up
- [ ] long-press (or right-click) a notebook: Rename / Duplicate / Delete work
- [ ] search finds notebooks inside folders
- [ ] ☰ sidebar: thumbnails update after drawing, tap jumps to page
- [ ] long-press a thumbnail then drag: page reorders
