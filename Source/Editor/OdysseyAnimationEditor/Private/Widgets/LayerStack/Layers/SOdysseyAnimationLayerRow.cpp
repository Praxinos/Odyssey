// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/SOdysseyAnimationLayerRow.h"
#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineControl.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"

//PUBLIC API-----------------------------------------------------------

void
SOdysseyAnimationLayerRow::Construct(
    const FArguments& iArgs,
    const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
    UOdysseyAnimationLayer* iLayer
)
{
	mTimelinePosition = iArgs._TimelinePosition;
	mTimelineCellSelection = iArgs._TimelineCellSelection;
	mCurrentFrame = iArgs._CurrentFrame;

    SOdysseyLayerRow::Construct(
        SOdysseyLayerRow::FArguments(),
        iOwnerTableView,
		iLayer
    );
}
