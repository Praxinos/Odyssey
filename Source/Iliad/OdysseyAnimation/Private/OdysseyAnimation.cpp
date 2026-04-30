// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimation.h"

#include "Misc/TransactionObjectEvent.h"
#include "CanvasTypes.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Cooker/CookEvents.h"
#include "RenderGraphBuilder.h"
#include "ScreenPass.h"
#include "TextureResource.h"
#include "OdysseyAnimationLayerStack.h"
#include "UObject/SavePackage.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerCell.h"
#include "OdysseyAnimationCellImageVector.h"

#define LOCTEXT_NAMESPACE "Animation"

int
UOdysseyAnimation::GetWidth() const
{
    return mWidth;
}

int
UOdysseyAnimation::GetHeight() const
{
    return mHeight;
}

int
UOdysseyAnimation::GetWidthFromHeightKeepingRatio( int iHeight )
{
    float ratio = mWidth / (float)mHeight;

    return int( iHeight * ratio );
}
int
UOdysseyAnimation::GetHeightFromWidthKeepingRatio( int iWidth )
{
    float ratio = mWidth / (float)mHeight;

    return int( iWidth / ratio );
}

EOdysseyAnimationFormat
UOdysseyAnimation::GetFormat() const
{
    return Format;
}

FTimespan
UOdysseyAnimation::GetDuration() const
{
    if (GetFrameCount() == 0)
    {
        return FTimespan::FromSeconds(0);
    }
    return FTimespan::FromSeconds(GetFrameCount() / GetFramesPerSecond()) - FTimespan(1);
}

FInt32Range
UOdysseyAnimation::GetFrameRange() const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimation::GetFrameRange);
    #if WITH_EDITOR
    return FInt32Range::Inclusive(GetLeftBoundValue(), GetRightBoundValue());
    #else
    if ( PreserveLayerStackAtRuntime )
        return FInt32Range::Inclusive(GetLeftBoundValue(), GetRightBoundValue());

    int frameCount = 0;
    for (const FOdysseyAnimationFrame& frame : Frames)
    {
        frameCount += frame.Exposure;
    }
    return FInt32Range::Inclusive(0, frameCount - 1);
    #endif
}

int
UOdysseyAnimation::GetFrameCount() const
{
    FInt32Range frameRange = GetFrameRange();
    return FMath::Max(0, frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1);
}

float
UOdysseyAnimation::GetFramesPerSecond() const
{
    return FramesPerSecond;
}

TRange<FTimespan>
UOdysseyAnimation::GetFrameTimeRange(int iFrameIndex) const
{
    FTimespan start = FTimespan::FromSeconds(iFrameIndex / GetFramesPerSecond());

    //Remove one tick because end timespan is included in the range
    //That way we never have two frame with overlapping timeranges
    FTimespan end = FTimespan::FromSeconds((iFrameIndex + 1) / GetFramesPerSecond()) - FTimespan(1);
    return TRange<FTimespan>(start, end);
}

/* UMediaSource overrides
 *****************************************************************************/

FString UOdysseyAnimation::GetUrl() const
{
    return FString(TEXT("odysseyanimation://")) + GetPathName();
}

bool UOdysseyAnimation::Validate() const
{
    return true;
}

int
UOdysseyAnimation::GetFrameIndexAtFrame(int iFrame) const
{
    int frameIndex = 0;
    int framePos = 0;
    for (const FOdysseyAnimationFrame& frame : Frames)
    {
        if ( iFrame >= framePos && iFrame < framePos + frame.Exposure)
            return frameIndex;

        framePos += frame.Exposure;
        frameIndex++;
    }
    return INDEX_NONE;
}

TArray<FGuid>
UOdysseyAnimation::GetRenderingComposition(uint64 iRenderType, int iFrameIndex) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimation::GetRenderingComposition);
    TArray<FGuid> idComposition = { GetRenderingId() };

#if WITH_EDITOR
    if ( !mLayerStack )
        return idComposition;

    idComposition.Append(mLayerStack->GetRenderingComposition(iRenderType, iFrameIndex));
#else
    if ( PreserveLayerStackAtRuntime )
    {
        if ( !mLayerStack )
            return idComposition;

        idComposition.Append(mLayerStack->GetRenderingComposition(iRenderType, iFrameIndex));
    }
    else
    {
        int frameIndex = GetFrameIndexAtFrame(iFrameIndex);
        if ( frameIndex == INDEX_NONE )
            return idComposition;

        idComposition.Append(Frames[frameIndex].RenderingComposition);
    }
#endif

    return idComposition;
}

FIntRect
UOdysseyAnimation::GetDefaultRenderRect() const
{
    return FIntRect(0, 0, GetWidth(), GetHeight());
}

bool
UOdysseyAnimation::BuildRenderPipelineInternal(
    FFrameNumber iFrame,
    uint64 iType,
    IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
    const IOdysseyTextureRenderingAbility::FCanRenderFunction& iCanRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& iParents
) const
{
#if WITH_EDITOR
    if ( !mLayerStack )
        return false;

    return mLayerStack->BuildRenderPipeline(iFrame, iType, oRenderFunction, iCanRenderFunction, iParents);
#else
    if ( PreserveLayerStackAtRuntime )
    {
        if ( !mLayerStack )
            return false;

        return mLayerStack->BuildRenderPipeline(iFrame, iType, oRenderFunction, iCanRenderFunction, iParents);
    }

    UTexture2D* srcTexture2D = nullptr;
    int frameIndex = GetFrameIndexAtFrame(iFrame.Value);
    if (frameIndex != INDEX_NONE)
    {
        if (Frames[frameIndex].Texture)
            srcTexture2D = Frames[frameIndex].Texture;
    }

    if (!srcTexture2D)
        return false;

    oRenderFunction = [srcTexture2D](
        FRDGBuilder& iGraphBuilder,
        ERHIFeatureLevel::Type iFeatureLevel,
        FRDGTextureRef iDestinationTexture,
        const FIntRect& iSrcRect,
        const FIntRect& iDstRect,
        const FMatrix& iSrcTransform
        )
    {
        AddClearRenderTargetPass(iGraphBuilder, iDestinationTexture, FLinearColor::Transparent, iDstRect);

        FRDGTextureRef sourceTexture = iGraphBuilder.RegisterExternalTexture(CreateRenderTarget(srcTexture2D->GetResource()->TextureRHI, TEXT("UOdysseyAnimation::sourceTexture")));

        AddDrawTexturePass(
            iGraphBuilder,
            FScreenPassViewInfo(),
            sourceTexture,
            iDestinationTexture,
            iSrcRect.Min,
            iSrcRect.Size(),
            iDstRect.Min,
            iDstRect.Size()
        );
    };

    return true;
#endif
}

UTextureRenderTarget2D*
UOdysseyAnimation::CreateRenderingRenderTarget() const
{
    UTextureRenderTarget2D* renderTarget = NewObject<UTextureRenderTarget2D>();
    ETextureRenderTargetFormat renderTargetFormat = RTF_RGBA8;
    switch(Format)
    {
        case EOdysseyAnimationFormat::BGRA8:
        {
            renderTarget->RenderTargetFormat = RTF_RGBA8_SRGB;
            renderTarget->bForceLinearGamma = false;
        }
        break;

        case EOdysseyAnimationFormat::RGBAF:
        {
            renderTarget->RenderTargetFormat = RTF_RGBA32f;
            renderTarget->bForceLinearGamma = true;
        }
        break;
        default: checkf(false, TEXT("Non implemented format"))
    }
    renderTarget->SRGB = renderTarget->IsSRGB();
    renderTarget->InitAutoFormat(mWidth, mHeight);

    return renderTarget;
}

EOdysseyAnimationBoundMode
UOdysseyAnimation::GetLeftBoundMode() const
{
    return LeftBoundMode;
}

EOdysseyAnimationBoundMode
UOdysseyAnimation::GetRightBoundMode() const
{
    return RightBoundMode;
}

int
UOdysseyAnimation::GetLeftBoundValue() const
{
    switch ( LeftBoundMode )
    {
        case EOdysseyAnimationBoundMode::Automatic:
        {
            if ( !mLayerStack )
                return 0;

            FInt32Range frameRange = mLayerStack->GetFrameRange();
            return frameRange.GetLowerBoundValue();
        }
        break;

        case EOdysseyAnimationBoundMode::Manual:
        {
            return FMath::Max(0, LeftBound);
        }
        break;
    }
    return LeftBound;
}

int
UOdysseyAnimation::GetRightBoundValue() const
{
    switch ( RightBoundMode )
    {
        case EOdysseyAnimationBoundMode::Automatic:
        {
            if ( !mLayerStack )
                return 0;

            FInt32Range frameRange = mLayerStack->GetFrameRange();
            return frameRange.GetUpperBoundValue();
        }
        break;

        case EOdysseyAnimationBoundMode::Manual:
        {
            return FMath::Max(0, RightBound);
        }
        break;
    }
    return RightBound;
}

void
UOdysseyAnimation::SetLeftBoundMode(EOdysseyAnimationBoundMode iMode)
{
    Modify();
    LeftBoundMode = iMode;

#if WITH_EDITOR
    OnLeftBoundModeChanged();
#endif
}

void
UOdysseyAnimation::SetRightBoundMode(EOdysseyAnimationBoundMode iMode)
{
    Modify();
    RightBoundMode = iMode;

#if WITH_EDITOR
    OnRightBoundModeChanged();
#endif
}

void
UOdysseyAnimation::SetLeftBoundValue(int iValue)
{
    Modify();
    LeftBound = iValue;

#if WITH_EDITOR
    OnLeftBoundChanged();
#endif
}

void
UOdysseyAnimation::SetRightBoundValue(int iValue)
{
    Modify();
    RightBound = iValue;

#if WITH_EDITOR
    OnRightBoundChanged();
#endif
}

UOdysseyLayerStack*
UOdysseyAnimation::GetLayerStack() const
{
    return mLayerStack;
}

#if WITH_EDITOR
void
UOdysseyAnimation::Init(int iWidth, int iHeight, EOdysseyAnimationFormat iFormat, float iFramesPerSecond)
{
    mWidth = iWidth;
    mHeight = iHeight;
    Format = iFormat;
    FramesPerSecond = iFramesPerSecond;

    UOdysseyAnimationLayerStack* layerStack = NewObject<UOdysseyAnimationLayerStack>(this, "LayerStack", RF_Public | RF_Transactional);
    SetLayerStack(layerStack);
}

void
UOdysseyAnimation::SetLayerStack(UOdysseyLayerStack* iLayerStack)
{
    if (iLayerStack == mLayerStack)
        return;

    mLayerStack = iLayerStack;
    RenderingCompositionChanged();
}

void
UOdysseyAnimation::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
}

void
UOdysseyAnimation::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    for ( const FName& propertyName : changedPropertyNames )
    {
        PropertyChanged(propertyName);
    }
}

void
UOdysseyAnimation::OnLeftBoundModeChanged()
{
    if (!mLayerStack)
        return;

    FInt32Range frameRange = mLayerStack->GetFrameRange();
    LeftBound = frameRange.GetLowerBoundValue();
    RightBound = FMath::Max(GetLeftBoundValue(), RightBound);
}

void
UOdysseyAnimation::OnRightBoundModeChanged()
{
    if (!mLayerStack)
        return;

    FInt32Range frameRange = mLayerStack->GetFrameRange();
    RightBound = frameRange.GetUpperBoundValue();
    LeftBound = FMath::Min(LeftBound, GetRightBoundValue());
}

void
UOdysseyAnimation::OnLeftBoundChanged()
{
    if (LeftBoundMode == EOdysseyAnimationBoundMode::Automatic)
    {
        if (!mLayerStack)
            return;

        FInt32Range frameRange = mLayerStack->GetFrameRange();
        LeftBound = frameRange.GetLowerBoundValue();
    }
    else
    {
        LeftBound = FMath::Clamp(LeftBound, 0, GetRightBoundValue());
    }
}

void
UOdysseyAnimation::OnRightBoundChanged()
{
    if (RightBoundMode == EOdysseyAnimationBoundMode::Automatic)
    {
        if (!mLayerStack)
            return;

        FInt32Range frameRange = mLayerStack->GetFrameRange();
        RightBound = frameRange.GetUpperBoundValue();
    }
    else
    {
        RightBound = FMath::Max( RightBound, GetLeftBoundValue() );
    }
}

void
UOdysseyAnimation::PropertyChanged(const FName& iPropertyName)
{
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, LeftBoundMode) )
    {
        OnLeftBoundModeChanged();
    }

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, RightBoundMode) )
    {
        OnRightBoundModeChanged();
    }

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, LeftBound) )
    {
        OnLeftBoundChanged();
    }

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, RightBound) )
    {
        OnRightBoundChanged();
    }
}

void
UOdysseyAnimation::CollectSaveOverrides(FObjectCollectSaveOverridesContext SaveContext)
{
    Super::CollectSaveOverrides(SaveContext);

    if (SaveContext.IsCooking()
        && !HasAnyFlags(RF_ArchetypeObject | RF_ClassDefaultObject)
        && !PreserveLayerStackAtRuntime)
    {
        FObjectSaveOverride ObjSaveOverride;

        // Add path to the conditions within the main definition
        FProperty* OverrideProperty = FindFProperty<FProperty>(GetClass(), GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, mLayerStack));
        check(OverrideProperty);
        FPropertySaveOverride PropOverride;
        PropOverride.PropertyPath = FFieldPath(OverrideProperty);
        PropOverride.bMarkTransient = true;

        ObjSaveOverride.PropOverrides.Add(PropOverride);

        SaveContext.AddSaveOverride(this, ObjSaveOverride);
    }
}

void
UOdysseyAnimation::GetAssetRegistryTags( FAssetRegistryTagsContext ioContext ) const
{
    Super::GetAssetRegistryTags( ioContext );

    ioContext.AddTag( { "Image Size", FString::Printf(TEXT("%dx%d"), mWidth, mHeight), FAssetRegistryTag::TT_Alphabetical } );

    // don't display the decimal part if none
    if( ((int)FramesPerSecond) == FramesPerSecond )
        ioContext.AddTag( { "Frame Rate", FString::Printf(TEXT("%d fps"), (int)FramesPerSecond), FAssetRegistryTag::TT_Alphabetical } );
    else
        ioContext.AddTag( { "Frame Rate", FString::Printf(TEXT("%.2f fps"), FramesPerSecond), FAssetRegistryTag::TT_Alphabetical } );
}

void
UOdysseyAnimation::GetAssetRegistryTagMetadata( TMap<FName, FAssetRegistryTagMetadata>& OutMetadata ) const
{
    Super::GetAssetRegistryTagMetadata( OutMetadata );

    OutMetadata.Add(
        "Image Size",
        FAssetRegistryTagMetadata()
        .SetDisplayName( NSLOCTEXT( "OdysseyAnimation", "ImageSize_Label", "Image Size" ) )
        .SetTooltip( NSLOCTEXT( "OdysseyAnimation", "ImageSize_Tooltip", "Image size" ) )
    );

    OutMetadata.Add(
        "Frame Rate",
        FAssetRegistryTagMetadata()
        .SetDisplayName( NSLOCTEXT( "OdysseyAnimation", "FrameRate_Label", "Frame Rate" ) )
        .SetTooltip( NSLOCTEXT( "OdysseyAnimation", "FrameRate_Tooltip", "Frame Rate" ) )
    );
}

void
UOdysseyAnimation::PreSave(FObjectPreSaveContext SaveContext)
{
    Super::PreSave(SaveContext);

    if ( SaveContext.IsCooking() )
        return;

    if( !FApp::CanEverRender() ) //Returns false while packaging a Game, in which case any rendering is forbidden and Render_GameThread would fail
        return;

    Frames.Empty();

    if ( PreserveLayerStackAtRuntime )
        return;

    TStrongObjectPtr<UTextureRenderTarget2D> renderTarget(NewObject<UTextureRenderTarget2D>());
    renderTarget->RenderTargetFormat = RTF_RGBA8_SRGB;
    renderTarget->bForceLinearGamma = false;
    renderTarget->InitAutoFormat(GetWidth(), GetHeight());
    renderTarget->UpdateResourceImmediate();

    FInt32Range range = GetFrameRange();
    TArray<FGuid> lastRenderingComposition;
    for ( int i = range.GetLowerBoundValue(); i <= range.GetUpperBoundValue(); i++ )
    {
        TArray<FGuid> renderingComposition = GetRenderingComposition(EOdysseyRenderingType::Render, i);
        if ( renderingComposition == lastRenderingComposition )
        {
            Frames.Last().Exposure++;
            continue;
        }

        lastRenderingComposition = renderingComposition;

        Render_GameThread(renderTarget.Get(), FFrameNumber(i), EOdysseyRenderingType::Render);


        FString Name = FString::Printf(TEXT("OdysseyAnimationFrameTexture%d"), i);

        UTexture2D* texture = Cast<UTexture2D>(renderTarget->ConstructTexture(this, Name, RF_Public, CTF_Default));
        texture->PreSave(SaveContext);

        FOdysseyAnimationFrame frame;
        frame.Texture = texture;
        frame.Exposure = 1;
        frame.RenderingComposition = renderingComposition;
        Frames.Add(frame);
    }
}

UTexture2D*
UOdysseyAnimation::CreateExportTexture(UObject* Outer, FName Name, EObjectFlags Flags)
{
    UTexture2D* texture = NewObject<UTexture2D>(Outer, Name, Flags);

    switch(Format)
    {
        case EOdysseyAnimationFormat::BGRA8:
        {
            texture->PreEditChange(nullptr);
            texture->Source.Init(mWidth, mHeight, 1, 1, TSF_BGRA8);
            texture->SRGB = true;
            texture->PostEditChange();
        }
        break;

        case EOdysseyAnimationFormat::RGBAF:
        {
            texture->PreEditChange(nullptr);
            texture->Source.Init(mWidth, mHeight, 1, 1, TSF_RGBA32F);
            texture->SRGB = false;
            texture->PostEditChange();
        }
        break;
        default: checkf(false, TEXT("Non implemented format"))
    }

    return texture;
}

void
UOdysseyAnimation::OnRefreshReferencedPalette(UOdysseyPalette* iPalette)
{
    //CanEverRender() returns false while packaging a Game, in which case any rendering is forbidden
    //Refreshing cells palette, leads to rendering, so we don't want that
    //Also if this function is called during Palette's PreSave
    //And we ABSOLUTELY don't want SavePackage() to be called at this moment
    //because it leads to assets file handles to be locked on windows, preventing any Game Packaging to succeed
    if( !FApp::CanEverRender() )
        return;

    //Save the animation before applying the palette, because all these refresh only affect the "on disk" version of the asset, not the dirty "on memory" one.
    //Therefore, if the palette is freshly added to the (then dirtied) animation, the refresh won't apply to it

    UPackage* package = GetOutermost();
    FSavePackageArgs saveArgs;
    saveArgs.TopLevelFlags = RF_Standalone;
    saveArgs.Error = GWarn;
    saveArgs.SaveFlags = SAVE_NoError;
    FString packageFilename = FPackageName::LongPackageNameToFilename(package->GetName(), FPackageName::GetAssetPackageExtension());
    UPackage::SavePackage(package, this, *packageFilename, saveArgs);
    FlushAsyncLoading();

    TArray<UOdysseyLayer*> layers = GetLayerStack()->GetLayers();
    for (UOdysseyLayer* layer : layers)
    {
        for (UOdysseyLayerCell* cell : layer->GetCells())
        {
            if (!cell->IsA(UOdysseyAnimationCellImageVector::StaticClass()))
                continue;

            UOdysseyAnimationCellImageVector* animationVectorCell = Cast<UOdysseyAnimationCellImageVector>(cell);
            animationVectorCell->OnRefreshReferencedPalette(iPalette);
        }
    }
}

#endif


#undef LOCTEXT_NAMESPACE
