// IDDN.FR.000.000000.000.S.X.0000.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#include "NoteSequencerFilters.h"

#include "Framework/Commands/Commands.h"
#include "Framework/Commands/UICommandInfo.h"
#include "LevelSequence.h"

#include "Board/BoardSequence.h"
#include "Styles/EposTracksEditorStyle.h"
#include "Shot/ShotSequence.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NoteSequencerFilters)

#define LOCTEXT_NAMESPACE "NoteSequencerTrackFilters"

class FSequencerTrackFilter_NoteFilterCommands
    : public TCommands<FSequencerTrackFilter_NoteFilterCommands>
{
public:
    FSequencerTrackFilter_NoteFilterCommands()
        : TCommands<FSequencerTrackFilter_NoteFilterCommands>(
            TEXT( "FSequencerTrackFilter_Note" ),
            LOCTEXT( "FSequencerTrackFilter_Note", "Note Filters" ),
            NAME_None,
            FEposTracksEditorStyle::Get().GetStyleSetName() )
    {
    }

    TSharedPtr<FUICommandInfo> ToggleFilter_Note;

    virtual void RegisterCommands() override
    {
        UI_COMMAND( ToggleFilter_Note, "Toggle Note Filter", "Toggle the filter for Note tracks", EUserInterfaceActionType::ToggleButton, FInputChord() );
    }
};

//////////////////////////////////////////////////////////////////////////
//

//static
FString
FSequencerTrackFilter_Note::StaticName()
{
    return TEXT( "Note" );
}

FSequencerTrackFilter_Note::FSequencerTrackFilter_Note( ISequencerTrackFilters& InFilterInterface, TSharedPtr<FFilterCategory> InCategory )
    : FSequencerTrackFilter_ClassType<UMovieSceneNoteTrack>( InFilterInterface, InCategory )
{
    FSequencerTrackFilter_NoteFilterCommands::Register();
}

FSequencerTrackFilter_Note::~FSequencerTrackFilter_Note()
{
    FSequencerTrackFilter_NoteFilterCommands::Unregister();
}

FString
FSequencerTrackFilter_Note::GetName() const //override
{
    return StaticName();
}

FText
FSequencerTrackFilter_Note::GetDisplayName() const //override
{
    return LOCTEXT( "SequencerTrackFilter_Note", "Note" );
}
FSlateIcon
FSequencerTrackFilter_Note::GetIcon() const //override
{
    return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), TEXT( "Sequencer.Tracks.Note" ) );
}

FText
FSequencerTrackFilter_Note::GetDefaultToolTipText() const //override
{
    return LOCTEXT( "SequencerTrackFilter_NoteToolTip", "Show only Note tracks" );
}

TSharedPtr<FUICommandInfo>
FSequencerTrackFilter_Note::GetToggleCommand() const //override
{
    return FSequencerTrackFilter_NoteFilterCommands::Get().ToggleFilter_Note;
}

bool
FSequencerTrackFilter_Note::SupportsSequence( UMovieSceneSequence* const InSequence ) const //override
{
    return InSequence->IsA<UBoardSequence>() || InSequence->IsA<UShotSequence>() || InSequence->IsA<ULevelSequence>();
    //return IsSequenceTrackSupported<UMovieSceneNoteTrack>( InSequence ); //TODO: maybe also check of sequence class itself ?
}

//////////////////////////////////////////////////////////////////////////
//

void UNoteSequencerTrackFilter::AddTrackFilterExtensions( ISequencerTrackFilters& InFilterInterface, const TSharedRef<FFilterCategory>& InPreferredCategory, TArray<TSharedRef<FSequencerTrackFilter>>& InOutFilterList ) const
{
    InOutFilterList.Add( MakeShared<FSequencerTrackFilter_Note>( InFilterInterface, InPreferredCategory ) );
}

#undef LOCTEXT_NAMESPACE
