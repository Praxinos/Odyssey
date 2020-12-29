// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyBlock.h"
#include <ULIS3>

/////////////////////////////////////////////////////
// Utlity

int UE4TextureSourceFormatBytesPerPixel(ETextureSourceFormat iFormat)
{
    switch (iFormat) {

    case TSF_G8:        return 1;
    case TSF_BGRA8:     return 4;
    case TSF_BGRE8:     return 4;
    case TSF_RGBA16:    return 8;
    case TSF_RGBA16F:   return 8;
    case TSF_RGBA8:     return 4;
    case TSF_RGBE8:     return 4;

    case TSF_Invalid:
    case TSF_MAX:
    default:                                        break;
    }
    return 0;
}

bool UE4TextureSourceFormatNeedsConversionToULISFormat( ETextureSourceFormat iFormat )
{
    switch( iFormat )
    {
        case TSF_BGRE8:
        case TSF_RGBA16F:
        case TSF_RGBE8:
            return true;
        default:
        break;
    }
    return false;
}

void
ConvertUE4TextureSourceFormatToULISFormat( const uint8* iSrc, uint8* oDst, int iWidth, int iHeight, ETextureSourceFormat iFormat)
{
    switch( iFormat )
    {
        case TSF_BGRE8:
        {
            FColor* bgre = (FColor*)(iSrc);
            ::ul3::FBlock* dst = new ::ul3::FBlock(oDst, iWidth, iHeight, ULIS3_FORMAT_RGBF);
            ::ul3::FPixelProxy proxy(oDst, ULIS3_FORMAT_RGBF);
            for (int y = 0; y < iHeight; y++)
            {
                for (int x = 0; x < iWidth; x++)
                {
                    int i = y * iWidth + x;
                    proxy.SetPtr(dst->PixelPtr(x, y));
                    FLinearColor rgba = bgre[i].FromRGBE();
                    proxy.SetRF(rgba.R);
                    proxy.SetGF(rgba.G);
                    proxy.SetBF(rgba.B);
                }
            }
            delete dst;
        }
        break;

        case TSF_RGBE8:
        {
            FColor* rgbe = (FColor*)(iSrc);
            ::ul3::FBlock* dst = new ::ul3::FBlock(oDst, iWidth, iHeight, ULIS3_FORMAT_RGBF);
            ::ul3::FPixelProxy proxy(oDst, ULIS3_FORMAT_RGBF);
            for (int y = 0; y < iHeight; y++)
            {
                for (int x = 0; x < iWidth; x++)
                {
                    int i = y * iWidth + x;
                    FColor bgre(rgbe[i].B, rgbe[i].G, rgbe[i].R, rgbe[i].A);
                    proxy.SetPtr(dst->PixelPtr(x, y));
                    FLinearColor rgba = bgre.FromRGBE();
                    proxy.SetRF(rgba.R);
                    proxy.SetGF(rgba.G);
                    proxy.SetBF(rgba.B);
                }
            }
            delete dst;
        }
        break;

        case TSF_RGBA16F:
        {
            FFloat16Color* rgba16 = (FFloat16Color*)(iSrc);
            FLinearColor* rgbaf = (FLinearColor*)(oDst);
            int size = iWidth * iHeight;
            for (int i = 0; i < size; i++)
            {
                rgbaf[i] = FLinearColor(rgba16[i]);
            }
        }
        break;
        
        default:
        break;
    }
}

void
ConvertULISFormatToUE4TextureSourceFormat( const uint8* iSrc, uint8* oDst, int iWidth, int iHeight, ETextureSourceFormat iFormat)
{
    switch( iFormat )
    {
        case TSF_BGRE8:
        {
            ::ul3::FBlock* src = new ::ul3::FBlock((uint8*)iSrc, iWidth, iHeight, ULIS3_FORMAT_RGBF);
            ::ul3::FPixelProxy proxy(iSrc, ULIS3_FORMAT_RGBF);
            FColor* bgre = (FColor*)(oDst);
            for (int y = 0; y < iHeight; y++)
            {
                for (int x = 0; x < iWidth; x++)
                {
                    int i = y * iWidth + x;
                    proxy.SetPtr(src->PixelPtr(x, y));
                    FLinearColor rgba(proxy.RF(), proxy.GF(), proxy.BF(), 1.0f);
                    bgre[i] = rgba.ToRGBE();
                }
            }
            delete src;
        }
        break;

        case TSF_RGBE8:
        {
            ::ul3::FBlock* src = new ::ul3::FBlock((uint8*)iSrc, iWidth, iHeight, ULIS3_FORMAT_RGBF);
            ::ul3::FPixelProxy proxy(iSrc, ULIS3_FORMAT_RGBF);
            FColor* rgbe = (FColor*)(oDst);
            for (int y = 0; y < iHeight; y++)
            {
                for (int x = 0; x < iWidth; x++)
                {
                    int i = y * iWidth + x;
                    proxy.SetPtr(src->PixelPtr(x, y));
                    FLinearColor rgba(proxy.RF(), proxy.GF(), proxy.BF(), 1.0f);
                    FColor bgre = rgba.ToRGBE();
                    rgbe[i] = FColor(bgre.B, bgre.G, bgre.R, bgre.A);
                }
            }
            delete src;
        }
        break;

        case TSF_RGBA16F:
        {
            FLinearColor* rgbaf = (FLinearColor*)(iSrc);
            FFloat16Color* rgba16 = (FFloat16Color*)(oDst);
            int size = iWidth * iHeight;
            for (int i = 0; i < size; i++)
            {
                rgba16[i] = FFloat16Color(rgbaf[i]);
            }
        }
        break;
        
        default:
        break;
    }
}

::ul3::tFormat ULISFormatForUE4TextureSourceFormat( ETextureSourceFormat iFormat )
{
    ::ul3::tFormat ret = 0;
    switch( iFormat ) {
        case TSF_Invalid:   ret = 0;                    break;
        case TSF_G8:        ret = ULIS3_FORMAT_G8;      break;
        case TSF_BGRA8:     ret = ULIS3_FORMAT_BGRA8;   break; 
        case TSF_BGRE8:     ret = ULIS3_FORMAT_RGBF;   break;
        case TSF_RGBA16:    ret = ULIS3_FORMAT_RGBA16;  break;
        case TSF_RGBA16F:   ret = ULIS3_FORMAT_RGBAF;   break; //TODO: Change to RGBA16G ULIS FORMAT (RGBA half floating points 16 bits, see UE4 implementation)
        case TSF_RGBA8:     ret = ULIS3_FORMAT_RGBA8;   break;
        case TSF_RGBE8:     ret = ULIS3_FORMAT_RGBF;   break;
        case TSF_MAX:       ret = 0;                    break;
        default:            ret = 0;                    break;
    }
    checkf( ret, TEXT( "Error, bad format !" ) ); // Crash
    return ret;
}

::ul3::tFormat ULISFormatForUE4PixelFormat( EPixelFormat iFormat )
{
    //FORCE use of pixel Source format
    return 0;

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
		case PF_R16G16B16A16_UNORM:	ret = ULIS3_FORMAT_RGBA16;  break;
        default:					ret = 0;                    break;
    }
    // checkf( ret, TEXT( "Error, bad format !" ) ); // Crash
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

void
FOdysseyBlock::Reallocate(  int							iWidth
						, int                           iHeight
						, ::ul3::tFormat                iFormat
						, ::ul3::fpInvalidateFunction   iInvFunc
						, void*                         iInvInfo
						, bool                          iInitializeData)
{
	// Retrieve spec info from ULIS format hash.
	::ul3::FFormatInfo fmt(iFormat);

	// Allocate and fill array ( primary data rep )
	if (iInitializeData)
		mArray.SetNumZeroed(iWidth * iHeight * fmt.BPP);
	else
		mArray.SetNumUninitialized(iWidth * iHeight * fmt.BPP);

	// Allocate block from external array data
	delete mBlock;
	mBlock = new ::ul3::FBlock(mArray.GetData(), iWidth, iHeight, iFormat, nullptr, ::ul3::FOnInvalid(iInvFunc, iInvInfo), ::ul3::FOnCleanup(&::ul3::OnCleanup_DoNothing));
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

