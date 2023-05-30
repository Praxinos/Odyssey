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
    , mIsOffsettingLayer(false)
    , mIsDraggingCellLengthHandle(false)
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
                        SAssignNew(mCellsBox, SHorizontalBox)
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
    RequestRefresh();
}

void
SOdysseyAnimationLayerImageRasterTimeline::RequestRefresh()
{
    mIsRefreshPending = true;
}

void
SOdysseyAnimationLayerImageRasterTimeline::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
    if ( mIsRefreshPending )
    {
        RefreshWidgets();
        mIsRefreshPending = false;
    }
}

float
SOdysseyAnimationLayerImageRasterTimeline::GetLayerOffset() const
{
    return mIsOffsettingLayer ? mLayerOffsetData.mOffset : mAnimationLayerImageRaster->GetOffset();
}

float
SOdysseyAnimationLayerImageRasterTimeline::GetCellHeight() const
{
    return mEditor->Timeline()->GetBaseFrameSize();
}

float
SOdysseyAnimationLayerImageRasterTimeline::GetCellLength(int iCellIndex) const
{
    if (mIsDraggingCellLengthHandle && mLengthHandleDragData.mCellIndex == iCellIndex)
        return mLengthHandleDragData.mLength;

    int cellLength = 0;
    mAnimationLayerImageRaster->GetCellLength(iCellIndex, cellLength);
    return cellLength;
}

void
SOdysseyAnimationLayerImageRasterTimeline::RefreshWidgets()
{
    mCellsBox->ClearChildren();
    AddCellsWidgets();
}

void
SOdysseyAnimationLayerImageRasterTimeline::AddCellsWidgets()
{
    TArray<TSharedPtr<FOdysseyAnimationCell>>& cells = mAnimationLayerImageRaster->GetCells();
    for (int i = 0; i < cells.Num(); i++)
    {
        TSharedPtr<FOdysseyAnimationCell> cell = cells[i];
        mCellsBox->AddSlot()
        .AutoWidth()
        [
            SNew(SOdysseyAnimationTimelineSection, mEditor)
            .WidthInFrames(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellLength, i)
            .HeightInScreenUnits(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellHeight)
            [
                SNew(SOverlay)
                +SOverlay::Slot()
                [
                    SNew(SOdysseyAnimationLayerImageRasterCell, mEditor, mAnimationLayerImageRaster, i)
                    .OnMapActions(this, &SOdysseyAnimationLayerImageRasterTimeline::OnCellsMapActions)
                    .OnBuildContextMenu(this, &SOdysseyAnimationLayerImageRasterTimeline::OnCellsBuildContextMenu)
                ]
                +SOverlay::Slot() //Timing Handle Top Left
                .Padding(-mLengthHandleBrush->ImageSize.X / 2, 0.f, 0.f, 0.f)
                .HAlign(HAlign_Left)
                .VAlign(VAlign_Top)
                [
                    SNullWidget::NullWidget
                ]
                +SOverlay::Slot() //Length Handle Top Right
                .Padding(0.0f, 0.0f, -mLengthHandleBrush->ImageSize.X/2, 0.f)
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Center)
                [
                    SNew(SBox)
					.Visibility(this, &SOdysseyAnimationLayerImageRasterTimeline::GetLengthHandleVisibility)
					.WidthOverride(mLengthHandleBrush->ImageSize.X)
					.HeightOverride(mLengthHandleBrush->ImageSize.Y)
					[
                        SNew(SOdysseyAnimationCellHandle)
                        .OnDragStarted(this, &SOdysseyAnimationLayerImageRasterTimeline::OnLengthHandleDragStarted, i)
                        .OnDragged(this, &SOdysseyAnimationLayerImageRasterTimeline::OnLengthHandleDragged)
                        .OnDragStopped(this, &SOdysseyAnimationLayerImageRasterTimeline::OnLengthHandleDragStopped)
                        [
                            SNew(SImage)
							.Image(mLengthHandleBrush)
                        ]
                    ]
                ]
            ]
        ];
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

    RequestRefresh();
}

FReply
SOdysseyAnimationLayerImageRasterTimeline::OnCellsMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (iEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        mIsOffsettingLayer = true;

        mLayerOffsetData.mMousePosition = iEvent.GetScreenSpacePosition().X;
        mLayerOffsetData.mStartOffset= mAnimationLayerImageRaster->GetOffset();
        mLayerOffsetData.mOffset = mAnimationLayerImageRaster->GetOffset();

        return FReply::Handled().CaptureMouse(mCellsBorder.ToSharedRef()).PreventThrottling();
    }

    return FReply::Unhandled();
}

FReply
SOdysseyAnimationLayerImageRasterTimeline::OnCellsMouseMove(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if ( mIsOffsettingLayer )
    {
        const int minOffset = 0;
        float mouseOffset = iEvent.GetScreenSpacePosition().X - mLayerOffsetData.mMousePosition;
        int offset = (int)(mLayerOffsetData.mStartOffset + (mouseOffset / mEditor->Timeline()->GetFrameWidth()));
        mLayerOffsetData.mOffset = FMath::Max(minOffset, offset);

        return FReply::Handled();
    }

    return FReply::Unhandled();
}

FReply
SOdysseyAnimationLayerImageRasterTimeline::OnCellsMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if ( mIsOffsettingLayer )
    {
#ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("Layer Image Raster", "Change Layer Offset"));
#endif
        FOdysseyAnimationCellsMutator mutator(mAnimationLayerImageRaster);
        mutator.SetOffset(mLayerOffsetData.mOffset);
        mutator.Commit();
        mIsOffsettingLayer = false;

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
SOdysseyAnimationLayerImageRasterTimeline::GetTimingHandleVisibility() const
{
	return (mEditor->Timeline()->GetFrameWidth() < mTimingHandleBrush->ImageSize.X / 2) ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility
SOdysseyAnimationLayerImageRasterTimeline::GetLengthHandleVisibility() const
{
	return (mEditor->Timeline()->GetFrameWidth() < mLengthHandleBrush->ImageSize.X / 2) ? EVisibility::Collapsed : EVisibility::Visible;
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnLengthHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, int iCellIndex)
{
    mIsDraggingCellLengthHandle = true;
    mLengthHandleDragData.mCellIndex = iCellIndex;
    mLengthHandleDragData.mMousePosition = iEvent.GetScreenSpacePosition().X;
    mAnimationLayerImageRaster->GetCellLength(iCellIndex, mLengthHandleDragData.mStartLength);
    mLengthHandleDragData.mLength = mLengthHandleDragData.mStartLength;
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

    int length = mLengthHandleDragData.mStartLength + mouseOffsetInt;
    mLengthHandleDragData.mLength = FMath::Max(minLength, length);
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnLengthHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("Layer Image Raster", "Change Cell Length"));
#endif
    FOdysseyAnimationCellsMutator mutator(mAnimationLayerImageRaster);
    mutator.SetLength(mLengthHandleDragData.mCellIndex, mLengthHandleDragData.mLength);
    mutator.Commit();

    mIsDraggingCellLengthHandle = false;
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
