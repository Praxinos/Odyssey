// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyVectorBrush.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorPath.h"
#include "Brush/OdysseyVectorBrushPath.h"
#include "TextureResource.h"
#include "TextureCompiler.h"

FOdysseyVectorBrush* demoBrush;

// static
void
FOdysseyVectorBrush::MakeDemoBrush( const std::list<FOdysseyVectorObject*>& iObjectList
                                  , const ::ULIS::FRectD& iBoundingBox )
{
    demoBrush = new FOdysseyVectorBrush( nullptr, iObjectList, iBoundingBox );
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

FOdysseyVectorBrush::FOdysseyVectorBrush( FOdysseyVectorObject* iOwner // can be NULL
                                        , const std::list<FOdysseyVectorObject*>& iObjectList
                                        , const ::ULIS::FRectD& iBoundingBox  )
    : FOdysseyVectorBrush( iOwner )
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
    width  = 0;
    height = 0;
    pixels = nullptr;
    bitsPerPixel = 0;

    if( iTexture )
    {
        // We have to change the texture settings or else we won't be able to read the pixels.
        // This loader is only vector-brush oriented, so it does not really matter, we can
        // leave the textures that way.
        iTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
        iTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
        // According to tutorials I found, this is set to false in order to be able to read the pixels
        // but then it triggers a breakpoint in TextureDerivedDataTask.cpp:422 at
        // check(MipView.GammaSpace == LayerData.SourceGammaSpace);
        // it turns out if I leave it to its original value, we can still read the pixels and
        // the error disappears, so I'll leave it that way (commented-out).
        //iTexture->SRGB = false;
        iTexture->UpdateResource();

        width  = iTexture->GetSizeX();
        height = iTexture->GetSizeY();
    }

    texture = iTexture;

    if( owner )
        owner->Invalidate( FOdysseyVectorObject::INVALIDATE_COLOR );
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
        if( texture->GetPlatformData() && texture->GetPlatformData()->Mips.Num() && ( pixels == nullptr ) )
        {
            FTexture2DMipMap *mip = &texture->GetPlatformData()->Mips[0];
            const FColor* colors = static_cast<const FColor*>(mip->BulkData.LockReadOnly());
            EPixelFormat pixelFormat = texture->GetPixelFormat(0);
            uint32 bufferSize;

            pixels = const_cast<FColor*>(colors);
            // Commented-out: do not use these methods. They return a wrong
            // value when the texture is first loaded. then the right value
            // but it means that at first, the texture does not display correctly.
            //width  = texture->GetSurfaceWidth();
            //height = texture->GetSurfaceHeight();

            // Note: this is now useless as we now have the correct width and height values when loading textures
            // because SetTexture is now called in the UOdysseyTextureLayerImageVector::PostLoad, after texture are
            // actually really loaded. We can leave it here just in case, it won't hurt anyways.
            width  = mip->SizeX;
            height = mip->SizeY;

            bufferSize = width * height * sizeof(FColor);

            switch( pixelFormat )
            {
                case PF_B8G8R8A8:
                    bitsPerPixel = 32;

                    //pixels = ( FColor* ) malloc ( bufferSize );

                    //memcpy ( pixels, colors, bufferSize );
                break;

                default : // other formats are unsupported
                    width  = 0;
                    height = 0;
                    pixels = nullptr;
                    bitsPerPixel = 0;
                break;
            }

            texture->GetPlatformData()->Mips[0].BulkData.Unlock();
        }
    }
}

void
FOdysseyVectorBrush::Unlock()
{
/*
    width  = 0;
    height = 0;
    pixels = nullptr;
    bitsPerPixel = 0;
*/
/*
    if( texture )
    {
        if( texture->GetPlatformData()->Mips.Num() )
        {
            texture->GetPlatformData()->Mips[0].BulkData.Unlock();
        }
    }
*/
}
