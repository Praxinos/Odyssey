// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/SOdysseyAnimationLayerTimeline.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

//PUBLIC API-----------------------------------------------------------

void
SOdysseyAnimationLayerTimeline::Construct(
    const FArguments& iArgs,
    const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
    UOdysseyAnimationLayer* iLayer
)
{
    ensure(iLayer);

    SOdysseyLayerRowBase::Construct(
        SOdysseyLayerRowBase::FArguments(),
        iOwnerTableView,
        iLayer
    );
}

const FSlateBrush*
SOdysseyAnimationLayerTimeline::GetBorder() const
{
    const FSlateBrush* borderBrush = SMultiColumnTableRow<UOdysseyLayer*>::GetBorder();

    if (!GetLayer())
        return borderBrush;

    UOdysseyLayerStack* layerStack = GetLayer()->GetLayerStack();
    if ( !layerStack || layerStack->CurrentLayer != GetLayer())
        return borderBrush;

    return FOdysseyStyle::GetBrush("OdysseyLayerStack.CurrentLayerInactiveBackgroundBrush");
}

#undef LOCTEXT_NAMESPACE
