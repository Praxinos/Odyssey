# Known limitations {#architecture-known-limitations}

Keep this page up to date when a limitation is removed or found.

## Transitional CPU editing

The raster tools still edit CPU blocks (`FOdysseyRasterBlockEdit`, `Odyssey::RasterContentBlocks`). Each code site to
replace by an edit of the session render target on the GPU is marked with `TODO(GPU-tools)`:

```
git grep -n "TODO(GPU-tools)" -- Source
```

Consequences until then:
- each stroke, fill or transform reads the whole content back from the GPU and holds two canvas-sized CPU blocks, plus
  the canvas-sized paint block of the tool: RAM use and a short wait at the start of a stroke on very large canvases;
- the selection mask is kept twice: in a tiled image (undo) and in a canvas-sized CPU block read by the tools;
- the final texture of a texture layer stack is read back to the CPU at each update (`FastUpdateTexture`).

## Raster content writes

`FOdysseyRasterContent::Write()` and `Odyssey::RasterContentBlocks::WriteBlock()` begin a canvas-sized edit session,
even to write a small area (paste, clear of a selection, import).

## Tile manager

- Tiles are never released during an editor session: the tiles of previous versions (kept for undo), of deleted layers
  and of closed assets stay in the tile table, and the session disk file only grows. The disk file is deleted on exit.
- The first conversion of colors waits for the compilation of the OpenColorIO shaders (no asynchronous warm-up).
- `UOdysseyTiledImage::Serialize()` waits for the readback of pending tiles when saving.

## Behaviors to know

- A change of the selection during a transform discards the transform.
