// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/LayerStack/Layers/SOdysseyAnimationLayerTimeline.h"

class UOdysseyAnimationLayerFolder;
class SOdysseyAnimationTimelineTreeView;
/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerFolderTimeline
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