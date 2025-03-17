// IDDN.FR.000.000000.000.S.X.0000.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#pragma once

#include "Filters/SequencerTrackFilterBase.h"
#include "Filters/SequencerTrackFilterExtension.h"

#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"

#include "CinematicBoardSequencerFilters.generated.h"

UCLASS()
class UCinematicBoardSequencerTrackFilter
    : public USequencerTrackFilterExtension
{
public:
    GENERATED_BODY()

    //~ Begin USequencerTrackFilterExtension
    virtual void AddTrackFilterExtensions( ISequencerTrackFilters& InFilterInterface
                                           , const TSharedRef<FFilterCategory>& InPreferredCategory
                                           , TArray<TSharedRef<FSequencerTrackFilter>>& InOutFilterList ) const override;
    //~ End USequencerTrackFilterExtension
};

class FSequencerTrackFilter_CinematicBoard
    : public FSequencerTrackFilter_ClassType<UMovieSceneCinematicBoardTrack>
{
public:
    static FString StaticName();

public:
    FSequencerTrackFilter_CinematicBoard( ISequencerTrackFilters& InFilterInterface, TSharedPtr<FFilterCategory> InCategory = nullptr );

    virtual ~FSequencerTrackFilter_CinematicBoard();

    //~ Begin IFilter
    virtual FString GetName() const override;
    //~ End IFilter

    //~ Begin FFilterBase
    virtual FText GetDisplayName() const override;
    virtual FSlateIcon GetIcon() const override;
    //~ End FFilterBase

    //~ Begin FSequencerTrackFilter

    virtual FText GetDefaultToolTipText() const override;
    virtual TSharedPtr<FUICommandInfo> GetToggleCommand() const override;
    virtual bool SupportsSequence( UMovieSceneSequence* const InSequence ) const override;
    //~ End FSequencerTrackFilter
};
