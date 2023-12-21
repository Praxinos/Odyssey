// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Cells/SOdysseyAnimationCells.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineSection.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineFrameSelector.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "Widgets/Layout/SMissingWidget.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineScrollBox.h"
#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
SOdysseyAnimationCells::~SOdysseyAnimationCells()
{
    mCellsContainer->OnCellsChanged().RemoveAll(this);
}

SOdysseyAnimationCells::SOdysseyAnimationCells()
    : mTimingHandleBrush(nullptr)
    , mLengthHandleBrush(nullptr)
    , mAddCellsHandleRightBrush(nullptr)
    , mAddCellsHandleLeftBrush(nullptr)
    , mCommandList(MakeShared<FUICommandList>())
    , mIsRefreshPending(false)
    , mIsRebuildPending(false)
    , mCellsMutator(nullptr)
    , mLockHandlesVisibility(false)
    , mLengthHandleCell(nullptr)
    , mTimingHandleCells()
{
}

void
SOdysseyAnimationCells::Construct(
    const FArguments& InArgs,
    FOdysseyAnimationEditorExtension* iExtension,
    UOdysseyAnimationLayer* iAnimationLayer,
    TSharedPtr<FOdysseyAnimationCellsContainer> iCellsContainer
)
{
    ensure(iAnimationLayer);

    mExtension = iExtension;
    mTimingHandleBrush = FOdysseyStyle::GetBrush("Animation.CellTimingHandle");
	mLengthHandleBrush = FOdysseyStyle::GetBrush("Animation.CellLengthHandle");
    mAddCellsHandleLeftBrush = FOdysseyStyle::GetBrush("Animation.AddCellsHandleLeft");
    mAddCellsHandleRightBrush = FOdysseyStyle::GetBrush("Animation.AddCellsHandleRight");
    mAnimationLayer = iAnimationLayer;
    mCellsContainer = iCellsContainer;

    mOnCreateCellWidget = InArgs._OnCreateCellWidget;
    mOnCreateCell = InArgs._OnCreateCell;
    mShowHandles = InArgs._ShowHandles;
    
    ChildSlot
    .Padding(FMargin(0, 4.f, 0, 4.f))
    [
        SNew(SOdysseyAnimationTimelineScrollBox, mExtension)
        + SOdysseyAnimationTimelineScrollBox::Slot()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                //Timeline Section for Layer Offset
                SNew(SOdysseyAnimationTimelineSection, mExtension)
                .WidthInFrames(this, &SOdysseyAnimationCells::GetOffset)
                .Content()
                [
                    //Add Cells Handle
                    CreateAddCellsHandleLeftWidget()
                ]
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                    SAssignNew(mCellsBox, SHorizontalBox)
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Top)
            [
                //Add Cells Handle
                CreateAddCellsHandleRightWidget()
            ]
        ]
    ];

    mCellsContainer->OnCellsChanged().AddRaw(this, &SOdysseyAnimationCells::OnCellsChanged);
    RequestRebuild();
}

void
SOdysseyAnimationCells::RequestRefresh()
{
    mIsRefreshPending = true;
}

void
SOdysseyAnimationCells::RequestRebuild()
{
    mIsRebuildPending = true;
    RequestRefresh();
}

void
SOdysseyAnimationCells::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
    if ( mIsRebuildPending )
    {
        //mEditingOffset = false;
        BuildCellsData();
        mIsRebuildPending = false;
    }
    if ( mIsRefreshPending )
    {
        RefreshWidgets();
        mIsRefreshPending = false;
    }
}

float
SOdysseyAnimationCells::GetOffset() const
{
    return mCellsContainer->GetOffset();
}

float
SOdysseyAnimationCells::GetCellLength(TSharedPtr<SOdysseyAnimationCells::FCellData> iCellData) const
{
    if ( !iCellData->mCell || iCellData->mEditingLength )
        return iCellData->mLength;

    return iCellData->mCell->GetLength();
}

TSharedPtr<SOdysseyAnimationCells::FCellData>
SOdysseyAnimationCells::InsertCellData(int iIndex, TSharedPtr<FOdysseyAnimationCell> iCell, int iCellIndex)
{
    TSharedPtr<FCellData> cellData = MakeShared<FCellData>();
    cellData->mCell = iCell;
    cellData->mCellIndex = iCellIndex;
    cellData->mIsVisible = true;
    cellData->mIsTimingHandleVisible = true;
    cellData->mIsLengthHandleVisible = true;
    cellData->mEditingLength = false;
    cellData->mLength = 1;
    mCellsData.Insert(cellData, iIndex);

    return cellData;
}

TSharedPtr<SOdysseyAnimationCells::FCellData>
SOdysseyAnimationCells::AddCellData(TSharedPtr<FOdysseyAnimationCell> iCell, int iCellIndex)
{
    TSharedPtr<FCellData> cellData = MakeShared<FCellData>();
    cellData->mCell = iCell;
    cellData->mCellIndex = iCellIndex;
    cellData->mIsVisible = true;
    cellData->mIsTimingHandleVisible = true;
    cellData->mIsLengthHandleVisible = true;
    cellData->mEditingLength = false;
    cellData->mLength = 1;
    mCellsData.Add(cellData);

    return cellData;
}

void
SOdysseyAnimationCells::RemoveCellData(int iIndex)
{
    mCellsData.RemoveAt(iIndex);
}

void
SOdysseyAnimationCells::InsertCellSection(int iIndex, TSharedPtr<FCellData> iCellData)
{
    iCellData->mCellWidget = SNew(SOdysseyAnimationTimelineSection, mExtension)
        .WidthInFrames(this, &SOdysseyAnimationCells::GetCellLength, iCellData)
        [
            SNew(SOverlay)
            + SOverlay::Slot() //Cell Widget
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Fill)
            [
                CreateCellWidget(iCellData)
            ]
            
            + SOverlay::Slot() //Timing Handle Top Left
            //.Padding(-mLengthHandleBrush->ImageSize.X / 2, 0.f, -mLengthHandleBrush->ImageSize.X / 2, 0.f)
            .Padding(0.f, 0.f, -mLengthHandleBrush->ImageSize.X, 0.f)
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Top)
            [
                CreateTimingHandleWidget(iCellData)
            ]

            + SOverlay::Slot() //Length Handle Top Right
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Bottom)
            [
                CreateLengthHandleWidget(iCellData)
            ]
        ];

    //Cells widgets
    mCellsBox->InsertSlot(iIndex)
    .AutoWidth()
    [
        iCellData->mCellWidget.ToSharedRef()
    ];
}

void
SOdysseyAnimationCells::AddCellSection(TSharedPtr<FCellData> iCellData)
{
    InsertCellSection(mCellsBox->NumSlots(), iCellData);
}

void
SOdysseyAnimationCells::RemoveCellSection(TSharedPtr<FCellData> iCellData)
{
    mCellsBox->RemoveSlot(iCellData->mCellWidget.ToSharedRef());
    //mHandlesBox->RemoveSlot(iCellData->mHandlesSectionWidget.ToSharedRef());
}

void
SOdysseyAnimationCells::RefreshWidgets()
{
    mCellsBox->ClearChildren();
    //mHandlesBox->ClearChildren();

    for ( int i = 0; i < mCellsData.Num(); i++ )
    {
        TSharedPtr<FCellData> cellData = mCellsData[i];
        AddCellSection(cellData);
    }
}

TSharedRef<SWidget>
SOdysseyAnimationCells::CreateTimingHandleWidget(TSharedPtr<FCellData> iCellData)
{
    return SNew(SBox)
        .Visibility(this, &SOdysseyAnimationCells::GetTimingHandleVisibility, iCellData)
        .WidthOverride(mTimingHandleBrush->ImageSize.X)
        .HeightOverride(mTimingHandleBrush->ImageSize.Y)
        [
            SNew(SOdysseyAnimationCellHandle)
            .OnDragStarted(this, &SOdysseyAnimationCells::OnTimingHandleDragStarted, iCellData)
            .OnDragged(this, &SOdysseyAnimationCells::OnTimingHandleDragged)
            .OnDragStopped(this, &SOdysseyAnimationCells::OnTimingHandleDragStopped)
            [
                SNew(SImage)
                .Image(mTimingHandleBrush)
            ]
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationCells::CreateLengthHandleWidget(TSharedPtr<FCellData> iCellData)
{
    return SNew(SBox)
        .Visibility(this, &SOdysseyAnimationCells::GetLengthHandleVisibility, iCellData)
        .WidthOverride(mLengthHandleBrush->ImageSize.X)
        .HeightOverride(mLengthHandleBrush->ImageSize.Y)
        [
            SNew(SOdysseyAnimationCellHandle)
            .OnDragStarted(this, &SOdysseyAnimationCells::OnLengthHandleDragStarted, iCellData)
            .OnDragged(this, &SOdysseyAnimationCells::OnLengthHandleDragged)
            .OnDragStopped(this, &SOdysseyAnimationCells::OnLengthHandleDragStopped)
            [
                SNew(SImage)
                .Image(mLengthHandleBrush)
            ]
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
SOdysseyAnimationCells::CreateCellWidget(TSharedPtr<FCellData> iCellData)
{
    TSharedPtr<SWidget> cellWidget;
    if (!mOnCreateCellWidget.IsBound())
        cellWidget = SMissingWidget::MakeMissingWidget();
    else
        cellWidget = mOnCreateCellWidget.Execute(iCellData->mCell);

    return SNew(SOdysseyAnimationTimelineSection, mExtension)
    .WidthInFrames(this, &SOdysseyAnimationCells::GetCellLength, iCellData)
    [
        SNew(SBorder)
		.BorderImage(FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameBackground"))
		.BorderBackgroundColor(FLinearColor(1.f, 1.f, 1.f))
        .Visibility(this, &SOdysseyAnimationCells::GetCellVisibility, iCellData)
        [
            cellWidget.ToSharedRef()
        ]
    ];
}

void
SOdysseyAnimationCells::BuildCellsData()
{
    mCellsData.Empty();

    const TArray<TSharedPtr<FOdysseyAnimationCell>>& cells = mCellsContainer->GetCells();
    for (int i = 0; i < cells.Num(); i++)
    {
        AddCellData(cells[i], i);
    }
}

bool
SOdysseyAnimationCells::SupportsKeyboardFocus() const
{
    return true;
}

void
SOdysseyAnimationCells::OnCellsChanged()
{
    RequestRebuild();
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
    
    return mExtension->Timeline()->GetTool()->OnMouseButtonDown(params);
}

void
SOdysseyAnimationCells::UpdateHandlesVisibility()
{
    mLengthHandleCell = nullptr;
    mTimingHandleCells.Empty();

    //Find frame
    int timelineOffset = mExtension->Timeline()->GetOffset();
    float frameWidth = mExtension->Timeline()->GetFrameWidth();
    float frame = mMousePosition.X / frameWidth + timelineOffset;

    //Find Cell
    int cellIndex = mCellsContainer->GetCellIndexAtFrame(frame);
    if (cellIndex == INDEX_NONE)
    {
        if (frame < mCellsContainer->GetOffset())
        {
            mTimingHandleCells.Add(mCellsContainer->GetCells()[0]);
        }
    }
    else
    {
        mLengthHandleCell = mCellsContainer->GetCells()[cellIndex];
        mTimingHandleCells.Add(mCellsContainer->GetCells()[cellIndex]);
        if (cellIndex < mCellsContainer->GetCells().Num() - 1)
        {
            mTimingHandleCells.Add(mCellsContainer->GetCells()[cellIndex + 1]);
        }
    }
}

FReply
SOdysseyAnimationCells::OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    mMousePosition = iGeometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition());

    UpdateHandlesVisibility();
    
    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        SharedThis(this),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::CellsTimeline,
        mAnimationLayer
    };
    return mExtension->Timeline()->GetTool()->OnMouseMove(params);
}

FReply
SOdysseyAnimationCells::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{   
    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        SharedThis(this),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::CellsTimeline,
        mAnimationLayer
    };
    return mExtension->Timeline()->GetTool()->OnMouseButtonUp(params);
}

FReply
SOdysseyAnimationCells::OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        SharedThis(this),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::CellsTimeline,
        mAnimationLayer
    };
  	return mExtension->Timeline()->GetTool()->OnDragDetected(params);
}

EVisibility
SOdysseyAnimationCells::GetTimingHandleVisibility(TSharedPtr<FCellData> iCellData) const
{
    if ((IsHovered() || mLockHandlesVisibility) && iCellData->mCell == mLengthHandleCell)
        return EVisibility::Visible;
    
    return EVisibility::Hidden;
}

EVisibility
SOdysseyAnimationCells::GetLengthHandleVisibility(TSharedPtr<FCellData> iCellData) const
{
    if ((IsHovered() || mLockHandlesVisibility) && iCellData->mCell == mLengthHandleCell)
        return EVisibility::Visible;
    
    return EVisibility::Hidden;
}

EVisibility
SOdysseyAnimationCells::GetCellVisibility(TSharedPtr<FCellData> iCellData) const
{
    return iCellData->mCell->GetLength() > 0 ? EVisibility::Visible : EVisibility::Collapsed;
}

void
SOdysseyAnimationCells::OnLengthHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, TSharedPtr<FCellData> iCellData)
{
    mCellsMutator = MakeShared<FOdysseyAnimationCellsMutator>(mAnimationLayer, mCellsContainer.ToSharedRef());
    mLockHandlesVisibility = true;

    //iCellData->mEditingLength = true;
    mLengthHandleDragData.mCellData = iCellData;
    mLengthHandleDragData.mMousePosition = iEvent.GetScreenSpacePosition().X;
    //iCellData->mLength = iCellData->mCell->GetLength();
}

void
SOdysseyAnimationCells::OnLengthHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    mCellsMutator->Revert();

    const int minLength = 1;
    float mouseOffset = iEvent.GetScreenSpacePosition().X - mLengthHandleDragData.mMousePosition;
    int mouseOffsetInt = 0;
    if ( mouseOffset > 0 )
        mouseOffsetInt = (int)(mouseOffset / mExtension->Timeline()->GetFrameWidth() + 0.5f);
    else
        mouseOffsetInt = (int)(mouseOffset / mExtension->Timeline()->GetFrameWidth() - 0.5f);

    int length = mLengthHandleDragData.mCellData->mCell->GetLength() + mouseOffsetInt;
    if (length != mLengthHandleDragData.mCellData->mCell->GetLength())
        mCellsMutator->SetLength(mLengthHandleDragData.mCellData->mCellIndex, FMath::Max(minLength, length));
}

void
SOdysseyAnimationCells::OnLengthHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.cells.transaction.set-cell-length", "Change Cell Length"));
#endif
    mCellsMutator->Commit();
    mCellsMutator = nullptr;
    
    mLengthHandleDragData.mCellData = nullptr;

    mLockHandlesVisibility = false;
}

void
SOdysseyAnimationCells::OnTimingHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, TSharedPtr<FCellData> iCellData)
{
    mCellsMutator = MakeShared<FOdysseyAnimationCellsMutator>(mAnimationLayer, mCellsContainer.ToSharedRef());
    mLockHandlesVisibility = true;
    
    FInt32Range cellRange = mCellsContainer->GetCellFrameRange(iCellData->mCellIndex);
    FInt32Range layerRange = mCellsContainer->GetFrameRange();

    mTimingHandleDragData.mMinOffset = -cellRange.GetLowerBoundValue();
    mTimingHandleDragData.mHasMaxOffset = iCellData->mCellIndex == 0;
    mTimingHandleDragData.mMaxOffset = layerRange.GetUpperBoundValue() - cellRange.GetLowerBoundValue();
    mTimingHandleDragData.mCellData = iCellData;
    mTimingHandleDragData.mMousePosition = iEvent.GetScreenSpacePosition().X;
}

void
SOdysseyAnimationCells::OnTimingHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    mCellsMutator->Revert();

    //Compute Mouse Offset
    float mouseOffset = iEvent.GetScreenSpacePosition().X - mTimingHandleDragData.mMousePosition;
    int mouseOffsetInt = 0;
    if ( mouseOffset > 0 )
        mouseOffsetInt = (int)(mouseOffset / mExtension->Timeline()->GetFrameWidth() + 0.5f);
    else
        mouseOffsetInt = (int)(mouseOffset / mExtension->Timeline()->GetFrameWidth() - 0.5f);

    mouseOffsetInt = FMath::Max(mouseOffsetInt, mTimingHandleDragData.mMinOffset);

    if ( mTimingHandleDragData.mHasMaxOffset )
        mouseOffsetInt = FMath::Min(mouseOffsetInt, mTimingHandleDragData.mMaxOffset);

    //Compute what needs to change
    if ( mouseOffsetInt > 0 )
    {
        // When dragging to the right, the previous cell length is always edited if it exists
        // It cannot be removed
        int previousCellIndex = mTimingHandleDragData.mCellData->mCellIndex - 1;
        if ( previousCellIndex >= 0 )
        {
            TSharedPtr<FCellData> previousCellData = mCellsData[previousCellIndex];
            int length = previousCellData->mCell->GetLength() + mouseOffsetInt;
            mCellsMutator->SetLength(previousCellIndex, length);
        }
        else //If no previous cell exists, we need to edit the layer's offset value
        {
            mCellsMutator->SetOffset(mCellsContainer->GetOffset() + mouseOffsetInt);
        }

        //for each cell, adjust cell length or hide it
        for ( int i = mTimingHandleDragData.mCellData->mCellIndex; i < mCellsData.Num() && mouseOffsetInt > 0; i++ )
        {
            TSharedPtr<FCellData> cellData = mCellsData[i];
            int lengthToRemove = FMath::Min(cellData->mCell->GetLength(), mouseOffsetInt);
            int length = cellData->mCell->GetLength() - lengthToRemove;
            mCellsMutator->SetLength(cellData->mCellIndex, length);
            mouseOffsetInt -= lengthToRemove;
        }
    }
    else if ( mouseOffsetInt < 0 )
    {
        // When dragging to the left, the current cell length is always edited
        // It cannot be removed
        {
            int length = mTimingHandleDragData.mCellData->mCell->GetLength() - mouseOffsetInt;
            mCellsMutator->SetLength(mTimingHandleDragData.mCellData->mCellIndex, length);
        }

        //for each cell adjust its length or hide it 
        for ( int i = mTimingHandleDragData.mCellData->mCellIndex - 1; i >= 0 && mouseOffsetInt < 0; i-- )
        {
            TSharedPtr<FCellData> cellData = mCellsData[i];
            int lengthToRemove = FMath::Min(cellData->mCell->GetLength(), -mouseOffsetInt);
            int length = cellData->mCell->GetLength() - lengthToRemove;
            mCellsMutator->SetLength(cellData->mCellIndex, length);
            mouseOffsetInt += lengthToRemove;
        }

        //If the edited cell is the first cell, we also need to adjust the layer offset value
        //if we dragged further than the first cell adjust layer's offset 
        if ( mouseOffsetInt < 0 )
        {
            mCellsMutator->SetOffset(mCellsContainer->GetOffset() + mouseOffsetInt);
        }
    }
}

void
SOdysseyAnimationCells::OnTimingHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
#ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("timeline.cells.transaction.set-cell-timing", "Change Cell Timing"));
#endif
    mCellsMutator->Commit();
    mCellsMutator = nullptr;
    
    mTimingHandleDragData.mCellData = nullptr;
    mLockHandlesVisibility = false;
}

EVisibility
SOdysseyAnimationCells::GetAddCellsHandleRightVisibility() const
{
    bool isZoomedEnough = mExtension->Timeline()->GetFrameWidth() > mAddCellsHandleRightBrush->ImageSize.X;
    return (mShowHandles.Get() && isZoomedEnough) ? EVisibility::Visible : EVisibility::Hidden;
}

EVisibility
SOdysseyAnimationCells::GetAddCellsHandleLeftVisibility() const
{
    bool isZoomedEnough = mExtension->Timeline()->GetFrameWidth() > mAddCellsHandleLeftBrush->ImageSize.X;
    return (mShowHandles.Get() && isZoomedEnough && GetOffset() > 0) ? EVisibility::Visible : EVisibility::Hidden;
}

void
SOdysseyAnimationCells::OnAddCellsHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, bool iIsRightHandle)
{
    mCellsMutator = MakeShared<FOdysseyAnimationCellsMutator>(mAnimationLayer, mCellsContainer.ToSharedRef());
    mLockHandlesVisibility = true;

    FInt32Range layerRange = mCellsContainer->GetFrameRange();
    mAddCellsHandleDragData.mIsRightHandle = iIsRightHandle;
    mAddCellsHandleDragData.mMousePosition = iEvent.GetScreenSpacePosition().X;

    if (iIsRightHandle)
    {
        mAddCellsHandleDragData.mMinOffset = layerRange.GetLowerBoundValue() - layerRange.GetUpperBoundValue();
        mAddCellsHandleDragData.mHasMaxOffset = false;
    }
    else
    {
        mAddCellsHandleDragData.mMinOffset = -mCellsContainer->GetOffset();
        mAddCellsHandleDragData.mMaxOffset = layerRange.GetUpperBoundValue() - layerRange.GetLowerBoundValue();
        mAddCellsHandleDragData.mHasMaxOffset = true;
    }
}

void
SOdysseyAnimationCells::OnAddCellsHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    mCellsMutator->Revert();
    
    //Compute Mouse Offset
    /* float mouseOffset = iEvent.GetScreenSpacePosition().X - mAddCellsHandleDragData.mMousePosition;
    int mouseOffsetInt = 0;
    if ( mouseOffset > 0 )
        mouseOffsetInt = (int)(mouseOffset / mExtension->Timeline()->GetFrameWidth() + 0.5f);
    else
        mouseOffsetInt = (int)(mouseOffset / mExtension->Timeline()->GetFrameWidth() - 0.5f);

    mouseOffsetInt = FMath::Max(mouseOffsetInt, mAddCellsHandleDragData.mMinOffset);

    if (mAddCellsHandleDragData.mHasMaxOffset)
        mouseOffsetInt = FMath::Min(mouseOffsetInt, mAddCellsHandleDragData.mMaxOffset);

    if (mAddCellsHandleDragData.mIsRightHandle)
    {
        if ( mouseOffsetInt > 0 )
        {
            for ( int i = 0; i < mouseOffsetInt; i++ )
            {
                TSharedPtr<FOdysseyAnimationCell> cell = MakeShared<FOdysseyAnimationCell>(); //TEMP Cell
                //TODO: we must create the right type of cell immeditely(raster / stagger / vector)
                //      this will ensure the undo will work as expected
                //TODO: store added cells to know which one must be Initialized
                //TODO: Add a cell section to render the new cell

                //TSharedPtr<FCellData> cellData = AddCellData(nullptr, 0); //0 is ok as it is a temporary cell
                //AddCellSection(cellData);
            }
        }
        else
        {
            //for each cell adjust its length or hide it 
            for ( int i = mCellsData.Num() - 1; i >= 0 && mouseOffsetInt < 0; i-- )
            {
                TSharedPtr<FCellData> cellData = mCellsData[i];
                int length = cellData->mCell->GetLength();
                int lengthToRemove = FMath::Min(length, -mouseOffsetInt);

                cellData->mEditingLength = true;
                cellData->mLength = cellData->mCell->GetLength() - lengthToRemove;
                if ( cellData->mLength <= 0 )
                {
                    //hide the cell and cell's handles if needed
                    cellData->mIsVisible = false;
                    cellData->mIsTimingHandleVisible = false;
                    cellData->mIsLengthHandleVisible = false;
                    mAddCellsHandleDragData.mNumCellsToRemove++;
                    mAddCellsHandleDragData.mFirstCellToRemove = i;
                }
                else
                {
                    mAddCellsHandleDragData.mEditedCellData.Add(cellData);
                }
                mouseOffsetInt += lengthToRemove;
            }
        }
    }
    else
    {
        mEditingOffset = true;
        mOffset = mCellsContainer->GetOffset() + mAddCellsHandleDragData.mOffset;

        if ( mAddCellsHandleDragData.mOffset < 0 )
        {
            for ( int i = 0; i < -mAddCellsHandleDragData.mOffset; i++ )
            {
                TSharedPtr<FCellData> cellData = InsertCellData(0, nullptr, 0); //0 is ok as it is a temporary cell
                InsertCellSection(0, cellData);
            }
        }
        else
        {
            mAddCellsHandleDragData.mFirstCellToRemove = 0;

            //for each cell adjust its length or hide it 
            for ( int i = 0; i < mCellsData.Num() && mouseOffsetInt > 0; i++ )
            {
                TSharedPtr<FCellData> cellData = mCellsData[i];
                int length = cellData->mCell->GetLength();
                int lengthToRemove = FMath::Min(length, mouseOffsetInt);

                cellData->mEditingLength = true;
                cellData->mLength = cellData->mCell->GetLength() - lengthToRemove;
                if ( cellData->mLength <= 0 )
                {
                    //hide the cell and cell's handles if needed
                    cellData->mIsVisible = false;
                    cellData->mIsTimingHandleVisible = false;
                    cellData->mIsLengthHandleVisible = false;
                    mAddCellsHandleDragData.mNumCellsToRemove++;
                }
                else
                {
                    mAddCellsHandleDragData.mEditedCellData.Add(cellData);
                }
                mouseOffsetInt -= lengthToRemove;
            }
        }
    }*/
    
}

void
SOdysseyAnimationCells::OnAddCellsHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
#ifdef WITH_EDITOR
    FText undoText;
    if ( mAddCellsHandleDragData.mIsRightHandle )
    {
        if ( mAddCellsHandleDragData.mOffset > 0 )
        {
            undoText = LOCTEXT("timeline.cells.transaction.add-cells-at-start", "Add Cells");
        }
        else
        {
            undoText = LOCTEXT("timeline.cells.transaction.shrink-cells-at-start", "Change Cell Timing");
        }
    }
    else
    {
        if ( mAddCellsHandleDragData.mOffset < 0 )
        {
            undoText = LOCTEXT("timeline.cells.transaction.add-cells-at-end", "Add Cells");
        }
        else
        {
            undoText = LOCTEXT("timeline.cells.transaction.shrink-cells-at-end", "Change Cell Timing");
        }
    }
    FScopedTransaction ScopedTransaction(undoText);
#endif
    mCellsMutator->Commit();
    mCellsMutator = nullptr;
    
    mLockHandlesVisibility = false;
}

#undef LOCTEXT_NAMESPACE
