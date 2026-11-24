# Rendering {#architecture-rendering}

Textures layers, layer stacks, animations, layers and cells are render nodes: they implement
`IOdysseyTextureRenderingAbility`. A render request is described by an `FOdysseyRenderContext`.

## Render pipeline of a node

`IOdysseyTextureRenderingAbility::BuildRenderPipeline()` runs on the game thread and builds a render function, run later
in a render graph on the render thread. For each node, in order:

```
 1. Filter                  the context may skip the node
 2. RenderReplacement       replaces the whole render of the node, or else BuildRenderPipelineInternal()
 3. ContentReplacement      replaces the content of a non-composite node (rendered with GetContentTransform())
 4. ContentDecorator        wraps the content render
 5. color conversion        GetContentColorConversion() of the node, non-composite nodes only (see @ref architecture-color-management)
 6. RenderDecorator         wraps the whole render of the node
```

Steps 2, 3, 4 and 6 come from the `FOdysseyRenderOverride` registered in the context for the rendering id of the node;
step 5 comes from the node itself. When RenderReplacement is set, steps 3 to 5 are skipped.
A node is composite when `BuildsCompositeContent()` is true: every `UOdysseyLayer` (a texture layer only when it can have
children), whose content is the blend of its children. A layer stack is not composite: its output is converted as a whole.

## FOdysseyRenderContext

- `Overrides`: one `FOdysseyRenderOverride` per rendering id. The edit sessions use `ContentReplacement`
  (see @ref architecture-raster-editing); the other callbacks are extension points of the API.
- `Filter` and `Type` (`EOdysseyRenderingType` flags).
- Extensions, cloned with the context and found by type: `FindOrAddExtension<T>()`, where `T` derives from
  `IOdysseyRenderContextExtension` and defines `static FName TypeName()`.
  - `FOdysseyAnimationRenderContext`: the frame to render. `FOdysseyAnimationRenderContext::MakeForFrame(Frame)` builds a
    context for one animation frame.
  - `FOdysseyLayerStackRenderContext`: overrides of the properties of layers (visibility, opacity, blending, lighttable)
    for one request, without changing the layers.

## Previews

An `FOdysseyRenderPreview` is a view-local render context, with a revision and an `OnInvalidated(rects)` delegate.
`UOdysseyTextureLayerStack::SetRenderPreview()` and `UOdysseyAnimationPlayer::SetRenderPreview()` make a view render
with it. The painter editor sources give them the preview of their `FOdysseyRasterEditPreview`.
