// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "TrackEditors/SubTrackEditorBase.h"
#include "Widgets/Layout/SBox.h"

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
    UOdysseyAnimationComponent* GetComponent() const;
    EVisibility GetLayersVisibility() const;

private:
    TWeakPtr<ISequencer> mSequencer;
    UOdysseyAnimationComponent* mComponent; //used to remove callbacks
    TSharedPtr<SWidget> mSectionWidget;
    UOdysseyAnimationTimelineSection* mSection;

    FFrameNumber mInitialStartOffsetDuringResize;
    FFrameNumber mInitialStartTimeDuringResize;
};
