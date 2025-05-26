// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "TrackEditors/SubTrackEditorBase.h"
#include "Widgets/Layout/SBox.h"
#include "OdysseyAnimationPlayer.h"

class UOdysseyAnimation;
class UOdysseyAnimationTimelineSection;
class UOdysseyAnimationComponent;
class UOdysseyLayer;

class FOdysseyAnimationTimelineSectionEditor
    : public ISequencerSection
    , public TSharedFromThis<FOdysseyAnimationTimelineSectionEditor>
{
public:
    FOdysseyAnimationTimelineSectionEditor(TSharedPtr<ISequencer> InSequencer, UOdysseyAnimationTimelineSection* InSection);
    virtual ~FOdysseyAnimationTimelineSectionEditor();

public:
    virtual UMovieSceneSection* GetSectionObject() override;
    virtual float GetSectionHeight( const UE::Sequencer::FViewDensityInfo& ViewDensity ) const override;
    virtual float GetSectionGripHeight(float iSectionHeight) const override;
    virtual FText GetSectionTitle() const override;
    virtual FText GetSectionToolTip() const override;
    virtual TSharedRef<SWidget> GenerateSectionWidget() override;
    virtual bool IsReadOnly() const override;
    virtual int32 OnPaintSection( FSequencerSectionPainter& InPainter ) const override;
    virtual void BeginResizeSection() override;
    virtual void ResizeSection(ESequencerSectionResizeMode ResizeMode, FFrameNumber ResizeTime) override;

    static float GetCollapsedSectionHeight();

public:
    TSharedPtr<ISequencer> GetSequencer() const;

private:
    EVisibility GetLayersVisibility() const;
    UOdysseyAnimation* GetAnimation() const;
    EOdysseyAnimationPlayerPostBehaviour GetPreBehaviour() const;
    EOdysseyAnimationPlayerPostBehaviour GetPostBehaviour() const;
    FFrameNumber GetStartFrameOffset() const;

    void OnPreBehaviourChanged(EOdysseyAnimationPlayerPostBehaviour iValue);
    void OnPostBehaviourChanged(EOdysseyAnimationPlayerPostBehaviour iValue);

private:
    TWeakPtr<ISequencer> mSequencer;
    UOdysseyAnimationComponent* mComponent; //used to remove callbacks
    TWeakPtr<SWidget> mSectionWidget;
    UOdysseyAnimationTimelineSection* mSection;

    FFrameNumber mInitialStartOffsetDuringResize;
    FFrameNumber mInitialStartTimeDuringResize;
};
