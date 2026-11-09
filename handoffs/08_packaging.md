# Step 08 — Packaging | Status: TODO
Scope: working PKGBUILD (source from git tag), icons/desktop/metainfo, AppImage via linuxdeploy-plugin-qt, CI script, version bump + tar.gz update flow.
Checklist:
- [ ] makepkg -si installs and launches
- [ ] AppImage runs on clean Arch + Fedora container
- [ ] icon and name show in GNOME launcher

## Self-update (DONE)
src/Updater.*: on start (+every 6h) GET GitHub release `nightly`; its body = commit sha of the build. If it differs from compiled INKWELL_BUILD and $APPIMAGE is set, downloads the asset, checks ELF header/size, atomically replaces $APPIMAGE; toast asks to restart. CI passes -DINKWELL_BUILD=$GITHUB_SHA and sets release body to the sha. Disable: QSettings autoUpdate=false (no UI yet; add in step 07 settings). Test override: INKWELL_UPDATE_URL.
