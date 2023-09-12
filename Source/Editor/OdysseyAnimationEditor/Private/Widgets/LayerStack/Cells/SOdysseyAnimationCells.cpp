// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Cells/SOdysseyAnimationCells.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineSection.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineFrameSelector.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "Widgets/Layout/SMissingWidget.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineScrollBox.h"
#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationCells"

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
    , mOffsettingLayer(false)
    , mOffset(0)
{
    MapActions(mCommandList);
}

void
SOdysseyAnimationCells::Construct(
    const FArguments& InArgs,
    FOdysseyAnimationEditorExtension* iExtension,
    UOdysseyAnimationLayer* iAnimationLayer,
    TSharedRef<FOdysseyAnimationCellsContainer> iCellsContainer
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
    mOnBuildCellContextMenu = InArgs._OnBuildCellContextMenu;
    
    ChildSlot
    [
        SNew(SOdysseyAnimationTimelineScrollBox, mExtension)
        + SOdysseyAnimationTimelineScrollBox::Slot()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SOdysseyAnimationTimelineSection, mExtension)
                .WidthInFrames(this, &SOdysseyAnimationCells::GetOffset)
                .HeightInScreenUnits(this, &SOdysseyAnimationCells::GetCellHeight)
                [
                    //Add Cells Handle
                    SNew(SOverlay)
                    + SOverlay::Slot()
                    .Padding(0.f, 0.f, 0.f, 0.f)
                    .HAlign(HAlign_Right)
                    .VAlign(VAlign_Bottom)
                    [
                        CreateAddCellsHandleLeftWidget()
                    ]
                ]
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SAssignNew(mCellsBorder, SBorder)
                .Padding(FMargin(0.f))
                .OnMouseButtonDown(this, &SOdysseyAnimationCells::OnCellsMouseButtonDown)
                .OnMouseMove(this, &SOdysseyAnimationCells::OnCellsMouseMove)
                .OnMouseButtonUp(this, &SOdysseyAnimationCells::OnCellsMouseButtonUp)
                [
                    SNew(SOverlay)
                    + SOverlay::Slot()
                    [
                        SAssignNew(mCellsBox, SHorizontalBox)
                    ]
                    + SOverlay::Slot()
                    [
                        SAssignNew(mHandlesBox, SHorizontalBox)
                    ]
                ]
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                //Add Cells Handle
                SNew(SOverlay)
                + SOverlay::Slot()
                .Padding(0.f, 0.f, 0.f, 0.f)
                .HAlign(HAlign_Left)
                .VAlign(VAlign_Top)
                [
                    CreateAddCellsHandleRightWidget()
                ]
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
        mEditingOffset = false;
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
    return mEditingOffset ? mOffset : mCellsContainer->GetOffset();
}

float
SOdysseyAnimationCells::GetCellHeight() const
{
    return mExtension->Timeline()->GetBaseFrameSize();
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
    iCellData->mCellSectionWidget = SNew(SOdysseyAnimationTimelineSection, mExtension)
        .WidthInFrames(this, &SOdysseyAnimationCells::GetCellLength, iCellData)
        .HeightInScreenUnits(this, &SOdysseyAnimationCells::GetCellHeight)
        [
            CreateCellWidget(iCellData)
        ];

    iCellData->mHandlesSectionWidget = SNew(SOdysseyAnimationTimelineSection, mExtension)
        .WidthInFrames(this, &SOdysseyAnimationCells::GetCellLength, iCellData)
        .HeightInScreenUnits(this, &SOdysseyAnimationCells::GetCellHeight)
        [
            SNew(SOverlay)
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
        iCellData->mCellSectionWidget.ToSharedRef()
    ];

    //Handle widgets
    mHandlesBox->InsertSlot(iIndex)
    .AutoWidth()
    [
        iCellData->mHandlesSectionWidget.ToSharedRef()
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
    mCellsBox->RemoveSlot(iCellData->mCellSectionWidget.ToSharedRef());
    mHandlesBox->RemoveSlot(iCellData->mHandlesSectionWidget.ToSharedRef());
}

void
SOdysseyAnimationCells::RefreshWidgets()
{
    mCellsBox->ClearChildren();
    mHandlesBox->ClearChildren();

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
    return SNew(SBox)
        .Visibility(this, &SOdysseyAnimationCells::GetAddCellsHandleRightVisibility)
        .WidthOverride(mAddCellsHandleRightBrush->ImageSize.X)
        .HeightOverride(mAddCellsHandleRightBrush->ImageSize.Y)
        [
            SNew(SOdysseyAnimationCellHandle)
            .OnDragStarted(this, &SOdysseyAnimationCells::OnAddCellsHandleDragStarted, true)
            .OnDragged(this, &SOdysseyAnimationCells::OnAddCellsHandleDragged)
            .OnDragStopped(this, &SOdysseyAnimationCells::OnAddCellsHandleDragStopped)
            [
                SNew(SImage)
                .Image(mAddCellsHandleRightBrush)
            ]
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationCells::CreateAddCellsHandleLeftWidget()
{
    return SNew(SBox)
        .Visibility(this, &SOdysseyAnimationCells::GetAddCellsHandleLeftVisibility)
        .WidthOverride(mAddCellsHandleLeftBrush->ImageSize.X)
        .HeightOverride(mAddCellsHandleLeftBrush->ImageSize.Y)
        [
            SNew(SOdysseyAnimationCellHandle)
            .OnDragStarted(this, &SOdysseyAnimationCells::OnAddCellsHandleDragStarted, false)
            .OnDragged(this, &SOdysseyAnimationCells::OnAddCellsHandleDragged)
            .OnDragStopped(this, &SOdysseyAnimationCells::OnAddCellsHandleDragStopped)
            [
                SNew(SImage)
                .Image(mAddCellsHandleLeftBrush)
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
    .HeightInScreenUnits(this, &SOdysseyAnimationCells::GetCellHeight)
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

FReply
SOdysseyAnimationCells::OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
	if (mCommandList->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return SCompoundWidget::OnKeyDown(iGeometry, iKeyEvent);
}

FReply
SOdysseyAnimationCells::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
	if (iEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        int frame = mExtension->Timeline()->GetFrameIndexAtMousePosition(iGeometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition()).X);
        if (frame == INDEX_NONE)
            return FReply::Unhandled();

		FMenuBuilder menuBuilder(true, mCommandList);
		BuildContextMenu(menuBuilder, frame);

		TSharedRef<SWidget> menuContents = menuBuilder.MakeWidget();
		FWidgetPath widgetPath = iEvent.GetEventPath() != nullptr ? *iEvent.GetEventPath() : FWidgetPath();
		FSlateApplication::Get().PushMenu(AsShared(), widgetPath, menuContents, iEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
    	return FReply::Handled();
	}
	return FReply::Unhandled();
}

void
SOdysseyAnimationCells::OnCellsChanged()
{
    RequestRebuild();
}

FReply
SOdysseyAnimationCells::OnCellsMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (iEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        mOffsettingLayer = true;

        mLayerOffsetData.mMousePosition = iEvent.GetScreenSpacePosition().X;
        mOffset = mCellsContainer->GetOffset();

        return FReply::Handled().CaptureMouse(mCellsBorder.ToSharedRef()).PreventThrottling();
    }

    return FReply::Unhandled();
}

FReply
SOdysseyAnimationCells::OnCellsMouseMove(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if ( mOffsettingLayer )
    {
        const int minOffset = 0;
        float mouseOffset = iEvent.GetScreenSpacePosition().X - mLayerOffsetData.mMousePosition;
        int offset = (int)(mCellsContainer->GetOffset() + (mouseOffset / mExtension->Timeline()->GetFrameWidth()));
        mOffset = FMath::Max(minOffset, offset);

        mEditingOffset = mOffset != mCellsContainer->GetOffset();

        return FReply::Handled();
    }

    return FReply::Unhandled();
}

FReply
SOdysseyAnimationCells::OnCellsMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if ( mOffsettingLayer )
    {
#ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("Layer", "Change Layer Offset"));
#endif
        FOdysseyAnimationCellsMutator mutator(mAnimationLayer, mCellsContainer.ToSharedRef());
        mutator.SetOffset(mOffset);
        mutator.Commit();
        mEditingOffset = false;
        mOffsettingLayer = false;

        return FReply::Handled().ReleaseMouseCapture();
    }
    return FReply::Unhandled();
}

/* void
SOdysseyAnimationCells::OnCellsBuildContextMenu(FMenuBuilder& iMenuBuilder, TSharedPtr<FCellData> iCellData)
{
    BuildContextMenu(iMenuBuilder);
    mOnBuildCellContextMenu.ExecuteIfBound(iMenuBuilder, iCellData->mCell);
} */

/* void
SOdysseyAnimationCells::OnFrameSelectorBuildContextMenu(FMenuBuilder& iMenuBuilder)
{
    BuildContextMenu(iMenuBuilder);
} */

void
SOdysseyAnimationCells::MapActions(TSharedPtr<FUICommandList> iCommandList)
{
	iCommandList->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SelectAllFrames)
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::DeleteSelectedFrames)
    );
}

void
SOdysseyAnimationCells::BuildContextMenu(FMenuBuilder& iMenuBuilder, int iFrame)
{
    const FText commonSectionTitle = LOCTEXT("OdysseyAnimationTimelineCommonSection", "Common");
    iMenuBuilder.BeginSection("Common", commonSectionTitle);
        iMenuBuilder.PushCommandList(mCommandList);
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().SelectAll);
        iMenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);
        iMenuBuilder.PopCommandList();
    iMenuBuilder.EndSection();

    mOnBuildCellContextMenu.ExecuteIfBound(iMenuBuilder, iFrame);
}

/*
EVisibility
SOdysseyAnimationCells::GetFrameSelectorVisibility() const
{
    bool isCurrentLayer = mAnimationLayer->GetLayerStack()->CurrentLayer == mAnimationLayer;
	return isCurrentLayer ? EVisibility::Visible : EVisibility::Hidden;
}
*/

EVisibility
SOdysseyAnimationCells::GetTimingHandleVisibility(TSharedPtr<FCellData> iCellData) const
{
    bool isZoomedEnough = mExtension->Timeline()->GetFrameWidth() > mTimingHandleBrush->ImageSize.X;
	return (isZoomedEnough && iCellData->mIsTimingHandleVisible) ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
SOdysseyAnimationCells::GetLengthHandleVisibility(TSharedPtr<FCellData> iCellData) const
{
    bool isZoomedEnough = mExtension->Timeline()->GetFrameWidth() > mLengthHandleBrush->ImageSize.X;
    return (isZoomedEnough && iCellData->mIsLengthHandleVisible) ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
SOdysseyAnimationCells::GetCellVisibility(TSharedPtr<FCellData> iCellData) const
{
    return iCellData->mIsVisible ? EVisibility::Visible : EVisibility::Collapsed;
}

void
SOdysseyAnimationCells::OnLengthHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, TSharedPtr<FCellData> iCellData)
{
    iCellData->mEditingLength = true;
    mLengthHandleDragData.mCellData = iCellData;
    mLengthHandleDragData.mMousePosition = iEvent.GetScreenSpacePosition().X;
    iCellData->mLength = iCellData->mCell->GetLength();
}

void
SOdysseyAnimationCells::OnLengthHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    const int minLength = 1;
    float mouseOffset = iEvent.GetScreenSpacePosition().X - mLengthHandleDragData.mMousePosition;
    int mouseOffsetInt = 0;
    if ( mouseOffset > 0 )
        mouseOffsetInt = (int)(mouseOffset / mExtension->Timeline()->GetFrameWidth() + 0.5f);
    else
        mouseOffsetInt = (int)(mouseOffset / mExtension->Timeline()->GetFrameWidth() - 0.5f);

    int length = mLengthHandleDragData.mCellData->mCell->GetLength() + mouseOffsetInt;
    mLengthHandleDragData.mCellData->mLength = FMath::Max(minLength, length);
}

void
SOdysseyAnimationCells::OnLengthHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("Layer", "Change Cell Length"));
#endif
    FOdysseyAnimationCellsMutator mutator(mAnimationLayer, mCellsContainer.ToSharedRef());
    mutator.SetLength(mLengthHandleDragData.mCellData->mCellIndex, mLengthHandleDragData.mCellData->mLength);
    mutator.Commit();

    mLengthHandleDragData.mCellData->mEditingLength = false;
}

void
SOdysseyAnimationCells::OnTimingHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, TSharedPtr<FCellData> iCellData)
{
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
        

    //Reinit everything before computing what needs to change
    mEditingOffset = false;
    for ( TSharedPtr<FCellData> cellData : mCellsData )
    {
        cellData->mEditingLength = false;
        cellData->mIsVisible = true;
        cellData->mIsTimingHandleVisible = true;
        cellData->mIsLengthHandleVisible = true;
    }

    mTimingHandleDragData.mNumCellsToRemove = 0;
    mTimingHandleDragData.mFirstCellToRemove = 0;
    mTimingHandleDragData.mEditedCellData.Empty();

    //Compute what needs to change
    if ( mouseOffsetInt > 0 )
    {
        // When dragging to the right, the previous cell length is always edited if it exists
        // It cannot be removed
        int previousCellIndex = mTimingHandleDragData.mCellData->mCellIndex - 1;
        if ( previousCellIndex >= 0 )
        {
            TSharedPtr<FCellData> previousCellData = mCellsData[previousCellIndex];
            previousCellData->mEditingLength = true;
            previousCellData->mLength = previousCellData->mCell->GetLength() + mouseOffsetInt;

            mTimingHandleDragData.mEditedCellData.Add(previousCellData);
        }
        else //If no previous cell exists, we need to edit the layer's offset value
        {
            mEditingOffset = true;
            mOffset = mCellsContainer->GetOffset() + mouseOffsetInt;
        }

        //this is the first cell to if we need to remove some cells
        //this line does not mean the cell is giong to be removed
        //as mNumCellsToRemove is still 0 here
        mTimingHandleDragData.mFirstCellToRemove = mTimingHandleDragData.mCellData->mCellIndex;

        //for each cell, adjust cell length or hide it
        for ( int i = mTimingHandleDragData.mCellData->mCellIndex; i < mCellsData.Num() && mouseOffsetInt > 0; i++ )
        {
            TSharedPtr<FCellData> cellData = mCellsData[i];
            int length = cellData->mCell->GetLength();
            int lengthToRemove = FMath::Min(length, mouseOffsetInt);

            cellData->mEditingLength = true;
            cellData->mLength = cellData->mCell->GetLength() - lengthToRemove;
            if ( cellData->mLength <= 0 )
            {
                //hide the cell if needed
                cellData->mIsVisible = false;
                cellData->mIsLengthHandleVisible = false;


                //hide next cell timing handle to avoid 2 handle to overlap
                //and also to avoid the currently edited handle to be hidden
                //which would stop any event to be generated
                if ( i < mCellsData.Num() - 1 )
                {
                    TSharedPtr<FCellData> nextCellData = mCellsData[i + 1];
                    nextCellData->mIsTimingHandleVisible = false;
                }

                mTimingHandleDragData.mNumCellsToRemove++;
            }
            else
            {
                mTimingHandleDragData.mEditedCellData.Add(cellData);
            }
            mouseOffsetInt -= lengthToRemove;
        }
    }
    else if ( mouseOffsetInt < 0 )
    {
        // When dragging to the left, the current cell length is always edited
        // It cannot be removed
        mTimingHandleDragData.mCellData->mEditingLength = true;
        mTimingHandleDragData.mCellData->mLength = mTimingHandleDragData.mCellData->mCell->GetLength() - mouseOffsetInt;
        mTimingHandleDragData.mEditedCellData.Add(mTimingHandleDragData.mCellData);

        //If the edited cell is the first cell, we also need to adjust the layer offset value
        if ( mTimingHandleDragData.mCellData->mCellIndex == 0 )
        {
            mEditingOffset = true;
            mOffset = mCellsContainer->GetOffset() + mouseOffsetInt;
        }

        //for each cell adjust its length or hide it 
        for ( int i = mTimingHandleDragData.mCellData->mCellIndex - 1; i >= 0 && mouseOffsetInt < 0; i-- )
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
                mTimingHandleDragData.mNumCellsToRemove++;
                mTimingHandleDragData.mFirstCellToRemove = i;
            }
            else
            {
                mTimingHandleDragData.mEditedCellData.Add(cellData);
            }
            mouseOffsetInt += lengthToRemove;
        }

        //adjust layer's offset if we dragged further than the first cell
        if ( mouseOffsetInt < 0 )
        {
            mEditingOffset = true;
            mOffset = mCellsContainer->GetOffset() + mouseOffsetInt;
        }
    }
}

void
SOdysseyAnimationCells::OnTimingHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if ( mEditingOffset || !mTimingHandleDragData.mEditedCellData.IsEmpty() || mTimingHandleDragData.mNumCellsToRemove > 0 )
    {
#ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("Layer", "Change Cell Timing"));
#endif

        FOdysseyAnimationCellsMutator mutator(mAnimationLayer, mCellsContainer.ToSharedRef());

        if ( mEditingOffset )
        {
            mutator.SetOffset(mOffset);
        }

        for ( TSharedPtr<FCellData> cellData : mTimingHandleDragData.mEditedCellData )
        {
            mutator.SetLength(cellData->mCellIndex, cellData->mLength);
        }

        if ( mTimingHandleDragData.mNumCellsToRemove > 0 )
        {
            mutator.Remove(mTimingHandleDragData.mFirstCellToRemove, mTimingHandleDragData.mNumCellsToRemove);
        }
        mutator.Commit();
    }
}

EVisibility
SOdysseyAnimationCells::GetAddCellsHandleRightVisibility() const
{
    bool isZoomedEnough = mExtension->Timeline()->GetFrameWidth() > mAddCellsHandleRightBrush->ImageSize.X;
    return isZoomedEnough ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
SOdysseyAnimationCells::GetAddCellsHandleLeftVisibility() const
{
    bool isZoomedEnough = mExtension->Timeline()->GetFrameWidth() > mAddCellsHandleLeftBrush->ImageSize.X;
    return (isZoomedEnough && GetOffset() > 0) ? EVisibility::Visible : EVisibility::Collapsed;
}

void
SOdysseyAnimationCells::OnAddCellsHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, bool iIsRightHandle)
{
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
    //Remove temporary cells at the start
    for ( TSharedPtr<FCellData> cellData = mCellsData[0]; !cellData->mCell; cellData = mCellsData[0] ) //detect temporary cells
    {
        RemoveCellSection(cellData);
        RemoveCellData(0);
    }

    //Remove temporary cells at the end
    for ( TSharedPtr<FCellData> lastCellData = mCellsData.Last(); !lastCellData->mCell; lastCellData = mCellsData.Last() ) //detect temporary cells
    {
        RemoveCellSection(lastCellData);
        RemoveCellData(mCellsData.Num() - 1);
    }

    //Compute Mouse Offset
    float mouseOffset = iEvent.GetScreenSpacePosition().X - mAddCellsHandleDragData.mMousePosition;
    int mouseOffsetInt = 0;
    if ( mouseOffset > 0 )
        mouseOffsetInt = (int)(mouseOffset / mExtension->Timeline()->GetFrameWidth() + 0.5f);
    else
        mouseOffsetInt = (int)(mouseOffset / mExtension->Timeline()->GetFrameWidth() - 0.5f);

    mouseOffsetInt = FMath::Max(mouseOffsetInt, mAddCellsHandleDragData.mMinOffset);

    if (mAddCellsHandleDragData.mHasMaxOffset)
        mouseOffsetInt = FMath::Min(mouseOffsetInt, mAddCellsHandleDragData.mMaxOffset);

    mAddCellsHandleDragData.mOffset = mouseOffsetInt;

    mEditingOffset = false;
    for ( TSharedPtr<FCellData> cellData : mCellsData )
    {
        cellData->mEditingLength = false;
        cellData->mIsVisible = true;
        cellData->mIsTimingHandleVisible = true;
        cellData->mIsLengthHandleVisible = true;
    }

    mAddCellsHandleDragData.mNumCellsToRemove = 0;
    mAddCellsHandleDragData.mFirstCellToRemove = 0;
    mAddCellsHandleDragData.mEditedCellData.Empty();

    if (mAddCellsHandleDragData.mIsRightHandle)
    {
        if ( mAddCellsHandleDragData.mOffset > 0 )
        {
            for ( int i = 0; i < mAddCellsHandleDragData.mOffset; i++ )
            {
                TSharedPtr<FCellData> cellData = AddCellData(nullptr, 0); //0 is ok as it is a temporary cell
                AddCellSection(cellData);
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
    }
    
}

void
SOdysseyAnimationCells::OnAddCellsHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if ( mAddCellsHandleDragData.mOffset == 0 )
        return;


    if ( mAddCellsHandleDragData.mIsRightHandle )
    {
        if ( mAddCellsHandleDragData.mOffset > 0 )
        {
#ifdef WITH_EDITOR
            FScopedTransaction ScopedTransaction(LOCTEXT("Layer", "Add Cells"));
#endif

            //Create cells to add
            TArray<TSharedPtr<FOdysseyAnimationCell>> cells;
            for ( int i = 0; i < mAddCellsHandleDragData.mOffset; i++ )
            {
                TSharedPtr<FOdysseyAnimationCell> cell = mOnCreateCell.Execute();
                cells.Add(cell);
            }

            FOdysseyAnimationCellsMutator mutator(mAnimationLayer, mCellsContainer.ToSharedRef());
            mutator.Add(cells);
            mutator.Commit();
        }
        else
        {
#ifdef WITH_EDITOR
            FScopedTransaction ScopedTransaction(LOCTEXT("Layer", "Change Cell Timing"));
#endif

            FOdysseyAnimationCellsMutator mutator(mAnimationLayer, mCellsContainer.ToSharedRef());

            for ( TSharedPtr<FCellData> cellData : mAddCellsHandleDragData.mEditedCellData )
            {
                mutator.SetLength(cellData->mCellIndex, cellData->mLength);
            }

            if ( mAddCellsHandleDragData.mNumCellsToRemove > 0 )
            {
                mutator.Remove(mAddCellsHandleDragData.mFirstCellToRemove, mAddCellsHandleDragData.mNumCellsToRemove);
            }
            mutator.Commit();
        }
    }
    else
    {
        if ( mAddCellsHandleDragData.mOffset < 0 )
        {
#ifdef WITH_EDITOR
            FScopedTransaction ScopedTransaction(LOCTEXT("Layer", "Add Cells"));
#endif

            //Create cells to add
            TArray<TSharedPtr<FOdysseyAnimationCell>> cells;
            for ( int i = 0; i < -mAddCellsHandleDragData.mOffset; i++ )
            {
                TSharedPtr<FOdysseyAnimationCell> cell = mOnCreateCell.Execute();
                cells.Add(cell);
            }

            FOdysseyAnimationCellsMutator mutator(mAnimationLayer, mCellsContainer.ToSharedRef());

            if ( mEditingOffset )
            {
                mutator.SetOffset(mOffset);
            }

            mutator.Add(cells, 0);
            mutator.Commit();
        }
        else
        {
#ifdef WITH_EDITOR
            FScopedTransaction ScopedTransaction(LOCTEXT("Layer", "Change Cell Timing"));
#endif

            FOdysseyAnimationCellsMutator mutator(mAnimationLayer, mCellsContainer.ToSharedRef());

            if ( mEditingOffset )
            {
                mutator.SetOffset(mOffset);
            }

            for ( TSharedPtr<FCellData> cellData : mAddCellsHandleDragData.mEditedCellData )
            {
                mutator.SetLength(cellData->mCellIndex, cellData->mLength);
            }

            if ( mAddCellsHandleDragData.mNumCellsToRemove > 0 )
            {
                mutator.Remove(mAddCellsHandleDragData.mFirstCellToRemove, mAddCellsHandleDragData.mNumCellsToRemove);
            }
            mutator.Commit();
        }
    }
}

void
SOdysseyAnimationCells::SelectAllFrames()
{
    FInt32Range frameRange = mCellsContainer->GetFrameRange();
    mExtension->Timeline()->SetSelectedFrames(frameRange);
}

void
SOdysseyAnimationCells::DeleteSelectedFrames()
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("Layer", "Remove Frames"));
#endif

    FOdysseyAnimationCellsMutator mutator(mAnimationLayer, mCellsContainer.ToSharedRef());

    bool isLowerClosed = mExtension->Timeline()->GetSelectedFrames().GetLowerBound().IsClosed();
    bool isUpperClosed = mExtension->Timeline()->GetSelectedFrames().GetUpperBound().IsClosed();

    if ( !isLowerClosed || !isUpperClosed )
    {
        mutator.RemoveFrame(mExtension->Animation()->CurrentFrame);
    }
    else
    {
        mutator.RemoveFrameRange(mExtension->Timeline()->GetSelectedFrames());
    }

    mutator.Commit();
}

#undef LOCTEXT_NAMESPACE
