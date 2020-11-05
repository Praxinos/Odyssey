// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyFolderLayer.h"

#include "ULISLoaderModule.h"
#include <ULIS3>

#define LOCTEXT_NAMESPACE "OdysseyFolderLayer"

//---

FOdysseyFolderLayer::~FOdysseyFolderLayer()
{
}

FOdysseyFolderLayer::FOdysseyFolderLayer( const FOdysseyFolderLayer& iLayer)
    : IOdysseyLayer(iLayer)
    , IOdysseyLayerImageBlendingCapability()
    , mIsOpen( iLayer.mIsOpen )
{
}

FOdysseyFolderLayer::FOdysseyFolderLayer( const FName& iName )
    : IOdysseyLayer( iName, IOdysseyLayer::eType::kFolder )
    , IOdysseyLayerImageBlendingCapability()
    , mIsOpen( true )
{
}

FOdysseyFolderLayer*
FOdysseyFolderLayer::Clone() const
{
	return new FOdysseyFolderLayer(*this);
}

//---

bool
FOdysseyFolderLayer::IsOpen() const
{
    return mIsOpen;
}

void
FOdysseyFolderLayer::SetIsOpen( bool iIsOpen )
{
    bool oldValue = mIsOpen;
    mIsOpen = iIsOpen;
    mIsOpenChangedDelegate.Broadcast(oldValue);
}

bool
FOdysseyFolderLayer::ImplementsCapability(FGuid iGuid) const
{
    return IOdysseyLayerImageBlendingCapability::GetGuids().Contains(iGuid);
}

void*
FOdysseyFolderLayer::GetCapabilityPtrFromGuid(FGuid iGuid)
{
    if (IOdysseyLayerImageBlendingCapability::GetGuids().Contains(iGuid))
        return IOdysseyLayerImageBlendingCapability::GetCapabilityPtrFromGuid(this, iGuid);
        
    return nullptr;
}

void
FOdysseyFolderLayer::AddNode(TSharedPtr<IOdysseyLayer> iLayer, int iIndex)
{
    IOdysseyLayer::AddNode(iLayer, iIndex);
    bool isBlendable = iLayer->ImplementsCapability(IOdysseyLayerImageBlendingCapability::GetGuid());
    if (isBlendable)
    {
        IOdysseyLayerImageBlendingCapability* layerBlendable = iLayer->GetCapability<IOdysseyLayerImageBlendingCapability>();
		layerBlendable->ImageResultChangedDelegate().AddRaw(this, &FOdysseyFolderLayer::OnChildImageResultChanged, iLayer);
        if (iLayer->IsVisible())
        {
            mImageResultChangedDelegate.Broadcast();
        }
    }
}

void
FOdysseyFolderLayer::DeleteNode(int iIndex)
{
    TSharedPtr<IOdysseyLayer> layer = GetNode(iIndex);
    bool isBlendable = layer->ImplementsCapability(IOdysseyLayerImageBlendingCapability::GetGuid());
    if (isBlendable)
    {
        IOdysseyLayerImageBlendingCapability* layerBlendable = layer->GetCapability<IOdysseyLayerImageBlendingCapability>();
		layerBlendable->ImageResultChangedDelegate().RemoveAll(this);
    }

    IOdysseyLayer::DeleteNode(iIndex);

    if (isBlendable && layer->IsVisible())
    {
        mImageResultChangedDelegate.Broadcast();
    }
}

void
FOdysseyFolderLayer::OnChildImageResultChanged(TSharedPtr<IOdysseyLayer> iLayer)
{
    bool isBlendable = iLayer->ImplementsCapability(IOdysseyLayerImageBlendingCapability::GetGuid());
    if (isBlendable)
    {
        mImageResultChangedDelegate.Broadcast();
    }
}

void
FOdysseyFolderLayer::Blend(::ul3::FBlock* ioBlock, const ::ul3::FRect& iRect, ::ul3::FVec2F iPos)
{
    if (!IsVisible())
        return;

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = /*ULIS3_PERF_MT |*/ ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;

    ::ul3::FBlock* folderBlock = new ::ul3::FBlock(iRect.w, iRect.h, ioBlock->Format());
    ::ul3::FVec2F pos( 0, 0 );
    RenderImage(folderBlock, iRect, pos);

    ::ul3::FRect rect = ::ul3::FRect::FromXYWH(0, 0, iRect.w, iRect.h);
    ::ul3::Blend( hULIS.ThreadPool()
        , ULIS3_BLOCKING
        , perfIntent
        , hULIS.HostDeviceInfo()
        , ULIS3_NOCB
        , folderBlock
        , ioBlock
        , rect
        , iPos
        , ULIS3_NOAA
        , GetBlendingMode()
        , ::ul3::AM_NORMAL
        , GetOpacity() );
}

void
FOdysseyFolderLayer::RenderImage(::ul3::FBlock* ioBlock, const ::ul3::FRect& iRect, ::ul3::FVec2F iPos)
{
    if (!IsVisible())
        return;

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = /*ULIS3_PERF_MT |*/ ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::FRect rect = ::ul3::FRect::FromXYWH(iPos.x, iPos.y, iRect.w, iRect.h);
    ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, ioBlock, rect );

    TArray<TSharedPtr<IOdysseyLayer>> children = GetNodes();
    for (int i = children.Num() - 1; i >= 0; i--)
    {
        TSharedPtr<IOdysseyLayer> child = children[i];
        if (!child->ImplementsCapability(IOdysseyLayerImageBlendingCapability::GetGuid()))
            continue;

        IOdysseyLayerImageBlendingCapability* layerBlendable = child->GetCapability<IOdysseyLayerImageBlendingCapability>();
        if (!layerBlendable)
            continue;

        layerBlendable->Blend(ioBlock, iRect, iPos);
    }
}

// Custom serialization version for FOdysseyImageLayer
struct FOdysseyFolderLayerObjectVersion
{
	enum Type
	{
		// Before any version changes were made
		SaveBlendable,
		
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};

	// The GUID for this custom version number
	const static FGuid GUID;

private:
	FOdysseyFolderLayerObjectVersion() {}
};

const FGuid FOdysseyFolderLayerObjectVersion::GUID(0x86E82FF2, 0x93913CF7, 0x85b41FC2, 0x4CCC10D9);
FCustomVersionRegistration FOdysseyFolderLayerObjectVersionRegistration(FOdysseyFolderLayerObjectVersion::GUID, FOdysseyFolderLayerObjectVersion::LatestVersion, TEXT("FOdysseyFolderLayerObjectVersion::SavePixelFormat"));

void
FOdysseyFolderLayer::Serialize(FArchive &Ar)
{
    IOdysseyLayer::Serialize(Ar);

	//Set the Object Version
	Ar.UsingCustomVersion(FOdysseyFolderLayerObjectVersion::GUID);

    //Manage old saving order
    if (Ar.CustomVer(FOdysseyFolderLayerObjectVersion::GUID) < FOdysseyFolderLayerObjectVersion::SaveBlendable)
    {
        int bm = static_cast< int >( mBlendingMode );
        Ar << bm;
        Ar << mIsOpen;
        Ar << mOpacity;
        if (Ar.IsLoading())
        {
            mBlendingMode = (::ul3::eBlendingMode)bm;
        }
        return;
    }

    SerializeImageBlendingCapability(Ar);
    Ar << mIsOpen;
}

FOdysseyFolderLayer::FOdysseyLayerIsOpenChanged&
FOdysseyFolderLayer::IsOpenChangedDelegate()
{
    return mIsOpenChangedDelegate;
}

void
FOdysseyFolderLayer::SetIsVisible(bool iIsVisible)
{
    IOdysseyLayer::SetIsVisible(iIsVisible);
    mImageResultChangedDelegate.Broadcast();
}

#undef LOCTEXT_NAMESPACE
