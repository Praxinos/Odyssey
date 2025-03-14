// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023
#pragma once

#include "Widgets/Animation/Timeline/Layers/SOdysseyAnimationLayerTimeline.h"

class UOdysseyAnimationLayerFolder;
class SOdysseyAnimationTimelineTreeView;
/**
 * Implements a layer row widget
 */
class SOdysseyAnimationLayerFolderTimeline
    : public SOdysseyAnimationLayerTimeline
{
public:
    // Construction / Destruction
    void Construct(
        const FArguments& iArgs,
        const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
        UOdysseyAnimationLayerFolder* iLayer
    );
};
