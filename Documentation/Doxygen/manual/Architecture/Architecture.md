# Architecture {#architecture}

This section explains how the main subsystems of the Odyssey plugin fit together.
Each page covers one subsystem and stays short: the details live in the comments of the headers it links to.

- @subpage architecture-tiled-image : how layers and cells store their pixels in tiles, cached on the GPU, in RAM and on disk.
- @subpage architecture-raster-editing : how tools edit raster content, and how an edit is shown before it is committed.
- @subpage architecture-rendering : how a layer stack or an animation is rendered, and how a request overrides parts of it.
- @subpage architecture-color-management : which colors are stored, which are blended, and how they are converted.
- @subpage architecture-known-limitations : known limitations and the work planned to remove them.

## Overview

```
 Texture layer stack / Animation layer stack (editor only)
   |
   +-- raster layer / raster cell -----> FOdysseyRasterContent ---> UOdysseyTiledImage (transactional: undo = Modify())
   |                                          |                          |
   |                                          +-- BeginEditSession() --> FOdysseyRasterEditSession (render target shown
   |                                                                       instead of the tiles until it is committed)
   |
   +-- vector layer / vector cell -----> FOdysseyVectorContent ---> UOdysseyTiledImage (transient render cache)
                                                                         |
                                                                         v
                                                          FOdysseyTileManager (one per editor session)
                                                          GPU atlases (budget, LRU) / RAM (deduplicated, compressed, LRU) / disk
```

- The layer stacks are editor data. At runtime, an animation plays the frames baked into `UOdysseyAnimation`.
- In the editor, `UOdysseyAnimation` renders through the `IOdysseyAnimationRenderingOverride` found on its asset user data
  (`UOdysseyAnimationLayerStackUserData`). A texture renders its `UOdysseyTextureLayerStack` into its source.
- Every render goes through `IOdysseyTextureRenderingAbility::BuildRenderPipeline()` and an `FOdysseyRenderContext`
  (see @ref architecture-rendering).

## Modules

| Module | Type | Role |
|---|---|---|
| OdysseyRendering | Runtime | Render contexts, render pipeline of a node, color conversions (OpenColorIO), tile shaders. |
| OdysseyAnimation | Runtime | `UOdysseyAnimation` (baked frames), `UOdysseyAnimationPlayer`, `FOdysseyAnimationRenderContext`. |
| OdysseyImaging | Editor | `UOdysseyTiledImage`, `FOdysseyTileManager`, raster and vector contents, edit sessions, legacy loaders. |
| OdysseyLayerStack | Editor | Generic layer stack (`UOdysseyLayerStack`, `UOdysseyLayer`), its widgets and commands. |
| OdysseyAnimationEditor | Editor | Animation layers, cells, lighttables, the animation layer stack and its user data. |
| OdysseyTexture | Editor | Texture layers and the texture layer stack. |
| OdysseyPainterEditor | Editor | The painter editor: tools, paint engine, raster selection, sources, shortcuts. |
