// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
#include "OdysseySurfaceEditable.h"
#include "OdysseyBlock.h"
#include "ULISLoaderModule.h"
#include <ULIS3>

/////////////////////////////////////////////////////
// Utlity
void
CopyUTextureDataIntoBlock(FOdysseyBlock* iBlock,UTexture2D* iTexture)
{
    checkf(iBlock->Width() == iTexture->GetSizeX() &&
           iBlock->Height() == iTexture->GetSizeY()
           ,TEXT("Sizes do not match"));

    iTexture->Source.GetMipData(iBlock->GetArray(),0);
    iBlock->ResyncData();
}

void
CopyBlockDataIntoUTexture(const FOdysseyBlock* iBlock,UTexture2D* iTexture)
{
    checkf(iBlock->Width() == iTexture->GetSizeX() &&
           iBlock->Height() == iTexture->GetSizeY()
           ,TEXT("Sizes do not match"));

    iTexture->Source.Init(iBlock->Width(),iBlock->Height(),1,1,iTexture->Source.GetFormat(),iBlock->GetBlock()->DataPtr());
}

void
InitTextureWithBlockData(const FOdysseyBlock* iBlock, UTexture2D* iTexture, ETextureSourceFormat iFormat)
{
    iTexture->Source.Init(iBlock->Width(),iBlock->Height(),1,1,iFormat,iBlock->GetBlock()->DataPtr());
}

FOdysseyBlock*
NewOdysseyBlockFromUTextureData(UTexture2D* iTexture)
{
    FOdysseyBlock* ret = new FOdysseyBlock(iTexture->GetSizeX(),iTexture->GetSizeY(), ULISFormatForUE4TextureSourceFormat(iTexture->Source.GetFormat()));
    CopyUTextureDataIntoBlock(ret,iTexture);

    return ret;
}

void
InvalidateSurfaceFromData(const FOdysseyBlock* iData,FOdysseySurfaceEditable* iSurface)
{
    InvalidateTextureFromData(iData,iSurface->Texture());
}

void
InvalidateSurfaceFromData(const FOdysseyBlock* iData,FOdysseySurfaceEditable* iSurface,int x1,int y1,int x2,int y2)
{
    InvalidateTextureFromData(iData,iSurface->Texture(),x1,y1,x2,y2);
}

void
InvalidateTextureFromData(const FOdysseyBlock* iData,UTexture2D* iTexture)
{
    InvalidateTextureFromData(iData,iTexture,0,0,iData->Width(),iData->Height());
}

void
InvalidateTextureFromData(const FOdysseyBlock* iData,UTexture2D* iTexture,const ::ul3::FRect& iRect)
{
    checkf(iData,TEXT("Error"));
    checkf(iTexture,TEXT("Error"));

    InvalidateTextureFromData(iData->GetBlock(), iTexture, iRect);
}

void
InvalidateTextureFromData(const FOdysseyBlock* iData,UTexture2D* iTexture,int x1,int y1,int x2,int y2)
{
    checkf(iData,TEXT("Error"));
    checkf(iTexture,TEXT("Error"));

    ::ul3::FRect rect = ::ul3::FRect::FromMinMax(x1, y1, x2, y2);
    InvalidateTextureFromData(iData->GetBlock(), iTexture, rect);
}

void
InvalidateTextureFromData(const ::ul3::FBlock* iData,UTexture2D* iTexture,const ::ul3::FRect& iRect)
{
    checkf(iData,TEXT("Error"));
    checkf(iTexture,TEXT("Error"));

    checkf(iData->Width() == iTexture->GetSizeX() &&
           iData->Height() == iTexture->GetSizeY()
           ,TEXT("Sizes do not match"));

    int x = iRect.x;
    int y = iRect.y;
    int w = iRect.w;
    int h = iRect.h;
    checkf(x >= 0 &&
           y >= 0 &&
           w > 0  &&
           h > 0
           ,TEXT("Error"));

    // Considering only one region is an assumption that works but you have to be more carefull with several regions.
    ::ul3::tFormat pixelFormat = ULISFormatForUE4PixelFormat(iTexture->GetPixelFormat());
    if (iData->Format() == pixelFormat)
    {
		FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D(x, y, x, y, w, h);
		TFunction<void(uint8* SrcData, const FUpdateTextureRegion2D* Regions)> dataCleanupFunc = [&](uint8*, const FUpdateTextureRegion2D* Regions) {
			delete Regions;
		};
		uint32 bpp = iData->BytesPerPixel();
		uint32 pitch = iData->BytesPerScanLine();
        iTexture->UpdateTextureRegions(0,1,region,pitch,bpp,const_cast<uint8*>(iData->DataPtr()),dataCleanupFunc);
    }
    else
    {

        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        ::ul3::uint32 MT_bit = iData->Height() > 256 ? ULIS3_PERF_MT : 0;
        ::ul3::uint32 perfIntent = MT_bit | 0;

        ::ul3::FBlock* block = new ::ul3::FBlock(w, h, iData->Format());
		::ul3::FBlock* conv = new ::ul3::FBlock(w, h, pixelFormat);
        ::ul3::FVec2I pos(0, 0);
		FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D(x, y, 0, 0, w, h);
        ::ul3::Copy(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, iData, block, iRect, pos);
        ::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, block, conv);

		uint32 bpp = conv->BytesPerPixel();
		uint32 pitch = conv->BytesPerScanLine();

        delete block;

		TFunction<void(uint8* SrcData, const FUpdateTextureRegion2D* Regions)> dataCleanupFunc = [conv](uint8*, const FUpdateTextureRegion2D* Regions) {
			delete Regions;
			if (conv)
				delete conv;
		};
        iTexture->UpdateTextureRegions(0,1,region,pitch,bpp,const_cast<uint8*>(conv->DataPtr()),dataCleanupFunc);
        // conv destruction is handled in dataCleanupFunc
    }
    
}

void
InvalidateSurfaceFromData(const ::ul3::FBlock* iData,FOdysseySurfaceEditable* iSurface,const ::ul3::FRect& iRect)
{
    InvalidateTextureFromData(iData,iSurface->Texture(),iRect);
}

void
InvalidateSurfaceCallback(const FOdysseyBlock* iData,void* iInfo,int iX1,int iY1,int iX2,int iY2)
{
    FOdysseySurfaceEditable* surface = static_cast<FOdysseySurfaceEditable*>(iInfo);
    InvalidateSurfaceFromData(iData,surface,iX1,iY1,iX2,iY2);
}

void
InvalidateSurfaceCallback(const ::ul3::FBlock* iData,void* iInfo,const ::ul3::FRect& iRect)
{
    FOdysseySurfaceEditable* surface = static_cast<FOdysseySurfaceEditable*>(iInfo);
    InvalidateSurfaceFromData(iData,surface,iRect);
}

/////////////////////////////////////////////////////
// FOdysseySurfaceEditable
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseySurfaceEditable::~FOdysseySurfaceEditable()
{
    mTexture->RemoveFromRoot();
    if(!mIsBorrowedTexture) // If not borrowed, that means transient hence we are responsible for dealloc
    {
        checkf(mTexture,TEXT("Error: texture should be a valid pointer"));
        //texture->RemoveFromRoot(); // RM Prevent GC
        //delete texture;
        if(mTexture->IsValidLowLevel())
            mTexture->ConditionalBeginDestroy();
        mTexture = nullptr;
    }

    if(!mIsBorrowedBlock)
    {
        if(mBlock)
        {
            delete mBlock;
            mBlock = nullptr;
        }
    }
}

FOdysseySurfaceEditable::FOdysseySurfaceEditable(int iWidth,int iHeight, ::ul3::tFormat iFormat)
    : mIsBorrowedTexture(false)
    ,mIsBorrowedBlock(false)
{
    mTexture = UTexture2D::CreateTransient(iWidth, iHeight, UE4PixelFormatForULISFormat(iFormat));
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
    mBlock = new FOdysseyBlock(iWidth,iHeight, iFormat, &InvalidateSurfaceCallback, static_cast<void*>(this), true);
    // load texture data from block
    CopyBlockDataIntoUTexture(mBlock,mTexture);
}

FOdysseySurfaceEditable::FOdysseySurfaceEditable(UTexture2D* iTexture, FOdysseyBlock* iBlock)
    : mIsBorrowedTexture(true)
    , mIsBorrowedBlock(true)
{
    mTexture = iTexture;
    mTexture->AddToRoot();

    mBlock = iBlock;
}

FOdysseySurfaceEditable::FOdysseySurfaceEditable(UTexture2D* iTexture)
    : mIsBorrowedTexture(true)
    ,mIsBorrowedBlock(false)
{
    checkf(iTexture,TEXT("Cannot Initialize with Null borrowed texture"));
    mTexture = iTexture;
    mTexture->AddToRoot();

    // Warning: the block is allocated, then the texture data is copied into it.
    mBlock = new FOdysseyBlock(mTexture->GetSizeX(),mTexture->GetSizeY(), ULISFormatForUE4TextureSourceFormat(iTexture->Source.GetFormat()),&InvalidateSurfaceCallback,static_cast<void*>(this));
    // load block data from texture
    CopyUTextureDataIntoBlock(mBlock,mTexture);
}

FOdysseySurfaceEditable::FOdysseySurfaceEditable(FOdysseyBlock* iBlock)
    : mIsBorrowedTexture(false)
    ,mIsBorrowedBlock(true)
{
    checkf(iBlock,TEXT("Cannot Initialize with Null borrowed block"));
    mBlock = iBlock;

    mTexture = UTexture2D::CreateTransient(mBlock->Width(),mBlock->Height(), UE4PixelFormatForULISFormat(mBlock->Format()));
    #if WITH_EDITORONLY_DATA
    mTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    #endif
    mTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    mTexture->SRGB = 1;
    mTexture->Filter = TextureFilter::TF_Nearest;
    mTexture->UpdateResource();
    mTexture->AddToRoot();

    mBlock->GetBlock()->SetOnInvalid(::ul3::FOnInvalid(&InvalidateSurfaceCallback,static_cast<void*>(this)));

    // load texture data from block
    CopyBlockDataIntoUTexture(mBlock,mTexture);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

FOdysseyBlock*
FOdysseySurfaceEditable::Block()
{
    return mBlock;
}

const FOdysseyBlock*
FOdysseySurfaceEditable::Block() const
{
    return mBlock;
}

UTexture2D*
FOdysseySurfaceEditable::Texture()
{
    return mTexture;
}

const UTexture2D*
FOdysseySurfaceEditable::Texture() const
{
    return mTexture;
}

bool
FOdysseySurfaceEditable::IsBorrowedTexture() const
{
    return mIsBorrowedTexture;
}

void
FOdysseySurfaceEditable::CommitBlockChangesIntoTextureBulk()
{
    CopyBlockDataIntoUTexture(mBlock,mTexture);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- Public Tampon Methods
int
FOdysseySurfaceEditable::Width()
{
    return mBlock->Width();
}

int
FOdysseySurfaceEditable::Height()
{
    return mBlock->Height();
}

void
FOdysseySurfaceEditable::Invalidate()
{
    mBlock->GetBlock()->Invalidate();
}

void
FOdysseySurfaceEditable::Invalidate(int iX1,int iY1,int iX2,int iY2)
{
    mBlock->GetBlock()->Invalidate(::ul3::FRect::FromMinMax(iX1,iY1,iX2,iY2));
}

void
FOdysseySurfaceEditable::Invalidate(const ::ul3::FRect& iRect)
{
    mBlock->GetBlock()->Invalidate(iRect);
}
