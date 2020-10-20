// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyBlock.h"
#include <ULIS3>

/////////////////////////////////////////////////////
// Utlity

::ul3::tFormat ULISFormatForUE4TextureSourceFormat( ETextureSourceFormat iFormat )
{
    ::ul3::tFormat ret = 0;
    switch( iFormat ) {
        case TSF_Invalid:   ret = 0;                    break;
        case TSF_G8:        ret = ULIS3_FORMAT_G8;      break;
        case TSF_BGRA8:     ret = ULIS3_FORMAT_BGRA8;   break;
        case TSF_BGRE8:     ret = ULIS3_FORMAT_BGRA8;   break;
        case TSF_RGBA16:    ret = ULIS3_FORMAT_RGBA16;  break;
        case TSF_RGBA16F:   ret = ULIS3_FORMAT_RGBA16;  break;
        case TSF_RGBA8:     ret = ULIS3_FORMAT_RGBA8;   break;
        case TSF_RGBE8:     ret = ULIS3_FORMAT_RGBA8;   break;
        case TSF_MAX:       ret = 0;                    break;
        default:            ret = 0;                    break;
    }
    checkf( ret, TEXT( "Error, bad format !" ) ); // Crash
    return ret;
}

::ul3::tFormat ULISFormatForUE4PixelFormat( EPixelFormat iFormat )
{
	//TODO: if there is no correspondance between PF_* and ULIS3_* formats
	//		Then we should use a decompress/compress system to translate it to a viable ULIS3_* format

    ::ul3::tFormat ret = 0;
    switch( iFormat ) {
        case PF_Unknown:			ret = ULIS3_FORMAT_BGRA8;   break;
		case PF_A8:					ret = ULIS3_FORMAT_G8;      break;
        case PF_G8:					ret = ULIS3_FORMAT_G8;      break;
		case PF_R8_UINT:			ret = ULIS3_FORMAT_G8;      break;
		case PF_L8:					ret = ULIS3_FORMAT_G8;      break;
        case PF_G16:				ret = ULIS3_FORMAT_G16;		break;
		case PF_A8R8G8B8:			ret = ULIS3_FORMAT_ARGB8;   break;
        case PF_B8G8R8A8:			ret = ULIS3_FORMAT_BGRA8;   break;
        case PF_A32B32G32R32F:		ret = ULIS3_FORMAT_ABGRF;	break;
		case PF_R32G32B32A32_UINT:	ret = ULIS3_FORMAT_RGBA32;	break;
        case PF_R16G16B16A16_UINT:	ret = ULIS3_FORMAT_RGBA16;	break;
		case PF_R16_UINT:			ret = ULIS3_FORMAT_G16;		break;
        case PF_R32_UINT:			ret = ULIS3_FORMAT_G32;		break;
        case PF_R8G8B8A8_UINT:		ret = ULIS3_FORMAT_RGBA8;   break;
		case PF_R8G8B8A8_SNORM:		ret = ULIS3_FORMAT_RGBA8;   break;
		case PF_R16G16B16A16_UNORM:		ret = ULIS3_FORMAT_RGBA16;   break;
		case PF_R16G16B16A16_SNORM:		ret = ULIS3_FORMAT_RGBA16;   break;
        default:					ret = 0;                    break;
    }
    checkf( ret, TEXT( "Error, bad format !" ) ); // Crash
    return ret;
}

EPixelFormat UE4PixelFormatForULISFormat( ::ul3::tFormat iFormat )
{
    EPixelFormat ret = PF_Unknown;
    switch( iFormat ) {
        case ULIS3_FORMAT_G8:		ret = PF_G8;                break;
        case ULIS3_FORMAT_G16:		ret = PF_G16;		        break;
		case ULIS3_FORMAT_ARGB8:	ret = PF_A8R8G8B8;          break;
        case ULIS3_FORMAT_BGRA8:	ret = PF_B8G8R8A8;          break;
        case ULIS3_FORMAT_ABGRF:	ret = PF_A32B32G32R32F;	    break;
		case ULIS3_FORMAT_RGBA32:	ret = PF_R32G32B32A32_UINT;	break;
        case ULIS3_FORMAT_RGBA16:	ret = PF_R16G16B16A16_UINT;	break;
        case ULIS3_FORMAT_RGBA8:	ret = PF_R8G8B8A8_UINT;     break;
        default:					ret = PF_Unknown;                    break;
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
                            , ::ul3::tFormat                iFormat
                            , ::ul3::fpInvalidateFunction   iInvFunc
                            , void*                         iInvInfo
                            , bool                          iInitializeData )
    // : mUE4TextureSourceFormat( iTextureSourceFormat )
    // , mUE4PixelFormat( iPixelFormat )
    // , mULISFormat( ULISFormatForUE4PixelFormat(mUE4PixelFormat) )
    : mBlock( nullptr )
    , mArray()
{
    // Retrieve spec info from ULIS format hash.
    ::ul3::FFormatInfo fmt( iFormat );

    // Allocate and fill array ( primary data rep )
    if( iInitializeData )
        mArray.SetNumZeroed( iWidth * iHeight * fmt.BPP );
    else
        mArray.SetNumUninitialized( iWidth * iHeight * fmt.BPP );

    // Allocate block from external array data
    mBlock = new ::ul3::FBlock( mArray.GetData(), iWidth, iHeight, iFormat, nullptr, ::ul3::FOnInvalid(  iInvFunc, iInvInfo ), ::ul3::FOnCleanup( &::ul3::OnCleanup_DoNothing ) );
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

/* ETextureSourceFormat
FOdysseyBlock::GetUE4TextureSourceFormat() const
{
    return mUE4TextureSourceFormat;
}

EPixelFormat
FOdysseyBlock::GetUE4PixelFormat() const
{
    return mUE4PixelFormat;
} */

::ul3::tFormat
FOdysseyBlock::Format() const
{
    return mBlock->Format();
}

void
FOdysseyBlock::ResyncData()
{
    checkf( mArray.Num() == mBlock->BytesTotal(), TEXT( "Error, resync sizes don't match !" ) );
    mBlock->ResyncNonOwnedData( mArray.GetData() );
}

