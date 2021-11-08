// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Tools/EposSequenceTools.h"

#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "EditorAssetLibrary.h"
#include "ISequencer.h"
#include "MovieScene.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "MovieSceneToolHelpers.h"
#include "Tracks/MovieSceneMaterialTrack.h"

#include "Board/BoardSequence.h"
#include "EposSequenceHelpers.h"

#define LOCTEXT_NAMESPACE "EposSequenceTools_Opacity"

//static
bool
BoardSequenceTools::CanCreateOpacity( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return false;

    if( !iSubSection.GetTrueRange().Contains( iFrameNumber ) )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    FFrameTime inner_frame = iFrameNumber * iSubSection.OuterToInnerTransform();
    FKeyOpacity opacity = ShotSequenceHelpers::GetOpacityKey( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, inner_frame.GetFrame(), iPlaneBinding );

    return !opacity.Exists();
}

//static
bool
BoardSequenceTools::CanCreateOpacity( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    FKeyOpacity opacity = ShotSequenceHelpers::GetOpacityKey( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame(), iPlaneBinding );

    return !opacity.Exists();
}

//static
bool
ShotSequenceTools::CanCreateOpacity( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    FKeyOpacity opacity = ShotSequenceHelpers::GetOpacityKey( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, iPlaneBinding );

    return !opacity.Exists();
}

//---

//static
void
BoardSequenceTools::CreateOpacity( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, FGuid iPlaneBinding, float iOpacity )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    if( !iSubSection.GetTrueRange().Contains( iFrameNumber ) )
        return;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return;

    FFrameTime inner_frame = iFrameNumber * iSubSection.OuterToInnerTransform();
    ShotSequenceTools::CreateOpacity( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, inner_frame.GetFrame(), iPlaneBinding, iOpacity );
}

//static
void
BoardSequenceTools::CreateOpacity( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding, float iOpacity )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::CreateOpacity( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame(), iPlaneBinding, iOpacity );
}

//static
void
ShotSequenceTools::CreateOpacity( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding, float iOpacity )
{
    CreateOpacity( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, iPlaneBinding, iOpacity );
}

//static
void
ShotSequenceTools::CreateOpacity( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, FGuid iPlaneBinding, float iOpacity )
{
    const FScopedTransaction transaction( LOCTEXT( "CreateOpacity", "Create an opacity" ) );

    ShotSequenceHelpers::FFindOrCreateMaterialParameterResult result_track = ShotSequenceHelpers::FindOrCreateMaterialParameterTrackAndSections( iSequencer, iSequence, iSequenceID, iPlaneBinding, iFrameNumber );
    if( !result_track.mSections.Num() )
        return;

    ShotSequenceHelpers::FFindOrCreateParameterChannelResult result_channel = ShotSequenceHelpers::FindOrCreateMaterialOpacityChannel( iSequencer, iSequence, iSequenceID, iPlaneBinding, result_track.mSections[0] );
    if( !result_channel.mChannel )
        return;

    //---

    result_track.mSections[0]->Modify();

    AddKeyToChannel( result_channel.mChannel, iFrameNumber, FMath::Clamp( iOpacity, 0.f, 1.f ), iSequencer.GetKeyInterpolation() );

    //---

    if( result_track.mTrackCreated || result_track.mSectionsCreated || result_channel.mChannelCreated )
        iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
    else
        iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

//---

//static
void
BoardSequenceTools::DeleteOpacity( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, UMovieSceneSection* iSection, const FMovieSceneChannelHandle& iChannelHandle, FKeyHandle iKeyHandle )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UBoardSequence>() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return;

    ShotSequenceTools::DeleteOpacity( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iSection, iChannelHandle, iKeyHandle );
}

//static
void
ShotSequenceTools::DeleteOpacity( ISequencer* iSequencer, UMovieSceneSection* iSection, const FMovieSceneChannelHandle& iChannelHandle, FKeyHandle iKeyHandle )
{
    DeleteOpacity( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iSection, iChannelHandle, iKeyHandle );
}

//static
void
ShotSequenceTools::DeleteOpacity( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UMovieSceneSection* iSection, const FMovieSceneChannelHandle& iChannelHandle, FKeyHandle iKeyHandle )
{
    if( !iSection )
        return;

    TMovieSceneChannelHandle<FMovieSceneFloatChannel> channel_handle = iChannelHandle.Cast<FMovieSceneFloatChannel>();
    FMovieSceneFloatChannel* material_channel = channel_handle.Get();
    if( !material_channel )
        return;

    //---

    const FScopedTransaction transaction( LOCTEXT( "DeletePlaneOpacity", "Delete plane opacity" ) );

    iSection->Modify();

    material_channel->DeleteKeys( MakeArrayView( &iKeyHandle, 1 ) );

    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}


#undef LOCTEXT_NAMESPACE
