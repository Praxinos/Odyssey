// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerFolder/SOdysseyAnimationLayerFolderTimeline.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineTreeView.h"
#include "LayerStack/Layers/LayerFolder/OdysseyAnimationLayerFolder.h"

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
