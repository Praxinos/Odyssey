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
}

void SOdysseyAnimationLayerImageRasterTimeline::Construct(
    const FArguments& InArgs,
    TSharedPtr<SOdysseyAnimationLayerStack> iLayerStackWidget,
    UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
)
{
    ensure(iAnimationLayerImageRaster);
    
	SOdysseyAnimationTimelineWidget::Construct(
		SOdysseyAnimationTimelineWidget::FArguments(),
        iLayerStackWidget
	);
    
    mAnimationLayerImageRaster = iAnimationLayerImageRaster;
    UOdysseyAnimationLayerImageRaster::OnCellsChanged().AddRaw(this, &SOdysseyAnimationLayerImageRasterTimeline::OnCellsChanged);

    ChildSlot
    [
        SAssignNew(mListView, SListView<TSharedPtr<FOdysseyAnimationCell>>)
        .ListItemsSource(&mAnimationLayerImageRaster->GetCells())
        .OnGenerateRow(this, &SOdysseyAnimationLayerImageRasterTimeline::OnGenerateRow)
        //.ExternalScrollbar() //Should I use this
        .Orientation(Orient_Horizontal)
    ];
}

TSharedRef<ITableRow>
SOdysseyAnimationLayerImageRasterTimeline::OnGenerateRow(TSharedPtr<FOdysseyAnimationCell> iCell, const TSharedRef<STableViewBase>& iOwnerTable)
{
    check(iCell);

    if (iCell->GetType() == FOdysseyAnimationCellImageRaster::StaticType())
    {
        //TODO: Create a SOdysseyAnimationLayerImageRasterCell widget
        return SNew(STableRow<TSharedPtr<FOdysseyAnimationCell>>, iOwnerTable)
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

    return SNew(STableRow<TSharedPtr<FOdysseyAnimationCell>>, iOwnerTable);
}

void
SOdysseyAnimationLayerImageRasterTimeline::OnCellsChanged(UOdysseyAnimationLayerImageRaster* iLayer)
{
    if (iLayer != mAnimationLayerImageRaster)
        return;

    mListView->RebuildList();
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
