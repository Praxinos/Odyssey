// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "Templates/SubclassOf.h"
#include "ISequencer.h"
#include "MovieSceneTrack.h"
#include "MovieSceneTrackEditor.h"
#include "ISequencerSection.h"
#include "ISequencerTrackEditor.h"
#include "IContentBrowserSingleton.h"

/**
 * Class for audio sections, handles drawing of all waveform previews.
 */
class FNoteSection
    : public ISequencerSection
    , public TSharedFromThis<FNoteSection>
{
public:
    /** Constructor. */
    FNoteSection( UMovieSceneSection& InSection, TWeakPtr<ISequencer> InSequencer );

    /** Virtual destructor. */
    virtual ~FNoteSection();

public:
    // ISequencerSection interface

    virtual TSharedRef<SWidget> GenerateSectionWidget() override;
    virtual UMovieSceneSection* GetSectionObject() override;
    virtual FText GetSectionTitle() const override;
    virtual FText GetSectionToolTip() const override;
    virtual float GetSectionHeight() const override;
    virtual int32 OnPaintSection( FSequencerSectionPainter& iPainter ) const override;
    //virtual void Tick( const FGeometry& AllottedGeometry, const FGeometry& ParentGeometry, const double InCurrentTime, const float InDeltaTime ) override;

public:
    TSharedPtr<ISequencer> GetSequencer() const;

private:
    /** The section we are visualizing. */
    UMovieSceneSection& mSection;

    TWeakPtr<ISequencer> mSequencer;
};
