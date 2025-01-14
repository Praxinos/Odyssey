// IDDN.FR.000.000000.000.S.X.0000.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#include "NoteSequencerFilters.h"

#include "Filters/SequencerTrackFilterBase.h"
#include "Framework/Commands/Commands.h"
#include "Framework/Commands/UICommandInfo.h"
#include "LevelSequence.h"

#include "Board/BoardSequence.h"
#include "NoteTrack/MovieSceneNoteTrack.h"
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

class FSequencerTrackFilter_Note: public FSequencerTrackFilter_ClassType<UMovieSceneNoteTrack>
{
public:
    FSequencerTrackFilter_Note( ISequencerTrackFilters& InFilterInterface, TSharedPtr<FFilterCategory> InCategory = nullptr )
        : FSequencerTrackFilter_ClassType<UMovieSceneNoteTrack>( InFilterInterface, InCategory )
    {
        FSequencerTrackFilter_NoteFilterCommands::Register();
    }

    virtual ~FSequencerTrackFilter_Note() override
    {
        FSequencerTrackFilter_NoteFilterCommands::Unregister();
    }

    //~ Begin IFilter
    virtual FString GetName() const override
    {
        return TEXT( "Note" );
    }
    //~ End IFilter

    //~ Begin FFilterBase
    virtual FText GetDisplayName() const override
    {
        return LOCTEXT( "SequencerTrackFilter_Note", "Note" );
    }
    virtual FSlateIcon GetIcon() const override
    {
        return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), TEXT( "Sequencer.Tracks.Note" ) );
    }
    //~ End FFilterBase

    //~ Begin FSequencerTrackFilter

    virtual FText GetDefaultToolTipText() const override
    {
        return LOCTEXT( "SequencerTrackFilter_NoteToolTip", "Show only Note tracks" );
    }

    virtual TSharedPtr<FUICommandInfo> GetToggleCommand() const override
    {
        return FSequencerTrackFilter_NoteFilterCommands::Get().ToggleFilter_Note;
    }

    virtual bool SupportsSequence( UMovieSceneSequence* const InSequence ) const override
    {
        return InSequence->IsA<UBoardSequence>() || InSequence->IsA<UShotSequence>() || InSequence->IsA<ULevelSequence>();
        //return IsSequenceTrackSupported<UMovieSceneNoteTrack>( InSequence ); //TODO: maybe also check of sequence class itself ?
    }

    //~ End FSequencerTrackFilter
};

//////////////////////////////////////////////////////////////////////////
//

void UNoteSequencerTrackFilter::AddTrackFilterExtensions( ISequencerTrackFilters& InFilterInterface, const TSharedRef<FFilterCategory>& InPreferredCategory, TArray<TSharedRef<FSequencerTrackFilter>>& InOutFilterList ) const
{
    InOutFilterList.Add( MakeShared<FSequencerTrackFilter_Note>( InFilterInterface, InPreferredCategory ) );
}

#undef LOCTEXT_NAMESPACE
