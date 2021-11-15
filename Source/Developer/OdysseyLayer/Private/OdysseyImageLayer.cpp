// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyImageLayer.h"
#include "OdysseyBlock.h"
#include "ULISLoaderModule.h"

#define LOCTEXT_NAMESPACE "OdysseyImageLayer"

void
OnBlockInvalidated( const ::ULIS::FBlock* iBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects, void* iInfo )
{
    FOdysseyImageLayer* layer = static_cast< FOdysseyImageLayer* >( iInfo );
    layer->mImageResultChangedDelegate.Broadcast( iRects, iNumRects );
}

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

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iLayer.mBlock->Format());
    ctx.Copy( *(iLayer.mBlock->GetBlock()), *(mBlock->GetBlock()), mBlock->GetBlock()->Rect() );
    ctx.Finish();

    static uint32 numCopy = 0;
    numCopy++;
    UE_LOG(LogTemp, Warning, TEXT("FOdysseyImageLayer::CopyCtor %d"), numCopy);

    mBlock->GetBlock()->OnInvalid( ::ULIS::FOnInvalidBlock( &OnBlockInvalidated, static_cast<void*>( this ) ) );
}

FOdysseyImageLayer::FOdysseyImageLayer( const FName& iName, FVector2D iSize, ::ULIS::eFormat iFormat)
    : IOdysseyLayer( iName, IOdysseyLayer::eType::kImage )
    , IOdysseyLayerImageBlendingCapability()
    , mBlock( nullptr )
    , mIsAlphaLocked( false )
{
    check( iSize.X >= 0 && iSize.Y >= 0 );

    mBlock = new FOdysseyBlock( iSize.X, iSize.Y, iFormat);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mBlock->Format());
    ctx.Clear( *(mBlock->GetBlock()) );
    ctx.Finish();

    static uint32 numCreate = 0;
    numCreate++;
    UE_LOG(LogTemp, Warning, TEXT("FOdysseyImageLayer::Constructor %d"), numCreate);

    mBlock->GetBlock()->OnInvalid(::ULIS::FOnInvalidBlock(&OnBlockInvalidated, static_cast<void*>(this)));
}

FOdysseyImageLayer::FOdysseyImageLayer( const FName& iName, FOdysseyBlock* iBlock )
    : IOdysseyLayer( iName, IOdysseyLayer::eType::kImage )
    , IOdysseyLayerImageBlendingCapability()
    , mBlock( iBlock )
    , mIsAlphaLocked( false )
{
    if( mBlock )
        mBlock->GetBlock()->OnInvalid( ::ULIS::FOnInvalidBlock( &OnBlockInvalidated, static_cast< void* >( this ) ) );
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
    mBlock->GetBlock()->OnInvalid( ::ULIS::FOnInvalidBlock( &OnBlockInvalidated, static_cast< void* >( this ) ) );

    if (iSendEvents)
        mImageResultChangedDelegate.Broadcast( nullptr, 0 );

    if (iDestroyPreviousBlock)
        delete block;
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
    IOdysseyLayer::SetIsVisible( iIsVisible );
    mImageResultChangedDelegate.Broadcast( nullptr, 0 );
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

TArray<::ULIS::FEvent>
FOdysseyImageLayer::Blend( ::ULIS::FBlock** ioBlocks, const ::ULIS::FRectI* iRects, const ::ULIS::FVec2I* iPositions, const uint32 iNum, const ::ULIS::FEvent* iEvents )
{
    TArray<::ULIS::FEvent> eventBlend;
    if( !IsVisible() || iNum == 0 )
        return TArray<::ULIS::FEvent>(iEvents, iNum);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( mBlock->GetBlock()->Format() );
    eventBlend.SetNum(iNum);
    if( ioBlocks[0]->Format() != mBlock->Format() ) {
        TArray< ::ULIS::FBlock* > convBlocks;
        convBlocks.Reserve( iNum );
        for( uint32 i = 0; i < iNum; ++i ) {
            convBlocks.Emplace( new ::ULIS::FBlock( iRects[i].w, iRects[i].h, mBlock->GetBlock()->Format() ) );
            ::ULIS::FEvent eventConvertForward;
            ctx.ConvertFormat( *ioBlocks[i], *( convBlocks[i] ), iRects[i], ::ULIS::FVec2I( 0 ), ::ULIS::FSchedulePolicy::MonoScanlines, 1, &iEvents[i], &eventConvertForward );
            ::ULIS::FEvent eventBlend1;
            ctx.Blend( *( mBlock->GetBlock() ), *( convBlocks[i] ), iRects[i], ::ULIS::FVec2I( 0 ), GetBlendingMode(), ::ULIS::Alpha_Normal, GetOpacity(), ::ULIS::FSchedulePolicy::MonoScanlines, 1, &eventConvertForward, &eventBlend1 );
            eventBlend[i] = ::ULIS::FEvent(
                ::ULIS::FOnEventComplete(
                    [ convBlocks, i ]( const ::ULIS::FRectI& ) {
                        delete  convBlocks[i];
                    }
                )
            );
            ctx.ConvertFormat( *( convBlocks[i] ), *ioBlocks[i], ::ULIS::FRectI::Auto, iPositions[i], ::ULIS::FSchedulePolicy::MonoScanlines, 1, &eventBlend1, &eventBlend[i] );
        }
    } else {
        for( uint32 i = 0; i < iNum; ++i ) {
            ctx.Blend( *( mBlock->GetBlock() ), *ioBlocks[i], iRects[i], iPositions[i], GetBlendingMode(), ::ULIS::Alpha_Normal, GetOpacity(), ::ULIS::FSchedulePolicy::MonoScanlines, 1, &iEvents[i], &eventBlend[i] );
        }
    }
    ctx.Flush();
    //ctx.Finish();

    return eventBlend;
}

TArray<::ULIS::FEvent>
FOdysseyImageLayer::RenderImage( ::ULIS::FBlock** ioBlocks, const ::ULIS::FRectI* iRects, const ::ULIS::FVec2I* iPositions, const uint32 iNum )
{
    TArray<::ULIS::FEvent> eventRender;

    if( !IsVisible() || iNum == 0 )
    {
        for (uint32 i = 0; i < iNum; i++)
            eventRender.Add(::ULIS::FEvent::NoOP());
        return eventRender;
    }

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( mBlock->Format() );
    eventRender.SetNum(iNum);
    for( uint32 i = 0; i < iNum; ++i ) {
        // Auto fallback to copy if appropriate.
        ctx.ConvertFormat( *( mBlock->GetBlock() ), *ioBlocks[i], iRects[i], iPositions[i], ::ULIS::FSchedulePolicy::MonoScanlines, 0, nullptr, &eventRender[i] );
        ctx.Flush();
    }
    //ctx.Finish();

    return eventRender;
}

// Custom serialization version for FOdysseyImageLayer
struct FOdysseyImageLayerObjectVersion
{
    enum Type
    {
        // Before any version changes were made
        SavePixelFormat,
        SaveBlendable,
        SaveBlockArray64,
        
        VersionPlusOne,
        LatestVersion = VersionPlusOne - 1
    };

    // The GUID for this custom version number
    const static FGuid GUID;

private:
    FOdysseyImageLayerObjectVersion() {}
};

const FGuid FOdysseyImageLayerObjectVersion::GUID(0xE2CA928C, 0x4FCB03A0, 0xE22252AA, 0xA88FC0B5);
//FCustomVersionRegistration FOdysseyImageLayerObjectVersionRegistration(FOdysseyImageLayerObjectVersion::GUID, FOdysseyImageLayerObjectVersion::LatestVersion, TEXT("FOdysseyImageLayerObjectVersion::SavePixelFormat"));

void
FOdysseyImageLayer::Serialize(FArchive &Ar)
{
    IOdysseyLayer::Serialize(Ar);

    //Set the Object Version
    //Ar.UsingCustomVersion(FOdysseyImageLayerObjectVersion::GUID);

    //Manage old saving order
    //if (Ar.CustomVer(FOdysseyImageLayerObjectVersion::GUID) >= FOdysseyImageLayerObjectVersion::SaveBlendable)
    //{
        SerializeImageBlendingCapability( Ar );
        Ar << mIsAlphaLocked;
    //}
    /*else
    {
        //Manage old saving order
        Ar << mIsAlphaLocked;
        SerializeImageBlendingCapability( Ar );
    }*/

    //Load/Save Size
    int width = mBlock ? mBlock->Width() : 0;
    int height = mBlock ? mBlock->Height() : 0;

    Ar << width;
    Ar << height;

    //Load/Save Format (compatibility with version version which don't save format)
    ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
    //if( Ar.CustomVer(FOdysseyImageLayerObjectVersion::GUID) >= FOdysseyImageLayerObjectVersion::SavePixelFormat )
    //{
        uint32 fmt = static_cast< uint32 >( format );
        Ar << fmt;

        // Hotfix to replace profile code in older texture files
        if( ULIS_R_PROFILE( fmt ) == ULIS_None )
            fmt = (fmt & ULIS_E_PROFILE) | ULIS_W_PROFILE( ::ULIS::FFormatMetrics::DefaultProfileCodeForColorModel( static_cast< ::ULIS::eColorModel >( ULIS_R_MODEL( fmt ) ) ) );

        format = static_cast< ULIS::eFormat >( fmt );
    //}

    //Create mBlock if we are loading
    if (Ar.IsLoading())
    {
        check(!mBlock);
        mBlock = new FOdysseyBlock(width, height, format);
        mBlock->GetBlock()->OnInvalid( ::ULIS::FOnInvalidBlock( &OnBlockInvalidated, static_cast< void* >( this ) ) );
    }

    //Load/Save mBlock content (compatibility with version which were saving/loading a TArray, but now we use TArray64)
    //if (Ar.CustomVer(FOdysseyImageLayerObjectVersion::GUID) >= FOdysseyImageLayerObjectVersion::SaveBlockArray64)
    //{
        Ar << mBlock->GetArray();
    //}
    /*else
    {
        TArray< uint8 > layerData = TArray< uint8 >();
        layerData.AddUninitialized(mBlock->GetBlock()->BytesTotal());
        Ar << layerData;

        for (int j = 0; j < layerData.Num(); j++) {
            *( mBlock->GetBlock()->Bits() + j ) = layerData[j];
        }
    }*/
}

//---

FOdysseyImageLayer::FOdysseyLayerIsAlphaLockedChanged&
FOdysseyImageLayer::IsAlphaLockedChangedDelegate()
{
    return mIsAlphaLockedChangedDelegate;
}

#undef LOCTEXT_NAMESPACE
