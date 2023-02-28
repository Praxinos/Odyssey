// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/LayerImageRaster/SOdysseyAnimationLayerImageRasterTimeline.h"
#include "LayerStack/OdysseyAnimationLayerImageRaster.h"
#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerImageRasterTimeline"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
SOdysseyAnimationLayerImageRasterTimeline::~SOdysseyAnimationLayerImageRasterTimeline()
{
    UOdysseyAnimationLayerImageRaster::OnCellsChanged().RemoveAll(this);
}

void SOdysseyAnimationLayerImageRasterTimeline::Construct(const FArguments& InArgs, UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster)
{
    ensure(iAnimationLayerImageRaster);
    mAnimationLayerImageRaster = iAnimationLayerImageRaster;
    UOdysseyAnimationLayerImageRaster::OnCellsChanged().AddRaw(this, &SOdysseyAnimationLayerImageRasterTimeline::OnCellsChanged);

    ChildSlot
    [
        SAssignNew(mListView, SListView<TSharedPtr<FOdysseyRasterBlock>>)
        .ListItemsSource(&mAnimationLayerImageRaster->GetRasterBlocks())
        .OnGenerateRow(this, &SOdysseyAnimationLayerImageRasterTimeline::OnGenerateRow)
        //.ExternalScrollbar() //Should I use this
        .Orientation(Orient_Horizontal)
    ];
}

TSharedRef<ITableRow>
SOdysseyAnimationLayerImageRasterTimeline::OnGenerateRow(TSharedPtr<FOdysseyRasterBlock> iRasterBlock, const TSharedRef<STableViewBase>& iOwnerTable)
{
    check(iRasterBlock);
    return SNew(STableRow<TSharedPtr<FOdysseyRasterBlock>>, iOwnerTable)
        [
            SNew(SBox)
		    .HeightOverride(50.f) //TODO: Move the size in an other widget or directly in the view or track itself
		    .WidthOverride(50.f)
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

void
SOdysseyAnimationLayerImageRasterTimeline::OnCellsChanged(UOdysseyAnimationLayerImageRaster* iLayer)
{
    if (iLayer != mAnimationLayerImageRaster)
        return;

    mListView->RebuildList();
}

#undef LOCTEXT_NAMESPACE
