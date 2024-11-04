// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "SequencerCoreFwd.h"
#include "MVVM/Extensions/IOutlinerExtension.h"

class UOdysseyLayer;
class UOdysseyAnimationComponent;
class UOdysseyAnimationComponentTrack;
class ISequencerTreeViewRow;

class SOdysseyAnimationComponentTrack
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationComponentTrack)
    {}
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& iArgs, UOdysseyAnimationComponent* iComponent, UOdysseyAnimationComponentTrack* iTrack, const FBuildColumnWidgetParams& iParams);

private:
	void RebuildWidgets();

	void OnDisplayLayersCheckBoxStateChanged(ECheckBoxState iState);
	ECheckBoxState GetDisplayLayersCheckBoxState() const;
	EVisibility GetLayersVisibility() const;
	FMargin GetDisplayLayersPadding() const;

	void OnAnimationChanged();
	void OnPlayerChanged();
	void OnModeChanged();

	FOptionalSize GetTreeViewHeight() const;

private:
	UOdysseyAnimationComponent* mComponent;
	UOdysseyAnimationComponentTrack* mTrack;
	TWeakPtr<UE::Sequencer::ISequencerTreeViewRow> mRow; //Must be a WeakPtr, otherwise the row is never killed and is still displayed when it should be hidden
};