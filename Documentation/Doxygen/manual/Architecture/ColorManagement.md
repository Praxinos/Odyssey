# Color management {#architecture-color-management}

## Stored colors

The tiles of a tiled image always hold **linear** colors (see @ref architecture-tiled-image):

| Content | Tile format |
|---|---|
| 8-bit, sRGB (texture with sRGB, BGRA8 animation) | `RGBA16F`, decoded: exact round trip of the bytes |
| 8-bit, linear (texture without sRGB) | `RGBA8` |
| 16-bit or float | `RGBA32F` |

The CPU blocks of the transitional tools (`Odyssey::RasterContentBlocks`) are in the owner format, with 8-bit values
sRGB encoded when the owner is sRGB.

## Blended colors

Layers are blended with **encoded** colors by default, as painting applications do, or with linear colors when asked:

- texture: `UOdysseyTextureLayerStackUserData::bLinearCompositing`;
- animation: `UOdysseyAnimation::bLinearCompositing`. Animations saved before this option are loaded as linear,
  so that they keep their look.

The tiles stay linear in both cases. Each node converts its content when it is rendered:
`IOdysseyTextureRenderingAbility::GetContentColorConversion()` returns `Encode` for a layer blended with encoded colors,
and the stack converts its output back. A render written back into a content uses the inverse conversion
(`InverseColorConversion()`), e.g. a merge or a cross-fade.

The colors displayed to the user, e.g. those the paint bucket compares, are the encoded ones
(`UOdysseyLayerStack::GetDisplayColorConversion()`).

## Conversions

`EOdysseyColorConversion` (`None`, `Encode`: linear to sRGB, `Decode`) is applied on the GPU by an OpenColorIO pass
(`FOdysseyColorConversions::Prepare()` then `FOdysseyColorConversionPass::Add()`), with the built-in configuration
`ocio://cg-config-latest` (Linear Rec.709 and sRGB Encoded Rec.709). Blending itself stays in the Odyssey blend shader.

The OpenColorIO shaders are compiled on first use: the first render of a converted content waits for them.

## sRGB setting of a texture

When the sRGB setting of an existing texture changes, `UOdysseyTextureLayerStackUserData::SRGBChangePolicy` tells whether
its raster layers keep their bytes (default: the image looks different) or their colors (the bytes are converted).
The policy is shown in the details view of the texture. Vector layers always keep their bytes.
