// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimation.h"

#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"

#include "Misc/TransactionObjectEvent.h"
#include "OdysseyAnimationImageRenderingAbility.h"

#include <ULIS>
#include "ULISLoaderModule.h"

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

void UOdysseyAnimation::Init(const FOdysseyAnimationConfiguration& iConfiguration)
{
	mWidth = iConfiguration.Width;
	mHeight = iConfiguration.Height;
	mFormat = iConfiguration.ULISFormat();
	FramesPerSecond = iConfiguration.FramesPerSecond;

	mLayerStack = NewObject<UOdysseyAnimationLayerStack>(this, "LayerStack", RF_Public | RF_Transactional);
	UOdysseyLayer* layer = mLayerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass()); //Move in OdysseyAnimationFactor
    mLayerStack->CurrentLayer = TSoftObjectPtr<UOdysseyLayer>(layer);
}

uint32
UOdysseyAnimation::Width() const
{
	return mWidth;
}

uint32
UOdysseyAnimation::Height() const
{
	return mHeight;
}

::ULIS::eFormat
UOdysseyAnimation::Format() const
{
	return ::ULIS::eFormat(mFormat);
}

FTimespan
UOdysseyAnimation::GetDuration() const
{
	return GetFrameTimeRange(GetFrameCount() - 1).GetUpperBoundValue();
}

FInt32Range
UOdysseyAnimation::GetFrameRange() const
{
	//TODO: deduce frame count from :
	// - startPoint / endPoint
	return mLayerStack->GetFrameRange();
}

uint32
UOdysseyAnimation::GetFrameCount() const
{
	FInt32Range frameRange = mLayerStack->GetFrameRange();
	int startFrame = frameRange.GetUpperBound().IsInclusive() ? frameRange.GetLowerBoundValue() : frameRange.GetLowerBoundValue() + 1;
	int endFrame = frameRange.GetUpperBound().IsInclusive() ? frameRange.GetUpperBoundValue() : frameRange.GetUpperBoundValue() - 1;
	return endFrame - startFrame + 1;
}

double
UOdysseyAnimation::GetFramesPerSecond() const
{
	return FramesPerSecond;
}

int
UOdysseyAnimation::GetFrameIndexAtTime(FTimespan iTime) const
{
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
UOdysseyAnimation::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
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
UOdysseyAnimation::PostInitProperties()
{
	Super::PostInitProperties();
	
	mProxy = MakeShared<FOdysseyAnimationProxy>(this);
	SetAbility(MakeShared<FOdysseyAnimationImageRenderingAbility>(this));
}

void
UOdysseyAnimation::PropertyChanged(const FName& iPropertyName)
{
	if ( iPropertyName == "CurrentFrame" )
        CurrentFrameChanged();
    if ( iPropertyName == "FramesPerSecond" )
        FramesPerSecondChanged();
}

void
UOdysseyAnimation::CurrentFrameChanged()
{
	OnCurrentFrameChanged().Broadcast(this);
}

void
UOdysseyAnimation::FramesPerSecondChanged()
{
	OnFramesPerSecondChanged().Broadcast(this);
}

/* IMediaSource overrides
 *****************************************************************************/

bool UOdysseyAnimation::GetMediaOption(const FName& Key, bool DefaultValue) const
{
	return Super::GetMediaOption(Key, DefaultValue);
}


bool UOdysseyAnimation::HasMediaOption(const FName& Key) const
{
	return Super::HasMediaOption(Key);
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
