// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/LayerImageRaster/SOdysseyAnimationLayerImageRasterTimeline.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerImageRasterTimeline"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
SOdysseyAnimationLayerImageRasterTimeline::~SOdysseyAnimationLayerImageRasterTimeline()
{
    UOdysseyAnimationLayerImageRaster::OnCellsChanged().RemoveAll(this);

    //
    TSharedPtr<SOdysseyAnimationLayerStack> layerstackWidget = GetLayerStackWidget();
    if (layerstackWidget)
    {
        layerstackWidget->OnTimelineOffsetChanged().RemoveAll(this);
    }
}

SOdysseyAnimationLayerImageRasterTimeline::SOdysseyAnimationLayerImageRasterTimeline()
    : mIsRefreshPending(false)
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

void
SOdysseyAnimationLayerImageRasterTimeline::RefreshWidgets()
{

    ClearChildren();

    TArray<TSharedPtr<FOdysseyAnimationCell>>& cells = mAnimationLayerImageRaster->GetCells();
    for (TSharedPtr<FOdysseyAnimationCell> cell : cells)
    {
        AddChild()
        [
            SNew(SBox)
            .WidthOverride_Raw(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellWidth)
            .HeightOverride_Raw(this, &SOdysseyAnimationLayerImageRasterTimeline::GetCellHeight)
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
            [
                SNew(SBorder)
                .BorderImage(FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameBackground"))
                .BorderBackgroundColor(FLinearColor(1.f, 1.f, 1.f))
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Fill)
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

FOptionalSize
SOdysseyAnimationLayerImageRasterTimeline::GetCellWidth() const
{
    return GetLayerStackWidget()->GetTimelineFrameWidth();
}

FOptionalSize
SOdysseyAnimationLayerImageRasterTimeline::GetCellHeight() const
{
    return GetLayerStackWidget()->GetTimelineBaseFrameSize();
}

#undef LOCTEXT_NAMESPACE
