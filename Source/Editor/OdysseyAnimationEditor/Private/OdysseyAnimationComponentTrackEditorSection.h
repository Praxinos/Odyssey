// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "TrackEditors/SubTrackEditorBase.h"

class UOdysseyAnimationComponentSection;

class FOdysseyAnimationComponentTrackEditorSection
	: public TSubSectionMixin<>
	, public TSharedFromThis<FOdysseyAnimationComponentTrackEditorSection>
{
public:
	FOdysseyAnimationComponentTrackEditorSection(TSharedPtr<ISequencer> InSequencer, UOdysseyAnimationComponentSection* InSection);
	virtual ~FOdysseyAnimationComponentTrackEditorSection();

public:
	virtual float GetSectionHeight( const UE::Sequencer::FViewDensityInfo& ViewDensity ) const override;
	virtual float GetSectionGripHeight(float iSectionHeight) const override;
	virtual FText   GetSectionTitle() const override;
    virtual FText   GetSectionToolTip() const override;
	virtual TSharedRef<SWidget> GenerateSectionWidget() override;

	virtual void Tick( const FGeometry& AllottedGeometry, const FGeometry& ClippedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

private:
	float GetLayerHeight(UOdysseyLayer* iLayer) const;

private:
	UOdysseyAnimationComponentSection* mSection;
	TSharedRef<FOdysseyAnimationEditorTimelinePosition> mTimelinePosition;
};
