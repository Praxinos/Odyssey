// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "OdysseySurface.h"
#include "OdysseyBlock.h"
#include <ULIS_CORE>

/////////////////////////////////////////////////////
// Utlity


void
CopyUTextureDataIntoBlock( FOdysseyBlock* block, UTexture2D* texture )
{
    checkf( block->Width() == texture->GetSizeX() &&
            block->Height() == texture->GetSizeY()
            , TEXT( "Sizes do not match" ) );
    texture->Source.GetMipData( block->GetArray(), 0 );
}


void
CopyBlockDataIntoUTexture( FOdysseyBlock* block, UTexture2D* texture )
{
    checkf( block->Width() == texture->GetSizeX() &&
            block->Height() == texture->GetSizeY()
            , TEXT( "Sizes do not match" ) );
    texture->Source.Init( block->Width(), block->Height(), 1, 1, block->GetUE4TextureSourceFormat(), block->GetIBlock()->DataPtr() );
}


FOdysseyBlock*
NewOdysseyBlockFromUTextureData( UTexture2D* texture )
{
    FOdysseyBlock* ret = new FOdysseyBlock( texture->GetSizeX(), texture->GetSizeY(), texture->Source.GetFormat() );
    CopyUTextureDataIntoBlock( ret, texture );
    return ret;
}


void
InvalidateSurfaceFromData( FOdysseyBlock* data, FOdysseySurface* surface )
{
    InvalidateTextureFromData( data, surface->Texture() );
}


void
InvalidateSurfaceFromData( FOdysseyBlock* data, FOdysseySurface* surface, int x1, int y1, int x2, int y2 )
{
    InvalidateTextureFromData( data, surface->Texture(), x1, y1, x2, y2 );
}


void
InvalidateTextureFromData( FOdysseyBlock* data, UTexture2D* texture )
{
    InvalidateTextureFromData( data, texture, 0, 0, data->Width(), data->Height() );
}


void
InvalidateTextureFromData( FOdysseyBlock* data, UTexture2D* texture, const  ::ULIS::FRect& iRect )
{
    checkf( data, TEXT( "Error" ) );
    checkf( texture, TEXT( "Error" ) );

    checkf( data->GetUE4TextureSourceFormat() == texture->Source.GetFormat(), TEXT( "Bad format" ) );
    checkf( data->Width()  == texture->GetSizeX() &&
            data->Height() == texture->GetSizeY()
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

    TFunction<void(uint8* SrcData, const FUpdateTextureRegion2D* Regions)> DataCleanupFunc = [&](uint8*, const FUpdateTextureRegion2D* Regions ) {
        delete Regions;
    };

    uint32 bpp = data->GetIBlock()->BytesPerPixel();
    uint32 pitch = data->GetIBlock()->BytesPerScanLine();
    texture->UpdateTextureRegions( 0, 1, region, pitch, bpp, data->GetIBlock()->DataPtr(), DataCleanupFunc );
}


void
InvalidateTextureFromData( FOdysseyBlock* data, UTexture2D* texture, int x1, int y1, int x2, int y2 )
{
    checkf( data, TEXT( "Error" ) );
    checkf( texture, TEXT( "Error" ) );

    checkf( data->GetUE4TextureSourceFormat() == texture->Source.GetFormat(), TEXT( "Bad format" ) );
    checkf( data->Width()  == texture->GetSizeX() &&
            data->Height() == texture->GetSizeY()
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

    TFunction<void(uint8* SrcData, const FUpdateTextureRegion2D* Regions)> DataCleanupFunc = [&](uint8*, const FUpdateTextureRegion2D* Regions ) {
        delete Regions;
    };

    uint32 bpp = data->GetIBlock()->BytesPerPixel();
    uint32 pitch = data->GetIBlock()->BytesPerScanLine();
    texture->UpdateTextureRegions( 0, 1, region, pitch, bpp, data->GetIBlock()->DataPtr(), DataCleanupFunc );
}


void
InvalidateTextureFromData( ::ULIS::IBlock* iData, UTexture2D* iTexture, const  ::ULIS::FRect& iRect )
{
    checkf( iData, TEXT( "Error" ) );
    checkf( iTexture, TEXT( "Error" ) );

    checkf( iData->Width()  == iTexture->GetSizeX() &&
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

    TFunction<void(uint8* SrcData, const FUpdateTextureRegion2D* Regions)> DataCleanupFunc = [&](uint8*, const FUpdateTextureRegion2D* Regions ) {
        delete Regions;
    };

    uint32 bpp = iData->BytesPerPixel();
    uint32 pitch = iData->BytesPerScanLine();
    iTexture->UpdateTextureRegions( 0, 1, region, pitch, bpp, iData->DataPtr(), DataCleanupFunc );
}


void
InvalidateSurfaceFromData( ::ULIS::IBlock* iData, FOdysseySurface* iSurface, const  ::ULIS::FRect& iRect )
{
    InvalidateTextureFromData( iData, iSurface->Texture(), iRect );
}

void
InvalidateSurfaceCallback( FOdysseyBlock* data, void* info, int x1, int y1, int x2, int y2 )
{
    FOdysseySurface* surface = static_cast< FOdysseySurface* >( info );
    InvalidateSurfaceFromData( data, surface, x1, y1, x2, y2 );
}


void
InvalidateLiveSurfaceCallback( FOdysseyBlock* data, void* info, int x1, int y1, int x2, int y2 )
{
    FOdysseyLiveUpdateInfo* liveUpdateInfo = static_cast< FOdysseyLiveUpdateInfo* >( info );
    InvalidateTextureFromData( data, liveUpdateInfo->main, x1, y1, x2, y2 );

    if( liveUpdateInfo->enabled )
        InvalidateTextureFromData( data, liveUpdateInfo->live, x1, y1, x2, y2 );
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
UE4PixelFormatForUE4TextureSourceFormat( ETextureSourceFormat fmt )
{
    EPixelFormat ret = PF_Unknown;
    switch( fmt ) {
        case TSF_Invalid:   ret = PF_Unknown;           break;
        case TSF_G8:        ret = PF_G8;                break;
        case TSF_BGRA8:     ret = PF_B8G8R8A8;          break;
        case TSF_BGRE8:     ret = PF_Unknown;           break;
        case TSF_RGBA16:    ret = PF_R16G16B16A16_UINT; break;
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
    if( !mBorrowedTex ) // If not borrowed, that means transient hence we are responsible for dealloc
    {
        checkf( mTexture, TEXT( "Error: texture should be a valid pointer" ) );
        //texture->RemoveFromRoot(); // RM Prevent GC
        //delete texture;
        if( mTexture->IsValidLowLevel() )
            mTexture->ConditionalBeginDestroy();
        mTexture = nullptr;
    }

    mBlock->GetIBlock()->SetInvalidateCB( NULL, NULL );
    if( !mBorrowedBlock )
    {
        if( mBlock )
            delete  mBlock;
    }
}


FOdysseySurface::FOdysseySurface( int iWidth, int iHeight, ETextureSourceFormat fmt )
    : mBorrowedTex( false )
    , mBorrowedBlock( false )
{
    mTexture = UTexture2D::CreateTransient( iWidth, iHeight, ::detail::UE4PixelFormatForUE4TextureSourceFormat( fmt ) );
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
    mBlock = new FOdysseyBlock( iWidth, iHeight, fmt, &InvalidateSurfaceCallback, static_cast< void* >(this), true);
    // load texture data from block
    CopyBlockDataIntoUTexture( mBlock, mTexture );
}


FOdysseySurface::FOdysseySurface( UTexture2D* iTex )
    : mBorrowedTex( true )
    , mBorrowedBlock( false )
{
    checkf( iTex, TEXT( "Cannot Initialize with Null borrowed texture" ) );
    mTexture = iTex;
    mTexture->AddToRoot();

    // Warning: the block is allocated, then the texture data is copied into it.
    mBlock = new FOdysseyBlock( mTexture->GetSizeX(), mTexture->GetSizeY(), iTex->Source.GetFormat(), &InvalidateSurfaceCallback, static_cast< void* >(this));
    // load block data from texture
    CopyUTextureDataIntoBlock( mBlock, mTexture );
}


FOdysseySurface::FOdysseySurface( FOdysseyBlock* iBlock )
    : mBorrowedTex( false )
    , mBorrowedBlock( true )
{
    checkf( iBlock, TEXT( "Cannot Initialize with Null borrowed block" ) );
    mBlock = iBlock;

    mTexture = UTexture2D::CreateTransient( mBlock->Width(), mBlock->Height(), PF_B8G8R8A8 );
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
    return  mBlock;
}


const FOdysseyBlock*
FOdysseySurface::Block() const
{
    return  mBlock;
}


UTexture2D*
FOdysseySurface::Texture()
{
    return  mTexture;
}


const UTexture2D*
FOdysseySurface::Texture() const
{
    return  mTexture;
}


bool
FOdysseySurface::IsBorrowedTexture() const
{
    return  mBorrowedTex;
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
    return  mBlock->Width();
}


int
FOdysseySurface::Height()
{
    return  mBlock->Height();
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

