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

void
FOdysseyVectorBrush::Lock()
{
    if( texture )
    {
        const FColor* colors = static_cast<const FColor*>(texture->PlatformData->Mips[0].BulkData.LockReadOnly());

        pixels = const_cast<FColor*>(colors);
        width  = texture->GetSurfaceWidth();
        height = texture->GetSurfaceHeight();
        bitsPerPixel = 32;
    }
}

void
FOdysseyVectorBrush::Unlock()
{
    if( texture )
    {
        texture->PlatformData->Mips[0].BulkData.Unlock();
    }

    width  = 0;
    height = 0;
    pixels = nullptr;
    bitsPerPixel = 0;
}
