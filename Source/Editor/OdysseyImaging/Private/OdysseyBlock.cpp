// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyBlock.h"
#include <ULIS3>

/////////////////////////////////////////////////////
// Utlity
uint32 ULISFormatForUE4TextureSourceFormat( ETextureSourceFormat iFormat )
{
    uint32 ret = 0;
    switch( iFormat ) {
        case TSF_Invalid:   ret = 0;                    break;
        case TSF_G8:        ret = ULIS3_FORMAT_G8;      break;
        case TSF_BGRA8:     ret = ULIS3_FORMAT_BGRA8;   break;
        case TSF_BGRE8:     ret = ULIS3_FORMAT_BGR8;    break;
        case TSF_RGBA16:    ret = ULIS3_FORMAT_RGBA16;  break;
        case TSF_RGBA16F:   ret = ULIS3_FORMAT_RGBA16;  break;
        case TSF_RGBA8:     ret = ULIS3_FORMAT_RGBA8;   break;
        case TSF_RGBE8:     ret = ULIS3_FORMAT_RGB8;    break;
        case TSF_MAX:       ret = 0;                    break;
        default:            ret = 0;                    break;
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
    delete mBlock;
    mBlock = nullptr;
}

FOdysseyBlock::FOdysseyBlock( int                           iWidth
                            , int                           iHeight
                            , ETextureSourceFormat          iFormat
                            , ::ul3::fpInvalidateFunction   iInvFunc
                            , void*                         iInvInfo
                            , bool                          iInitializeData )
    : mUE4TextureSourceFormat( iFormat )
    , mULISFormat( ULISFormatForUE4TextureSourceFormat( mUE4TextureSourceFormat ) )
    , mBlock( nullptr )
    , mArray()
{
    // Retrieve spec info from ULIS format hash.
    ::ul3::FFormatInfo fmt( mULISFormat );

    // Allocate and fill array ( primary data rep )
    if( iInitializeData )
        mArray.SetNumZeroed( iWidth * iHeight * fmt.BPP );
    else
        mArray.SetNumUninitialized( iWidth * iHeight * fmt.BPP );

    // Allocate block from external array data
    mBlock = new ::ul3::FBlock( mArray.GetData(), iWidth, iHeight, mULISFormat, nullptr, ::ul3::FOnInvalid(  iInvFunc, iInvInfo ), ::ul3::FOnCleanup( &::ul3::OnCleanup_DoNothing ) );
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

::ul3::FBlock*
FOdysseyBlock::GetBlock()
{
    return mBlock;
}

const ::ul3::FBlock*
FOdysseyBlock::GetBlock() const
{
    return mBlock;
}

int
FOdysseyBlock::Width() const
{
    return mBlock->Width();
}

int
FOdysseyBlock::Height() const
{
    return mBlock->Height();
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
FOdysseyBlock::ResyncData()
{
    checkf( mArray.Num() == mBlock->BytesTotal(), TEXT( "Error, resync sizes don't match !" ) );
    mBlock->ResyncNonOwnedData( mArray.GetData() );
}

