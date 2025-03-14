// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023
#pragma once

#include "CoreMinimal.h"
#include "SequencerCoreFwd.h"
#include "MVVM/Extensions/IOutlinerExtension.h"
#include "ISequencerTrackEditor.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SCompoundWidget.h"

class UOdysseyLayer;
class UOdysseyAnimationComponent;
class UOdysseyAnimationTimelineTrack;
namespace UE::Sequencer { class ISequencerTreeViewRow; }

class SOdysseyAnimationTimelineTrack
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineTrack)
    {}
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& iArgs, UOdysseyAnimationComponent* iComponent, UOdysseyAnimationTimelineTrack* iTrack, const FBuildColumnWidgetParams& iParams, TSharedPtr<ISequencer> iSequencer);
    virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    void RebuildWidgets();

    void OnAnimationChanged();
    void OnPlayerChanged();
    void OnModeChanged();

private:
    UOdysseyAnimationComponent* mComponent;
    UOdysseyAnimationTimelineTrack* mTrack;
    TWeakPtr<UE::Sequencer::ISequencerTreeViewRow> mRow; //Must be a WeakPtr, otherwise the row is never killed and is still displayed when it should be hidden
    TWeakPtr<ISequencer> mSequencer;
};
