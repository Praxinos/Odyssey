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
    
    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew(mScrollBox, SOdysseyAnimationTimelineScrollBox, iEditor)
            + SOdysseyAnimationTimelineScrollBox::Slot()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SOdysseyAnimationTimelineSection, mEditor)
                    .WidthInFrames(this, &SOdysseyAnimationLayerImageRasterTimeline::GetLayerOffset)
                    .HeightInScreenUnits(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellHeight)
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
            ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SBox)
            .HeightOverride(20.f)
            [
                SNew(SOdysseyAnimationTimelineFrameSelector, mEditor)
                .OnMapActions(this, &SOdysseyAnimationLayerImageRasterTimeline::OnFrameSelectorMapActions)
                .OnBuildContextMenu(this, &SOdysseyAnimationLayerImageRasterTimeline::OnFrameSelectorBuildContextMenu)
            ]
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
    if (iCellData->mEditingLength)
        return iCellData->mLength;

    return iCellData->mCell ? iCellData->mCell->GetLength() : 0;
}

void
SOdysseyAnimationLayerImageRasterTimeline::RefreshWidgets()
{
    mCellsBox->ClearChildren();
    mHandlesBox->ClearChildren();

    TArray<TSharedPtr<FOdysseyAnimationCell>>& cells = mAnimationLayerImageRaster->GetCells();
    for ( TSharedPtr<FCellData> cellData : mCellsData )
    {
        mCellsBox->AddSlot()
        .AutoWidth()
        [
            SNew(SOdysseyAnimationTimelineSection, mEditor)
            .WidthInFrames(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellLength, cellData)
            .HeightInScreenUnits(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellHeight)
            [
                CreateCellWidget(cellData)
            ]
        ];

        mHandlesBox->AddSlot()
        .AutoWidth()
        [
            SNew(SOdysseyAnimationTimelineSection, mEditor)
            .WidthInFrames(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellLength, cellData)
            .HeightInScreenUnits(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellHeight)
            [
                SNew(SOverlay)
                + SOverlay::Slot() //Timing Handle Top Left
                .Padding(-mLengthHandleBrush->ImageSize.X / 2, 0.f, -mLengthHandleBrush->ImageSize.X / 2, 0.f)
                .HAlign(HAlign_Left)
                .VAlign(VAlign_Top)
                [
                    CreateTimingHandleWidget(cellData)
                ]
        
                +SOverlay::Slot() //Length Handle Top Right
                .Padding(0.0f, 0.0f, -mLengthHandleBrush->ImageSize.X/2, 0.f)
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Center)
                [
                    CreateLengthHandleWidget(cellData)
                ]
            ]
        ];
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

    /*
    return SNew(SOdysseyAnimationTimelineSection, mEditor)
        .WidthInFrames(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellLength, iCellData)
        .HeightInScreenUnits(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellHeight)
        [
            SNew(SOverlay)
            +SOverlay::Slot()
            [
                SNew(SOdysseyAnimationLayerImageRasterCell, mEditor, mAnimationLayerImageRaster)
                .Visibility(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellVisibility, iCellData)
                .OnMapActions(this, &SOdysseyAnimationLayerImageRasterTimeline::OnCellsMapActions)
                .OnBuildContextMenu(this, &SOdysseyAnimationLayerImageRasterTimeline::OnCellsBuildContextMenu)
            ]
            +SOverlay::Slot() //Timing Handle Top Left
            .Padding(-mLengthHandleBrush->ImageSize.X / 2, 0.f, -mLengthHandleBrush->ImageSize.X / 2, 0.f)
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Top)
            [
                SNew(SBox)
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
                ]
            ]
            +SOverlay::Slot() //Length Handle Top Right
            .Padding(0.0f, 0.0f, -mLengthHandleBrush->ImageSize.X/2, 0.f)
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Center)
            [
                SNew(SBox)
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
                ]
            ]
        ];
        */
}

void
SOdysseyAnimationLayerImageRasterTimeline::BuildCellsData()
{
    mCellsData.Empty();

    TArray<TSharedPtr<FOdysseyAnimationCell>>& cells = mAnimationLayerImageRaster->GetCells();
    for (int i = 0; i < cells.Num(); i++)
    {
        TSharedPtr<FCellData> cellData = MakeShared<FCellData>();
        cellData->mCell = cells[i];
        cellData->mCellIndex = i;
        cellData->mIsVisible = true;
        cellData->mIsTimingHandleVisible = true;
        cellData->mIsLengthHandleVisible = true;
        cellData->mEditingLength = false;
        cellData->mLength = 0;
        mCellsData.Add(cellData);
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

        if ( mTimingHandleDragData.mNumCellsToRemove > 0 )
        {
            mutator.Remove(mTimingHandleDragData.mFirstCellToRemove, mTimingHandleDragData.mNumCellsToRemove);
        }

        for ( TSharedPtr<FCellData> cellData : mTimingHandleDragData.mEditedCellData )
        {
            mutator.SetLength(cellData->mCellIndex, cellData->mLength);
        }
        mutator.Commit();
    }

    //Reset every cellsdata
    mEditingOffset = false;
    for ( TSharedPtr<FCellData> cellData : mCellsData )
    {
        cellData->mEditingLength = false;
        cellData->mIsVisible = true;
        cellData->mIsTimingHandleVisible = true;
        cellData->mIsLengthHandleVisible = true;
    }
    RequestRebuild();
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
