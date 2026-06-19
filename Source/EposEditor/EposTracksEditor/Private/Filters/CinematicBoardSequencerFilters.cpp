// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "CinematicBoardSequencerFilters.h"

#include "Framework/Commands/Commands.h"
#include "Framework/Commands/UICommandInfo.h"
#include "Styles/EposTracksEditorStyle.h"

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

//static
FString
FSequencerTrackFilter_CinematicBoard::StaticName()
{
    return TEXT( "CinematicBoard" );
}

FSequencerTrackFilter_CinematicBoard::FSequencerTrackFilter_CinematicBoard( ISequencerTrackFilters& InFilterInterface, TSharedPtr<FFilterCategory> InCategory )
    : FSequencerTrackFilter_ClassType<UMovieSceneCinematicBoardTrack>( InFilterInterface, InCategory )
{
    FSequencerTrackFilter_CinematicBoardFilterCommands::Register();
}

FSequencerTrackFilter_CinematicBoard::~FSequencerTrackFilter_CinematicBoard()
{
    FSequencerTrackFilter_CinematicBoardFilterCommands::Unregister();
}

FString
FSequencerTrackFilter_CinematicBoard::GetName() const //override
{
    return StaticName();
}

FText
FSequencerTrackFilter_CinematicBoard::GetDisplayName() const //override
{
    return LOCTEXT( "SequencerTrackFilter_CinematicBoard", "Board" );
}
FSlateIcon
FSequencerTrackFilter_CinematicBoard::GetIcon() const //override
{
    return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), TEXT( "Sequencer.Tracks.CinematicBoard" ) );
    //return FSlateIcon( FAppStyle::GetAppStyleSetName(), TEXT( "Sequencer.Tracks.CinematicBoard" ) );
}

FText
FSequencerTrackFilter_CinematicBoard::GetDefaultToolTipText() const //override
{
    return LOCTEXT( "SequencerTrackFilter_CinematicBoardToolTip", "Show only Board tracks" );
}

TSharedPtr<FUICommandInfo>
FSequencerTrackFilter_CinematicBoard::GetToggleCommand() const //override
{
    return FSequencerTrackFilter_CinematicBoardFilterCommands::Get().ToggleFilter_CinematicBoard;
}

bool
FSequencerTrackFilter_CinematicBoard::SupportsSequence( UMovieSceneSequence* const InSequence ) const //override
{
    return IsSequenceTrackSupported<UMovieSceneCinematicBoardTrack>( InSequence ); //TODO: maybe also check of sequence class itself ?
}

//////////////////////////////////////////////////////////////////////////
//

void UCinematicBoardSequencerTrackFilter::AddTrackFilterExtensions( ISequencerTrackFilters& InFilterInterface, const TSharedRef<FFilterCategory>& InPreferredCategory, TArray<TSharedRef<FSequencerTrackFilter>>& InOutFilterList ) const
{
    InOutFilterList.Add( MakeShared<FSequencerTrackFilter_CinematicBoard>( InFilterInterface, InPreferredCategory ) );
}

#undef LOCTEXT_NAMESPACE
