// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyImageLayer.h"
#include "OdysseyBlock.h"
#include "ULISLoaderModule.h"

#define LOCTEXT_NAMESPACE "OdysseyImageLayer"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyImageLayer::~FOdysseyImageLayer()
{
    delete mBlock;
}

FOdysseyImageLayer::FOdysseyImageLayer( const FOdysseyImageLayer& iLayer)
    : IOdysseyLayer(iLayer)
    , IOdysseyLayerImageBlendingCapability()
    , mBlock( nullptr )
    , mIsAlphaLocked( iLayer.mIsAlphaLocked )
{
    mBlock = new FOdysseyBlock( iLayer.mBlock->Width(), iLayer.mBlock->Height(), iLayer.mBlock->Format());
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::FVec2I pos(0, 0);
    ::ul3::Copy( hULIS.ThreadPool()
                , ULIS3_BLOCKING
                , perfIntent
                , hULIS.HostDeviceInfo()
                , ULIS3_NOCB
                , iLayer.mBlock->GetBlock()
                , mBlock->GetBlock()
                , mBlock->GetBlock()->Rect()
                , pos );
}

FOdysseyImageLayer::FOdysseyImageLayer( const FName& iName, FVector2D iSize, ::ul3::tFormat iFormat)
    : IOdysseyLayer( iName, IOdysseyLayer::eType::kImage )
    , IOdysseyLayerImageBlendingCapability()
    , mBlock( nullptr )
    , mIsAlphaLocked( false )
{
    check( iSize.X >= 0 && iSize.Y >= 0 );

    mBlock = new FOdysseyBlock( iSize.X, iSize.Y, iFormat);
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::Clear( hULIS.ThreadPool()
                , ULIS3_BLOCKING
                , perfIntent
                , hULIS.HostDeviceInfo()
                , ULIS3_NOCB
                , mBlock->GetBlock()
                , mBlock->GetBlock()->Rect() );
}

FOdysseyImageLayer::FOdysseyImageLayer( const FName& iName, FOdysseyBlock* iBlock )
    : IOdysseyLayer( iName, IOdysseyLayer::eType::kImage )
    , IOdysseyLayerImageBlendingCapability()
    , mBlock( iBlock )
    , mIsAlphaLocked( false )
{
}

FOdysseyImageLayer*
FOdysseyImageLayer::Clone() const
{
    return new FOdysseyImageLayer(*this);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

FOdysseyBlock*
FOdysseyImageLayer::GetBlock() const
{
    return mBlock;
}

void
FOdysseyImageLayer::SetBlock(FOdysseyBlock* iBlock, bool iSendEvents, bool iDestroyPreviousBlock)
{
    FOdysseyBlock* block = mBlock;
    mBlock = iBlock;   

    if (iSendEvents)
    {
        mImageResultChangedDelegate.Broadcast();
    }

    if (iDestroyPreviousBlock)
    {
        delete block;
    }
}

bool
FOdysseyImageLayer::IsAlphaLocked() const
{
    return mIsAlphaLocked;
}

void
FOdysseyImageLayer::SetIsAlphaLocked( bool iIsAlphaLocked )
{
    bool oldValue = mIsAlphaLocked;
	mIsAlphaLocked = iIsAlphaLocked;
    mIsAlphaLockedChangedDelegate.Broadcast(oldValue);
}

void
FOdysseyImageLayer::SetIsVisible(bool iIsVisible)
{
    IOdysseyLayer::SetIsVisible(iIsVisible);
    mImageResultChangedDelegate.Broadcast();
}

bool
FOdysseyImageLayer::ImplementsCapability(FGuid iGuid) const
{
    return IOdysseyLayerImageBlendingCapability::GetGuids().Contains(iGuid);
}

void*
FOdysseyImageLayer::GetCapabilityPtrFromGuid(FGuid iGuid)
{
    if (IOdysseyLayerImageBlendingCapability::GetGuids().Contains(iGuid))
        return IOdysseyLayerImageBlendingCapability::GetCapabilityPtrFromGuid(this, iGuid);
        
    return nullptr;
}

void
FOdysseyImageLayer::Blend(::ul3::FBlock* ioBlock, const ::ul3::FRect& iRect, ::ul3::FVec2F iPos)
{
    if (!IsVisible())
        return;

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = /*ULIS3_PERF_MT |*/ ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;

    ::ul3::FVec2F pos( iPos.x, iPos.y );
    ::ul3::FBlock* convBlock = nullptr;
    if (ioBlock->Format() != mBlock->Format())
    {
        convBlock = new ::ul3::FBlock(iRect.w, iRect.h, mBlock->Format());
        ::ul3::Conv( hULIS.ThreadPool()
            , ULIS3_BLOCKING
            , perfIntent
            , hULIS.HostDeviceInfo()
            , ULIS3_NOCB
            , ioBlock
            , convBlock );
        pos.x = 0;
        pos.y = 0;
    }

    ::ul3::Blend( hULIS.ThreadPool()
        , ULIS3_BLOCKING
        , perfIntent
        , hULIS.HostDeviceInfo()
        , ULIS3_NOCB
        , mBlock->GetBlock()
        , convBlock ? convBlock : ioBlock
        , iRect
        , pos
        , ULIS3_NOAA
        , GetBlendingMode()
        , ::ul3::AM_NORMAL
        , GetOpacity() );
    
    if (convBlock)
    {
        ::ul3::FBlock* convBlock2 = new ::ul3::FBlock(convBlock->Width(), convBlock->Height(), ioBlock->Format());
        ::ul3::Conv( hULIS.ThreadPool()
            , ULIS3_BLOCKING
            , perfIntent
            , hULIS.HostDeviceInfo()
            , ULIS3_NOCB
            , convBlock
            , convBlock2 );

        ::ul3::FRect rect = ::ul3::FRect::FromXYWH(0, 0, iRect.w, iRect.h);
        ::ul3::Copy( hULIS.ThreadPool()
            , ULIS3_BLOCKING
            , perfIntent
            , hULIS.HostDeviceInfo()
            , ULIS3_NOCB
            , convBlock2
            , ioBlock
            , rect
            , iPos );
        
        delete convBlock;
        delete convBlock2;
    }
}

void
FOdysseyImageLayer::RenderImage(::ul3::FBlock* ioBlock, const ::ul3::FRect& iRect, ::ul3::FVec2F iPos)
{
    if (!IsVisible())
        return;
        
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = /*ULIS3_PERF_MT |*/ ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;

    ::ul3::FVec2F pos( iPos.x, iPos.y );
    ::ul3::FBlock* convBlock = nullptr;
    if (ioBlock->Format() != mBlock->Format())
    {
        convBlock = new ::ul3::FBlock(iRect.w, iRect.h, mBlock->Format());
        ::ul3::Conv( hULIS.ThreadPool()
            , ULIS3_BLOCKING
            , perfIntent
            , hULIS.HostDeviceInfo()
            , ULIS3_NOCB
            , ioBlock
            , convBlock );
        pos.x = 0;
        pos.y = 0;
    }

    ::ul3::Copy( hULIS.ThreadPool()
        , ULIS3_BLOCKING
        , perfIntent
        , hULIS.HostDeviceInfo()
        , ULIS3_NOCB
        , mBlock->GetBlock()
        , convBlock ? convBlock : ioBlock
        , iRect
        , pos );
    
    if (convBlock)
    {
        ::ul3::FBlock* convBlock2 = new ::ul3::FBlock(convBlock->Width(), convBlock->Height(), ioBlock->Format());
        ::ul3::Conv( hULIS.ThreadPool()
            , ULIS3_BLOCKING
            , perfIntent
            , hULIS.HostDeviceInfo()
            , ULIS3_NOCB
            , convBlock
            , convBlock2 );

        ::ul3::FRect rect = ::ul3::FRect::FromXYWH(0, 0, iRect.w, iRect.h);
        ::ul3::Copy( hULIS.ThreadPool()
            , ULIS3_BLOCKING
            , perfIntent
            , hULIS.HostDeviceInfo()
            , ULIS3_NOCB
            , convBlock2
            , ioBlock
            , rect
            , iPos );
        
        delete convBlock;
        delete convBlock2;
    }
}

// Custom serialization version for FOdysseyImageLayer
struct FOdysseyImageLayerObjectVersion
{
	enum Type
	{
		// Before any version changes were made
		SavePixelFormat,
        SaveBlendable,
		
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};

	// The GUID for this custom version number
	const static FGuid GUID;

private:
	FOdysseyImageLayerObjectVersion() {}
};

const FGuid FOdysseyImageLayerObjectVersion::GUID(0xE2CA928C, 0x4FCB03A0, 0xE22252AA, 0xA88FC0B5);
FCustomVersionRegistration FOdysseyImageLayerObjectVersionRegistration(FOdysseyImageLayerObjectVersion::GUID, FOdysseyImageLayerObjectVersion::LatestVersion, TEXT("FOdysseyImageLayerObjectVersion::SavePixelFormat"));

void
FOdysseyImageLayer::Serialize(FArchive &Ar)
{
    IOdysseyLayer::Serialize(Ar);

    //Set the Object Version
	Ar.UsingCustomVersion(FOdysseyImageLayerObjectVersion::GUID);

    //Manage old saving order
    if (Ar.CustomVer(FOdysseyImageLayerObjectVersion::GUID) < FOdysseyImageLayerObjectVersion::SaveBlendable)
    {   
        Ar << mIsAlphaLocked;
        SerializeImageBlendingCapability(Ar);
    }
    else
    {
        SerializeImageBlendingCapability(Ar);
        Ar << mIsAlphaLocked;
    }

    if( Ar.IsSaving() )
    {
        int width = mBlock->Width();
        int height = mBlock->Height();
		::ul3::tFormat format = mBlock->Format();

        Ar << width;
        Ar << height;
		if( Ar.CustomVer(FOdysseyImageLayerObjectVersion::GUID) >= FOdysseyImageLayerObjectVersion::SavePixelFormat )
		{
			Ar << format;
		}

        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
        ::ul3::FBlock* blockLayerData = ::ul3::XCopy( hULIS.ThreadPool()
                                                    , ULIS3_BLOCKING
                                                    , perfIntent
                                                    , hULIS.HostDeviceInfo()
                                                    , ULIS3_NOCB
                                                    , mBlock->GetBlock()
                                                    , ::ul3::FRect( 0, 0, mBlock->Width(), mBlock->Height() ) );

        TArray<uint8> layerData = TArray<uint8>();
        layerData.AddUninitialized(blockLayerData->BytesTotal());
        FMemory::Memcpy(layerData.GetData(),blockLayerData->DataPtr(),blockLayerData->BytesTotal());
        delete blockLayerData;

        Ar << layerData;
    }
    else if(Ar.IsLoading())
    {
        int width;
        int height;

        Ar << width;
        Ar << height;

		::ul3::tFormat format = ULIS3_FORMAT_BGRA8;
		if (Ar.CustomVer(FOdysseyImageLayerObjectVersion::GUID) >= FOdysseyImageLayerObjectVersion::SavePixelFormat)
		{
			Ar << format;
		}

        check(!mBlock);
        mBlock = new FOdysseyBlock(width,height,format);

        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
        ::ul3::Clear( hULIS.ThreadPool()
                    , ULIS3_BLOCKING
                    , perfIntent
                    , hULIS.HostDeviceInfo()
                    , ULIS3_NOCB
                    , mBlock->GetBlock()
                    , mBlock->GetBlock()->Rect() );

        TArray< uint8 > layerData = TArray< uint8 >();
        layerData.AddUninitialized(mBlock->GetBlock()->BytesTotal());

        Ar << layerData;

        for(int j = 0; j < layerData.Num(); j++) {
            *(mBlock->GetBlock()->DataPtr() + j) = layerData[j];
        }
    }
}

//---

FOdysseyImageLayer::FOdysseyLayerIsAlphaLockedChanged&
FOdysseyImageLayer::IsAlphaLockedChangedDelegate()
{
    return mIsAlphaLockedChangedDelegate;
}

#undef LOCTEXT_NAMESPACE
