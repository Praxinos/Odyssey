// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineHeader.h"
#include "Widgets/SOdysseyLayerStackAddLayerButton.h"
#include "Widgets/SOdysseyAnimationPlaybackControls.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "Widgets/LayerStack/SOdysseyAnimationLayerStackTreeView.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineTreeView.h"
#include "OdysseyAnimationEditorTimelinePosition.h"
#include "OdysseyAnimationEditorTimelineCellSelection.h"
#include "Widgets/Input/SSegmentedControl.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "OdysseyStyleSet.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineControl.h"
#include "LayerStack/Layers/LayerFolder/OdysseyAnimationLayerFolder.h"
#include "Widgets/LayerStack/Layers/LayerFolder/SOdysseyAnimationLayerFolderRow.h"
#include "Widgets/LayerStack/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterRow.h"
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorRow.h"
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
	, mTimelineScrollBar(nullptr)
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
	mTimelineCellSelection = InArgs._TimelineCellSelection;
	mOnActivateOutOfPegs = InArgs._OnActivateOutOfPegs;
	mOnInactivateOutOfPegs = InArgs._OnInactivateOutOfPegs;
	mOnIsOutOfPegsChecked = InArgs._OnIsOutOfPegsChecked;

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
    
    TSharedPtr<SWidget> widget =
    SNew(SVerticalBox)
    + SVerticalBox::Slot()
    .AutoHeight()
    [
        SNew(SHorizontalBox)
        //Left part 
        + SHorizontalBox::Slot()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SOdysseyLayerStackAddLayerButton)
                .LayerStack(layerStack)
                .OnAdded( this, &SOdysseyAnimationLayerStack::OnLayerAdded)
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            [
                SNew(SSegmentedControl<EOdysseyTimelineTool>)
                .Value(this, &SOdysseyAnimationLayerStack::GetCurrentTool)
                .OnValueChecked(this, &SOdysseyAnimationLayerStack::OnToolChecked)

                //Selection Tool
                + SSegmentedControl<EOdysseyTimelineTool>::Slot(EOdysseyTimelineTool::Selection)
                .Icon(FOdysseyStyle::GetBrush( "Animation.Timeline.Tools.Selection" ))
                .ToolTip(LOCTEXT("timeline.selection-tool.tooltip", "Selection Tool"))

                //Move Tool
                + SSegmentedControl<EOdysseyTimelineTool>::Slot(EOdysseyTimelineTool::Move)
                .Icon(FOdysseyStyle::GetBrush( "Animation.Timeline.Tools.Move" ))
                .ToolTip(LOCTEXT("timeline.move-tool.tooltip", "Move Tool"))

                //Cut Tool
                + SSegmentedControl<EOdysseyTimelineTool>::Slot(EOdysseyTimelineTool::Cut)
                .Icon(FOdysseyStyle::GetBrush( "Animation.Timeline.Tools.Cut" ))
                .ToolTip(LOCTEXT("timeline.cut-tool.tooltip", "Cut Tool"))
            ]
        ]

        //Center part
        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign( VAlign_Center )
        [
            SNew(SOdysseyAnimationPlaybackControls)
			.Visibility(mPlayerControlsVisibility)
			.Animation(mAnimation.Get())
			.Player(mPlayer.Get())
			.TimelineCellSelection(mTimelineCellSelection.Get())
            .PlaybackFramesPerSecond(mPlaybackFramesPerSecond)
        ]

        //Right part (empty but needed to center the center part)
        + SHorizontalBox::Slot()
        
    ]
    +SVerticalBox::Slot()
    .FillHeight(1.0f)
    [
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		[
			SAssignNew(mTreeView, SOdysseyAnimationLayerStackTreeView)
			.LayerStack(layerStack)
			.TimelineCellSelection(mTimelineCellSelection.Get())
			.OnGenerateRow(this, &SOdysseyAnimationLayerStack::OnGenerateRow)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNullWidget::NullWidget //TODO: Add a drag bar
		]
		+ SHorizontalBox::Slot()
		[
			SNew(SOdysseyAnimationTimelineTreeView)
			.LayerStack(layerStack)
			.Player(mPlayer.Get())
			.TimelineCellSelection(mTimelineCellSelection.Get())
			.TimelinePosition(mTimelinePosition.Get())
		]
    ]
    +SVerticalBox::Slot()
    .AutoHeight()
    [
        SAssignNew(mTimelineScrollBar, SScrollBar)
		.Visibility(mScrollbarVisibility)
        .Orientation( Orient_Horizontal )
        .OnUserScrolled_Raw(this, &SOdysseyAnimationLayerStack::OnTimelineScrollBarScrolled)
    ];

    this->ChildSlot.AttachWidget(widget.ToSharedRef());
}

EOdysseyTimelineTool
SOdysseyAnimationLayerStack::GetCurrentTool() const
{
	return FOdysseyAnimationTimelineTools::Get().GetCurrentTool();
}

void
SOdysseyAnimationLayerStack::OnToolChecked(EOdysseyTimelineTool iTool, ECheckBoxState iState)
{
    if (iState == ECheckBoxState::Checked)
		FOdysseyAnimationTimelineTools::Get().SetCurrentTool(iTool);
}

TSharedRef<ITableRow>
SOdysseyAnimationLayerStack::OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable)
{
    check(iLayer);

    UClass* layerClass = iLayer->GetClass();
    if (layerClass == UOdysseyAnimationLayerFolder::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerFolderRow, GetTreeView().ToSharedRef(), Cast<UOdysseyAnimationLayerFolder>(iLayer))
			.CurrentFrame(this, &SOdysseyAnimationLayerStack::GetCurrentFrame)
			.TimelinePosition(mTimelinePosition.Get())
			.TimelineCellSelection(mTimelineCellSelection.Get());
    }
    else if (layerClass == UOdysseyAnimationLayerImageRaster::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerImageRasterRow, GetTreeView().ToSharedRef(), Cast<UOdysseyAnimationLayerImageRaster>(iLayer))
			.CurrentFrame(this, &SOdysseyAnimationLayerStack::GetCurrentFrame)
			.TimelinePosition(mTimelinePosition.Get())
			.TimelineCellSelection(mTimelineCellSelection.Get())
			.OnActivateOutOfPegs(mOnActivateOutOfPegs)
			.OnInactivateOutOfPegs(mOnInactivateOutOfPegs)
			.OnIsOutOfPegsChecked(mOnIsOutOfPegsChecked);
    }
    else if (layerClass == UOdysseyAnimationLayerImageVector::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerImageVectorRow, GetTreeView().ToSharedRef(), Cast<UOdysseyAnimationLayerImageVector>(iLayer))
			.CurrentFrame(this, &SOdysseyAnimationLayerStack::GetCurrentFrame)
			.TimelinePosition(mTimelinePosition.Get())
			.TimelineCellSelection(mTimelineCellSelection.Get())
			.OnActivateOutOfPegs(mOnActivateOutOfPegs)
			.OnInactivateOutOfPegs(mOnInactivateOutOfPegs)
			.OnIsOutOfPegsChecked(mOnIsOutOfPegsChecked);
    }

    return SNew(STableRow<UOdysseyLayer*>, iOwnerTable);
}

void
SOdysseyAnimationLayerStack::OnTimelineScrollBarScrolled(float iOffset)
{
	UOdysseyAnimation* animation = mAnimation.Get();
	if (!animation)
		return;

	TSharedPtr<FOdysseyAnimationEditorTimelinePosition> timelinePosition = mTimelinePosition.Get();

    int lastFrameIndex = animation->GetFrameRange().GetUpperBoundValue();
    float frameWidth = timelinePosition->GetFrameSize();
    float columnWidth = mTimelineControl->GetPaintSpaceGeometry().GetLocalSize().X;
    float contentWidth = (lastFrameIndex + 1) * frameWidth;
    float adjustedContentWidth = FMath::Max(contentWidth, columnWidth) + columnWidth - frameWidth;
    float visiblePercent = columnWidth / adjustedContentWidth;
    float scrollbarOffset = FMath::Clamp(iOffset, 0.f, 1.f - visiblePercent);
    float offsetPercent = (scrollbarOffset / (1.f - visiblePercent));
    float offsetAmount = FMath::Max(lastFrameIndex, columnWidth / frameWidth - 1.f);
    timelinePosition->SetOffset( offsetPercent * offsetAmount );
}

TSharedPtr<SOdysseyLayerStackTreeView>
SOdysseyAnimationLayerStack::GetTreeView() const
{
    return mTreeView;
}

void
SOdysseyAnimationLayerStack::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
    if (!mTreeView)
        return;
        
	UOdysseyAnimation* animation = mAnimation.Get();
	if (!animation)
		return;

    TSharedPtr<SHeaderRow> headerRow = mTreeView->GetHeaderRow();
    if (!headerRow)
        return;

    const TIndirectArray<SHeaderRow::FColumn>& columns = headerRow->GetColumns();

	TSharedPtr<FOdysseyAnimationEditorTimelinePosition> timelinePosition = mTimelinePosition.Get();

    for ( const SHeaderRow::FColumn& column : columns )
    {
        if ( column.ColumnId != "Timeline" )
            continue;

        int lastFrameIndex = animation->GetFrameRange().GetUpperBoundValue();
        float frameWidth = timelinePosition->GetFrameSize();
        float offset = timelinePosition->GetOffset() * frameWidth;

        float columnWidth = mTimelineControl->GetPaintSpaceGeometry().GetLocalSize().X;
        float contentWidth = (lastFrameIndex + 1) * frameWidth;
        float adjustedContentWidth = FMath::Max(contentWidth, columnWidth) + columnWidth - frameWidth;

        float visiblePercent = columnWidth / adjustedContentWidth;


        float offsetPercent = offset / adjustedContentWidth;
        float scrollbarOffset = FMath::Clamp(offsetPercent, 0.f, 1.f - visiblePercent);
        mTimelineScrollBar->SetState(scrollbarOffset, visiblePercent);

        break;
    }
}

void
SOdysseyAnimationLayerStack::OnLayerAdded(UOdysseyLayer* iLayer)
{
    if (iLayer->GetClass() == UOdysseyAnimationLayerImageRaster::StaticClass())
    {
		UOdysseyAnimationLayerImageRaster* layer = Cast<UOdysseyAnimationLayerImageRaster>(iLayer);
		layer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass());
    }
    else if (iLayer->GetClass() == UOdysseyAnimationLayerImageVector::StaticClass())
    {
		UOdysseyAnimationLayerImageVector* layer = Cast<UOdysseyAnimationLayerImageVector>(iLayer);
        layer->AddCell(UOdysseyAnimationCellImageVector::StaticClass());
    }
}

int
SOdysseyAnimationLayerStack::GetCurrentFrame() const
{
	return mAnimation.Get()->GetFrameIndexAtTime(mPlayer.Get()->GetCurrentTime());
}

#undef LOCTEXT_NAMESPACE
