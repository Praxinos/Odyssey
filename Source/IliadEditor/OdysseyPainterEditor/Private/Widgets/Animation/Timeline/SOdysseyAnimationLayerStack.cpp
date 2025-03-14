// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Animation/Timeline/SOdysseyAnimationLayerStack.h"
#include "Widgets/Animation/SOdysseyAnimationPlaybackControls.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationLayerStackTreeView.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineTreeView.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "OdysseyStyle.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineControl.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationPlayer.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SLATE_IMPLEMENT_WIDGET(SOdysseyAnimationLayerStack)
void
SOdysseyAnimationLayerStack::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mAnimation, EInvalidateWidgetReason::Layout)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyAnimationLayerStack&>(Widget).RebuildWidgets();
        }
    ));
}

SOdysseyAnimationLayerStack::~SOdysseyAnimationLayerStack()
{
}

SOdysseyAnimationLayerStack::SOdysseyAnimationLayerStack()
    : mAnimation(*this, nullptr)
    , mPlayerControlsVisibility(EVisibility::Visible)
    , mScrollbarVisibility(EVisibility::Visible)
    , mPlaybackFramesPerSecond(24.0f)
    , mTreeView()
    , mTimelineScrollBarH(nullptr)
    , mTimelineScrollBarV(nullptr)
{
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void
SOdysseyAnimationLayerStack::Construct(const FArguments& InArgs)
{
    mAnimation.Assign(*this, InArgs._Animation);
    mPlayer = InArgs._Player;
    mPlayerControlsVisibility = InArgs._PlayerControlsVisibility;
    mPlaybackFramesPerSecond = InArgs._PlaybackFramesPerSecond;
    mScrollbarVisibility = InArgs._ScrollbarVisibility;
    mTimelinePosition = InArgs._TimelinePosition;
    mOnActivateOutOfPegs = InArgs._OnActivateOutOfPegs;
    mOnInactivateOutOfPegs = InArgs._OnInactivateOutOfPegs;
    mOnIsOutOfPegsChecked = InArgs._OnIsOutOfPegsChecked;
    mCustomValidRange = InArgs._CustomValidRange;
    mEditor = InArgs._PainterEditor;
    mOnScrubStart = InArgs._OnScrubStart;
    mOnScrubEnd = InArgs._OnScrubEnd;

    RebuildWidgets();
}

void
SOdysseyAnimationLayerStack::RebuildWidgets()
{
    this->ChildSlot.DetachWidget();
    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = animation->GetLayerStack();

    mTimelineScrollBarV = SNew(SScrollBar)
        .Visibility(mScrollbarVisibility)
        .Orientation(Orient_Vertical);

    TSharedPtr<SScrollBar> dummyScrollBar = SNew(SScrollBar);

    TSharedPtr<SWidget> widget =
    SNew(SVerticalBox)
    + SVerticalBox::Slot()
    .AutoHeight()
    .HAlign( HAlign_Center )
    .VAlign( VAlign_Center )
    [
        SNew(SOdysseyAnimationPlaybackControls)
        .Visibility(mPlayerControlsVisibility)
        .Animation(mAnimation.Get())
        .Player(mPlayer.Get())
        .PlaybackFramesPerSecond(mPlaybackFramesPerSecond)
    ]
    +SVerticalBox::Slot()
    .FillHeight(1.0f)
    [
        SAssignNew(mSplitter, SSplitter)
        .Orientation(EOrientation::Orient_Horizontal)
        .OnSplitterFinishedResizing_Lambda(
            [this]()
            {
                mAnimation.Get()->TimelineSplitterPosition = mSplitter->SlotAt(0).GetSizeValue();
                mAnimation.Get()->SaveConfig();
            }
        )
        + SSplitter::Slot()
        .Value_Lambda(
            [this]()
            {
                return mAnimation.Get()->TimelineSplitterPosition;
            }
        )
        .OnSlotResized_Lambda(
            [this](float iSize)
            {
                mAnimation.Get()->TimelineSplitterPosition = iSize;
            }
        )
        [
            SAssignNew(mTreeView, SOdysseyAnimationLayerStackTreeView)
            .PainterEditor(mEditor)
            .LayerStack(layerStack)
            .TimelinePosition(mTimelinePosition.Get())
            .ExternalScrollbar(mTimelineScrollBarV)
            .OnTreeViewScrolled(this, &SOdysseyAnimationLayerStack::OnTreeViewScrolled)
        ]
        + SSplitter::Slot()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(SOdysseyAnimationTimelineControl)
                .Animation(mAnimation.Get())
                .CurrentFrame(this, &SOdysseyAnimationLayerStack::GetCurrentFrame)
                .TimelinePosition(mTimelinePosition.Get())
                .CustomValidRange(mCustomValidRange)
                [
                    SAssignNew(mTimelineTreeView, SOdysseyAnimationTimelineTreeView)
                    .PainterEditor(mEditor)
                    .LayerStack(layerStack)
                    .Player(mPlayer.Get())
                    .TimelinePosition(mTimelinePosition.Get())
                    .OnActivateOutOfPegs(mOnActivateOutOfPegs)
                    .OnInactivateOutOfPegs(mOnInactivateOutOfPegs)
                    .OnIsOutOfPegsChecked(mOnIsOutOfPegsChecked)
                    .ExternalScrollbar(dummyScrollBar)
                    .OnTreeViewScrolled(this, &SOdysseyAnimationLayerStack::OnTimelineTreeViewScrolled)
                    .OnScrubStart(mOnScrubStart)
                    .OnScrubEnd(mOnScrubEnd)
                ]
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                mTimelineScrollBarV.ToSharedRef()
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SBox)
                .Visibility(EVisibility::Collapsed)
                [
                    dummyScrollBar.ToSharedRef() //a dummy scrollbar that is never shown
                    //is only here to avoid to have a second automatic scrollbar shown in the treeview
                ]
            ]
        ]
    ]
    +SVerticalBox::Slot()
    .AutoHeight()
    [
        SAssignNew(mTimelineScrollBarH, SScrollBar)
        .Visibility(mScrollbarVisibility)
        .Orientation( Orient_Horizontal )
        .OnUserScrolled_Raw(this, &SOdysseyAnimationLayerStack::OnTimelineScrollBarHScrolled)
    ];

    this->ChildSlot.AttachWidget(widget.ToSharedRef());
}

void
SOdysseyAnimationLayerStack::OnTimelineScrollBarHScrolled(float iOffset)
{
    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return;

    TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition> timelinePosition = mTimelinePosition.Get();

    int lastFrameIndex = animation->GetFrameRange().GetUpperBoundValue();
    float frameWidth = timelinePosition->GetFrameSize();
    float columnWidth = mTimelineTreeView->GetPaintSpaceGeometry().GetLocalSize().X;
    float contentWidth = (lastFrameIndex + 1) * frameWidth;
    float adjustedContentWidth = FMath::Max(contentWidth, columnWidth) + columnWidth - frameWidth;
    float visiblePercent = columnWidth / adjustedContentWidth;
    float scrollbarOffset = FMath::Clamp(iOffset, 0.f, 1.f - visiblePercent);
    float offsetPercent = (scrollbarOffset / (1.f - visiblePercent));
    float offsetAmount = FMath::Max(lastFrameIndex, columnWidth / frameWidth - 1.f);
    timelinePosition->SetOffset( offsetPercent * offsetAmount );
}

void
SOdysseyAnimationLayerStack::OnTreeViewScrolled(double iOffset)
{
    mTimelineTreeView->ScrollTo(iOffset);
}

void
SOdysseyAnimationLayerStack::OnTimelineTreeViewScrolled(double iOffset)
{
    mTreeView->ScrollTo(iOffset);
}

TSharedPtr<SOdysseyLayerStackTreeView>
SOdysseyAnimationLayerStack::GetTreeView() const
{
    return mTreeView;
}

TSharedPtr<SOdysseyAnimationTimelineTreeView>
SOdysseyAnimationLayerStack::GetTimelineTreeView() const
{
    return mTimelineTreeView;
}

void
SOdysseyAnimationLayerStack::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
    if (!mTimelineTreeView)
        return;

    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return;

    TSharedPtr<SHeaderRow> headerRow = mTimelineTreeView->GetHeaderRow();
    if (!headerRow)
        return;

    const TIndirectArray<SHeaderRow::FColumn>& columns = headerRow->GetColumns();

    TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition> timelinePosition = mTimelinePosition.Get();

    for ( const SHeaderRow::FColumn& column : columns )
    {
        if ( column.ColumnId != "Timeline" )
            continue;

        int lastFrameIndex = animation->GetFrameRange().GetUpperBoundValue();
        float frameWidth = timelinePosition->GetFrameSize();
        float offset = timelinePosition->GetOffset() * frameWidth;

        float columnWidth = mTimelineTreeView->GetPaintSpaceGeometry().GetLocalSize().X;
        float contentWidth = (lastFrameIndex + 1) * frameWidth;
        float adjustedContentWidth = FMath::Max(contentWidth, columnWidth) + columnWidth - frameWidth;

        float visiblePercent = columnWidth / adjustedContentWidth;


        float offsetPercent = offset / adjustedContentWidth;
        float scrollbarOffset = FMath::Clamp(offsetPercent, 0.f, 1.f - visiblePercent);
        mTimelineScrollBarH->SetState(scrollbarOffset, visiblePercent);

        break;
    }
}

int
SOdysseyAnimationLayerStack::GetCurrentFrame() const
{
    int frame = 0;
    if (!mPlayer.Get()->GetCurrentFrameInAnimationBounds(frame))
        return INDEX_NONE;
    return frame;
}

#undef LOCTEXT_NAMESPACE
