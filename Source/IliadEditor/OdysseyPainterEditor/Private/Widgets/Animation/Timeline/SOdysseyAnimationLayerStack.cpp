// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Animation/Timeline/SOdysseyAnimationLayerStack.h"
#include "Widgets/Animation/SOdysseyAnimationPlaybackControls.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationLayerStackTreeView.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineTreeView.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyAnimationCurrentFrameMutator.h"
#include "OdysseyStyle.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineControl.h"
#include "OdysseyAnimation.h"

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

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
    if (!layerStack)
        return;

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
    ]
    +SVerticalBox::Slot()
    .FillHeight(1.0f)
    [
        SAssignNew(mSplitter, SSplitter)
        .Orientation(EOrientation::Orient_Horizontal)
        .OnSplitterFinishedResizing_Lambda(
            [this, layerStack]()
            {
                layerStack->SetTimelineSplitterPosition(mSplitter->SlotAt(0).GetSizeValue());
                layerStack->SaveConfig();
            }
        )
        + SSplitter::Slot()
        .Value_Lambda(
            [this, layerStack]()
            {
                return layerStack->GetTimelineSplitterPosition();
            }
        )
        .OnSlotResized_Lambda(
            [this, layerStack](float iSize)
            {
                layerStack->SetTimelineSplitterPosition(iSize);
            }
        )
        [
            SAssignNew(mTreeView, SOdysseyAnimationLayerStackTreeView)
            .LayerStack(layerStack)
            .TimelinePosition(mTimelinePosition.Get())
            .ExternalScrollbar(mTimelineScrollBarV)
            .OnTreeViewScrolled(this, &SOdysseyAnimationLayerStack::OnTreeViewScrolled)
            .CurrentFrame(this, &SOdysseyAnimationLayerStack::GetCurrentFrame)
            .OnTransactCurrentFrame(this, &SOdysseyAnimationLayerStack::OnTransactCurrentFrame)
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
                    .LayerStack(layerStack)
                    .CurrentFrame(this, &SOdysseyAnimationLayerStack::GetCurrentFrame)
                    .OnTransactCurrentFrame(this, &SOdysseyAnimationLayerStack::OnTransactCurrentFrame)
                    .TimelinePosition(mTimelinePosition.Get())
                    .OnActivateOutOfPegs(mOnActivateOutOfPegs)
                    .OnInactivateOutOfPegs(mOnInactivateOutOfPegs)
                    .OnIsOutOfPegsChecked(mOnIsOutOfPegsChecked)
                    .ExternalScrollbar(dummyScrollBar)
                    .OnTreeViewScrolled(this, &SOdysseyAnimationLayerStack::OnTimelineTreeViewScrolled)
                    .OnScrubStart(mOnScrubStart)
                    .OnScrubEnd(mOnScrubEnd)
                    .OnCurrentFrameChanged(this, &SOdysseyAnimationLayerStack::OnCurrentFrameChanged)
                    .OnCurrentFrameCommited(this, &SOdysseyAnimationLayerStack::OnCurrentFrameCommited)
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

FNavigationReply
SOdysseyAnimationLayerStack::OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent)
{
    UOdysseyAnimationPlayer* player = mPlayer.Get();
    if (!player)
        return FNavigationReply::Stop();

    if (InNavigationEvent.GetNavigationType() == EUINavigation::Left)
    {
        player->SeekToFrame(player->GetCurrentFrame() - 1);
    }
    else if (InNavigationEvent.GetNavigationType() == EUINavigation::Right)
    {
        player->SeekToFrame(player->GetCurrentFrame() + 1);
    }
    return FNavigationReply::Stop();
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
    UOdysseyAnimationPlayer* player = mPlayer.Get();
    if (!player)
        return 0;

    return player->GetCurrentFrame().FrameNumber.Value;
}

void
SOdysseyAnimationLayerStack::OnCurrentFrameChanged(int iFrame)
{
    UOdysseyAnimationPlayer* player = mPlayer.Get();
    if (!player)
        return;

    player->SeekToFrame(iFrame);
}

void
SOdysseyAnimationLayerStack::OnCurrentFrameCommited(int iFrame)
{
    UOdysseyAnimationPlayer* player = mPlayer.Get();
    if (!player)
        return;

    player->SeekToFrame(iFrame);
}

void
SOdysseyAnimationLayerStack::OnTransactCurrentFrame(TOptional<int> iFrame)
{
    UOdysseyAnimationPlayer* player = mPlayer.Get();
    if (!player)
        return;

    int frame = iFrame.Get(player->GetCurrentFrame().FrameNumber.Value);

    FOdysseyAnimationCurrentFrameMutator currentFrameMutator(player);
    currentFrameMutator.Set(frame);
    currentFrameMutator.Commit();
}

#undef LOCTEXT_NAMESPACE
