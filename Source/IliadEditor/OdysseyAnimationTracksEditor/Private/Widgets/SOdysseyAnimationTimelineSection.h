// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationPlayer.h"

#include "CoreMinimal.h"
#include "SequencerCoreFwd.h"
#include "MVVM/Extensions/IOutlinerExtension.h"
#include "ISequencerTrackEditor.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SCompoundWidget.h"

class UOdysseyAnimationTimelineSection;
class FOdysseyPainterEditorAnimationTimelinePosition;
class UOdysseyLayerCell;
class UOdysseyAnimation;
class UOdysseyAnimationPlayer;

class SOdysseyAnimationTimelineSection
    : public SCompoundWidget
{
    SLATE_DECLARE_WIDGET(SOdysseyAnimationTimelineSection, SCompoundWidget)

    DECLARE_DELEGATE_OneParam(FOnBehaviourChanged, EOdysseyAnimationPlayerPostBehaviour)

public:
    SOdysseyAnimationTimelineSection();

    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineSection)
        : _Animation(nullptr)
        , _StartFrameOffset(FFrameNumber(0))
    {}
        SLATE_ATTRIBUTE(UOdysseyAnimation*, Animation)
        SLATE_ATTRIBUTE(EOdysseyAnimationPlayerPostBehaviour, PreBehaviour)
        SLATE_ATTRIBUTE(EOdysseyAnimationPlayerPostBehaviour, PostBehaviour)
        SLATE_ATTRIBUTE(FFrameNumber, StartFrameOffset)
        SLATE_EVENT(FOnBehaviourChanged, OnPreBehaviourChanged)
        SLATE_EVENT(FOnBehaviourChanged, OnPostBehaviourChanged)
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& iArgs, TSharedPtr<ISequencer> iSequencer, UOdysseyAnimationComponent* iComponent, UOdysseyAnimationTimelineSection* iSection);

protected:
    virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
    void RebuildWidgets();
    EVisibility GetLayersVisibility() const;

    void OnAnimationChanged();

    void OnActivateOutOfPegs(UOdysseyLayerCell* iCell);
    void OnInactivateOutOfPegs();
    ECheckBoxState OnIsOutOfPegsChecked(UOdysseyLayerCell* iCell);

    FOptionalSize GetPreBehaviourWidth() const;
    FOptionalSize GetPostBehaviourWidth() const;

    const FSlateBrush* GetPrePostBehaviourBrush() const;

    int GetCurrentFrame() const;

    void OnPrebehaviourComboBoxChanged(int32 iValue, ESelectInfo::Type iSelectInfo);
    void OnPostbehaviourComboBoxChanged(int32 iValue, ESelectInfo::Type iSelectInfo);

private:
    TSlateAttribute<UOdysseyAnimation*> mAnimation;
    TAttribute<FFrameNumber> mStartFrameOffset;
    TAttribute<EOdysseyAnimationPlayerPostBehaviour> mPreBehaviour;
    TAttribute<EOdysseyAnimationPlayerPostBehaviour> mPostBehaviour;

    FOnBehaviourChanged mOnPreBehaviourChanged;
    FOnBehaviourChanged mOnPostBehaviourChanged;

    TWeakPtr<ISequencer> mSequencer;
    UOdysseyAnimationComponent* mComponent;
    UOdysseyAnimationTimelineSection* mSection;
    TSharedRef<FOdysseyPainterEditorAnimationTimelinePosition> mTimelinePosition;
};
