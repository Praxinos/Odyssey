// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimation.h"

#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationProxyImageRenderer.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseyAnimationProxy.h"
#include "Misc/OdysseyUndoDelegates.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#include "Misc/TransactionObjectEvent.h"

#include <ULIS>
#include "ULISLoaderModule.h"

#include "../../Engine/Public/ObjectEditorUtils.h"
#define LOCTEXT_NAMESPACE "Animation"

UOdysseyAnimation::FOnCurrentFrameChanged&
UOdysseyAnimation::OnCurrentFrameChanged()
{
    static FOnCurrentFrameChanged onCurrentFrameChanged;
    return onCurrentFrameChanged;
}

UOdysseyAnimation::FOnFramesPerSecondChanged&
UOdysseyAnimation::OnFramesPerSecondChanged()
{
    static FOnFramesPerSecondChanged onFramesPerSecondChanged;
    return onFramesPerSecondChanged;
}

void
UOdysseyAnimation::CurrentFrameBlueprintSetter(int Value)
{
    FObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), Value);
}

void
UOdysseyAnimation::FramesPerSecondBlueprintSetter(float Value)
{
    FObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, FramesPerSecond), Value);
}

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

::ULIS::eFormat
UOdysseyAnimation::GetFormat() const
{
    switch(Format)
    {
        case EOdysseyAnimationFormat::BGRA8: return ::ULIS::Format_BGRA8;
        case EOdysseyAnimationFormat::RGBAF: return ::ULIS::Format_RGBAF;
    }
    checkf(false, TEXT("Format not found"));
    return ::ULIS::Format_BGRA8;
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

    if ( !mLayerStack )
        return FInt32Range::Empty();

    return mLayerStack->GetFrameRange();
}

int
UOdysseyAnimation::GetFrameCount() const
{
    FInt32Range frameRange = GetFrameRange();
    return FMath::Max(0, frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1);
}

double
UOdysseyAnimation::GetFramesPerSecond() const
{
    return FramesPerSecond;
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
#if WITH_EDITOR
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
#endif
    return LeftBound;
}

int
UOdysseyAnimation::GetRightBoundValue() const
{
#if WITH_EDITOR
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
#endif
    return RightBound;
}

void
UOdysseyAnimation::SetLeftBoundMode(EOdysseyAnimationBoundMode iMode)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, LeftBoundMode), iMode);
}

void
UOdysseyAnimation::SetRightBoundMode(EOdysseyAnimationBoundMode iMode)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, RightBoundMode), iMode);
}

void
UOdysseyAnimation::SetLeftBoundValue(int iValue)
{
    if (LeftBoundMode != EOdysseyAnimationBoundMode::Manual)
        return;

    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, LeftBound), iValue);
}

void
UOdysseyAnimation::SetRightBoundValue(int iValue)
{
    if (RightBoundMode != EOdysseyAnimationBoundMode::Manual)
        return;

    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, RightBound), iValue);
}

int
UOdysseyAnimation::GetFrameIndexAtTime(FTimespan iTime) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimation::GetFrameIndexAtTime);
    //Add 1 tick to be sure to retrieve the right frame in case the frame starts between iTime and iTime + 1 tick
    FTimespan time = iTime + FTimespan(1);
    return int(time.GetTotalSeconds() * GetFramesPerSecond());
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

UOdysseyAnimationLayerStack*
UOdysseyAnimation::GetLayerStack() const
{
    return mLayerStack;
}

TSharedPtr<FOdysseyAnimationProxy>
UOdysseyAnimation::GetProxy() const
{
    return mProxy;
}

void
UOdysseyAnimation::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
    PostPropertyChanged(PropertyChangedEvent.GetPropertyName());
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
        FOdysseyUndoDelegates::Get().OnAfterUndoRedo().AddLambda(
            [this, propertyName](bool iIsRedo)
            {
                PostPropertyChanged(propertyName);
            }
        );
    }
}

void
UOdysseyAnimation::PostInitProperties()
{
    Super::PostInitProperties();

    if (HasAnyFlags(RF_ClassDefaultObject))
        return;

    mLayerStack = NewObject<UOdysseyAnimationLayerStack>(this, "LayerStack", RF_Public | RF_Transactional);
    mProxy = MakeShared<FOdysseyAnimationProxy>(this);

    OnImageRenderingChangedDelegate().AddUObject(this, &UOdysseyAnimation::OnImageRenderingChanged);
}

void
UOdysseyAnimation::OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimation::OnImageRenderingChanged);
    if (iEvent.IsInteractive())
        return;

    const FGuid& eventId = iEvent.GetId();
    FInt32Range frameRange = GetFrameRange();
    int startFrame = frameRange.GetUpperBound().IsInclusive() ? frameRange.GetLowerBoundValue() : frameRange.GetLowerBoundValue() + 1;
    int endFrame = frameRange.GetUpperBound().IsInclusive() ? frameRange.GetUpperBoundValue() : frameRange.GetUpperBoundValue() - 1;
    for (int i = startFrame; i <= endFrame; i++)
    {
        TArray<FGuid> composition = GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Editor, i);
        if (composition.Contains(eventId))
        {
            MarkPackageDirty();
            break;
        }
    }
}

void
UOdysseyAnimation::PostLoad()
{
    Super::PostLoad();

    if (mFormat == ::ULIS::Format_BGRA8)
    {
        Format = EOdysseyAnimationFormat::BGRA8;
    }
    if (mFormat == ::ULIS::Format_RGBAF)
    {
        Format = EOdysseyAnimationFormat::RGBAF;
    }

    mProxy->PostLoad();
}

void
UOdysseyAnimation::PropertyChanged(const FName& iPropertyName)
{
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame) )
        CurrentFrame = FMath::Max(0, CurrentFrame);

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, LeftBoundMode) )
    {
        FInt32Range frameRange = GetFrameRange();
        LeftBound = frameRange.GetLowerBoundValue();
        RightBound = FMath::Max(GetLeftBoundValue(), RightBound);
    }

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, RightBoundMode) )
    {
        FInt32Range frameRange = GetFrameRange();
        RightBound = frameRange.GetUpperBoundValue();
        LeftBound = FMath::Min(LeftBound, GetRightBoundValue());
    }

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, LeftBound) )
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

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, RightBound) )
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
}

void
UOdysseyAnimation::PostPropertyChanged(const FName& iPropertyName)
{
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame) )
        OnCurrentFrameChanged().Broadcast(this);
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, FramesPerSecond) )
        OnFramesPerSecondChanged().Broadcast(this);
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

TSharedPtr<IOdysseyImageRenderer>
UOdysseyAnimation::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter) const
{
    if (iFilter.IsBound() && !iFilter.Execute(this))
        return nullptr;

    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimation::BuildImageRenderer);
    return MakeShared<FOdysseyAnimationProxyImageRenderer>(this, iFrame, iRenderType, GetImageRenderingRects(), iFilter);
}

TArray<FGuid>
UOdysseyAnimation::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimation::GetImageRenderingComposition);
    TArray<FGuid> idComposition = { GetImageRenderingId() };

    if (!mLayerStack)
        return idComposition;

    idComposition.Append(mLayerStack->GetImageRenderingComposition(iRenderType, iFrameIndex));

    return idComposition;
}

TArray<::ULIS::FRectI>
UOdysseyAnimation::GetImageRenderingRects() const
{
    return { ::ULIS::FRectI::FromXYWH(0, 0, GetWidth(), GetHeight()) };
}

#undef LOCTEXT_NAMESPACE
