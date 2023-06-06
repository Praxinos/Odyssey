// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterTimeline.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "Widgets/LayerStack/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterCell.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineSection.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineFrameSelector.h"
#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerImageRasterTimeline"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
SOdysseyAnimationLayerImageRasterTimeline::~SOdysseyAnimationLayerImageRasterTimeline()
{
    UOdysseyAnimationLayerImageRaster::OnCellsChanged().RemoveAll(this);
}

SOdysseyAnimationLayerImageRasterTimeline::SOdysseyAnimationLayerImageRasterTimeline()
    : mIsRefreshPending(false)
    , mIsRebuildPending(false)
    , mOffsettingLayer(false)
    , mOffset(0)
    , mEditingOffset(false)
	, mTimingHandleBrush(nullptr)
	, mLengthHandleBrush(nullptr)
    , mAddCellsHandleBrush(nullptr)
{

}

void
SOdysseyAnimationLayerImageRasterTimeline::Construct(
    const FArguments& InArgs,
    FOdysseyAnimationEditor* iEditor,
    UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
)
{
    ensure(iAnimationLayerImageRaster);

    mEditor = iEditor;
    mTimingHandleBrush = FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameTimingHandle");
	mLengthHandleBrush = FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameLengthHandle");
    mAddCellsHandleBrush = FOdysseyStyle::GetBrush("AnimationLayerTimeline.AddCellsHandle");
    
    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOdysseyAnimationTimelineScrollBox, iEditor)
            + SOdysseyAnimationTimelineScrollBox::Slot()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SOdysseyAnimationTimelineSection, mEditor)
                    .WidthInFrames(this, &SOdysseyAnimationLayerImageRasterTimeline::GetLayerOffset)
                    .HeightInScreenUnits(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellHeight)
                    [
                        SAssignNew(mPreCellsBox, SBox)
                    ]
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SAssignNew(mCellsBorder, SBorder)
                    .Padding(FMargin(0.f))
                    .OnMouseButtonDown(this, &SOdysseyAnimationLayerImageRasterTimeline::OnCellsMouseButtonDown)
                    .OnMouseMove(this, &SOdysseyAnimationLayerImageRasterTimeline::OnCellsMouseMove)
                    .OnMouseButtonUp(this, &SOdysseyAnimationLayerImageRasterTimeline::OnCellsMouseButtonUp)
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
                    .Padding(0.f, 0.f, mAddCellsHandleBrush->ImageSize.X, 0.f)
                    .HAlign(HAlign_Left)
                    .VAlign(VAlign_Top)
                    [
                        CreateAddCellsHandleWidget()
                    ]
                ]
            ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOdysseyAnimationTimelineFrameSelector, mEditor)
            .Visibility(this, &SOdysseyAnimationLayerImageRasterTimeline::GetFrameSelectorVisibility)
            .OnMapActions(this, &SOdysseyAnimationLayerImageRasterTimeline::OnFrameSelectorMapActions)
            .OnBuildContextMenu(this, &SOdysseyAnimationLayerImageRasterTimeline::OnFrameSelectorBuildContextMenu)
        ]
    ];

    mAnimationLayerImageRaster = iAnimationLayerImageRaster;
    UOdysseyAnimationLayerImageRaster::OnCellsChanged().AddRaw(this, &SOdysseyAnimationLayerImageRasterTimeline::OnCellsChanged);
    RequestRebuild();
}

void
SOdysseyAnimationLayerImageRasterTimeline::RequestRefresh()
{
    mIsRefreshPending = true;
}

void
SOdysseyAnimationLayerImageRasterTimeline::RequestRebuild()
{
    mIsRebuildPending = true;
    RequestRefresh();
}

void
SOdysseyAnimationLayerImageRasterTimeline::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
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
SOdysseyAnimationLayerImageRasterTimeline::GetLayerOffset() const
{
    return mEditingOffset ? mOffset : mAnimationLayerImageRaster->GetOffset();
}

float
SOdysseyAnimationLayerImageRasterTimeline::GetCellHeight() const
{
    return mEditor->Timeline()->GetBaseFrameSize();
}

float
SOdysseyAnimationLayerImageRasterTimeline::GetCellLength(TSharedPtr<SOdysseyAnimationLayerImageRasterTimeline::FCellData> iCellData) const
{
    if ( !iCellData->mCell || iCellData->mEditingLength )
        return iCellData->mLength;

    return iCellData->mCell->GetLength();
}

TSharedPtr<SOdysseyAnimationLayerImageRasterTimeline::FCellData>
SOdysseyAnimationLayerImageRasterTimeline::AddCellData(TSharedPtr<FOdysseyAnimationCell> iCell, int iCellIndex)
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
SOdysseyAnimationLayerImageRasterTimeline::RemoveCellData(int iIndex)
{
    mCellsData.RemoveAt(iIndex);
}

void
SOdysseyAnimationLayerImageRasterTimeline::AddCellSection(TSharedPtr<FCellData> iCellData)
{
    iCellData->mCellSectionWidget = SNew(SOdysseyAnimationTimelineSection, mEditor)
        .WidthInFrames(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellLength, iCellData)
        .HeightInScreenUnits(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellHeight)
        [
            CreateCellWidget(iCellData)
        ];

    iCellData->mHandlesSectionWidget = SNew(SOdysseyAnimationTimelineSection, mEditor)
        .WidthInFrames(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellLength, iCellData)
        .HeightInScreenUnits(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellHeight)
        [
            SNew(SOverlay)
            + SOverlay::Slot() //Timing Handle Top Left
            .Padding(-mLengthHandleBrush->ImageSize.X / 2, 0.f, -mLengthHandleBrush->ImageSize.X / 2, 0.f)
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Top)
            [
                CreateTimingHandleWidget(iCellData)
            ]

            + SOverlay::Slot() //Length Handle Top Right
            .Padding(0.0f, 0.0f, -mLengthHandleBrush->ImageSize.X / 2, 0.f)
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Center)
            [
                CreateLengthHandleWidget(iCellData)
            ]
        ];

    //Cells widgets
    mCellsBox->AddSlot()
    .AutoWidth()
    [
        iCellData->mCellSectionWidget.ToSharedRef()
    ];

    //Handle widgets
    mHandlesBox->AddSlot()
    .AutoWidth()
    [
        iCellData->mHandlesSectionWidget.ToSharedRef()
    ];
}

void
SOdysseyAnimationLayerImageRasterTimeline::RemoveCellSection(TSharedPtr<FCellData> iCellData)
{
    mCellsBox->RemoveSlot(iCellData->mCellSectionWidget.ToSharedRef());
    mHandlesBox->RemoveSlot(iCellData->mHandlesSectionWidget.ToSharedRef());
}

void
SOdysseyAnimationLayerImageRasterTimeline::RefreshWidgets()
{
    mCellsBox->ClearChildren();
    mHandlesBox->ClearChildren();

    TArray<TSharedPtr<FOdysseyAnimationCell>>& cells = mAnimationLayerImageRaster->GetCells();
    for ( int i = 0; i < mCellsData.Num(); i++ )
    {
        TSharedPtr<FCellData> cellData = mCellsData[i];
        AddCellSection(cellData);
    }
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageRasterTimeline::CreateTimingHandleWidget(TSharedPtr<FCellData> iCellData)
{
    return SNew(SBox)
        .Visibility(this, &SOdysseyAnimationLayerImageRasterTimeline::GetTimingHandleVisibility, iCellData)
        .WidthOverride(mTimingHandleBrush->ImageSize.X)
        .HeightOverride(mTimingHandleBrush->ImageSize.Y)
        [
            SNew(SOdysseyAnimationCellHandle)
            .OnDragStarted(this, &SOdysseyAnimationLayerImageRasterTimeline::OnTimingHandleDragStarted, iCellData)
            .OnDragged(this, &SOdysseyAnimationLayerImageRasterTimeline::OnTimingHandleDragged)
            .OnDragStopped(this, &SOdysseyAnimationLayerImageRasterTimeline::OnTimingHandleDragStopped)
            [
                SNew(SImage)
                .Image(mTimingHandleBrush)
            ]
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageRasterTimeline::CreateLengthHandleWidget(TSharedPtr<FCellData> iCellData)
{
    return SNew(SBox)
        .Visibility(this, &SOdysseyAnimationLayerImageRasterTimeline::GetLengthHandleVisibility, iCellData)
        .WidthOverride(mLengthHandleBrush->ImageSize.X)
        .HeightOverride(mLengthHandleBrush->ImageSize.Y)
        [
            SNew(SOdysseyAnimationCellHandle)
            .OnDragStarted(this, &SOdysseyAnimationLayerImageRasterTimeline::OnLengthHandleDragStarted, iCellData)
            .OnDragged(this, &SOdysseyAnimationLayerImageRasterTimeline::OnLengthHandleDragged)
            .OnDragStopped(this, &SOdysseyAnimationLayerImageRasterTimeline::OnLengthHandleDragStopped)
            [
                SNew(SImage)
                .Image(mLengthHandleBrush)
            ]
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageRasterTimeline::CreateAddCellsHandleWidget()
{
    return SNew(SBox)
        .Visibility(this, &SOdysseyAnimationLayerImageRasterTimeline::GetAddCellsHandleVisibility)
        .WidthOverride(mAddCellsHandleBrush->ImageSize.X)
        .HeightOverride(mAddCellsHandleBrush->ImageSize.Y)
        [
            SNew(SOdysseyAnimationCellHandle)
            .OnDragStarted(this, &SOdysseyAnimationLayerImageRasterTimeline::OnAddCellsHandleDragStarted)
            .OnDragged(this, &SOdysseyAnimationLayerImageRasterTimeline::OnAddCellsHandleDragged)
            .OnDragStopped(this, &SOdysseyAnimationLayerImageRasterTimeline::OnAddCellsHandleDragStopped)
            [
                SNew(SImage)
                .Image(mAddCellsHandleBrush)
            ]
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageRasterTimeline::CreateCellWidget(TSharedPtr<FCellData> iCellData)
{
    return SNew(SOdysseyAnimationTimelineSection, mEditor)
    .WidthInFrames(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellLength, iCellData)
    .HeightInScreenUnits(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellHeight)
    [
        SNew(SOdysseyAnimationLayerImageRasterCell, mEditor, mAnimationLayerImageRaster)
        .Visibility(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellVisibility, iCellData)
        .OnMapActions(this, &SOdysseyAnimationLayerImageRasterTimeline::OnCellsMapActions)
        .OnBuildContextMenu(this, &SOdysseyAnimationLayerImageRasterTimeline::OnCellsBuildContextMenu)
    ];
}

void
SOdysseyAnimationLayerImageRasterTimeline::BuildCellsData()
{
    mCellsData.Empty();

    TArray<TSharedPtr<FOdysseyAnimationCell>>& cells = mAnimationLayerImageRaster->GetCells();
    for (int i = 0; i < cells.Num(); i++)
    {
        AddCellData(cells[i], i);
    }
}

bool
SOdysseyAnimationLayerImageRasterTimeline::SupportsKeyboardFocus() const
{
    return true;
}

FReply
SOdysseyAnimationLayerImageRasterTimeline::OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
	TSharedPtr<FUICommandList> commandList = MakeShared<FUICommandList>();
    MapActions(commandList);
	if (commandList->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return SCompoundWidget::OnKeyDown(iGeometry, iKeyEvent);
}

FReply
SOdysseyAnimationLayerImageRasterTimeline::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
	if (iEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
		TSharedPtr<FUICommandList> commandList = MakeShared<FUICommandList>();
		MapActions(commandList);

		FMenuBuilder menuBuilder(true, commandList);
		BuildContextMenu(menuBuilder);

		TSharedRef<SWidget> menuContents = menuBuilder.MakeWidget();
		FWidgetPath widgetPath = iEvent.GetEventPath() != nullptr ? *iEvent.GetEventPath() : FWidgetPath();
		FSlateApplication::Get().PushMenu(AsShared(), widgetPath, menuContents, iEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
    	return FReply::Handled();
	}
	return FReply::Unhandled();
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnCellsChanged(UOdysseyAnimationLayerImageRaster* iLayer)
{
    if (iLayer != mAnimationLayerImageRaster)
        return;

    RequestRebuild();
}

FReply
SOdysseyAnimationLayerImageRasterTimeline::OnCellsMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (iEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        mOffsettingLayer = true;

        mLayerOffsetData.mMousePosition = iEvent.GetScreenSpacePosition().X;
        mOffset = mAnimationLayerImageRaster->GetOffset();

        return FReply::Handled().CaptureMouse(mCellsBorder.ToSharedRef()).PreventThrottling();
    }

    return FReply::Unhandled();
}

FReply
SOdysseyAnimationLayerImageRasterTimeline::OnCellsMouseMove(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if ( mOffsettingLayer )
    {
        const int minOffset = 0;
        float mouseOffset = iEvent.GetScreenSpacePosition().X - mLayerOffsetData.mMousePosition;
        int offset = (int)(mAnimationLayerImageRaster->GetOffset() + (mouseOffset / mEditor->Timeline()->GetFrameWidth()));
        mOffset = FMath::Max(minOffset, offset);

        mEditingOffset = mOffset != mAnimationLayerImageRaster->GetOffset();

        return FReply::Handled();
    }

    return FReply::Unhandled();
}

FReply
SOdysseyAnimationLayerImageRasterTimeline::OnCellsMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if ( mOffsettingLayer )
    {
#ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("Layer Image Raster", "Change Layer Offset"));
#endif
        FOdysseyAnimationCellsMutator mutator(mAnimationLayerImageRaster);
        mutator.SetOffset(mOffset);
        mutator.Commit();
        mEditingOffset = false;
        mOffsettingLayer = false;

        return FReply::Handled().ReleaseMouseCapture();
    }
    return FReply::Unhandled();
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnCellsMapActions(TSharedPtr<FUICommandList> iCommandList)
{
    MapActions(iCommandList);
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnCellsBuildContextMenu(FMenuBuilder& iMenuBuilder)
{
    BuildContextMenu(iMenuBuilder);
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnFrameSelectorMapActions(TSharedPtr<FUICommandList> iCommandList)
{
    MapActions(iCommandList);
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnFrameSelectorBuildContextMenu(FMenuBuilder& iMenuBuilder)
{
    BuildContextMenu(iMenuBuilder);
}

void
SOdysseyAnimationLayerImageRasterTimeline::MapActions(TSharedPtr<FUICommandList> iCommandList)
{
	iCommandList->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageRasterTimeline::SelectAllFrames)
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerImageRasterTimeline::DeleteSelectedFrames)
    );
}

void
SOdysseyAnimationLayerImageRasterTimeline::BuildContextMenu(FMenuBuilder& iMenuBuilder)
{
    const FText commonSectionTitle = LOCTEXT("OdysseyAnimationTimelineCommonSection", "Common");
    iMenuBuilder.BeginSection("Common", commonSectionTitle);
    iMenuBuilder.AddMenuEntry(FGenericCommands::Get().SelectAll);
    iMenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);
    //iMenuBuilder.AddMenuSeparator();
    iMenuBuilder.EndSection();
}

EVisibility
SOdysseyAnimationLayerImageRasterTimeline::GetFrameSelectorVisibility() const
{
    bool isCurrentLayer = mAnimationLayerImageRaster->GetLayerStack()->CurrentLayer == mAnimationLayerImageRaster;
	return isCurrentLayer ? EVisibility::Visible : EVisibility::Hidden;
}

EVisibility
SOdysseyAnimationLayerImageRasterTimeline::GetTimingHandleVisibility(TSharedPtr<FCellData> iCellData) const
{
    bool isZoomedEnough = mEditor->Timeline()->GetFrameWidth() > mTimingHandleBrush->ImageSize.X / 2;
	return (isZoomedEnough && iCellData->mIsTimingHandleVisible) ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
SOdysseyAnimationLayerImageRasterTimeline::GetLengthHandleVisibility(TSharedPtr<FCellData> iCellData) const
{
    bool isZoomedEnough = mEditor->Timeline()->GetFrameWidth() > mLengthHandleBrush->ImageSize.X / 2;
    return (isZoomedEnough && iCellData->mIsLengthHandleVisible) ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
SOdysseyAnimationLayerImageRasterTimeline::GetCellVisibility(TSharedPtr<FCellData> iCellData) const
{
    return iCellData->mIsVisible ? EVisibility::Visible : EVisibility::Collapsed;
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnLengthHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, TSharedPtr<FCellData> iCellData)
{
    iCellData->mEditingLength = true;
    mLengthHandleDragData.mCellData = iCellData;
    mLengthHandleDragData.mMousePosition = iEvent.GetScreenSpacePosition().X;
    iCellData->mLength = iCellData->mCell->GetLength();
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnLengthHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    const int minLength = 1;
    float mouseOffset = iEvent.GetScreenSpacePosition().X - mLengthHandleDragData.mMousePosition;
    int mouseOffsetInt = 0;
    if ( mouseOffset > 0 )
        mouseOffsetInt = (int)(mouseOffset / mEditor->Timeline()->GetFrameWidth() + 0.5f);
    else
        mouseOffsetInt = (int)(mouseOffset / mEditor->Timeline()->GetFrameWidth() - 0.5f);

    int length = mLengthHandleDragData.mCellData->mCell->GetLength() + mouseOffsetInt;
    mLengthHandleDragData.mCellData->mLength = FMath::Max(minLength, length);
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnLengthHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("Layer Image Raster", "Change Cell Length"));
#endif
    FOdysseyAnimationCellsMutator mutator(mAnimationLayerImageRaster);
    mutator.SetLength(mLengthHandleDragData.mCellData->mCellIndex, mLengthHandleDragData.mCellData->mLength);
    mutator.Commit();

    mLengthHandleDragData.mCellData->mEditingLength = false;
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnTimingHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, TSharedPtr<FCellData> iCellData)
{
    FInt32Range cellRange;
    mAnimationLayerImageRaster->GetCellFrameRange(iCellData->mCellIndex, cellRange);
    FInt32Range layerRange = mAnimationLayerImageRaster->GetFrameRange();

    mTimingHandleDragData.mMinOffset = -cellRange.GetLowerBoundValue();
    mTimingHandleDragData.mHasMaxOffset = iCellData->mCellIndex == 0;
    mTimingHandleDragData.mMaxOffset = layerRange.GetUpperBoundValue() - cellRange.GetLowerBoundValue();
    mTimingHandleDragData.mCellData = iCellData;
    mTimingHandleDragData.mMousePosition = iEvent.GetScreenSpacePosition().X;
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnTimingHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    //Compute Mouse Offset
    float mouseOffset = iEvent.GetScreenSpacePosition().X - mTimingHandleDragData.mMousePosition;
    int mouseOffsetInt = 0;
    if ( mouseOffset > 0 )
        mouseOffsetInt = (int)(mouseOffset / mEditor->Timeline()->GetFrameWidth() + 0.5f);
    else
        mouseOffsetInt = (int)(mouseOffset / mEditor->Timeline()->GetFrameWidth() - 0.5f);

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
            mOffset = mAnimationLayerImageRaster->GetOffset() + mouseOffsetInt;
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
            mOffset = mAnimationLayerImageRaster->GetOffset() + mouseOffsetInt;
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
            mOffset = mAnimationLayerImageRaster->GetOffset() + mouseOffsetInt;
        }
    }
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnTimingHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if ( mEditingOffset || !mTimingHandleDragData.mEditedCellData.IsEmpty() || mTimingHandleDragData.mNumCellsToRemove > 0 )
    {
#ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("Layer Image Raster", "Change Cell Timing"));
#endif

        FOdysseyAnimationCellsMutator mutator(mAnimationLayerImageRaster);

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
SOdysseyAnimationLayerImageRasterTimeline::GetAddCellsHandleVisibility() const
{
    return EVisibility::Visible; //Could be more complicated than that one day
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnAddCellsHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    FInt32Range layerRange = mAnimationLayerImageRaster->GetFrameRange();
    mAddCellsHandleDragData.mMinOffset = layerRange.GetLowerBoundValue() - layerRange.GetUpperBoundValue();
    mAddCellsHandleDragData.mMousePosition = iEvent.GetScreenSpacePosition().X;
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnAddCellsHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    //Remove temporary cells
    for ( TSharedPtr<FCellData> lastCellData = mCellsData.Last(); !lastCellData->mCell; lastCellData = mCellsData.Last() ) //detect temporary cells
    {
        RemoveCellSection(lastCellData);
        RemoveCellData(mCellsData.Num() - 1);
    }

    //Compute Mouse Offset
    float mouseOffset = iEvent.GetScreenSpacePosition().X - mAddCellsHandleDragData.mMousePosition;
    int mouseOffsetInt = 0;
    if ( mouseOffset > 0 )
        mouseOffsetInt = (int)(mouseOffset / mEditor->Timeline()->GetFrameWidth() + 0.5f);
    else
        mouseOffsetInt = (int)(mouseOffset / mEditor->Timeline()->GetFrameWidth() - 0.5f);

    mouseOffsetInt = FMath::Max(mouseOffsetInt, mAddCellsHandleDragData.mMinOffset);

    mAddCellsHandleDragData.mOffset = mouseOffsetInt;

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

void
SOdysseyAnimationLayerImageRasterTimeline::OnAddCellsHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if ( mAddCellsHandleDragData.mOffset == 0 )
        return;

    if ( mAddCellsHandleDragData.mOffset > 0 )
    {
    #ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("Layer Image Raster", "Change Cell Timing"));
    #endif

        //Create cells to add
        UOdysseyAnimation* animation = mAnimationLayerImageRaster->GetAnimation();
        TArray<TSharedPtr<FOdysseyAnimationCell>> cells;
        for ( int i = 0; i < mAddCellsHandleDragData.mOffset; i++)
        {
            TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(mAnimationLayerImageRaster, animation->Width(), animation->Height(), animation->Format());
            cells.Add(cell);
        }

        FOdysseyAnimationCellsMutator mutator(mAnimationLayerImageRaster);
        mutator.Add(cells);
        mutator.Commit();
    }
    else
    {
#ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("Layer Image Raster", "Change Cell Timing"));
#endif

        FOdysseyAnimationCellsMutator mutator(mAnimationLayerImageRaster);

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

void
SOdysseyAnimationLayerImageRasterTimeline::SelectAllFrames()
{
    FInt32Range frameRange = mAnimationLayerImageRaster->GetFrameRange();
    mEditor->Timeline()->SetSelectedFrames(frameRange);
}

void
SOdysseyAnimationLayerImageRasterTimeline::DeleteSelectedFrames()
{
    //TODO: Simplify this

    bool isLowerClosed = mEditor->Timeline()->GetSelectedFrames().GetLowerBound().IsClosed();
    bool isUpperClosed = mEditor->Timeline()->GetSelectedFrames().GetUpperBound().IsClosed();

    int startFrame = mEditor->Animation()->CurrentFrame;
    int endFrame = mEditor->Animation()->CurrentFrame;
    if ( isLowerClosed && isUpperClosed )
    {
        startFrame = mEditor->Timeline()->GetSelectedFrames().GetLowerBoundValue();
        endFrame = mEditor->Timeline()->GetSelectedFrames().GetUpperBoundValue();
    }

    TMap<int, int> cellsLength;
    for (int i = endFrame; i >= startFrame; i--)
    {
        int cellIndex = INDEX_NONE;
        int cellFrameIndex = INDEX_NONE;
        if (!mAnimationLayerImageRaster->GetCellIndexAtFrame(i, cellIndex, cellFrameIndex))
            continue;

        if (!cellsLength.Contains(cellIndex))
        {
            int cellLength = INDEX_NONE;
            if ( !mAnimationLayerImageRaster->GetCellLength(cellIndex, cellLength) )
                continue;

            cellsLength.Add(cellIndex, cellLength);
        }
        cellsLength[cellIndex]--;
    }

    TArray< int > cellIndexes;
    cellsLength.GetKeys(cellIndexes);
    cellIndexes.Sort();

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("Layer Image Raster", "Remove Frames"));
#endif

    FOdysseyAnimationCellsMutator mutator(mAnimationLayerImageRaster);
    for (int i = cellIndexes.Num() - 1; i >= 0; i--)
    {
        int cellIndex = cellIndexes[i];
        int cellLength = cellsLength[cellIndex];

        if (cellLength <= 0)
        {
            //Remove the cell
            mutator.Remove(cellIndex);
            continue;
        }

        mutator.SetLength(cellIndex, cellLength);
    }
    mutator.Commit();
}

#undef LOCTEXT_NAMESPACE
