#include "OdysseyVectorBrush.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorPath.h"
#include "Brush/OdysseyVectorBrushPath.h"

FOdysseyVectorBrush* demoBrush;

// static
void
FOdysseyVectorBrush::MakeDemoBrush( const std::list<FOdysseyVectorObject*>& iObjectList
                                  , const ::ULIS::FRectD& iBoundingBox )
{
    demoBrush = new FOdysseyVectorBrush( iObjectList, iBoundingBox );
}

FOdysseyVectorBrush*
FOdysseyVectorBrush::GetDemoBrush()
{
    return demoBrush;
}

void
FOdysseyVectorBrush::Draw( BLContext* iBLContext
                         , const ::ULIS::FRectD& iInvalidationArea
                         , double iAncestorsOpacity
                         , FOdysseyVectorChain* iChain
                         , uint64 iDrawingFlags )
{
    for( FOdysseyVectorBrushObject* brushObject : brushObjectArray )
    {
        brushObject->Draw( iBLContext
                         , iInvalidationArea
                         , iAncestorsOpacity
                         , iChain
                         , iDrawingFlags );
    }
}

FOdysseyVectorBrush::FOdysseyVectorBrush( const std::list<FOdysseyVectorObject*>& iObjectList
                                        , const ::ULIS::FRectD& iBoundingBox  )
{
    width  = 0;
    height = 0;
    bitsPerPixel = 0;
    pixels = nullptr;
    ColorFromBrush = false;
    ExtensionMode = eBrushExtensionMode::Adapt;
    Revert = false;
    BilinearFiltering = false;
    texture = nullptr;

    for( FOdysseyVectorObject* vectorObject : iObjectList )
    {
       if( vectorObject->GetClass() == FOdysseyVectorPath::StaticClass() )
       {
           FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(vectorObject);

           brushObjectArray.push_back( new FOdysseyVectorBrushPath( path, iBoundingBox ) );
       }
    }
}

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
        const FColor* colors = static_cast<const FColor*>(texture->GetPlatformData()->Mips[0].BulkData.LockReadOnly());

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
        texture->GetPlatformData()->Mips[0].BulkData.Unlock();
    }

    width  = 0;
    height = 0;
    pixels = nullptr;
    bitsPerPixel = 0;
}
