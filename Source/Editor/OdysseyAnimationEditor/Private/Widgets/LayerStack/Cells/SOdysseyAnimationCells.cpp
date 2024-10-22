// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Cells/SOdysseyAnimationCells.h"
#include "Widgets/LayerStack/Cells/SOdysseyAnimationCell.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineSection.h"
#include "Widgets/Layout/SMissingWidget.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineScrollBox.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "Widgets/LayerStack/Cells/SOdysseyAnimationCellHandle.h"
#include "LayerStack/Tools/OdysseyAnimationTimelineTool.h"
#include "LayerStack/Tools/OdysseyAnimationTimelineTools.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
SOdysseyAnimationCells::~SOdysseyAnimationCells()
{
    mAnimationLayer->OnCellsChanged().RemoveAll(this);
}

SOdysseyAnimationCells::SOdysseyAnimationCells()
    : mLockHandlesVisibility(false)
    , mHoveredCell(nullptr)
    , mTimingHandleCells()
    , mTimingHandleBrush(nullptr)
    , mExposureHandleBrush(nullptr)
    , mAddCellsHandleRightBrush(nullptr)
    , mAddCellsHandleLeftBrush(nullptr)
    , mCellBreakIndicatorBrush(nullptr)
    , mNumTempCellsToPrepend(0)
    , mNumTempCellsToAppend(0)
    
{
}

void
SOdysseyAnimationCells::Construct(
    const FArguments& InArgs,
    UOdysseyAnimationLayer* iAnimationLayer
)
{
    ensure(iAnimationLayer);

	mTimelinePosition = InArgs._TimelinePosition;
	mTimelineCellSelection = InArgs._TimelineCellSelection;

    mTimingHandleBrush = FOdysseyStyle::GetBrush("Animation.CellTimingHandle");
	mExposureHandleBrush = FOdysseyStyle::GetBrush("Animation.CellExposureHandle");
    mAddCellsHandleLeftBrush = FOdysseyStyle::GetBrush("Animation.AddCellsHandleLeft");
    mAddCellsHandleRightBrush = FOdysseyStyle::GetBrush("Animation.AddCellsHandleRight");
    mCellBreakIndicatorBrush = FOdysseyStyle::GetBrush("Animation.CellBreakIndicator");
    FSlateColor preBehaviourColor( FOdysseyStyle::GetColor( "Animation.Layer.PreBehaviourColor" ) );
	FSlateColor postBehaviourColor( FOdysseyStyle::GetColor( "Animation.Layer.PostBehaviourColor" ) );
	float preBehaviourPadding = FOdysseyStyle::GetFloat(TEXT("Animation.Timeline.Padding"));
    mAnimationLayer = iAnimationLayer;

    mOnCreateCellWidget = InArgs._OnCreateCellWidget;
    mShowHandles = InArgs._ShowHandles;
    
    ChildSlot
    .Padding(FMargin(0, 4.f, 0, 4.f))
    [
        SNew(SOdysseyAnimationTimelineScrollBox)
		.TimelinePosition(mTimelinePosition)
        + SOdysseyAnimationTimelineScrollBox::Slot()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                //Timeline Section for Layer Offset
                SNew(SOdysseyAnimationTimelineSection)
				.TimelinePosition(mTimelinePosition)
                .WidthInFrames(this, &SOdysseyAnimationCells::GetOffset)
                .Content()
                [
					SNullWidget::NullWidget
                ]
            ]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(FMargin(-preBehaviourPadding, 0, 0, 0))
			[
				SNew(SBox)
				.WidthOverride(preBehaviourPadding)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Center)
					[
						SNew(SComboButton)
						.ButtonStyle(&FAppStyle::Get().GetWidgetStyle< FButtonStyle >( "SimpleButton" ))
						.HasDownArrow(false)
						.OnGetMenuContent(this, &SOdysseyAnimationCells::GetPreBehaviourMenuContent)
						.ButtonContent()
						[
							SNew(SImage)
							.Image(this, &SOdysseyAnimationCells::GetPreBehaviourBrush)
							.ColorAndOpacity(preBehaviourColor)
						]
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						//Add Cells Handle
						CreateAddCellsHandleLeftWidget()
					]
				]
			]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SAssignNew(mTempPreCellsBox, SHorizontalBox)
            ]
			+ SHorizontalBox::Slot()
            .AutoWidth()
            [
                SAssignNew(mCellsBox, SHorizontalBox)
            ]
			+ SHorizontalBox::Slot()
            .AutoWidth()
            [
                SAssignNew(mTempPostCellsBox, SHorizontalBox)
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Top)
            [
                //Add Cells Handle
                CreateAddCellsHandleRightWidget()
            ]
			+ SHorizontalBox::Slot()
            .AutoWidth()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Center)
            [
                SNew(SComboButton)
				.ButtonStyle(&FAppStyle::Get().GetWidgetStyle< FButtonStyle >( "SimpleButton" ))
				.HasDownArrow(false)
				.OnGetMenuContent(this, &SOdysseyAnimationCells::GetPostBehaviourMenuContent)
				.ButtonContent()
				[
					SNew(SImage)
					.Image(this, &SOdysseyAnimationCells::GetPostBehaviourBrush)
					.ColorAndOpacity(postBehaviourColor)
				]
            ]
        ]
    ];

    mAnimationLayer->OnCellsChanged().AddRaw(this, &SOdysseyAnimationCells::OnCellsChanged);
    RefreshCells();
}


float
SOdysseyAnimationCells::GetOffset() const
{
    return mAnimationLayer->CellsOffset;
}

float
SOdysseyAnimationCells::GetCellExposure(UOdysseyAnimationCell* iCell) const
{
    return iCell->Exposure;
}

void
SOdysseyAnimationCells::AddTempCellSection(bool iPost)
{
    TSharedRef<SWidget> widget = SNew(SOdysseyAnimationTimelineSection)
	.TimelinePosition(mTimelinePosition)
    .WidthInFrames(1)
    [
        SNew(SImage)
        .Image(FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameBackground"))
        //.BorderBackgroundColor(FLinearColor(1.f, 1.f, 1.f))
        .ColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.3f))
    ];

    //Cells widgets
	if (iPost)
	{
		mTempPostCellsBox->AddSlot()
		.AutoWidth()
		[
			widget
		];
	}
	else
	{
		mTempPreCellsBox->AddSlot()
		.AutoWidth()
		[
			widget
		];
	}
}

void
SOdysseyAnimationCells::AddCellSection(int iCellIndex)
{
	if (mAnimationLayer->GetCells().IsEmpty())
		return;

    UOdysseyAnimationCell* cell = mAnimationLayer->GetCells()[iCellIndex];

    TSharedRef<SWidget> widget = SNew(SOdysseyAnimationTimelineSection)
		.TimelinePosition(mTimelinePosition)
        .WidthInFrames(this, &SOdysseyAnimationCells::GetCellExposure, cell)
        [
            SNew(SOverlay)
            + SOverlay::Slot() //Cell Widget
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Fill)
            [
                CreateCellWidget(iCellIndex)
            ]
            
            + SOverlay::Slot() //Timing Handle Top Left
            .Padding(0.f, 0.f, -mExposureHandleBrush->ImageSize.X, 0.f)
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Top)
            [
                CreateTimingHandleWidget(iCellIndex)
            ]

            + SOverlay::Slot() //Exposure Handle Top Right
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Bottom)
            [
                CreateExposureHandleWidget(iCellIndex)
            ]

            + SOverlay::Slot() //Exposure Handle Top Right
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Fill)
            [
                CreateCellBreakIndicatorWidget(iCellIndex)
            ]
        ];

    //Cells widgets
    mCellsBox->AddSlot()
    .AutoWidth()
    [
        widget
    ];
}

void
SOdysseyAnimationCells::RefreshCells()
{   
    mCellsBox->ClearChildren();

    for ( int i = 0; i < mAnimationLayer->GetCells().Num(); i++ )
    {
        AddCellSection(i);
    }
}

void
SOdysseyAnimationCells::RefreshTempCells()
{
    mTempPreCellsBox->ClearChildren();
	mTempPostCellsBox->ClearChildren();

    for ( uint32 i = 0; i < mNumTempCellsToPrepend; i++ )
    {
        AddTempCellSection(false);
    }

    for ( uint32 i = 0; i < mNumTempCellsToAppend; i++ )
    {
        AddTempCellSection(true);
    }
}

TSharedRef<SWidget>
SOdysseyAnimationCells::CreateTimingHandleWidget(int iCellIndex)
{
    UOdysseyAnimationCell* cell = mAnimationLayer->GetCells()[iCellIndex];
    return SNew(SBox)
        .Visibility(this, &SOdysseyAnimationCells::GetTimingHandleVisibility, cell)
        .WidthOverride(mTimingHandleBrush->ImageSize.X)
        .HeightOverride(mTimingHandleBrush->ImageSize.Y)
        [
            SNew(SOdysseyAnimationCellHandle)
            .OnDragStarted(this, &SOdysseyAnimationCells::OnTimingHandleDragStarted, iCellIndex)
            .OnDragged(this, &SOdysseyAnimationCells::OnTimingHandleDragged)
            .OnDragStopped(this, &SOdysseyAnimationCells::OnTimingHandleDragStopped)
            [
                SNew(SImage)
                .Image(mTimingHandleBrush)
            ]
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationCells::CreateExposureHandleWidget(int iCellIndex)
{
    UOdysseyAnimationCell* cell = mAnimationLayer->GetCells()[iCellIndex];
    return SNew(SBox)
        .Visibility(this, &SOdysseyAnimationCells::GetExposureHandleVisibility, cell)
        .WidthOverride(mExposureHandleBrush->ImageSize.X)
        .HeightOverride(mExposureHandleBrush->ImageSize.Y)
        [
            SNew(SOdysseyAnimationCellHandle)
            .OnDragStarted(this, &SOdysseyAnimationCells::OnExposureHandleDragStarted, iCellIndex)
            .OnDragged(this, &SOdysseyAnimationCells::OnExposureHandleDragged)
            .OnDragStopped(this, &SOdysseyAnimationCells::OnExposureHandleDragStopped)
            [
                SNew(SImage)
                .Image(mExposureHandleBrush)
            ]
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationCells::CreateCellBreakIndicatorWidget(int iCellIndex)
{
    UOdysseyAnimationCell* cell = mAnimationLayer->GetCells()[iCellIndex];
    return SNew(SOdysseyAnimationTimelineSection)
		.TimelinePosition(mTimelinePosition)
        .WidthInFrames(this, &SOdysseyAnimationCells::GetCellBreakIndicatorOffset, cell)
        .Visibility(this, &SOdysseyAnimationCells::GetCellBreakIndicatorVisibility, cell)
        .HAlign(HAlign_Right)
        [
            SNew(SImage)
            .Image(mCellBreakIndicatorBrush)
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationCells::CreateAddCellsHandleRightWidget()
{
    return SNew(SOdysseyAnimationCellHandle)
        .OnDragStarted(this, &SOdysseyAnimationCells::OnAddCellsHandleDragStarted, true)
        .OnDragged(this, &SOdysseyAnimationCells::OnAddCellsHandleDragged)
        .OnDragStopped(this, &SOdysseyAnimationCells::OnAddCellsHandleDragStopped)
        [
            SNew(SBox)
            .Visibility(this, &SOdysseyAnimationCells::GetAddCellsHandleRightVisibility)
            .WidthOverride(mAddCellsHandleRightBrush->ImageSize.X)
            .HeightOverride(mAddCellsHandleRightBrush->ImageSize.Y)
            [
                SNew(SImage)
                .Image(mAddCellsHandleRightBrush)
            ]
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationCells::CreateAddCellsHandleLeftWidget()
{
    return SNew(SBox) //Box for alignment
        .HAlign(HAlign_Right)
        .VAlign(VAlign_Bottom)
        [
            SNew(SOdysseyAnimationCellHandle)
            .OnDragStarted(this, &SOdysseyAnimationCells::OnAddCellsHandleDragStarted, false)
            .OnDragged(this, &SOdysseyAnimationCells::OnAddCellsHandleDragged)
            .OnDragStopped(this, &SOdysseyAnimationCells::OnAddCellsHandleDragStopped)
            [
                //Box for size
                SNew(SBox)
                .Visibility(this, &SOdysseyAnimationCells::GetAddCellsHandleLeftVisibility)
                .WidthOverride(mAddCellsHandleLeftBrush->ImageSize.X)
                .HeightOverride(mAddCellsHandleLeftBrush->ImageSize.Y)
                [
                    SNew(SImage)
                    .Image(mAddCellsHandleLeftBrush)
                ]
            ]
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationCells::CreateCellWidget(int iCellIndex)
{
    UOdysseyAnimationCell* cell = mAnimationLayer->GetCells()[iCellIndex];

    TSharedPtr<SWidget> cellWidget;
    if (!mOnCreateCellWidget.IsBound())
        cellWidget = SMissingWidget::MakeMissingWidget();
    else
        cellWidget = mOnCreateCellWidget.Execute(cell);

    return SNew(SOdysseyAnimationTimelineSection)
	.TimelinePosition(mTimelinePosition)
    .WidthInFrames(this, &SOdysseyAnimationCells::GetCellExposure, cell)
    [
        SNew(SBorder)
        .BorderImage(FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameBackground"))
        .BorderBackgroundColor(FLinearColor(1.f, 1.f, 1.f))
        .Visibility(this, &SOdysseyAnimationCells::GetCellVisibility, cell)
        [
			SNew(SOdysseyAnimationCell, mAnimationLayer, cell)
			.TimelineCellSelection(mTimelineCellSelection)
			[
				cellWidget.ToSharedRef()
			]
        ]
    ];
}

bool
SOdysseyAnimationCells::SupportsKeyboardFocus() const
{
    return true;
}

void
SOdysseyAnimationCells::OnCellsChanged()
{
	mNeedsCellsRefresh = true;
}

FReply
SOdysseyAnimationCells::OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        SharedThis(this),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::CellsTimeline,
        mAnimationLayer
    };

	mTool = FOdysseyAnimationTimelineTools::Get().CreateTool(mTimelinePosition.ToSharedRef(), mTimelineCellSelection.ToSharedRef());
	if (!mTool)
		return FReply::Unhandled();
    
    return mTool->OnMouseButtonDown(params);
}

void
SOdysseyAnimationCells::UpdateHandlesVisibility()
{
    mHoveredCell = nullptr;
    mTimingHandleCells.Empty();

	if (mAnimationLayer->GetCells().IsEmpty())
		return;

    //Find frame
    float frame = mTimelinePosition->MousePositionToFrame(mMousePosition.X);

    //Find Cell
	UOdysseyAnimationCell* cell = mAnimationLayer->GetCellAtFrame(frame);
    if (!cell)
    {
        if (frame < mAnimationLayer->CellsOffset)
        {
            mTimingHandleCells.Add(mAnimationLayer->GetCells()[0]);
        }
    }
    else
    {
    	int cellIndex = cell->IndexInLayer;
        mHoveredCell = mAnimationLayer->GetCells()[cellIndex];
        mTimingHandleCells.Add(mAnimationLayer->GetCells()[cellIndex]);
        if (cellIndex < mAnimationLayer->GetCells().Num() - 1)
        {
            mTimingHandleCells.Add(mAnimationLayer->GetCells()[cellIndex + 1]);
        }
    }
}

FReply
SOdysseyAnimationCells::OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    mMousePosition = iGeometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition());

    UpdateHandlesVisibility();
    
	if (!mTool)
		return FReply::Unhandled();

    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        SharedThis(this),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::CellsTimeline,
        mAnimationLayer
    };
	return mTool->OnMouseMove(params);
}

FReply
SOdysseyAnimationCells::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{   
	if (!mTool)
		return FReply::Unhandled();

    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        SharedThis(this),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::CellsTimeline,
        mAnimationLayer
    };
	return mTool->OnMouseButtonUp(params);
}

FReply
SOdysseyAnimationCells::OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
	if (!mTool)
		return FReply::Unhandled();

    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        SharedThis(this),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::CellsTimeline,
        mAnimationLayer
    };
	
	return mTool->OnDragDetected(params);
}

float
SOdysseyAnimationCells::GetCellBreakIndicatorOffset(UOdysseyAnimationCell* iCell) const
{
    if (FOdysseyAnimationTimelineTools::Get().GetCurrentTool() != EOdysseyTimelineTool::Cut)
        return 0.f;

    if (GetCellBreakIndicatorVisibility(iCell) != EVisibility::Visible)
        return 0.f;
    
    //Find frame
    float frame = mTimelinePosition->MousePositionToFrame(mMousePosition.X) + 0.5f;

	UOdysseyAnimationCell* cell = mAnimationLayer->GetCellAtFrame(frame);
	if (!cell)
		return 0.f;

	return (int)frame - cell->GetFrameRange().GetLowerBoundValue();
}

EVisibility
SOdysseyAnimationCells::GetCellBreakIndicatorVisibility(UOdysseyAnimationCell* iCell) const
{
    if (mAnimationLayer->IsLockedRecursively())
        return EVisibility::Hidden;
        
    if (FOdysseyAnimationTimelineTools::Get().GetCurrentTool() != EOdysseyTimelineTool::Cut)
        return EVisibility::Hidden;

    if (!IsHovered() || iCell != mHoveredCell)
        return EVisibility::Hidden;

    //Find frame
    float frame = mTimelinePosition->MousePositionToFrame(mMousePosition.X) + 0.5f;	

	UOdysseyAnimationCell* cell = mAnimationLayer->GetCellAtFrame(frame);
	if (!cell)
		return EVisibility::Hidden;

	if (frame == cell->GetFrameRange().GetLowerBoundValue())
		return EVisibility::Hidden;
    
    return EVisibility::Visible;
}

EVisibility
SOdysseyAnimationCells::GetTimingHandleVisibility(UOdysseyAnimationCell* iCell) const
{
    if (mAnimationLayer->IsLockedRecursively())
        return EVisibility::Hidden;

    if (FOdysseyAnimationTimelineTools::Get().GetCurrentTool() != EOdysseyTimelineTool::Selection)
        return EVisibility::Hidden;

    if ((!IsHovered() && !mLockHandlesVisibility) || !mTimingHandleCells.Contains(iCell))
        return EVisibility::Hidden;
    
    return EVisibility::Visible;
}

EVisibility
SOdysseyAnimationCells::GetExposureHandleVisibility(UOdysseyAnimationCell* iCell) const
{
    if (mAnimationLayer->IsLockedRecursively())
        return EVisibility::Hidden;
        
    if (FOdysseyAnimationTimelineTools::Get().GetCurrentTool() != EOdysseyTimelineTool::Selection)
        return EVisibility::Hidden;

    if ((!IsHovered() && !mLockHandlesVisibility) || iCell != mHoveredCell)
        return EVisibility::Hidden;
    
    return EVisibility::Visible;
}

EVisibility
SOdysseyAnimationCells::GetCellVisibility(UOdysseyAnimationCell* iCell) const
{
    if ( iCell->Exposure > 0 )
        return EVisibility::Visible;

    return EVisibility::Collapsed;
}

void
SOdysseyAnimationCells::OnExposureHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, int iCellIndex)
{
	#ifdef WITH_EDITOR
		GEditor->BeginTransaction(LOCTEXT("timeline.cells.transaction.set-cell-exposure", "Change Cell Exposure"));
	#endif
    mLockHandlesVisibility = true;

	UOdysseyAnimationCell* cell = mAnimationLayer->GetCells()[iCellIndex];

    mExposureHandleDragData.mCellIndex = iCellIndex;
	mExposureHandleDragData.mInitialExposure = cell->Exposure;
    mExposureHandleDragData.mMousePosition = iEvent.GetScreenSpacePosition().X;
}

void
SOdysseyAnimationCells::OnExposureHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    const int minExposure = 1;
    float mouseOffset = iEvent.GetScreenSpacePosition().X - mExposureHandleDragData.mMousePosition;
    int mouseOffsetInt = 0;
    if ( mouseOffset > 0 )
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() + 0.5f);
    else
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() - 0.5f);

    UOdysseyAnimationCell* cell = mAnimationLayer->GetCells()[mExposureHandleDragData.mCellIndex];
    int exposure = FMath::Max(1, mExposureHandleDragData.mInitialExposure + mouseOffsetInt);
	FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), exposure, EPropertyChangeType::Interactive);
}

void
SOdysseyAnimationCells::OnExposureHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    mLockHandlesVisibility = false;
	UOdysseyAnimationCell* cell = mAnimationLayer->GetCells()[mExposureHandleDragData.mCellIndex];
	FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), cell->Exposure, EPropertyChangeType::ValueSet);
#ifdef WITH_EDITOR
	GEditor->EndTransaction();
#endif
}

void
SOdysseyAnimationCells::OnTimingHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, int iCellIndex)
{
	#ifdef WITH_EDITOR
		GEditor->BeginTransaction(LOCTEXT("timeline.cells.transaction.set-cell-timing", "Change Cell Timing"));
	#endif
    mLockHandlesVisibility = true;
    
	UOdysseyAnimationCell* cell = mAnimationLayer->GetCells()[iCellIndex];
    FInt32Range cellRange = cell->GetFrameRange();
    FInt32Range layerRange = mAnimationLayer->GetFrameRange();

    mTimingHandleDragData.mCellIndex = iCellIndex;
    mTimingHandleDragData.mMinOffset = -cellRange.GetLowerBoundValue();
    mTimingHandleDragData.mHasMaxOffset = iCellIndex == 0;
    mTimingHandleDragData.mMaxOffset = layerRange.GetUpperBoundValue() - cellRange.GetLowerBoundValue();
    mTimingHandleDragData.mMousePosition = iEvent.GetScreenSpacePosition().X;
	mTimingHandleDragData.mInitialOffset = mAnimationLayer->CellsOffset;
}

void
SOdysseyAnimationCells::OnTimingHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(SOdysseyAnimationCells::OnTimingHandleDragged);
	
	for (auto element : mTimingHandleDragData.mAffectedCells)
	{
		UOdysseyAnimationCell* affectedCell = element.Key;
		int exposure = element.Value;
		affectedCell->Exposure = exposure; //don't use SetPropertyValue to avoid refreshing rendering while dragging (slow)
	}

	FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), mTimingHandleDragData.mInitialOffset, EPropertyChangeType::Interactive );

    //Compute Mouse Offset
    float mouseOffset = iEvent.GetScreenSpacePosition().X - mTimingHandleDragData.mMousePosition;
    int mouseOffsetInt = 0;
    if ( mouseOffset > 0 )
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() + 0.5f);
    else
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() - 0.5f);

    mouseOffsetInt = FMath::Max(mouseOffsetInt, mTimingHandleDragData.mMinOffset);

    if ( mTimingHandleDragData.mHasMaxOffset )
        mouseOffsetInt = FMath::Min(mouseOffsetInt, mTimingHandleDragData.mMaxOffset);

    UOdysseyAnimationCell* cell = mAnimationLayer->GetCells()[mTimingHandleDragData.mCellIndex];

    //Compute what needs to change
    if ( mouseOffsetInt > 0 )
    {
        // When dragging to the right, the previous cell exposure is always edited if it exists
        // It cannot be removed
        int previousCellIndex = mTimingHandleDragData.mCellIndex - 1;
        if ( previousCellIndex >= 0 )
        {
            UOdysseyAnimationCell* previousCell = mAnimationLayer->GetCells()[previousCellIndex];
			
			if (!mTimingHandleDragData.mAffectedCells.Contains(previousCell))
				mTimingHandleDragData.mAffectedCells.Add(previousCell, previousCell->Exposure);

            int exposure = previousCell->Exposure + mouseOffsetInt;
			FOdysseyObjectEditorUtils::SetPropertyValue(previousCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), exposure, EPropertyChangeType::Interactive );
        }
        else //If no previous cell exists, we need to edit the layer's offset value
        {
			FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), mAnimationLayer->CellsOffset + mouseOffsetInt, EPropertyChangeType::Interactive );
        }

        //for each cell, adjust cell exposure or hide it
        for ( int i = mTimingHandleDragData.mCellIndex; i < mAnimationLayer->GetCells().Num() && mouseOffsetInt > 0; i++ )
        {
            UOdysseyAnimationCell* cellToAdjust = mAnimationLayer->GetCells()[i];

			if (!mTimingHandleDragData.mAffectedCells.Contains(cellToAdjust))
				mTimingHandleDragData.mAffectedCells.Add(cellToAdjust, cellToAdjust->Exposure);

            int exposureToRemove = FMath::Min(cellToAdjust->Exposure, mouseOffsetInt);
            int exposure = cellToAdjust->Exposure - exposureToRemove;

			FOdysseyObjectEditorUtils::SetPropertyValue(cellToAdjust, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), exposure, EPropertyChangeType::Interactive );
            mouseOffsetInt -= exposureToRemove;
        }
    }
    else if ( mouseOffsetInt < 0 )
    {
        // When dragging to the left, the current cell exposure is always edited
        // It cannot be removed
        {
			TRACE_CPUPROFILER_EVENT_SCOPE(SOdysseyAnimationCells::OnTimingHandleDragged::SetCurrentCellExposure);

			if (!mTimingHandleDragData.mAffectedCells.Contains(cell))
				mTimingHandleDragData.mAffectedCells.Add(cell, cell->Exposure);

            int exposure = cell->Exposure - mouseOffsetInt;
			FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), exposure, EPropertyChangeType::Interactive );
        }

        //for each cell adjust its exposure or hide it 
        for ( int i = mTimingHandleDragData.mCellIndex - 1; i >= 0 && mouseOffsetInt < 0; i-- )
        {
			TRACE_CPUPROFILER_EVENT_SCOPE(SOdysseyAnimationCells::OnTimingHandleDragged::SetCellExposure);
            UOdysseyAnimationCell* cellToAdjust = mAnimationLayer->GetCells()[i];
			
			if (!mTimingHandleDragData.mAffectedCells.Contains(cellToAdjust))
				mTimingHandleDragData.mAffectedCells.Add(cellToAdjust, cellToAdjust->Exposure);

            int exposureToRemove = FMath::Min(cellToAdjust->Exposure, -mouseOffsetInt);
            int exposure = cellToAdjust->Exposure - exposureToRemove;
            
			FOdysseyObjectEditorUtils::SetPropertyValue(cellToAdjust, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), exposure, EPropertyChangeType::Interactive );
            
			mouseOffsetInt += exposureToRemove;
        }

        //If the edited cell is the first cell, we also need to adjust the layer offset value
        //if we dragged further than the first cell adjust layer's offset 
        if ( mouseOffsetInt < 0 )
        {
			TRACE_CPUPROFILER_EVENT_SCOPE(SOdysseyAnimationCells::OnTimingHandleDragged::SetCellsOffset);
			FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), mAnimationLayer->CellsOffset + mouseOffsetInt, EPropertyChangeType::Interactive );
        }
    }
}

void
SOdysseyAnimationCells::OnTimingHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
	TArray<UOdysseyAnimationCell*> cellsToRemove;
	for (auto element : mTimingHandleDragData.mAffectedCells)
	{
		UOdysseyAnimationCell* affectedCell = element.Key;
		int exposure = element.Value;
		if (affectedCell->Exposure <= 0)
		{
			affectedCell->Exposure = exposure;
			cellsToRemove.Add(affectedCell);
		}
		else
		{
			int newExposure = affectedCell->Exposure;
			affectedCell->Exposure = exposure; //Needed here to have accurate Undos
			FOdysseyObjectEditorUtils::SetPropertyValue(affectedCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), newExposure, EPropertyChangeType::ValueSet );
		}
	}

	mAnimationLayer->RemoveCells(cellsToRemove);

	FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), mAnimationLayer->CellsOffset, EPropertyChangeType::ValueSet );

	mTimingHandleDragData.mAffectedCells.Empty();
    mLockHandlesVisibility = false;
	
#ifdef WITH_EDITOR
	GEditor->EndTransaction();
#endif
}

EVisibility
SOdysseyAnimationCells::GetAddCellsHandleRightVisibility() const
{
    if (mAnimationLayer->IsLockedRecursively())
        return EVisibility::Hidden;

    return mShowHandles.Get() ? EVisibility::Visible : EVisibility::Hidden;
}

EVisibility
SOdysseyAnimationCells::GetAddCellsHandleLeftVisibility() const
{
    if (mAnimationLayer->IsLockedRecursively())
        return EVisibility::Hidden;
        
    return mShowHandles.Get() ? EVisibility::Visible : EVisibility::Hidden;
}

void
SOdysseyAnimationCells::OnAddCellsHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, bool iIsRightHandle)
{
	#ifdef WITH_EDITOR
		GEditor->BeginTransaction(LOCTEXT("timeline.cells.transaction.add-cells-at-start", "Add Cells"));
	#endif

    mLockHandlesVisibility = true;

    FInt32Range layerRange = mAnimationLayer->GetFrameRange();
    mAddCellsHandleDragData.mIsRightHandle = iIsRightHandle;
    mAddCellsHandleDragData.mMousePosition = iEvent.GetScreenSpacePosition().X;
	mAddCellsHandleDragData.mInitialOffset = mAnimationLayer->CellsOffset;

    if (iIsRightHandle)
    {
        mAddCellsHandleDragData.mMinOffset = layerRange.GetLowerBoundValue() - layerRange.GetUpperBoundValue();
        mAddCellsHandleDragData.mHasMaxOffset = false;
    }
    else
    {
        mAddCellsHandleDragData.mMinOffset = -mAnimationLayer->CellsOffset;
        mAddCellsHandleDragData.mMaxOffset = layerRange.GetUpperBoundValue() - layerRange.GetLowerBoundValue();
        mAddCellsHandleDragData.mHasMaxOffset = true;
    }
}

void
SOdysseyAnimationCells::OnAddCellsHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
	for (auto element : mAddCellsHandleDragData.mAffectedCells)
	{
		UOdysseyAnimationCell* affectedCell = element.Key;
		int exposure = element.Value;
		affectedCell->Exposure = exposure; //don't use SetPropertyValue to avoid refreshing rendering while dragging (slow)
	}

	FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), mAddCellsHandleDragData.mInitialOffset, EPropertyChangeType::Interactive );

    mNumTempCellsToPrepend = 0;
    mNumTempCellsToAppend = 0;
    
    //Compute Mouse Offset
    float mouseOffset = iEvent.GetScreenSpacePosition().X - mAddCellsHandleDragData.mMousePosition;
    int mouseOffsetInt = 0;
    if ( mouseOffset > 0 )
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() + 0.5f);
    else
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() - 0.5f);

    mouseOffsetInt = FMath::Max(mouseOffsetInt, mAddCellsHandleDragData.mMinOffset);

    if (mAddCellsHandleDragData.mHasMaxOffset)
        mouseOffsetInt = FMath::Min(mouseOffsetInt, mAddCellsHandleDragData.mMaxOffset);

    if (mAddCellsHandleDragData.mIsRightHandle)
    {
        if ( mouseOffsetInt > 0 )
        {
            mNumTempCellsToAppend = mouseOffsetInt;
        }
        else
        {
            //for each cell adjust its exposure or hide it 
            for ( int i = mAnimationLayer->GetCells().Num() - 1; i >= 0 && mouseOffsetInt < 0; i-- )
            {
                UOdysseyAnimationCell* cellToAdjust = mAnimationLayer->GetCells()[i];
				
				if (!mAddCellsHandleDragData.mAffectedCells.Contains(cellToAdjust))
					mAddCellsHandleDragData.mAffectedCells.Add(cellToAdjust, cellToAdjust->Exposure);

                int exposureToRemove = FMath::Min(cellToAdjust->Exposure, -mouseOffsetInt);
                int exposure = cellToAdjust->Exposure - exposureToRemove;
                
				FOdysseyObjectEditorUtils::SetPropertyValue(cellToAdjust, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), exposure, EPropertyChangeType::Interactive );

                mouseOffsetInt += exposureToRemove;
            }
        }
    }
    else
    {
		FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), mAnimationLayer->CellsOffset + mouseOffsetInt, EPropertyChangeType::Interactive );

        if ( mouseOffsetInt < 0 )
        {
            mNumTempCellsToPrepend = -mouseOffsetInt;
        }
        else
        {
            //for each cell adjust its exposure or hide it 
            for ( int i = 0; i < mAnimationLayer->GetCells().Num() && mouseOffsetInt > 0; i++ )
            {
                UOdysseyAnimationCell* cellToAdjust = mAnimationLayer->GetCells()[i];
				
				if (!mAddCellsHandleDragData.mAffectedCells.Contains(cellToAdjust))
					mAddCellsHandleDragData.mAffectedCells.Add(cellToAdjust, cellToAdjust->Exposure);

                int exposureToRemove = FMath::Min(cellToAdjust->Exposure, mouseOffsetInt);
                int exposure = cellToAdjust->Exposure - exposureToRemove;
                
				FOdysseyObjectEditorUtils::SetPropertyValue(cellToAdjust, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), exposure, EPropertyChangeType::Interactive );
				
                mouseOffsetInt -= exposureToRemove;
            }
        }
    }

    RefreshTempCells();
}

void
SOdysseyAnimationCells::OnAddCellsHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
	TArray<UOdysseyAnimationCell*> cellsToRemove;
	for (auto element : mAddCellsHandleDragData.mAffectedCells)
	{
		UOdysseyAnimationCell* affectedCell = element.Key;
		int exposure = element.Value;
		if (affectedCell->Exposure <= 0)
		{
			affectedCell->Exposure = exposure;
			cellsToRemove.Add(affectedCell);
		}
		else
		{
			int newExposure = affectedCell->Exposure;
			affectedCell->Exposure = exposure; //Needed here to have accurate Undos
			FOdysseyObjectEditorUtils::SetPropertyValue(affectedCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), newExposure, EPropertyChangeType::ValueSet );
		}
	}

	mAnimationLayer->RemoveCells(cellsToRemove);

	FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), mAnimationLayer->CellsOffset, EPropertyChangeType::ValueSet );

	mAddCellsHandleDragData.mAffectedCells.Empty();

    if (mNumTempCellsToPrepend > 0)
		mAnimationLayer->AddCells(mAnimationLayer->DefaultCellClass, 0, mNumTempCellsToPrepend);

    if (mNumTempCellsToAppend > 0)
        mAnimationLayer->AddCells(mAnimationLayer->DefaultCellClass, mAnimationLayer->GetCells().Num(), mNumTempCellsToAppend);

    mNumTempCellsToPrepend = 0;
    mNumTempCellsToAppend = 0;
    mLockHandlesVisibility = false;

#ifdef WITH_EDITOR
	GEditor->EndTransaction();
#endif

    RefreshTempCells();
}

void
SOdysseyAnimationCells::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
	if (mNeedsCellsRefresh)
	{
		mNeedsCellsRefresh = false;
		RefreshCells();
	}
}

TSharedRef<SWidget>
SOdysseyAnimationCells::GetPreBehaviourMenuContent()
{
    TSharedRef<FUICommandList> commandList = MakeShared<FUICommandList>();
    FMenuBuilder menuBuilder(true, commandList);

	menuBuilder.AddMenuEntry(
		LOCTEXT("animation.layer.prebehaviour-menu.none", "None"),
		TAttribute<FText>(),
		FSlateIcon("OdysseyStyle", "Animation.Layer.PreBehaviour.None"),
		FUIAction(
			FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::None),
			FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::None),
			FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::None)
		),
		NAME_None,
		EUserInterfaceActionType::RadioButton
	);

	menuBuilder.AddMenuEntry(
		LOCTEXT("animation.layer.prebehaviour-menu.hold", "Hold"),
		TAttribute<FText>(),
		FSlateIcon("OdysseyStyle", "Animation.Layer.PreBehaviour.Hold"),
		FUIAction(
			FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Hold),
			FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Hold),
			FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Hold)
		),
		NAME_None,
		EUserInterfaceActionType::RadioButton
	);

	menuBuilder.AddMenuEntry(
		LOCTEXT("animation.layer.prebehaviour-menu.loop", "Loop"),
		TAttribute<FText>(),
		FSlateIcon("OdysseyStyle", "Animation.Layer.PreBehaviour.Loop"),
		FUIAction(
			FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Loop),
			FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Loop),
			FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Loop)
		),
		NAME_None,
		EUserInterfaceActionType::RadioButton
	);

	menuBuilder.AddMenuEntry(
		LOCTEXT("animation.layer.prebehaviour-menu.pingpong", "PingPong"),
		TAttribute<FText>(),
		FSlateIcon("OdysseyStyle", "Animation.Layer.PreBehaviour.PingPong"),
		FUIAction(
			FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::PingPong),
			FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::PingPong),
			FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::PingPong)
		),
		NAME_None,
		EUserInterfaceActionType::RadioButton
	);
	
    return menuBuilder.MakeWidget();
}

const FSlateBrush*
SOdysseyAnimationCells::GetPreBehaviourBrush() const
{
    switch(mAnimationLayer->PreBehaviour)
    {
        case EOdysseyAnimationLayerImagePostBehaviour::None:
            return FOdysseyStyle::GetBrush("Animation.Layer.PreBehaviour.None");
        break;
		case EOdysseyAnimationLayerImagePostBehaviour::Hold:
            return FOdysseyStyle::GetBrush("Animation.Layer.PreBehaviour.Hold");
        break;
		case EOdysseyAnimationLayerImagePostBehaviour::Loop:
            return FOdysseyStyle::GetBrush("Animation.Layer.PreBehaviour.Loop");
        break;
		case EOdysseyAnimationLayerImagePostBehaviour::PingPong:
            return FOdysseyStyle::GetBrush("Animation.Layer.PreBehaviour.PingPong");
        break;
    }
    return nullptr;
}

const FSlateBrush*
SOdysseyAnimationCells::GetPostBehaviourBrush() const
{
    switch(mAnimationLayer->PostBehaviour)
    {
        case EOdysseyAnimationLayerImagePostBehaviour::None:
            return FOdysseyStyle::GetBrush("Animation.Layer.PostBehaviour.None");
        break;
		case EOdysseyAnimationLayerImagePostBehaviour::Hold:
            return FOdysseyStyle::GetBrush("Animation.Layer.PostBehaviour.Hold");
        break;
		case EOdysseyAnimationLayerImagePostBehaviour::Loop:
            return FOdysseyStyle::GetBrush("Animation.Layer.PostBehaviour.Loop");
        break;
		case EOdysseyAnimationLayerImagePostBehaviour::PingPong:
            return FOdysseyStyle::GetBrush("Animation.Layer.PostBehaviour.PingPong");
        break;
    }
    return nullptr;
}

TSharedRef<SWidget>
SOdysseyAnimationCells::GetPostBehaviourMenuContent()
{
    TSharedRef<FUICommandList> commandList = MakeShared<FUICommandList>();
    FMenuBuilder menuBuilder(true, commandList);

	menuBuilder.AddMenuEntry(
		LOCTEXT("animation.layer.postbehaviour-menu.none", "None"),
		TAttribute<FText>(),
		FSlateIcon("OdysseyStyle", "Animation.Layer.PostBehaviour.None"),
		FUIAction(
			FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::None),
			FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::None),
			FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::None)
		),
		NAME_None,
		EUserInterfaceActionType::RadioButton
	);

	menuBuilder.AddMenuEntry(
		LOCTEXT("animation.layer.postbehaviour-menu.hold", "Hold"),
		TAttribute<FText>(),
		FSlateIcon("OdysseyStyle", "Animation.Layer.PostBehaviour.Hold"),
		FUIAction(
			FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Hold),
			FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Hold),
			FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Hold)
		),
		NAME_None,
		EUserInterfaceActionType::RadioButton
	);

	menuBuilder.AddMenuEntry(
		LOCTEXT("animation.layer.postbehaviour-menu.loop", "Loop"),
		TAttribute<FText>(),
		FSlateIcon("OdysseyStyle", "Animation.Layer.PostBehaviour.Loop"),
		FUIAction(
			FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Loop),
			FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Loop),
			FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Loop)
		),
		NAME_None,
		EUserInterfaceActionType::RadioButton
	);

	menuBuilder.AddMenuEntry(
		LOCTEXT("animation.layer.postbehaviour-menu.pingpong", "PingPong"),
		TAttribute<FText>(),
		FSlateIcon("OdysseyStyle", "Animation.Layer.PostBehaviour.PingPong"),
		FUIAction(
			FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::PingPong),
			FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::PingPong),
			FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::PingPong)
		),
		NAME_None,
		EUserInterfaceActionType::RadioButton
	);
	
    return menuBuilder.MakeWidget();
}

void
SOdysseyAnimationCells::SetPreBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour)
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("animation.layer.transaction.set-prebehaviour", "Set Layer Pre Behaviour"));
#endif
	FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, PreBehaviour), iBehaviour);
}

bool
SOdysseyAnimationCells::CanSetPreBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour) const
{
    return !mAnimationLayer->IsLockedRecursively();
}

bool
SOdysseyAnimationCells::IsPreBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour) const
{
    return mAnimationLayer->PreBehaviour == iBehaviour;
}

void
SOdysseyAnimationCells::SetPostBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour)
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("animation.layer.transaction.set-postbehaviour", "Set Layer Post Behaviour"));
#endif
	FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, PostBehaviour), iBehaviour);
}

bool
SOdysseyAnimationCells::CanSetPostBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour) const
{
    return !mAnimationLayer->IsLockedRecursively();
}

bool
SOdysseyAnimationCells::IsPostBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour) const
{
    return mAnimationLayer->PostBehaviour == iBehaviour;
}

#undef LOCTEXT_NAMESPACE
