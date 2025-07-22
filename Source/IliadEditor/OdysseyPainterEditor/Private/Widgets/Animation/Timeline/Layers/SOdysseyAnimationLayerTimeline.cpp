// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#include "Widgets/Animation/Timeline/Layers/SOdysseyAnimationLayerTimeline.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyStyle.h"

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
    if ( !layerStack || layerStack->GetCurrentLayer() != GetLayer())
        return borderBrush;

    return FOdysseyStyle::GetBrush("OdysseyLayerStack.CurrentLayerInactiveBackgroundBrush");
}

#undef LOCTEXT_NAMESPACE
