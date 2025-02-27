// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023
#pragma once

#include "CoreMinimal.h"
#include "SequencerCoreFwd.h"
#include "MVVM/Extensions/IOutlinerExtension.h"
#include "ISequencerTrackEditor.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SCompoundWidget.h"

class UOdysseyAnimationComponent;
class UOdysseyAnimationTimelineSection;
class FOdysseyPainterEditorAnimationTimelinePosition;

class SOdysseyAnimationTimelineSection
    : public SCompoundWidget
{
public:
    SOdysseyAnimationTimelineSection();

    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineSection)
    {}
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& iArgs, TSharedPtr<ISequencer> iSequencer, UOdysseyAnimationTimelineSection* iSection, UOdysseyAnimationComponent* iComponent);

protected:
    virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
    void RebuildWidgets();
    EVisibility GetLayersVisibility() const;

    void OnAnimationChanged();
    void OnPlayerChanged();
    void OnModeChanged();

private:
    TWeakPtr<ISequencer> mSequencer;
    UOdysseyAnimationTimelineSection* mSection;
    UOdysseyAnimationComponent* mComponent;
    TSharedRef<FOdysseyPainterEditorAnimationTimelinePosition> mTimelinePosition;
};
