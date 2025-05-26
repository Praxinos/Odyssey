// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "CoreMinimal.h"
#include "SequencerCoreFwd.h"
#include "MVVM/Extensions/IOutlinerExtension.h"
#include "ISequencerTrackEditor.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SCompoundWidget.h"

class UOdysseyAnimationTimelineTrack;
class UOdysseyAnimationLayerStack;
namespace UE::Sequencer { class ISequencerTreeViewRow; }

class SOdysseyAnimationTimelineTrack
    : public SCompoundWidget
{
    SLATE_DECLARE_WIDGET(SOdysseyAnimationTimelineTrack, SCompoundWidget)

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineTrack)
    {}
        SLATE_ATTRIBUTE(UOdysseyAnimationLayerStack*, LayerStack)
    SLATE_END_ARGS()

public:
    SOdysseyAnimationTimelineTrack();

    void Construct(const FArguments& iArgs, UOdysseyAnimationTimelineTrack* iTrack, const FBuildColumnWidgetParams& iParams, TSharedPtr<ISequencer> iSequencer);
    virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    void RebuildWidgets();
    void OnLayerStackChanged();

private:
    TSlateAttribute<UOdysseyAnimationLayerStack*> mLayerStack;
    UOdysseyAnimationTimelineTrack* mTrack;
    TWeakPtr<UE::Sequencer::ISequencerTreeViewRow> mRow; //Must be a WeakPtr, otherwise the row is never killed and is still displayed when it should be hidden
    TWeakPtr<ISequencer> mSequencer;
};
