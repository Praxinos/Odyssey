// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimation.h"

#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationProxyImageRenderer.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "OdysseyRasterBlockMutator.h"

#include "Misc/TransactionObjectEvent.h"

#include <ULIS>
#include "ULISLoaderModule.h"

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

void UOdysseyAnimation::Init(const FOdysseyAnimationConfiguration& iConfiguration)
{
	mWidth = iConfiguration.Width;
	mHeight = iConfiguration.Height;
	mFormat = iConfiguration.ULISFormat();
	FramesPerSecond = iConfiguration.FramesPerSecond;

	mLayerStack = NewObject<UOdysseyAnimationLayerStack>(this, "LayerStack", RF_Public | RF_Transactional);

	switch (iConfiguration.LayerType)
	{
		case EOdysseyAnimationDefaultLayerType::kRaster:
		{
			UOdysseyAnimationLayerImageRaster* layer = Cast<UOdysseyAnimationLayerImageRaster>(mLayerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass()));
			mLayerStack->CurrentLayer = TSoftObjectPtr<UOdysseyLayer>(layer);
			layer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass());
		}
		break;

		case EOdysseyAnimationDefaultLayerType::kVector:
		{
			UOdysseyAnimationLayerImageVector* layer = Cast<UOdysseyAnimationLayerImageVector>(mLayerStack->AddLayer(UOdysseyAnimationLayerImageVector::StaticClass()));
			mLayerStack->CurrentLayer = TSoftObjectPtr<UOdysseyLayer>(layer);
			layer->AddCell(UOdysseyAnimationCellImageVector::StaticClass());
		}
		break;

		default:
			check(false); //should not be called
	}

	//Background Layer
	if (iConfiguration.BackgroundColor != EOdysseyAnimationBackgroundColor::kTransparent)
	{	
		UOdysseyAnimationLayerImageRaster* backgroundLayer = Cast<UOdysseyAnimationLayerImageRaster>(mLayerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass(), nullptr, 1));
		backgroundLayer->PostBehaviour = EOdysseyAnimationLayerImagePostBehaviour::Hold;
		backgroundLayer->Name = LOCTEXT("animation.default-background-layer.name", "Background");

		UOdysseyAnimationCellImageRaster* backgroundCell = Cast<UOdysseyAnimationCellImageRaster>(backgroundLayer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass()));
		TSharedPtr<FOdysseyRasterBlock> backgroundRasterBlock = backgroundCell->GetRasterBlock();

		FLinearColor backgorundColor = iConfiguration.GetBackgroundColor();

		FOdysseyRasterBlockMutator rasterBlockMutator(backgroundRasterBlock);
		rasterBlockMutator.EditTilesFromRects(
			{ backgroundRasterBlock->GetRect() },
			[backgorundColor](TSharedPtr<::ULIS::FBlock> ioBlock, const FULISInvalidTileMap& iInvalidTileMap) -> TArray<::ULIS::FEvent>
			{
				::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ioBlock->Format());
				::ULIS::FColor color( ::ULIS::FColor::FromRGBAF( backgorundColor.R, backgorundColor.G, backgorundColor.B, backgorundColor.A ) );
				ctx.Fill(*ioBlock, color);
				ctx.Finish();
				return {};
			}
		);
	}
}

int
UOdysseyAnimation::Width() const
{
	return mWidth;
}

int
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
	FInt32Range range = GetFrameRange();
	return FTimespan::FromSeconds((range.GetUpperBoundValue() - range.GetLowerBoundValue() + 1) / GetFramesPerSecond()) - FTimespan(1);
}

FInt32Range
UOdysseyAnimation::GetFrameRange() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimation::GetFrameRange);
	//TODO: deduce frame count from :
	// - startPoint / endPoint

	if ( !mLayerStack )
		return FInt32Range::Empty();

	FInt32Range layerStackFrameRange = mLayerStack->GetFrameRange();
	return FInt32Range::Inclusive(0, layerStackFrameRange.GetUpperBoundValue()); //Animation starts always at 0 if there is no startPoint
}

int
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

    if (HasAnyFlags(RF_ClassDefaultObject))
        return;
	
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
	mProxy->PostLoad();
}

void
UOdysseyAnimation::PropertyChanged(const FName& iPropertyName)
{
	if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame) )
        CurrentFrameChanged();
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, FramesPerSecond) )
        FramesPerSecondChanged();
}

void
UOdysseyAnimation::CurrentFrameChanged()
{
	CurrentFrame = FMath::Max(0, CurrentFrame);
	OnCurrentFrameChanged().Broadcast(this);
}

void
UOdysseyAnimation::FramesPerSecondChanged()
{
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
    return { ::ULIS::FRectI::FromXYWH(0, 0, Width(), Height()) };
}

#undef LOCTEXT_NAMESPACE
