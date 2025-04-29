// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimation.h"

#include "Misc/TransactionObjectEvent.h"

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

    if (!mLayerStack || !mLayerStack->Implements<UOdysseyTextureRenderingAbility>())
        return FInt32Range::Empty();

    IOdysseyTextureRenderingAbility* renderingInterface = Cast<IOdysseyTextureRenderingAbility>(mLayerStack);
    return renderingInterface->GetFrameRange();
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
            FInt32Range frameRange = GetFrameRange();
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
            FInt32Range frameRange = GetFrameRange();
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
#endif

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

TArray<FGuid>
UOdysseyAnimation::GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrameIndex) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimation::GetRenderingComposition);

    if (!mLayerStack || !mLayerStack->Implements<UOdysseyTextureRenderingAbility>())
        return {};

    IOdysseyTextureRenderingAbility* renderingInterface = Cast<IOdysseyTextureRenderingAbility>(mLayerStack);

    TArray<FGuid> idComposition;
    idComposition.Append(renderingInterface->GetRenderingComposition(iRenderType, iFrameIndex));

    return idComposition;
}

TArray<FIntRect>
UOdysseyAnimation::GetRenderingRects() const
{
    return { FIntRect(0, 0, GetWidth(), GetHeight()) };
}

void
UOdysseyAnimation::RenderToTextureFromRects(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const TArray<FIntRect>& iRects, const FIntPoint& iPos) const
{
    if (!mLayerStack || !mLayerStack->Implements<UOdysseyTextureRenderingAbility>())
        return;

    IOdysseyTextureRenderingAbility::Execute_RenderRectsAtPosition(mLayerStack, iRenderTarget, iFrame, iRects, iPos);
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
    FInt32Range frameRange = GetFrameRange();
    LeftBound = frameRange.GetLowerBoundValue();
    RightBound = FMath::Max(GetLeftBoundValue(), RightBound);
}

void
UOdysseyAnimation::OnRightBoundModeChanged()
{
    FInt32Range frameRange = GetFrameRange();
    RightBound = frameRange.GetUpperBoundValue();
    LeftBound = FMath::Min(LeftBound, GetRightBoundValue());
}

void
UOdysseyAnimation::OnLeftBoundChanged()
{
    if (LeftBoundMode == EOdysseyAnimationBoundMode::Automatic)
    {
        FInt32Range frameRange = GetFrameRange();
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
        FInt32Range frameRange = GetFrameRange();
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


#undef LOCTEXT_NAMESPACE
