// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureLayerImageVector.h"

#include "OdysseyTextureLayerStack.h"
#include "OdysseyPixelFormat.h"
#include "EditorStyleSet.h"
#include "OdysseyMediaVector.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"
#include "Export/OdysseyVectorExport.h"
#include "Import/OdysseyVectorImport.h"

#include "blend2d.h"

#define LOCTEXT_NAMESPACE "UOdysseyTextureLayerImageVector"

UOdysseyTextureLayerImageVector::FOnBlendModeChanged&
UOdysseyTextureLayerImageVector::OnBlendModeChanged()
{
    static FOnBlendModeChanged onBlendModeChanged;
    return onBlendModeChanged;
}

UOdysseyTextureLayerImageVector::FOnOpacityChanged&
UOdysseyTextureLayerImageVector::OnOpacityChanged()
{
    static FOnOpacityChanged onOpacityChanged;
    return onOpacityChanged;
}

UOdysseyTextureLayerImageVector::~UOdysseyTextureLayerImageVector()
{
    // TODO: free the scene

    //mScene->OnUpdateDelegate().Remove(mOnRefreshHandle);
}

UOdysseyTextureLayerImageVector::UOdysseyTextureLayerImageVector()
    : mEngine(nullptr)
{
	LayerTypeName = LOCTEXT("LayerTypeName", "Vector Image Layer");
    Icon = *FOdysseyStyle::GetBrush( "OdysseyLayerStack.LayerVector16");
}

void
UOdysseyTextureLayerImageVector::Init( uint32 iWidth, uint32 iHeight )
{
    BLImageData imgData;

    Width  = iWidth;
    Height = iHeight;

    mEngine = new FOdysseyVectorEngine( new FOdysseyVectorScene( "Scene" )
                                       , (double)iWidth
                                       , (double)iHeight );
//UE_LOG(LogTemp, Warning, TEXT("UOdysseyTextureLayerImageVector::Init %X"), mEngine );
    // record a callback to refresh the layer when a property of an object's details view is changed
    //mOnRefreshHandle = mScene->OnUpdateDelegate().AddUObject( this, &UOdysseyTextureLayerImageVector::OnRefresh );

    //UE_LOG(LogTemp,Warning,TEXT("UOdysseyTextureLayerImageVector::Init %d %d %d"), iWidth, iHeight, mVEngine );

    mEngine->GetBLImage()->getData( &imgData );

    mBlock = MakeShared<::ULIS::FBlock>(static_cast<uint8*>(imgData.pixelData)
                               , iWidth
                               , iHeight
    // ::ULIS::eFormat::Format_BGRA8 is the same as Blend2D's BL_FORMAT_PRGB32
                               , ::ULIS::eFormat::Format_BGRA8
                               , nullptr);
}

FOdysseyVectorEngine*
UOdysseyTextureLayerImageVector::GetEngine()
{
    return mEngine;
}

FOdysseyMediaProvider
UOdysseyTextureLayerImageVector::GetMediaProvider() const
{
    FOdysseyMediaProvider mediaProvider;
    TSharedPtr<FOdysseyMediaVector> mediaVector = MakeShared<FOdysseyMediaVector>(mEngine->GetScene());

    bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(this);
    bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(this);
    mediaVector->IsLocked(!isActive || isLocked);

    mediaProvider.Add(mediaVector);
    return mediaProvider;
}

void
UOdysseyTextureLayerImageVector::OnCreated_Implementation()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if(!layerStack)
        return;

    UTexture2D* texture = layerStack->GetTexture();

    Init( texture->Source.GetSizeX(), texture->Source.GetSizeY() );
}

void
UOdysseyTextureLayerImageVector::RenderImageChanged( bool iIsInteractive )
{
    UOdysseyTextureLayer::RenderImageChanged( iIsInteractive );
}

void
UOdysseyTextureLayerImageVector::RenderImageChanged( const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());

    // render once to buffer, then the call to RenderImageChanged() will copy each rectangle from the buffer to the layer
    mEngine->Render();

    // HUD displaying only for the current layer.
    if( layerStack->CurrentLayer.Get() == this )
    {
        mEngine->RenderHUD();
    }

    UOdysseyTextureLayer::RenderImageChanged( iRects, iIsInteractive );
}

TArray<::ULIS::FEvent>
UOdysseyTextureLayerImageVector::RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!IsActivated)
        return iWaitList;

    if (!ioBlock)
        return iWaitList;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mBlock->Format());

    TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, mBlock->Format(), iRect, iPos, iWaitList,
        [this, &ctx](TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FEvent eventBlend = FULISEventBuilder().RetainBlock(mBlock).Build();

            ctx.Blend(
                *mBlock,
                *ioDest,
                iRect,
                iPos,
                ::ULIS::eBlendMode(BlendMode),
                ::ULIS::Alpha_Normal,
                Opacity,
                ::ULIS::FSchedulePolicy::/*CacheEfficient*/AsyncCacheEfficient,
                iWaitList.Num(),
                iWaitList.GetData(),
                &eventBlend
            );

            return { eventBlend };
        }
    );

    ctx.Flush();
    ctx.Finish();

    return eventConvertAndExecute;
}

void
UOdysseyTextureLayerImageVector::Serialize(FArchive& Ar)
{
    Super::Serialize( Ar );

    if( Ar.IsSaving() )
    {
        FOdysseyVectorExport::Write( mEngine ? mEngine->GetScene() : nullptr, Ar );
    }

    if( Ar.IsLoading() )
    {
        if ( mEngine == nullptr )
        {
            // commented out: at that point, the texture owning the layer stack doe snot have width and height values. 
            // This should be changed. As a bypass, I store dimensions in Width and Height UProperties.
            //UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
            //if(!layerStack)
            //    return;
            //UTexture2D* texture = layerStack->GetTexture();

            //Init( texture->Source.GetSizeX(), texture->Source.GetSizeY() );

            Init( Width, Height );
        }

        FOdysseyVectorImport::Read( mEngine->GetScene(), Ar );

        mEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                       | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY );
    }
}

void
UOdysseyTextureLayerImageVector::PropertyChanged(const FName& iPropertyName)
{
    Super::PropertyChanged(iPropertyName);

    if (iPropertyName == "BlendMode")
        BlendModeChanged();
    if (iPropertyName == "Opacity")
        OpacityChanged();
}

void
UOdysseyTextureLayerImageVector::OpacityChanged()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if (!layerStack)
        return;

    OnOpacityChanged().Broadcast(this);

    RenderImageChanged({ ::ULIS::FRectI::FromXYWH(0, 0, mBlock->Width(), mBlock->Height()) }, false);
}

void
UOdysseyTextureLayerImageVector::BlendModeChanged()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if (!layerStack)
        return;

    OnBlendModeChanged().Broadcast(this);

    RenderImageChanged({ ::ULIS::FRectI::FromXYWH(0, 0, mBlock->Width(), mBlock->Height()) }, false);
}

#undef LOCTEXT_NAMESPACE
