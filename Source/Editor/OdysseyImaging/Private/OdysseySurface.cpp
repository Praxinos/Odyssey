// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseySurface.h"
#include "OdysseyBlock.h"
#include <ULIS_CORE>

/////////////////////////////////////////////////////
// Utlity

void
CopyUTextureDataIntoBlock( FOdysseyBlock* iBlock, UTexture2D* iTexture )
{
    checkf( iBlock->Width() == iTexture->GetSizeX() &&
            iBlock->Height() == iTexture->GetSizeY()
            , TEXT( "Sizes do not match" ) );

    iTexture->Source.GetMipData( iBlock->GetArray(), 0 );
}

void
CopyBlockDataIntoUTexture( FOdysseyBlock* iBlock, UTexture2D* iTexture )
{
    checkf( iBlock->Width() == iTexture->GetSizeX() &&
            iBlock->Height() == iTexture->GetSizeY()
            , TEXT( "Sizes do not match" ) );

    iTexture->Source.Init( iBlock->Width(), iBlock->Height(), 1, 1, iBlock->GetUE4TextureSourceFormat(), iBlock->GetIBlock()->DataPtr() );
}

FOdysseyBlock*
NewOdysseyBlockFromUTextureData( UTexture2D* iTexture )
{
    FOdysseyBlock* ret = new FOdysseyBlock( iTexture->GetSizeX(), iTexture->GetSizeY(), iTexture->Source.GetFormat() );
    CopyUTextureDataIntoBlock( ret, iTexture );

    return ret;
}

void
InvalidateSurfaceFromData( FOdysseyBlock* iData, FOdysseySurface* iSurface )
{
    InvalidateTextureFromData( iData, iSurface->Texture() );
}

void
InvalidateSurfaceFromData( FOdysseyBlock* iData, FOdysseySurface* iSurface, int x1, int y1, int x2, int y2 )
{
    InvalidateTextureFromData( iData, iSurface->Texture(), x1, y1, x2, y2 );
}

void
InvalidateTextureFromData( FOdysseyBlock* iData, UTexture2D* iTexture )
{
    InvalidateTextureFromData( iData, iTexture, 0, 0, iData->Width(), iData->Height() );
}

void
InvalidateTextureFromData( FOdysseyBlock* iData, UTexture2D* iTexture, const ::ULIS::FRect& iRect )
{
    checkf( iData, TEXT( "Error" ) );
    checkf( iTexture, TEXT( "Error" ) );

    checkf( iData->GetUE4TextureSourceFormat() == iTexture->Source.GetFormat(), TEXT( "Bad format" ) );
    checkf( iData->Width() == iTexture->GetSizeX() &&
            iData->Height() == iTexture->GetSizeY()
            , TEXT( "Sizes do not match" ) );

    int x = iRect.x;
    int y = iRect.y;
    int w = iRect.w;
    int h = iRect.h;
    checkf( x >= 0 && 
            x >= 0 && 
            w > 0  && 
            h > 0
            , TEXT( "Error" ) );

    // Considering only one region is an assumption that works but you have to be more carefull with several regions.
    FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D( x, y, x, y, w, h );

    TFunction<void(uint8* SrcData, const FUpdateTextureRegion2D* Regions)> dataCleanupFunc = [&](uint8*, const FUpdateTextureRegion2D* Regions ) {
        delete Regions;
    };

    uint32 bpp = iData->GetIBlock()->BytesPerPixel();
    uint32 pitch = iData->GetIBlock()->BytesPerScanLine();
    iTexture->UpdateTextureRegions( 0, 1, region, pitch, bpp, iData->GetIBlock()->DataPtr(), dataCleanupFunc );
}

void
InvalidateTextureFromData( FOdysseyBlock* iData, UTexture2D* iTexture, int x1, int y1, int x2, int y2 )
{
    checkf( iData, TEXT( "Error" ) );
    checkf( iTexture, TEXT( "Error" ) );

    checkf( iData->GetUE4TextureSourceFormat() == iTexture->Source.GetFormat(), TEXT( "Bad format" ) );
    checkf( iData->Width() == iTexture->GetSizeX() &&
            iData->Height() == iTexture->GetSizeY()
            , TEXT( "Sizes do not match" ) );

    int w = x2 - x1;
    int h = y2 - y1;
    checkf( x1 >= 0 &&
            x2 >= 0 &&
            y1 >= 0 &&
            y2 >= 0 &&
            w > 0  &&
            h > 0
            , TEXT( "Error" ) );

    // Considering only one region is an assumption that works but you have to be more carefull with several regions.
    FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D( x1, y1, x1, y1, w, h );

    TFunction<void(uint8* SrcData, const FUpdateTextureRegion2D* Regions)> dataCleanupFunc = [&](uint8*, const FUpdateTextureRegion2D* Regions ) {
        delete Regions;
    };

    uint32 bpp = iData->GetIBlock()->BytesPerPixel();
    uint32 pitch = iData->GetIBlock()->BytesPerScanLine();
    iTexture->UpdateTextureRegions( 0, 1, region, pitch, bpp, iData->GetIBlock()->DataPtr(), dataCleanupFunc );
}

void
InvalidateTextureFromData( ::ULIS::IBlock* iData, UTexture2D* iTexture, const ::ULIS::FRect& iRect )
{
    checkf( iData, TEXT( "Error" ) );
    checkf( iTexture, TEXT( "Error" ) );

    checkf( iData->Width() == iTexture->GetSizeX() &&
            iData->Height() == iTexture->GetSizeY()
            , TEXT( "Sizes do not match" ) );

    int x = iRect.x;
    int y = iRect.y;
    int w = iRect.w;
    int h = iRect.h;
    checkf( x >= 0 &&
            y >= 0 &&
            w > 0  &&
            h > 0
            , TEXT( "Error" ) );

    // Considering only one region is an assumption that works but you have to be more carefull with several regions.
    FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D( x, y, x, y, w, h );

    TFunction<void(uint8* SrcData, const FUpdateTextureRegion2D* Regions)> dataCleanupFunc = [&](uint8*, const FUpdateTextureRegion2D* Regions ) {
        delete Regions;
    };

    uint32 bpp = iData->BytesPerPixel();
    uint32 pitch = iData->BytesPerScanLine();
    iTexture->UpdateTextureRegions( 0, 1, region, pitch, bpp, iData->DataPtr(), dataCleanupFunc );
}

void
InvalidateSurfaceFromData( ::ULIS::IBlock* iData, FOdysseySurface* iSurface, const ::ULIS::FRect& iRect )
{
    InvalidateTextureFromData( iData, iSurface->Texture(), iRect );
}

void
InvalidateSurfaceCallback( FOdysseyBlock* iData, void* iInfo, int iX1, int iY1, int iX2, int iY2 )
{
    FOdysseySurface* surface = static_cast< FOdysseySurface* >( iInfo );
    InvalidateSurfaceFromData( iData, surface, iX1, iY1, iX2, iY2 );
}

void
InvalidateLiveSurfaceCallback( FOdysseyBlock* iData, void* iInfo, int iX1, int iY1, int iX2, int iY2 )
{
    FOdysseyLiveUpdateInfo* liveUpdateInfo = static_cast< FOdysseyLiveUpdateInfo* >( iInfo );
    InvalidateTextureFromData( iData, liveUpdateInfo->main, iX1, iY1, iX2, iY2 );

    if( liveUpdateInfo->enabled )
        InvalidateTextureFromData( iData, liveUpdateInfo->live, iX1, iY1, iX2, iY2 );
}

void
InvalidateSurfaceCallback( ::ULIS::IBlock* iData, void* iInfo, const ::ULIS::FRect& iRect )
{
    FOdysseySurface* surface = static_cast< FOdysseySurface* >( iInfo );
    InvalidateSurfaceFromData( iData, surface, iRect );
}

void
InvalidateLiveSurfaceCallback( ::ULIS::IBlock* iData, void* iInfo, const ::ULIS::FRect& iRect )
{
    FOdysseyLiveUpdateInfo* liveUpdateInfo = static_cast< FOdysseyLiveUpdateInfo* >( iInfo );
    InvalidateTextureFromData( iData, liveUpdateInfo->main, iRect );

    if( liveUpdateInfo->enabled )
        InvalidateTextureFromData( iData, liveUpdateInfo->live, iRect );
}

namespace detail {

EPixelFormat
UE4PixelFormatForUE4TextureSourceFormat( ETextureSourceFormat iFormat )
{
    EPixelFormat ret = PF_Unknown;
    switch( iFormat )
    {
        case TSF_Invalid:   ret = PF_Unknown;           break;
        case TSF_G8:        ret = PF_G8;                break;
        case TSF_BGRA8:     ret = PF_B8G8R8A8;          break;
        case TSF_BGRE8:     ret = PF_Unknown;           break;
        case TSF_RGBA16:    ret = PF_A16B16G16R16; break;
        case TSF_RGBA16F:   ret = PF_FloatRGBA;         break;
        case TSF_RGBA8:     ret = PF_Unknown;           break;
        case TSF_RGBE8:     ret = PF_Unknown;           break;
        case TSF_MAX:       ret = PF_Unknown;           break;
        default:            ret = PF_Unknown;           break;
    }
    checkf( ret != PF_Unknown, TEXT( "Bad format" ) ); // Crash
    return ret;
}

} // namespace detail

/////////////////////////////////////////////////////
// FOdysseySurface
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseySurface::~FOdysseySurface()
{
    mTexture->RemoveFromRoot();
    if( !mIsBorrowedTexture ) // If not borrowed, that means transient hence we are responsible for dealloc
    {
        checkf( mTexture, TEXT( "Error: texture should be a valid pointer" ) );
        //texture->RemoveFromRoot(); // RM Prevent GC
        //delete texture;
        if( mTexture->IsValidLowLevel() )
            mTexture->ConditionalBeginDestroy();
        mTexture = nullptr;
    }

    mBlock->GetIBlock()->SetInvalidateCB( NULL, NULL );
    if( !mIsBorrowedBlock )
    {
        if( mBlock )
        {
            delete mBlock;
            mBlock = nullptr;
        }
    }
}

FOdysseySurface::FOdysseySurface( int iWidth, int iHeight, ETextureSourceFormat iFormat )
    : mIsBorrowedTexture( false )
    , mIsBorrowedBlock( false )
{
    mTexture = UTexture2D::CreateTransient( iWidth, iHeight, ::detail::UE4PixelFormatForUE4TextureSourceFormat( iFormat ) );
#if WITH_EDITORONLY_DATA
    mTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
#endif
    mTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    mTexture->SRGB = 1;
    //texture->AddToRoot(); // Prevent GC
    mTexture->Filter = TextureFilter::TF_Nearest;
    mTexture->UpdateResource();
    mTexture->AddToRoot();

    // Warning: the texture data source / bulk is allocated, then the block is allocated, then we copy the block content into bulk.
    mBlock = new FOdysseyBlock( iWidth, iHeight, iFormat, &InvalidateSurfaceCallback, static_cast< void* >( this ), true );
    // load texture data from block
    CopyBlockDataIntoUTexture( mBlock, mTexture );
}

FOdysseySurface::FOdysseySurface( UTexture2D* iTexture )
    : mIsBorrowedTexture( true )
    , mIsBorrowedBlock( false )
{
    checkf( iTexture, TEXT( "Cannot Initialize with Null borrowed texture" ) );
    mTexture = iTexture;
    mTexture->AddToRoot();

    // Warning: the block is allocated, then the texture data is copied into it.
    mBlock = new FOdysseyBlock( mTexture->GetSizeX(), mTexture->GetSizeY(), iTexture->Source.GetFormat(), &InvalidateSurfaceCallback, static_cast< void* >( this ) );
    // load block data from texture
    CopyUTextureDataIntoBlock( mBlock, mTexture );
}

FOdysseySurface::FOdysseySurface( FOdysseyBlock* iBlock )
    : mIsBorrowedTexture( false )
    , mIsBorrowedBlock( true )
{
    checkf( iBlock, TEXT( "Cannot Initialize with Null borrowed block" ) );
    mBlock = iBlock;

    mTexture = UTexture2D::CreateTransient( mBlock->Width(), mBlock->Height(), ::detail::UE4PixelFormatForUE4TextureSourceFormat( iBlock->GetUE4TextureSourceFormat() ) );
#if WITH_EDITORONLY_DATA
    mTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
#endif
    mTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    mTexture->SRGB = 1;
    mTexture->Filter = TextureFilter::TF_Nearest;
    mTexture->UpdateResource();
    mTexture->AddToRoot();

    mBlock->GetIBlock()->SetInvalidateCB( &InvalidateSurfaceCallback, static_cast< void* >( this ) );

    // load texture data from block
    CopyBlockDataIntoUTexture( mBlock, mTexture );
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

FOdysseyBlock*
FOdysseySurface::Block()
{
    return mBlock;
}

const FOdysseyBlock*
FOdysseySurface::Block() const
{
    return mBlock;
}

UTexture2D*
FOdysseySurface::Texture()
{
    return mTexture;
}

const UTexture2D*
FOdysseySurface::Texture() const
{
    return mTexture;
}

bool
FOdysseySurface::IsBorrowedTexture() const
{
    return mIsBorrowedTexture;
}

void
FOdysseySurface::CommitBlockChangesIntoTextureBulk()
{
    CopyBlockDataIntoUTexture( mBlock, mTexture );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- Public Tampon Methods
int
FOdysseySurface::Width()
{
    return mBlock->Width();
}

int
FOdysseySurface::Height()
{
    return mBlock->Height();
}

void
FOdysseySurface::Invalidate()
{
    mBlock->GetIBlock()->Invalidate();
}

void
FOdysseySurface::Invalidate( int iX1, int iY1, int iX2, int iY2 )
{
    mBlock->GetIBlock()->Invalidate( ::ULIS::FRect::FromMinMax( iX1, iY1, iX2, iY2 ) );
}

void
FOdysseySurface::Invalidate( const ::ULIS::FRect& iRect )
{
    mBlock->GetIBlock()->Invalidate( iRect );
}
