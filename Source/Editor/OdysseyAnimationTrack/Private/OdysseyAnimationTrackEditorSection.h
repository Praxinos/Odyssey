// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "TrackEditors/SubTrackEditorBase.h"

class UOdysseyAnimationComponentSection;
class FOdysseyAnimationEditorTimelinePosition;

class FOdysseyAnimationTrackEditorSection
	: public TSubSectionMixin<>
	, public TSharedFromThis<FOdysseyAnimationTrackEditorSection>
{
public:
	FOdysseyAnimationTrackEditorSection(TSharedPtr<ISequencer> InSequencer, UOdysseyAnimationComponentSection* InSection);
	virtual ~FOdysseyAnimationTrackEditorSection();

public:
	virtual float GetSectionHeight( const UE::Sequencer::FViewDensityInfo& ViewDensity ) const override;
	virtual float GetSectionGripHeight(float iSectionHeight) const override;
	virtual FText   GetSectionTitle() const override;
    virtual FText   GetSectionToolTip() const override;
	virtual TSharedRef<SWidget> GenerateSectionWidget() override;
	virtual void BeginResizeSection() override;
	virtual void ResizeSection(ESequencerSectionResizeMode ResizeMode, FFrameNumber ResizeTime) override;

	virtual void Tick( const FGeometry& AllottedGeometry, const FGeometry& ClippedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

	static float GetCollapsedSectionHeight();
	static float GetUncollapsedSectionHeight(UOdysseyAnimationComponent* iComponent);
	static float GetTreeViewHeight(UOdysseyAnimationComponent* iComponent);

private:
	static float GetLayerHeight(UOdysseyLayer* iLayer);
	void RebuildSectionWidget();

	UOdysseyAnimationComponent* GetComponent() const;
	EVisibility GetLayersVisibility() const;
	
	void OnAnimationChanged();
	void OnPlayerChanged();
	void OnModeChanged();

private:
	UOdysseyAnimationComponent* mComponent; //used to remove callbacks
	TSharedPtr<SBox> mSectionWidget;
	UOdysseyAnimationComponentSection* mSection;
	TSharedRef<FOdysseyAnimationEditorTimelinePosition> mTimelinePosition;


	FFrameNumber mInitialStartOffsetDuringResize;
	FFrameNumber mInitialStartTimeDuringResize;
};
