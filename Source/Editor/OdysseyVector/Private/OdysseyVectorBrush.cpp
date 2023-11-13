#include "OdysseyVectorBrush.h"

void
FOdysseyVectorBrush::SetTexture( UTexture2D* iTexture )
{
    if( iTexture )
    {
        // We have to change the texture settings or else we won't be able to read the pixels.
        // This loader is only vector-brush oriented, so it does not really matter, we can
        // leave the textures that way.
        iTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
        iTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
        iTexture->SRGB = false;
        iTexture->UpdateResource();
    }

    texture = iTexture;
}

UTexture2D*
FOdysseyVectorBrush::GetTexture() const
{
    return texture;
}
