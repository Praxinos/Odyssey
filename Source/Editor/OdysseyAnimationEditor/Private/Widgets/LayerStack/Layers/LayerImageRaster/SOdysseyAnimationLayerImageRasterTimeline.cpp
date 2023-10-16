// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterTimeline.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerImageRasterTimeline"

SOdysseyAnimationLayerImageRasterTimeline::~SOdysseyAnimationLayerImageRasterTimeline()
{
}

SOdysseyAnimationLayerImageRasterTimeline::SOdysseyAnimationLayerImageRasterTimeline()
{
}

void
SOdysseyAnimationLayerImageRasterTimeline::Construct(
    const FArguments& InArgs,
    FOdysseyAnimationEditorExtension* iExtension,
    UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
)
{
    ensure(iAnimationLayerImageRaster);
    SOdysseyAnimationLayerImageTimeline::Construct(iExtension, iAnimationLayerImageRaster);
}

TSharedRef<FOdysseyAnimationCell>
SOdysseyAnimationLayerImageRasterTimeline::OnCreateCell()
{
    UOdysseyAnimation* animation = mLayer->GetAnimation();
    return FOdysseyAnimationCellImageRaster::Create(Cast<UOdysseyAnimationLayerImageRaster>(mLayer), 1, animation->Width(), animation->Height(), animation->Format());
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageRasterTimeline::OnGenerateCellWidget(TSharedPtr<FOdysseyAnimationCell> iCell)
{
    return SNew(SOdysseyAnimationLayerImageRasterCell);
}

FReply
SOdysseyAnimationLayerImageRasterTimeline::OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    UOdysseyLayerStack* layerStack = mLayer->GetLayerStack();
    if (!layerStack)
        return SOdysseyAnimationLayerImageTimeline::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);

    if (layerStack->CurrentLayer.Get() == mLayer)
        return SOdysseyAnimationLayerImageTimeline::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);

    FOdysseyObjectEditorUtils::SetPropertyValue(layerStack, "CurrentLayer", TSoftObjectPtr<UOdysseyLayer>(mLayer));

    return SOdysseyAnimationLayerImageTimeline::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);
}

#undef LOCTEXT_NAMESPACE
