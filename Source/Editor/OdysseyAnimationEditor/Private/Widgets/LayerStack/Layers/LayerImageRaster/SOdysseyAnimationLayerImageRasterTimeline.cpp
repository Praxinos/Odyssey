// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterTimeline.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "Widgets/LayerStack/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterCell.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineSection.h"
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
    TSharedPtr<SOdysseyAnimationLayerStack> iLayerStackWidget,
    UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
)
{
    ensure(iAnimationLayerImageRaster);

    mTimingHandleBrush = FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameTimingHandle");
	mLengthHandleBrush = FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameLengthHandle");
    
    SOdysseyAnimationTimelineWidget::FArguments args;
    args.BaseOffset(4.f);

	SOdysseyAnimationTimelineWidget::Construct(
		args,
        iLayerStackWidget
	);
    
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
    return mIsOffsettingLayer ? mLayerOffsetData.mOffset : mAnimationLayerImageRaster->Offset;
}

float
SOdysseyAnimationLayerImageRasterTimeline::GetCellHeight() const
{
    return GetLayerStackWidget()->GetTimelineBaseFrameSize();
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
    ClearChildren();
    AddPreBehaviourWidget();
    AddCellsWidgets();
}

void
SOdysseyAnimationLayerImageRasterTimeline::AddPreBehaviourWidget()
{
    AddChild()
    [
        SNew(SOdysseyAnimationTimelineSection, GetLayerStackWidget())
        .WidthInFrames(this, &SOdysseyAnimationLayerImageRasterTimeline::GetLayerOffset)
        .HeightInScreenUnits(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellHeight)
    ];
}

void
SOdysseyAnimationLayerImageRasterTimeline::AddCellsWidgets()
{
    TArray<TSharedPtr<FOdysseyAnimationCell>>& cells = mAnimationLayerImageRaster->GetCells();
    for (int i = 0; i < cells.Num(); i++)
    {
        TSharedPtr<FOdysseyAnimationCell> cell = cells[i];
        AddChild()
        [
            SNew(SOdysseyAnimationTimelineSection, GetLayerStackWidget())
            .WidthInFrames(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellLength, i)
            .HeightInScreenUnits(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellHeight)
            [
                SNew(SOverlay)
                +SOverlay::Slot()
                [
                    SNew(SOdysseyAnimationLayerImageRasterCell, cell)
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

void
SOdysseyAnimationLayerImageRasterTimeline::OnCellsChanged(UOdysseyAnimationLayerImageRaster* iLayer)
{
    if (iLayer != mAnimationLayerImageRaster)
        return;

    RequestRefresh();
}

FReply
SOdysseyAnimationLayerImageRasterTimeline::OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    FReply reply = SOdysseyAnimationTimelineWidget::OnMouseButtonDown(iGeometry, iEvent);
    if (reply.IsEventHandled())
		return reply;

    mIsOffsettingLayer = true;

    mLayerOffsetData.mMousePosition = iEvent.GetScreenSpacePosition().X;
    mLayerOffsetData.mStartOffset= mAnimationLayerImageRaster->Offset;
    mLayerOffsetData.mOffset = mAnimationLayerImageRaster->Offset;

    return FReply::Handled().CaptureMouse(AsShared()).PreventThrottling();
}

FReply
SOdysseyAnimationLayerImageRasterTimeline::OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    FReply reply = SOdysseyAnimationTimelineWidget::OnMouseMove(iGeometry, iEvent);
    if (reply.IsEventHandled())
		return reply;

    if ( mIsOffsettingLayer )
    {
        const int minOffset = 0;
        float mouseOffset = iEvent.GetScreenSpacePosition().X - mLayerOffsetData.mMousePosition;
        int offset = (int)(mLayerOffsetData.mStartOffset + (mouseOffset / GetLayerStackWidget()->GetTimelineFrameWidth()));
        mLayerOffsetData.mOffset = FMath::Max(minOffset, offset);

        return FReply::Handled();
    }

    return FReply::Unhandled();
}

FReply
SOdysseyAnimationLayerImageRasterTimeline::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    FReply reply = SOdysseyAnimationTimelineWidget::OnMouseButtonUp(iGeometry, iEvent);
    if (reply.IsEventHandled())
		return reply;

    if ( mIsOffsettingLayer )
    {
        FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageRaster, "Offset", mLayerOffsetData.mOffset);
        mIsOffsettingLayer = false;

        return FReply::Handled().ReleaseMouseCapture();
    }

    return FReply::Unhandled();
}

EVisibility
SOdysseyAnimationLayerImageRasterTimeline::GetTimingHandleVisibility() const
{
	return (GetLayerStackWidget()->GetTimelineFrameWidth() < mTimingHandleBrush->ImageSize.X / 2) ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility
SOdysseyAnimationLayerImageRasterTimeline::GetLengthHandleVisibility() const
{
	return (GetLayerStackWidget()->GetTimelineFrameWidth() < mLengthHandleBrush->ImageSize.X / 2) ? EVisibility::Collapsed : EVisibility::Visible;
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
        mouseOffsetInt = (int)(mouseOffset / GetLayerStackWidget()->GetTimelineFrameWidth() + 0.5f);
    else
        mouseOffsetInt = (int)(mouseOffset / GetLayerStackWidget()->GetTimelineFrameWidth() - 0.5f);

    int length = mLengthHandleDragData.mStartLength + mouseOffsetInt;
    mLengthHandleDragData.mLength = FMath::Max(minLength, length);
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnLengthHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    mAnimationLayerImageRaster->SetCellLength(mLengthHandleDragData.mCellIndex, mLengthHandleDragData.mLength);
    mIsDraggingCellLengthHandle = false;
}

#undef LOCTEXT_NAMESPACE
