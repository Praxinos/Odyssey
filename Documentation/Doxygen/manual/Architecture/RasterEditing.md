# Raster editing {#architecture-raster-editing}

This page explains how the pixels of a raster layer or cell are read, edited and written, and how a pending edit is
shown before it is committed.

## Glossary

| Term | Class | Meaning |
|---|---|---|
| Content | `FOdysseyRasterContent` | The only access to the pixels of a raster layer or cell. Owned by the layer or cell. |
| Tiled image | `UOdysseyTiledImage` | The storage of the pixels, see @ref architecture-tiled-image. |
| Edit session | `FOdysseyRasterEditSession` | A GPU edit: a canvas-sized render target in the tile format, shown instead of the tiles, then committed into them. |
| Block edit | `FOdysseyRasterBlockEdit` | Transitional CPU front of an edit session, used by the current tools: two CPU blocks (original and working copy). |
| Preview | `FOdysseyRasterEditPreview` | Makes the views of a source show the active edit sessions. |
| Media | `FOdysseyMediaRaster` | A weak handle to a content, given to the tools by the layer or cell. |
| Paint engine | `FOdysseyPaintEngine` | Composes the stroke of a tool with the original pixels of a block edit. |

## Content API

`FOdysseyRasterContent` runs on the game thread. Writes are undoable within the caller's transaction, through
`UOdysseyTiledImage::Modify()`.

- `CreateRenderTarget()`, `Read()`, `Write()`, `Clear()`: GPU access through render targets in the tile format.
- `BeginEditSession()`: starts an edit session. At most one session exists per content; beginning another one ends it.
- `BuildRenderFunction()`: the render of the content, without side effect.
- `EnsureImage()`: creates the tiled image, or converts it when the format the owner expects changed
  (sRGB setting of a texture, format of an animation).
- `Odyssey::RasterContentBlocks` (`ReadBlock`, `WriteBlock`, `EditBlock`, `BeginBlockEdit`): transitional CPU helpers,
  in the owner format (8-bit values sRGB encoded when the owner is sRGB).

## Lifecycle of a tool edit

Today the tools paint on the CPU, into the working block of a block edit:

```
 OnMouseDown   BeginOrKeepEdit(): RasterContentBlocks::BeginBlockEdit(content)   (reads the content, waits for the GPU)
               PaintEngine.BeginEdit(edit)
 each tick     the brush draws into PaintEngine.PaintBlock()
               PaintEngine.Update(blend): original + stroke -> working block -> upload of the changed tiles
               into the session target -> the preview redraws: the views show the target
 shape commit  FScopedTransaction + PaintEngine.Commit(blend): the edited 64-pixel tiles are written (Modify) -> undo entry
 shape abort   PaintEngine.EndEdit(): nothing is written, the views show the tiles again
 Unload        PaintEngine.EndEdit(): also frees the paint block
```

`FOdysseyPaintEngine`:

| Function | Effect |
|---|---|
| `BeginEdit(edit)` | Starts painting into an edit. A previous edit ends without writing anything. The paint block is kept and reused. |
| `DiscardStroke()` | Cancels the uncommitted changes; the edit stays active. |
| `Commit(blend)` | Writes the stroke into the content and ends the edit; the paint block is kept for the next stroke. |
| `EndEdit()` | Stops editing completely: cancels the uncommitted changes, ends the edit and frees the paint block. |

An edit that ends by itself (another edit of the same content began, or an undo or a redo) is dropped at the next `Update()`.

Tools follow one rule: `Load()` prepares the tool for the current layer or frame, `Unload()` ends the edit and frees it.
Liquify and Transform commit their pending edit before.

## How an edit is shown

```
 FOdysseyRasterEditSession::Begin()  -> registry of active sessions -> OnActiveSessionsChanged
 FOdysseyRasterEditPreview (owned by the texture or animation source)
   -> rebuilds the FOdysseyRenderContext of its FOdysseyRenderPreview: one ContentReplacement per active session
 UOdysseyTextureLayerStack / UOdysseyAnimationPlayer render with that context
   -> the content of the edited node is replaced by the session target (its content transform still applies)
 FOdysseyRasterEditSession::Invalidate(rects) -> the preview redraws these rects only
```

Thumbnails and exports do not use the preview: they show the committed tiles.

## Vector content

`FOdysseyVectorContent` renders the vector scene of a layer or cell into a transient, non-transactional tiled image used
as a render cache. Interactive redraws go through an edit session that is never committed. An undo of a vector change
redraws from the scene (`FOdysseyVectorUndo`).

## Planned evolution

The tools will edit the session render target on the GPU, without CPU blocks.
`FOdysseyRasterBlockEdit`, `Odyssey::RasterContentBlocks` and the FBlock adapters of `OdysseyTiledImageBlock.h` will then go.
The code to replace is marked `TODO(GPU-tools)`, see @ref architecture-known-limitations.
