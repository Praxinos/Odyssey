// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyFolderLayer.h"

#include "ULISLoaderModule.h"
#include <ULIS>

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
FOdysseyFolderLayer::AddChild(TSharedPtr<IOdysseyLayer> iLayer, int iIndex)
{
    IOdysseyLayer::AddChild(iLayer, iIndex);
    bool isBlendable = iLayer->ImplementsCapability(IOdysseyLayerImageBlendingCapability::GetGuid());
    if( isBlendable )
    {
        IOdysseyLayerImageBlendingCapability* layerBlendable = iLayer->GetCapability<IOdysseyLayerImageBlendingCapability>();
        layerBlendable->ImageResultChangedDelegate().AddRaw( this, &FOdysseyFolderLayer::OnChildImageResultChanged, iLayer );
        if( iLayer->IsVisible() )
        {
            mImageResultChangedDelegate.Broadcast( nullptr, 0 );
        }
    }
}

void
FOdysseyFolderLayer::RemoveChild(int iIndex)
{
    TSharedPtr<IOdysseyLayer> layer = GetChild(iIndex);
    bool isBlendable = layer->ImplementsCapability(IOdysseyLayerImageBlendingCapability::GetGuid());
    if (isBlendable)
    {
        IOdysseyLayerImageBlendingCapability* layerBlendable = layer->GetCapability<IOdysseyLayerImageBlendingCapability>();
		layerBlendable->ImageResultChangedDelegate().RemoveAll(this);
    }

    IOdysseyLayer::RemoveChild(iIndex);

    if (isBlendable && layer->IsVisible())
    {
        mImageResultChangedDelegate.Broadcast( nullptr, 0 );
    }
}

void
FOdysseyFolderLayer::OnChildImageResultChanged( const ::ULIS::FRectI* iRects, const uint32 iNumRects, TSharedPtr< IOdysseyLayer > iLayer )
{
    bool isBlendable = iLayer->ImplementsCapability( IOdysseyLayerImageBlendingCapability::GetGuid() );
    if( isBlendable )
        mImageResultChangedDelegate.Broadcast( iRects, iNumRects );
}

TArray<::ULIS::FEvent>
FOdysseyFolderLayer::Blend( ::ULIS::FBlock** ioBlocks, const ::ULIS::FRectI* iRects, const ::ULIS::FVec2I* iPositions, const uint32 iNum, const ::ULIS::FEvent* iEvents )
{
    TArray< TSharedPtr< IOdysseyLayer > > children = GetChildren();
    if( !IsVisible() || iNum == 0 || children.Num() == 0)
        return TArray<::ULIS::FEvent>(iEvents, iNum);

    TArray< ::ULIS::FBlock* > folderBlocks;
    folderBlocks.Reserve( iNum );
    for( uint32 i = 0; i < iNum; ++i )
        folderBlocks.Emplace( new ::ULIS::FBlock( iRects[i].w, iRects[i].h, ioBlocks[i]->Format() ) );

    TArray< ::ULIS::FVec2I > pos;
    pos.SetNum( iNum );
    //RenderImage( folderBlocks.GetData(), iRects, pos.GetData(), iNum );
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( folderBlocks[0]->Format() );
    TArray<::ULIS::FEvent> eventClear;
    eventClear.SetNum(iNum);
    for( uint32 i = 0; i < iNum; ++i )
    {
        ::ULIS::FRectI rect = ::ULIS::FRectI::FromPositionAndSize( pos[i], iRects[i].Size() );
        ctx.Clear( *folderBlocks[i], rect, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 1, &iEvents[i], &eventClear[i] );
        ctx.Flush();
    }

    TArray<::ULIS::FEvent> eventRender = eventClear;
    for( int i = children.Num() - 1; i >= 0; --i ) {
        TSharedPtr< IOdysseyLayer > child = children[i];
        if( !child->ImplementsCapability( IOdysseyLayerImageBlendingCapability::GetGuid() ) )
            continue;

        IOdysseyLayerImageBlendingCapability* layerBlendable = child->GetCapability< IOdysseyLayerImageBlendingCapability >();
        if( !layerBlendable )
            continue;

        eventRender = layerBlendable->Blend( folderBlocks.GetData(), iRects, pos.GetData(), iNum, eventRender.GetData() );
    }

    //::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( ioBlocks[0]->Format() );
    TArray<::ULIS::FEvent> eventBlend;
    eventBlend.SetNum(iNum);
    for( uint32 i = 0; i < iNum; ++i ) {
        ::ULIS::FRectI rect = ::ULIS::FRectI::FromPositionAndSize( ::ULIS::FVec2I( 0 ), iRects[i].Size() );
        eventBlend[i] = ::ULIS::FEvent(
            ::ULIS::FOnEventComplete(
                [ folderBlocks, i ]( const ::ULIS::FRectI& ) {
                    delete  folderBlocks[i];
                }
            )
        );
        ctx.Blend( *( folderBlocks[i] ), *ioBlocks[i], rect, iPositions[i], GetBlendingMode(), ::ULIS::Alpha_Normal, GetOpacity(), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 1, &eventRender[i], &eventBlend[i] );
        ctx.Flush();
    }

    return eventBlend;
}

TArray<::ULIS::FEvent>
FOdysseyFolderLayer::RenderImage( ::ULIS::FBlock** ioBlocks, const ::ULIS::FRectI* iRects, const ::ULIS::FVec2I* iPositions, const uint32 iNum )
{
    TArray<::ULIS::FEvent> eventRender;

    if( !IsVisible() || iNum == 0 )
    {
        for (uint32 i = 0; i < iNum; i++)
            eventRender.Add(::ULIS::FEvent::NoOP());
        return eventRender;
    }

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( ioBlocks[0]->Format() );
    TArray<::ULIS::FEvent> eventClear;
    eventClear.SetNum(iNum);
    for( uint32 i = 0; i < iNum; ++i )
    {
        ::ULIS::FRectI rect = ::ULIS::FRectI::FromPositionAndSize( iPositions[i], iRects[i].Size() );
        ctx.Clear( *ioBlocks[i], rect, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventClear[i] );
        ctx.Flush();
    }

    TArray< TSharedPtr< IOdysseyLayer > > children = GetChildren();
    eventRender = eventClear;
    for( int i = children.Num() - 1; i >= 0; --i ) {
        TSharedPtr< IOdysseyLayer > child = children[i];
        if( !child->ImplementsCapability( IOdysseyLayerImageBlendingCapability::GetGuid() ) )
            continue;

        IOdysseyLayerImageBlendingCapability* layerBlendable = child->GetCapability< IOdysseyLayerImageBlendingCapability >();
        if( !layerBlendable )
            continue;

        eventRender = layerBlendable->Blend( ioBlocks, iRects, iPositions, iNum, eventRender.GetData() );
    }

    return eventRender;
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
//FCustomVersionRegistration FOdysseyFolderLayerObjectVersionRegistration(FOdysseyFolderLayerObjectVersion::GUID, FOdysseyFolderLayerObjectVersion::LatestVersion, TEXT("FOdysseyFolderLayerObjectVersion::SavePixelFormat"));

void
FOdysseyFolderLayer::Serialize(FArchive &Ar)
{
    IOdysseyLayer::Serialize(Ar);

	//Set the Object Version
	//Ar.UsingCustomVersion(FOdysseyFolderLayerObjectVersion::GUID);

    //Manage old saving order
    /*if (Ar.CustomVer(FOdysseyFolderLayerObjectVersion::GUID) < FOdysseyFolderLayerObjectVersion::SaveBlendable)
    {
        int bm = static_cast< int >( mBlendingMode );
        Ar << bm;
        Ar << mIsOpen;
        Ar << mOpacity;
        if (Ar.IsLoading())
        {
            mBlendingMode = (::ULIS::eBlendMode)bm;
        }
        return;
    }*/

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
    mImageResultChangedDelegate.Broadcast( nullptr, 0 );
}

#undef LOCTEXT_NAMESPACE
