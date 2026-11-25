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
- each tick of a stroke uploads the changed tiles of the working block and waits for the GPU
  (`FlushRenderingCommands()` in `CopyBlockToRenderTarget()`);
- the final texture of a texture layer stack is read back to the CPU at each update (`FastUpdateTexture`). Its texture
  source is only written at some moments (property change, save, deactivation): an `UpdateResource()` called meanwhile
  rebuilds the texture from older pixels.

## Raster content writes

`FOdysseyRasterContent::Write()` and `Odyssey::RasterContentBlocks::WriteBlock()` begin a canvas-sized edit session,
even to write a small area (paste, clear of a selection, import).

## Tile manager

- Tiles are never released during an editor session: the tiles of previous versions (kept for undo), of deleted layers
  and of closed assets stay in the tile table, and the session disk file only grows. The disk file is deleted on exit.
- The first conversion of colors waits for the compilation of the OpenColorIO shaders (no asynchronous warm-up).
- `UOdysseyTiledImage::Serialize()` waits for the readback of pending tiles when saving.
- Loading is not lazy: `UOdysseyTiledImage::Serialize()` reads every compressed tile of the asset into RAM, then copies
  them into the session disk file. The peak RAM at load is the compressed size of the asset, until the next tick evicts.
- The raw-tile RAM budget can be exceeded until tiles are written to disk (`EvictUncompressed` skips tiles not yet cached).
- The GPU tile budget (`MaxGPUSize`, 256 MB by default) is soft: it is exceeded until the next maintenance pass, and a
  frame whose visible tiles do not fit uploads them again at every render.

## Animation memory (inherited from odyssey-dev-5.8)

- `UOdysseyAnimation::PreSave()` bakes one `UTexture2D` per distinct frame at every editor save, for cooked playback.
  These textures are loaded with the asset in the editor: their cost grows with the number of frames.
- Each `UOdysseyAnimationPlayer` owns a canvas-sized `RGBA16F` render target with mips (about 171 MB at 4096 x 4096),
  one per animation component and per painter editor source.

## Behaviors to know

- A change of the selection during a transform discards the transform.
