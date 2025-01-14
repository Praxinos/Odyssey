// IDDN.FR.000.000000.000.S.X.0000.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#include "CinematicBoardSequencerFilters.h"

#include "Filters/SequencerTrackFilterBase.h"
#include "Framework/Commands/Commands.h"
#include "Framework/Commands/UICommandInfo.h"
#include "Styles/EposTracksEditorStyle.h"

#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CinematicBoardSequencerFilters)

#define LOCTEXT_NAMESPACE "CinematicBoardSequencerTrackFilters"

class FSequencerTrackFilter_CinematicBoardFilterCommands
    : public TCommands<FSequencerTrackFilter_CinematicBoardFilterCommands>
{
public:
    FSequencerTrackFilter_CinematicBoardFilterCommands()
        : TCommands<FSequencerTrackFilter_CinematicBoardFilterCommands>(
            TEXT( "FSequencerTrackFilter_CinematicBoard" ),
            LOCTEXT( "FSequencerTrackFilter_CinematicBoard", "Board Filters" ),
            NAME_None,
            FEposTracksEditorStyle::Get().GetStyleSetName() )
    {
    }

    TSharedPtr<FUICommandInfo> ToggleFilter_CinematicBoard;

    virtual void RegisterCommands() override
    {
        UI_COMMAND( ToggleFilter_CinematicBoard, "Toggle CinematicBoard Filter", "Toggle the filter for Board tracks", EUserInterfaceActionType::ToggleButton, FInputChord() );
    }
};

//////////////////////////////////////////////////////////////////////////
//

class FSequencerTrackFilter_CinematicBoard: public FSequencerTrackFilter_ClassType<UMovieSceneCinematicBoardTrack>
{
public:
    FSequencerTrackFilter_CinematicBoard( ISequencerTrackFilters& InFilterInterface, TSharedPtr<FFilterCategory> InCategory = nullptr )
        : FSequencerTrackFilter_ClassType<UMovieSceneCinematicBoardTrack>( InFilterInterface, InCategory )
    {
        FSequencerTrackFilter_CinematicBoardFilterCommands::Register();
    }

    virtual ~FSequencerTrackFilter_CinematicBoard() override
    {
        FSequencerTrackFilter_CinematicBoardFilterCommands::Unregister();
    }

    //~ Begin IFilter
    virtual FString GetName() const override
    {
        return TEXT( "CinematicBoard" );
    }
    //~ End IFilter

    //~ Begin FFilterBase
    virtual FText GetDisplayName() const override
    {
        return LOCTEXT( "SequencerTrackFilter_CinematicBoard", "Board" );
    }
    virtual FSlateIcon GetIcon() const override
    {
        return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), TEXT( "Sequencer.Tracks.CinematicBoard" ) );
        //return FSlateIcon( FAppStyle::GetAppStyleSetName(), TEXT( "Sequencer.Tracks.CinematicBoard" ) );
    }
    //~ End FFilterBase

    //~ Begin FSequencerTrackFilter

    virtual FText GetDefaultToolTipText() const override
    {
        return LOCTEXT( "SequencerTrackFilter_CinematicBoardToolTip", "Show only Board tracks" );
    }

    virtual TSharedPtr<FUICommandInfo> GetToggleCommand() const override
    {
        return FSequencerTrackFilter_CinematicBoardFilterCommands::Get().ToggleFilter_CinematicBoard;
    }

    virtual bool SupportsSequence( UMovieSceneSequence* const InSequence ) const override
    {
        return IsSequenceTrackSupported<UMovieSceneCinematicBoardTrack>( InSequence ); //TODO: maybe also check of sequence class itself ?
    }

    //~ End FSequencerTrackFilter
};

//////////////////////////////////////////////////////////////////////////
//

void UCinematicBoardSequencerTrackFilter::AddTrackFilterExtensions( ISequencerTrackFilters& InFilterInterface, const TSharedRef<FFilterCategory>& InPreferredCategory, TArray<TSharedRef<FSequencerTrackFilter>>& InOutFilterList ) const
{
    InOutFilterList.Add( MakeShared<FSequencerTrackFilter_CinematicBoard>( InFilterInterface, InPreferredCategory ) );
}

#undef LOCTEXT_NAMESPACE
