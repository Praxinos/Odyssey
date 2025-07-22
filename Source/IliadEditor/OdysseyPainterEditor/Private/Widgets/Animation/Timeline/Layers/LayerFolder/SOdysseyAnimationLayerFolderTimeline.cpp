// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#include "Widgets/Animation/Timeline/Layers/LayerFolder/SOdysseyAnimationLayerFolderTimeline.h"
#include "LayerFolder/OdysseyAnimationLayerFolder.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

void
SOdysseyAnimationLayerFolderTimeline::Construct(
    const FArguments& InArgs,
    const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
    UOdysseyAnimationLayerFolder* iLayer
)
{
    SOdysseyAnimationLayerTimeline::Construct(InArgs, iOwnerTableView, iLayer);
}

#undef LOCTEXT_NAMESPACE
