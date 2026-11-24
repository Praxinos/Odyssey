# Tiled images {#architecture-tiled-image}

Raster layers, raster cells and the render cache of vector layers and cells store their pixels in a `UOdysseyTiledImage`.
Only the tiles holding pixels exist, and their data is shared, compressed and evicted by the `FOdysseyTileManager`,
so that large canvases and long animations do not keep one texture per layer or cell in memory.

## UOdysseyTiledImage

- A map from tile coordinate (pixel position / tile size) to an `FOdysseyTileId`. A missing tile is transparent.
- Square tiles of 64 pixels, in one `EOdysseyTiledImageFormat`: `RGBA8`, `RGBA16F` or `RGBA32F`.
- **Tiles hold linear colors.** 8-bit sRGB content is decoded and stored in `RGBA16F`, an exact round trip;
  8-bit linear content is stored in `RGBA8`; 16-bit and float content in `RGBA32F`.
- **A tile whose alpha is 0 everywhere is empty**: it is dropped, whatever its RGB values.
- Writing: `CopyFromTexture()` replaces the tiles a rect touches. Reading: `BuildRenderFunction()` (game thread) returns a
  function that draws the tiles of a rect in a render graph (render thread); `Render()` wraps it in its own render command.
- `CreateRenderTarget()` creates a render target in the tile pixel format, so that a round trip involves no conversion.
- The pixels can be any size: they are not bound to the canvas. A shrunk canvas keeps the pixels outside it.

### Undo and persistence

```
 change:  Modify()  ->  the transaction records the map of tile ids (the tile data is immutable)
          CopyFromTexture() / RemoveAllTiles()  ->  new ids
 undo:    the transaction restores the previous ids  ->  PostEditUndo()  ->  OnTilesRestored (also on redo)
 save:    Serialize() writes the compressed data and hash of each tile (waits for pending readbacks)
 load:    Serialize() creates the tiles from their compressed data, decompressed on first use
```

The tile map is `Transient`: the ids are only valid during an editor session.
No `FChange` is needed: undo goes through `UObject::Modify()` of the tiled image.

## FOdysseyTileManager

The singleton, created and destroyed by the `OdysseyImaging` module, holds every tile of the editor session.

```
 CopyFromTexture (game thread)
   -> CreateOrUpdateTiles (render graph): draw the source into a GPU atlas, test the empty tiles, enqueue a readback
 Tick (game thread)
   -> readback ready: empty tile -> slot freed; otherwise hash -> deduplicate -> FTileData (uncompressed, in RAM)
   -> compress on the thread pool -> append to the session disk file
   -> eviction (LRU): uncompressed buffers above MaxUncompressedSize, then compressed buffers above MaxCompressedSize
      once they are on disk; a buffer is reloaded (decompressed, or read from disk) when needed
 BuildRenderFunction / GetTileTextures
   -> pending creation texture, or GPU cache hit, or upload; the GPU cache keeps the most recently used tiles
      within MaxGPUSize, repacked into compact atlases
```

- Identical tiles share one `FTileData` (deduplication by hash): duplicated cells and layers cost almost nothing.
- The budgets are per-user editor settings: *Editor Preferences > Plugins > Odyssey - Tile Manager*
  (`UOdysseyTileManagerSettings`, sizes in MB of 10^6 bytes).
- The session directory is under `Saved/Temp/OdysseyTileManager`, deleted on exit; the directories of crashed sessions
  are deleted by the next session (`FOdysseyTileDiskCacheDirectory`).
- `UOdysseyTileManagerBlueprintLibrary` gives Blueprint access to the statistics and to `EvictAllTiles`, for diagnostics.

See the overview comment of `OdysseyTileManager.h` for the threads and locks.

## Bridge with CPU blocks (transitional)

`Odyssey::TiledImage::CopyFromBlock()`, `CopyToBlock()` and `CopyBlockToRenderTarget()` (`OdysseyTiledImageBlock.h`)
convert between `ULIS::FBlock` and tiles. They exist while tools edit CPU blocks, see @ref architecture-raster-editing.
