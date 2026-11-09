# .inkwell file format v1 (QDataStream, Qt_6_0, big-endian)
"INKW" | u32 version=1 | u32 pageCount
per page: QString template | f32 w | f32 h | u32 strokeCount
  per stroke: u32 rgba | f32 width | u8 highlighter | u32 pointCount | pointCount x (f32 x, f32 y, f32 pressure)
  then QByteArray extra (reserved: PDF background, media; empty in v1)
Page units: A4 @96dpi = 794x1123. Writes are atomic (QSaveFile); autosave 3s after last edit once a path exists.
Bump version for incompatible changes; keep readers for older versions.
