// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterTimeline.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "Widgets/LayerStack/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterCellSection.h"
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
    return mIsOffsettingLayer ? mLayerOffset : mAnimationLayerImageRaster->Offset;
}

float
SOdysseyAnimationLayerImageRasterTimeline::GetCellHeight() const
{
    return GetLayerStackWidget()->GetTimelineBaseFrameSize();
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
    for (TSharedPtr<FOdysseyAnimationCell> cell : cells)
    {
        AddChild()
        [
            SNew(SOdysseyAnimationLayerImageRasterCellSection, GetLayerStackWidget(), cell)
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
    mOffsetMousePosition = iEvent.GetScreenSpacePosition();
    mOffsetMousePosition.Y = mAnimationLayerImageRaster->Offset;
    mLayerOffset = mAnimationLayerImageRaster->Offset;

    return FReply::Handled().CaptureMouse(AsShared()).PreventThrottling();
}

FReply
SOdysseyAnimationLayerImageRasterTimeline::OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    FReply reply = SOdysseyAnimationTimelineWidget::OnMouseMove(iGeometry, iEvent);
    if (reply.IsEventHandled())
		return reply;

    const float minOffset = 0.0f;
    float mouseOffset = iEvent.GetScreenSpacePosition().X - mOffsetMousePosition.X;
    //mOffsetMousePosition.Y contains the starting offset instead of the Y position
    mLayerOffset = (int)FMath::Max(minOffset, mOffsetMousePosition.Y + (mouseOffset / GetLayerStackWidget()->GetTimelineFrameWidth()));

    return FReply::Handled();
}

FReply
SOdysseyAnimationLayerImageRasterTimeline::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    FReply reply = SOdysseyAnimationTimelineWidget::OnMouseButtonUp(iGeometry, iEvent);
    if (reply.IsEventHandled())
		return reply;

    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageRaster, "Offset", mLayerOffset);
    mIsOffsettingLayer = false;

    return FReply::Handled().ReleaseMouseCapture();
}


#undef LOCTEXT_NAMESPACE
