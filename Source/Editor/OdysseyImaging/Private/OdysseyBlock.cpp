// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyBlock.h"
#include <ULIS_CORE>

/////////////////////////////////////////////////////
// Utlity
uint32 ULISFormatForUE4TextureSourceFormat( ETextureSourceFormat iFormat )
{
    uint32 ret = 0;
    switch( iFormat ) 
    {
        case TSF_Invalid:   ret = 0;                                                        break;
        case TSF_G8:        ret = ::ULIS::Format::Format_uint8GnoAlphaGtypeLimits;          break;
        case TSF_BGRA8:     ret = ::ULIS::Format::Format_uint8RGBhasAlphaBGRAtypeLimits;    break;
        case TSF_BGRE8:     ret = ::ULIS::Format::Format_uint8RGBnoAlphaBGRtypeLimits;      break;
        case TSF_RGBA16:    ret = ::ULIS::Format::Format_uint16RGBhasAlphaRGBAtypeLimits;   break;
        case TSF_RGBA16F:   ret = ::ULIS::Format::Format_floatRGBhasAlphaRGBAnormalized;    break;
        case TSF_RGBA8:     ret = ::ULIS::Format::Format_uint8RGBhasAlphaRGBAtypeLimits;    break;
        case TSF_RGBE8:     ret = ::ULIS::Format::Format_uint8RGBnoAlphaRGBtypeLimits;      break;
        case TSF_MAX:       ret = 0;                                                        break;
        default:            ret = 0;                                                        break;
    }

    checkf( ret, TEXT( "Error, bad format !" ) ); // Crash
    return ret;
}

/////////////////////////////////////////////////////
// FOdysseyBlock
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyBlock::~FOdysseyBlock()
{
    mArray.Empty();
    delete mIBlock;
    mIBlock = nullptr;
}

FOdysseyBlock::FOdysseyBlock( int                           iWidth
                            , int                           iHeight
                            , ETextureSourceFormat          iFormat
                            , ::ULIS::fpInvalidateFunction  iInvFunc
                            , void*                         iInvInfo
                            , bool                          iInitializeData )
    : mUE4TextureSourceFormat( iFormat )
    , mULISFormat( ULISFormatForUE4TextureSourceFormat( mUE4TextureSourceFormat ) )
    , mIBlock( nullptr )
    , mArray()
{
    // Retrieve spec info from ULIS format hash.
    ::ULIS::FSpec spec = ::ULIS::FSpecContext::BlockSpecFromHash( mULISFormat );
    int  bytesPerPixels = spec._pd;

    // Allocate and fill array ( primary data rep )
    if( iInitializeData )
        mArray.SetNumZeroed( iWidth * iHeight * bytesPerPixels );
    else
        mArray.SetNumUninitialized( iWidth * iHeight * bytesPerPixels );

    // Allocate block from external array data
    mIBlock = ::ULIS::FMakeContext::MakeBlockFromExternalData( iWidth, iHeight, mArray.GetData(), mULISFormat );

    // Setup invalid callback
    mIBlock->SetInvalidateCB( iInvFunc, iInvInfo );
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API
TArray64< uint8 >&
FOdysseyBlock::GetArray()
{
    return mArray;
}

const TArray64< uint8 >&
FOdysseyBlock::GetArray() const
{
    return mArray;
}

::ULIS::IBlock*
FOdysseyBlock::GetIBlock()
{
    return mIBlock;
}

const ::ULIS::IBlock*
FOdysseyBlock::GetIBlock() const
{
    return mIBlock;
}

int
FOdysseyBlock::Width() const
{
    return mIBlock->Width();
}

int
FOdysseyBlock::Height() const
{
    return mIBlock->Height();
}

FVector2D
FOdysseyBlock::Size() const
{
    return FVector2D( Width(), Height() );
}

ETextureSourceFormat
FOdysseyBlock::GetUE4TextureSourceFormat() const
{
    return mUE4TextureSourceFormat;
}

uint32
FOdysseyBlock::GetULISFormat() const
{
    return mULISFormat;
}

void
FOdysseyBlock::Reallocate( int                          iWidth
                         , int                          iHeight
                         , ETextureSourceFormat         iFormat
                         , ::ULIS::fpInvalidateFunction iInvFunc
                         , void*                        iInvInfo )
{
    // Set UE4 Format
    mUE4TextureSourceFormat = iFormat;

    // Retrieve ULIS format hash.
    mULISFormat = ULISFormatForUE4TextureSourceFormat( mUE4TextureSourceFormat );

    // Retrieve spec info from ULIS format hash.
    ::ULIS::FSpec spec = ::ULIS::FSpecContext::BlockSpecFromHash( mULISFormat );
    int bytesPerPixels = spec._pd;

    // Empty and realloc array ( primary data rep )
    // data is not initialized / filled !
    mArray.Empty( iWidth * iHeight * bytesPerPixels );

    // Delete existing block
    delete mIBlock;

    // Allocate block from external array data
    mIBlock = ::ULIS::FMakeContext::MakeBlockFromExternalData( iWidth, iHeight, mArray.GetData(), mULISFormat );

    // Setup invalid callback
    mIBlock->SetInvalidateCB( iInvFunc, iInvInfo );
}

