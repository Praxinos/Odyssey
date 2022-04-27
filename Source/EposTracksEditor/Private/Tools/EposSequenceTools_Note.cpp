// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/EposSequenceTools.h"

#include "Editor.h"
#include "ISequencer.h"
#include "MovieScene.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "MovieSceneTimeHelpers.h"
#include "Sections/MovieSceneSubSection.h"

#include "EposSequenceHelpers.h"
#include "NoteTrack/MovieSceneNoteTrack.h"
#include "StoryNote.h"
#include "Tools/ResourceAssetTools.h"

#define LOCTEXT_NAMESPACE "EposSequenceTools_Note"

//static
void
BoardSequenceTools::CreateNote( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    FFrameTime inner_frame = iFrameNumber * iSubSection.OuterToInnerTransform();
    ShotSequenceTools::CreateNote( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, inner_frame.GetFrame() );
}

//static
void
ShotSequenceTools::CreateNote( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    ShotSequenceTools::CreateNote( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
}

//static
void
ShotSequenceTools::CreateNote( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    UMovieScene* movie_scene = iSequence->GetMovieScene();

    //---

    const FScopedTransaction Transaction( LOCTEXT( "AddNote_Transaction", "Add Note" ) );

    cTemporarySwitchInner switch_to( iSequencer, iSequenceID );

    UStoryNote* note = ProjectAssetTools::CreateNote( iSequencer, iSequencer.GetRootMovieSceneSequence(), iSequence );
    if( !note )
        return;

    //TODO: maybe make an internal CreateNoteInternal() to manage all track creation/section size/duplication/... like CinematicBoardTrackTools::InsertSequence() does ?

    movie_scene->Modify();
    iSequence->Modify();

    auto newTrack = movie_scene->AddMasterTrack<UMovieSceneNoteTrack>();
    ensure( newTrack );

    TRange<FFrameNumber> range = movie_scene->GetPlaybackRange();
    UMovieSceneSection* NewSection = newTrack->AddNewNote( note, range.GetLowerBoundValue(), UE::MovieScene::DiscreteSize( range ) );

    iSequencer.OnAddTrack( newTrack, FGuid() );

    iSequencer.EmptySelection();
    iSequencer.SelectSection( NewSection );
    iSequencer.ThrobSectionSelection();

    // It's immediate, because otherwise the newly created note track will have a sorting order of -1 when the list view (in board section) will be rebuild
    // Immediate will refresh through a tree update of current sequence in the sequencer
    // The current sequence may be 'temporary' due to the switch_to
    //
    // All other not-immediate notifications won't work, as they will set flags but once they will be process,
    // it will be after this function, and so after the switch_to have set the 'real' current sequence
    //
    //TODO: check why it's not the case for the planes ...
    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::RefreshAllImmediately );
}

//---

//static
void
BoardSequenceTools::DeleteNote( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TWeakObjectPtr<UMovieSceneSection> iNoteSection )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::DeleteNote( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iNoteSection );
}

//static
void
ShotSequenceTools::DeleteNote( ISequencer* iSequencer, TWeakObjectPtr<UMovieSceneSection> iNoteSection )
{
    ShotSequenceTools::DeleteNote( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iNoteSection );
}

//static
void
ShotSequenceTools::DeleteNote( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TWeakObjectPtr<UMovieSceneSection> iNoteSection )
{
    UMovieScene* movie_scene = iSequence->GetMovieScene();

    if( !iNoteSection.IsValid() )
        return;

    UMovieSceneTrack* track = iNoteSection->GetTypedOuter<UMovieSceneTrack>();
    if( !track )
        return;

    //---

    const FScopedTransaction Transaction( LOCTEXT( "DeleteNote_Transaction", "Delete Note" ) );

    movie_scene->Modify();
    iSequence->Modify();

    iSequencer.EmptySelection();

    // Like in FSequencer::DeleteSections()
    track->SetFlags( RF_Transactional );
    track->Modify();
    track->RemoveSection( *iNoteSection );

    if( !track->GetAllSections().Num() )
        movie_scene->RemoveMasterTrack( *track );

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::RefreshAllImmediately );
}

//---

//static
TArray<TWeakObjectPtr<UMovieSceneNoteSection>>
BoardSequenceTools::GetAllNotes( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return TArray<TWeakObjectPtr<UMovieSceneNoteSection>>();

    return ShotSequenceTools::GetAllNotes( *iSequencer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
TArray<TWeakObjectPtr<UMovieSceneNoteSection>>
ShotSequenceTools::GetAllNotes( ISequencer* iSequencer )
{
    return ShotSequenceTools::GetAllNotes( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID() );
}

//static
TArray<TWeakObjectPtr<UMovieSceneNoteSection>>
ShotSequenceTools::GetAllNotes( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    return EposSequenceHelpers::GetNotes( iSequencer, iSequence, iSequenceID );
}

#undef LOCTEXT_NAMESPACE
