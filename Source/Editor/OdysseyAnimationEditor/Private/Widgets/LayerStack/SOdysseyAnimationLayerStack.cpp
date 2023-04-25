// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineHeader.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerStack"

//Define base frame width to be 50 pixels
#define BASE_FRAMEWIDTH 50.f
#define MIN_ZOOM 0.01f
#define MAX_ZOOM 1.0f
#define ZOOM_STEP 0.08f

SOdysseyAnimationLayerStack::~SOdysseyAnimationLayerStack()
{
}

SOdysseyAnimationLayerStack::SOdysseyAnimationLayerStack()
    : mAnimation(nullptr)
    , mPlayer(nullptr)
    , mTreeView()
    , mTimelineZoom(1.f)
	, mTimelineOffset(0.f)
	, mTimelineScrollBar(nullptr)
{
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void
SOdysseyAnimationLayerStack::Construct(const FArguments& InArgs)
{
    mAnimation = InArgs._Animation;
    mPlayer = InArgs._Player;
    ChildSlot
    [
        SNew(SVerticalBox)
        +SVerticalBox::Slot()
        .FillHeight(1.0f)
        [
            SAssignNew(mTreeView, SOdysseyLayerStackTreeView)
            .LayerStack(mAnimation->GetLayerStack())
            .OnGenerateRow(this, &SOdysseyAnimationLayerStack::OnGenerateRow)
            .HeaderManualWidth(200.f)
            .AdditionalColumns(
                {
                    SHeaderRow::Column("Timeline")
                    .DefaultLabel(LOCTEXT("", ""))
                    .VAlignCell(VAlign_Fill)
                    .HAlignCell(HAlign_Fill)
                    [
                        SNew(SOdysseyAnimationTimelineHeader, SharedThis(this))
                    ]
                }
            )
        ]
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew(mTimelineScrollBar, SScrollBar)
            .Orientation( Orient_Horizontal )
            .OnUserScrolled_Raw(this, &SOdysseyAnimationLayerStack::OnTimelineScrollBarScrolled)
        ]
    ];

    //Set Scrollbar Params
    mTimelineScrollBar->SetState(0.f, 0.5f);
}

TSharedRef<ITableRow>
SOdysseyAnimationLayerStack::OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable)
{
    check(iLayer);

    UClass* layerClass = iLayer->GetClass();
    if (layerClass == UOdysseyAnimationLayerFolder::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerFolderRow, SharedThis(this), Cast<UOdysseyAnimationLayerFolder>(iLayer));
    }
    else if (layerClass == UOdysseyAnimationLayerImageRaster::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerImageRasterRow, SharedThis(this), Cast<UOdysseyAnimationLayerImageRaster>(iLayer));
    }

    return SNew(SOdysseyAnimationLayerRow, SharedThis(this), Cast<UOdysseyAnimationLayer>(iLayer)); //Default widget
}

void
SOdysseyAnimationLayerStack::OnTimelineScrollBarScrolled(float iOffset)
{
    float visiblePercent = 0.5f;
    float scrollbarOffset = FMath::Clamp(iOffset, 0.f, visiblePercent);
    mTimelineScrollBar->SetState(scrollbarOffset, visiblePercent);

    int lastFrameIndex = mAnimation->GetFrameRange().GetUpperBoundValue();
    float offsetPercent = (scrollbarOffset / (1.f - visiblePercent));
    SetTimelineOffset(offsetPercent * lastFrameIndex);
    //Set mOffset
}

UOdysseyAnimation*
SOdysseyAnimationLayerStack::GetAnimation() const
{
    return mAnimation;
}

UOdysseyAnimationPlayer*
SOdysseyAnimationLayerStack::GetPlayer() const
{
    return mPlayer;
}

TSharedPtr<SOdysseyLayerStackTreeView>
SOdysseyAnimationLayerStack::GetTreeView() const
{
    return mTreeView;
}

void
SOdysseyAnimationLayerStack::TimelineZoomIn()
{
	mTimelineZoom = FMath::Clamp(mTimelineZoom * (1.0f - ZOOM_STEP), MIN_ZOOM, MAX_ZOOM);
}

void
SOdysseyAnimationLayerStack::TimelineZoomOut()
{
	mTimelineZoom = FMath::Clamp(mTimelineZoom * (1.0f + ZOOM_STEP), MIN_ZOOM, MAX_ZOOM);
}

void
SOdysseyAnimationLayerStack::SetTimelineZoom(float iZoom)
{
	mTimelineZoom = iZoom;
}

void
SOdysseyAnimationLayerStack::SetTimelineOffset( float iOffset )
{
    mTimelineOffset = iOffset;
    mOnTimelineOffsetChanged.Broadcast();
}

float
SOdysseyAnimationLayerStack::GetTimelineBaseFrameSize()
{
	return BASE_FRAMEWIDTH;
}

float
SOdysseyAnimationLayerStack::GetTimelineFrameWidth() const
{
	return BASE_FRAMEWIDTH * mTimelineZoom;
}

float
SOdysseyAnimationLayerStack::GetTimelineZoom() const
{
	return mTimelineZoom;
}

float
SOdysseyAnimationLayerStack::GetTimelineOffset() const
{
    //TODO: use SScrollBar offset to define that offset
	return mTimelineOffset;
}

FSimpleMulticastDelegate&
SOdysseyAnimationLayerStack::OnTimelineOffsetChanged()
{
    return mOnTimelineOffsetChanged;
}

#undef LOCTEXT_NAMESPACE
