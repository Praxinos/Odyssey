// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimation.h"

#include "Misc/TransactionObjectEvent.h"
#include "CanvasTypes.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Cooker/CookEvents.h"

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
UOdysseyAnimation::GetFrameIndexAtFrame(int iFrameIndex) const
{
    int frameIndex = 0;
    for (const FOdysseyAnimationFrame& frame : Frames)
    {
        if (iFrameIndex >= frameIndex && iFrameIndex < frameIndex + frame.Exposure)
            return frameIndex;

        frameIndex += frame.Exposure;
    }
    return INDEX_NONE;
}

TArray<FGuid>
UOdysseyAnimation::GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrameIndex) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimation::GetRenderingComposition);
#if WITH_EDITOR
    if (!mLayerStack || !mLayerStack->Implements<UOdysseyTextureRenderingAbility>())
        return {};

    IOdysseyTextureRenderingAbility* renderingInterface = Cast<IOdysseyTextureRenderingAbility>(mLayerStack);

    TArray<FGuid> idComposition;
    idComposition.Append(renderingInterface->GetRenderingComposition(iRenderType, iFrameIndex));

    return idComposition;
#else
    int frameIndex = GetFrameIndexAtFrame(iFrameIndex);
    if (frameIndex == INDEX_NONE)
        return {};

    return Frames[frameIndex].RenderingComposition;
#endif
}

FIntRect
UOdysseyAnimation::GetDefaultRenderRect() const
{
    return FIntRect(0, 0, GetWidth(), GetHeight());
}

void
UOdysseyAnimation::RenderToTexture_RenderThread(
    FRDGBuilder& iGraphBuilder,
    FRDGTextureRef iDestinationTexture,
    ERHIFeatureLevel::Type iFeatureLevel,
    FFrameNumber iFrame,
    const FMatrix& iSrcTransform,
    const FIntRect& iSrcRect,
    const FIntRect& iDstRect
) const
{
#if WITH_EDITOR
    if (!mLayerStack || !mLayerStack->Implements<UOdysseyTextureRenderingAbility>())
        return;

    IOdysseyTextureRenderingAbility* renderingInterface = Cast<IOdysseyTextureRenderingAbility>(mLayerStack);
    renderingInterface->RenderToTexture_RenderThread(iGraphBuilder, iDestinationTexture, iFeatureLevel, iFrame, FMatrix::Identity, iSrcRect, iDstRect);
#else
    FTextureResource* srcResource = nullptr;
    int frameIndex = GetFrameIndexAtFrame(iFrame.Value);
    if (frameIndex != INDEX_NONE)
    {
        if (Frames[frameIndex].Texture)
            srcResource = Frames[frameIndex].Texture->GetResource();
    }

    FIntPoint pos = iSrcRect.Min - iPos;

    double x = iDstRect.Min.X;
    double y = iDstRect.Min.Y;
    double w = iDstRect.Width();
    double h = iDstRect.Height();
    float u = float(iSrcRect.Min.X) / GetWidth();
    float v = float(iSrcRect.Min.Y) / GetHeight();
    float sizeU = float(iSrcRect.Max.X) / GetWidth();
    float sizeV = float(iSrcRect.Max.Y) / GetHeight();

    //Create a Canvas to draw with the shader
    FCanvas* canvas = FCanvas::Create(iGraphBuilder, iDestinationTexture, nullptr, FGameTime(), iFeatureLevel);
    iCanvas->DrawTile(x, y, w, h, u, v, sizeU, sizeV, FLinearColor::Transparent, GWhiteTexture, SE_BLEND_Opaque);

    if (srcResource)
        iCanvas->DrawTile(x, y, w, h, u, v, sizeU, sizeV, FLinearColor::White, srcResource, SE_BLEND_AlphaBlend);

    iCanvas->Flush_GameThread(true);
#endif
}

#if WITH_EDITOR
void
UOdysseyAnimation::Init(int iWidth, int iHeight, EOdysseyAnimationFormat iFormat, float iFramesPerSecond)
{
    mWidth = iWidth;
    mHeight = iHeight;
    Format = iFormat;
    FramesPerSecond = iFramesPerSecond;
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
    switch(LeftBoundMode)
    {
        case EOdysseyAnimationBoundMode::Automatic:
        {
            if (!mLayerStack || !mLayerStack->Implements<UOdysseyTextureRenderingAbility>())
                return 0;

            IOdysseyTextureRenderingAbility* renderingInterface = Cast<IOdysseyTextureRenderingAbility>(mLayerStack);
            FInt32Range frameRange = renderingInterface->GetFrameRange();
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
    switch(RightBoundMode)
    {
        case EOdysseyAnimationBoundMode::Automatic:
        {
            if (!mLayerStack || !mLayerStack->Implements<UOdysseyTextureRenderingAbility>())
                return 0;

            IOdysseyTextureRenderingAbility* renderingInterface = Cast<IOdysseyTextureRenderingAbility>(mLayerStack);
            FInt32Range frameRange = renderingInterface->GetFrameRange();
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
    LeftBoundMode = iMode;

    OnLeftBoundModeChanged();
}

void
UOdysseyAnimation::SetRightBoundMode(EOdysseyAnimationBoundMode iMode)
{
    RightBoundMode = iMode;

    OnRightBoundModeChanged();
}

void
UOdysseyAnimation::SetLeftBoundValue(int iValue)
{
    LeftBound = iValue;

    OnLeftBoundChanged();
}

void
UOdysseyAnimation::SetRightBoundValue(int iValue)
{
    RightBound = iValue;

    OnRightBoundChanged();
}

void
UOdysseyAnimation::SetLayerStack(UObject* iLayerStack)
{
    if (iLayerStack == mLayerStack)
        return;

    mLayerStack = iLayerStack;
    RenderingCompositionChanged();
}

UObject*
UOdysseyAnimation::GetLayerStack() const
{
    return mLayerStack;
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
    if (!mLayerStack || !mLayerStack->Implements<UOdysseyTextureRenderingAbility>())
        return;

    IOdysseyTextureRenderingAbility* renderingInterface = Cast<IOdysseyTextureRenderingAbility>(mLayerStack);
    FInt32Range frameRange = renderingInterface->GetFrameRange();
    LeftBound = frameRange.GetLowerBoundValue();
    RightBound = FMath::Max(GetLeftBoundValue(), RightBound);
}

void
UOdysseyAnimation::OnRightBoundModeChanged()
{
    if (!mLayerStack || !mLayerStack->Implements<UOdysseyTextureRenderingAbility>())
        return;

    IOdysseyTextureRenderingAbility* renderingInterface = Cast<IOdysseyTextureRenderingAbility>(mLayerStack);
    FInt32Range frameRange = renderingInterface->GetFrameRange();
    RightBound = frameRange.GetUpperBoundValue();
    LeftBound = FMath::Min(LeftBound, GetRightBoundValue());
}

void
UOdysseyAnimation::OnLeftBoundChanged()
{
    if (LeftBoundMode == EOdysseyAnimationBoundMode::Automatic)
    {
        if (!mLayerStack || !mLayerStack->Implements<UOdysseyTextureRenderingAbility>())
            return;

        IOdysseyTextureRenderingAbility* renderingInterface = Cast<IOdysseyTextureRenderingAbility>(mLayerStack);
        FInt32Range frameRange = renderingInterface->GetFrameRange();
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
        if (!mLayerStack || !mLayerStack->Implements<UOdysseyTextureRenderingAbility>())
            return;

        IOdysseyTextureRenderingAbility* renderingInterface = Cast<IOdysseyTextureRenderingAbility>(mLayerStack);
        FInt32Range frameRange = renderingInterface->GetFrameRange();
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
UOdysseyAnimation::PreSave(FObjectPreSaveContext SaveContext)
{
    Super::PreSave(SaveContext);

    if ( SaveContext.IsCooking() )
        return;

    Frames.Empty();

    TStrongObjectPtr<UTextureRenderTarget2D> renderTarget(NewObject<UTextureRenderTarget2D>());
    renderTarget->RenderTargetFormat = RTF_RGBA8_SRGB;
    renderTarget->bForceLinearGamma = true;
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

        Execute_Render(this, renderTarget.Get(), FFrameNumber(i));

        UTexture2D* texture = NewObject<UTexture2D>(this, NAME_None, RF_Public);
        renderTarget.Get()->UpdateTexture(texture);

        FOdysseyAnimationFrame frame;
        frame.Texture = texture;
        frame.Exposure = 1;
        frame.RenderingComposition = renderingComposition;
        Frames.Add(frame);
    }
}
#endif


#undef LOCTEXT_NAMESPACE
