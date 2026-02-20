// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "EposSequenceHelpers.h"

#include "Bindings/MovieSceneSpawnableBinding.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "Evaluation/MovieSceneSequenceHierarchy.h"
#include "Materials/MaterialInstanceConstant.h"
#include "IMovieScenePlayer.h"
#include "LevelSequence.h"
#include "MoviePipeline.h"
#include "MoviePipelineBlueprintLibrary.h"
#include "MoviePipelinePrimaryConfig.h"
#include "MovieScene.h"
#include "MovieSceneCommonHelpers.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "MovieSceneSequenceVisitor.h"
#include "Sections/MovieSceneSubSection.h"
#include "Sections/MovieScene3DTransformSection.h"
#include "Sections/MovieSceneBoolSection.h"
#include "Sections/MovieSceneParameterSection.h"
#include "Sections/MovieSceneComponentMaterialParameterSection.h"
#include "Sections/MovieScenePrimitiveMaterialSection.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Tracks/MovieSceneMaterialTrack.h"
#include "Tracks/MovieScenePrimitiveMaterialTrack.h"
#include "Tracks/MovieSceneVisibilityTrack.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "NoteTrack/MovieSceneNoteTrack.h"
#include "NoteTrack/MovieSceneNoteSection.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationActor.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationTimelineSection.h"
#include "OdysseyAnimationTimelineTrack.h"
#include "PlaneActor.h"
#include "Shot/ShotSequence.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"
#include "StoryNote.h"

#define LOCTEXT_NAMESPACE "EposSequenceHelpers"


//static
UMovieSceneSequence*
BoardSequenceHelpers::FindSequenceOfSubSection( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceID& oSequenceID )
{
    oSequenceID = MovieSceneSequenceID::Invalid;

    const FMovieSceneSequenceHierarchy* hierarchy = iPlayer.GetEvaluationTemplate().GetHierarchy();
    if( !hierarchy )
        return nullptr;

    FMovieSceneSequenceID subsequence_id = MovieSceneSequenceID::Invalid;
    for( const TTuple<FMovieSceneSequenceID, FMovieSceneSubSequenceData>& Pair : hierarchy->AllSubSequenceData() )
    {
        if( Pair.Value.DeterministicSequenceID == iSubSection.GetSequenceID() )
        {
            subsequence_id = Pair.Key;
            break;
        }
    }

    const FMovieSceneSequenceHierarchyNode* node = hierarchy->FindNode( subsequence_id );
    if( !node )
        return nullptr;

    oSequenceID = node->ParentID;

    if( oSequenceID == MovieSceneSequenceID::Root )
        return iPlayer.GetEvaluationTemplate().GetRootSequence();
    else
        return hierarchy->FindSubSequence( oSequenceID );

}

BoardSequenceHelpers::FInnerSequenceResult
BoardSequenceHelpers::GetInnerSequence( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceId )
{
    FInnerSequenceResult result;

    if( !IsValid( &iSubSection ) )
        return result;

    result.mInnerSequence = iSubSection.GetSequence();
    result.mInnerMovieScene = result.mInnerSequence ? result.mInnerSequence->GetMovieScene() : nullptr;

    //---

    const FMovieSceneSequenceID             thisSequenceID = iSequenceId;
    const FMovieSceneSequenceID             targetSequenceID = iSubSection.GetSequenceID();
    const FMovieSceneSequenceHierarchy*     hierarchy = iPlayer.GetSharedPlaybackState()->GetHierarchy();

    if( !hierarchy )
        return result;

    const FMovieSceneSequenceHierarchyNode* thisSequenceNode = hierarchy->FindNode( thisSequenceID );

    check( thisSequenceNode );

    // Find the TargetSequenceID by comparing deterministic sequence IDs for all children of the current node
    const FMovieSceneSequenceID* innerSequenceID = Algo::FindByPredicate( thisSequenceNode->Children,
        [hierarchy, targetSequenceID]( FMovieSceneSequenceID iSequenceID )
        {
            const FMovieSceneSubSequenceData* subData = hierarchy->FindSubData( iSequenceID );
            return subData && subData->DeterministicSequenceID == targetSequenceID;
        }
        );

    result.mInnerSequenceId = innerSequenceID ? *innerSequenceID : FMovieSceneSequenceID();

    return result;
}

BoardSequenceHelpers::FInnerSequenceResult
BoardSequenceHelpers::GetInnerSequence( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceId, const FFrameNumber& iFrameNumber )
{
    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    UMovieSceneCinematicBoardTrack* board_track = moviescene ? moviescene->FindTrack<UMovieSceneCinematicBoardTrack>() : nullptr;
    UMovieSceneSection* section = board_track ? MovieSceneHelpers::FindSectionAtTime( board_track->GetAllSections(), iFrameNumber ) : nullptr;
    UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );

    if( !subsection )
        return FInnerSequenceResult();

    FInnerSequenceResult result = GetInnerSequence( iPlayer, *subsection, iSequenceId );
    result.mInnerTime = iFrameNumber * subsection->OuterToInnerTransform();

    return result;
}

//---
//---
//---

//static
FGuid
BoardSequenceHelpers::GetCameraBinding( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSubSection, iSequenceID );

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return FGuid();

    return ShotSequenceHelpers::GetCameraBinding( iPlayer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
ACineCameraActor*
BoardSequenceHelpers::GetCameraSpawned( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID, FGuid iCameraBinding )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSubSection, iSequenceID );

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return nullptr;

    return ShotSequenceHelpers::GetCameraSpawned( iPlayer, result.mInnerSequence, result.mInnerSequenceId, iCameraBinding );
}

//static
ACineCameraActor*
BoardSequenceHelpers::GetCameraSpawnedOrTemplate( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID, FGuid iCameraBinding )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSubSection, iSequenceID );

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return nullptr;

    return ShotSequenceHelpers::GetCameraSpawnedOrTemplate( iPlayer, result.mInnerSequence, result.mInnerSequenceId, iCameraBinding );
}

//static
FGuid
BoardSequenceHelpers::GetCameraBinding( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FFrameNumber& iFrameNumber )
{
    check( !iSequence || iSequence->IsA<UBoardSequence>() );

    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSequence, iSequenceID, iFrameNumber );

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return FGuid();

    return ShotSequenceHelpers::GetCameraBinding( iPlayer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
ACineCameraActor*
BoardSequenceHelpers::GetCameraSpawned( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FFrameNumber& iFrameNumber, FGuid iCameraBinding )
{
    check( !iSequence || iSequence->IsA<UBoardSequence>() );

    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSequence, iSequenceID, iFrameNumber );

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return nullptr;

    return ShotSequenceHelpers::GetCameraSpawned( iPlayer, result.mInnerSequence, result.mInnerSequenceId, iCameraBinding );
}

//static
ACineCameraActor*
BoardSequenceHelpers::GetCameraSpawnedOrTemplate( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FFrameNumber& iFrameNumber, FGuid iCameraBinding )
{
    check( !iSequence || iSequence->IsA<UBoardSequence>() );

    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSequence, iSequenceID, iFrameNumber );

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return nullptr;

    return ShotSequenceHelpers::GetCameraSpawnedOrTemplate( iPlayer, result.mInnerSequence, result.mInnerSequenceId, iCameraBinding );
}

//static
FGuid
BoardSequenceHelpers::GetCameraBindingRecursive( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FFrameNumber& iFrameNumber, UMovieSceneSequence** oSequence, FMovieSceneSequenceID* oSequenceID )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSequence, iSequenceID, iFrameNumber );

    if( !result.mInnerSequence )
        return FGuid();

    // if we are on a shot subsequence
    if( result.mInnerSequence->IsA<UShotSequence>() )
    {
        *oSequence = result.mInnerSequence;
        *oSequenceID = result.mInnerSequenceId;

        FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( iPlayer, result.mInnerSequence, result.mInnerSequenceId );

        return camera_binding;
    }

    // if we are on a board subsequence
    if( result.mInnerSequence->IsA<UBoardSequence>() )
    {
        FGuid camera_binding = BoardSequenceHelpers::GetCameraBindingRecursive( iPlayer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.FloorToFrame(), oSequence, oSequenceID );

        return camera_binding;
    }

    return FGuid();
}

static
TArray<UObject*>
FindSpawnedObjectOrTemplate( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iBinding )
{
    TArray<UObject*> objects;

    TArrayView<TWeakObjectPtr<>> weakObjectsView = iPlayer.FindBoundObjects( iBinding, iSequenceID );
    CopyFromWeakArray( objects, weakObjectsView );
    if( objects.Num() )
        return objects;

    //---

    if( !iSequence )
        return TArray<UObject*>();

    if( FMovieSceneBindingReferences* bindingReferences = iSequence->GetBindingReferences() )
    {
        int32 reference_count = bindingReferences->GetReferences( iBinding ).Num();
        for( int binding_index = 0; binding_index < reference_count; binding_index++ )
        {
            UObject* object = MovieSceneHelpers::GetObjectTemplate( iSequence, iBinding, iPlayer.GetSharedPlaybackState(), binding_index );
            if( object )
                objects.Add( object );
        }
    }

    return objects;
}

//static
FGuid
ShotSequenceHelpers::GetCameraBinding( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    check( !iSequence || iSequence->IsA<UShotSequence>() );

    UMovieScene* movieScene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !movieScene )
        return FGuid();

    //TODO: maybe check the cameracut track to get the binding ?

    for( int i = 0; i < movieScene->GetPossessableCount(); i++ )
    {
        FMovieScenePossessable possessable = movieScene->GetPossessable( i );

        TArray<UObject*> objects = FindSpawnedObjectOrTemplate( iPlayer, iSequence, iSequenceID, possessable.GetGuid() );
        for( UObject* object : objects )
        {
            ACineCameraActor* camera = Cast<ACineCameraActor>( object );
            if( !camera )
                continue;

            return possessable.GetGuid();
        }
    }

    return FGuid();
}

//static
ACineCameraActor*
ShotSequenceHelpers::GetCameraSpawned( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iCameraBinding )
{
    check( !iSequence || iSequence->IsA<UShotSequence>() );

    if( !iCameraBinding.IsValid() )
        return nullptr;

    TArrayView<TWeakObjectPtr<>> weakObjects = iPlayer.FindBoundObjects( iCameraBinding, iSequenceID );
    if( weakObjects.Num() )
        return Cast<ACineCameraActor>( weakObjects[0].Get() );

    return nullptr;
}

//static
ACineCameraActor*
ShotSequenceHelpers::GetCameraSpawnedOrTemplate( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iCameraBinding )
{
    check( !iSequence || iSequence->IsA<UShotSequence>() );

    if( !iCameraBinding.IsValid() )
        return nullptr;

    TArray<UObject*> objects = FindSpawnedObjectOrTemplate( iPlayer, iSequence, iSequenceID, iCameraBinding );
    for( UObject* object : objects )
    {
        ACineCameraActor* camera = Cast<ACineCameraActor>( object );
        if( !camera )
            continue;

        return camera;
    }

    return nullptr;
}


//static
TArray<FGuid>
BoardSequenceHelpers::GetAnimationBindingsRecursive( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FFrameNumber& iFrameNumber, UMovieSceneSequence** oSequence, FMovieSceneSequenceID* oSequenceID )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSequence, iSequenceID, iFrameNumber );
    if( !result.mInnerSequence )
        return TArray<FGuid>();

    // if we are on a shot subsequence
    if( result.mInnerSequence->IsA<UShotSequence>() )
    {
        *oSequence = result.mInnerSequence;
        *oSequenceID = result.mInnerSequenceId;

        return ShotSequenceHelpers::GetAnimationBindings( iPlayer, result.mInnerSequence, result.mInnerSequenceId );
    }

    // if we are on a board subsequence
    if( result.mInnerSequence->IsA<UBoardSequence>() )
    {
        return BoardSequenceHelpers::GetAnimationBindingsRecursive( iPlayer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.FloorToFrame(), oSequence, oSequenceID );
    }

    return TArray<FGuid>();
}


//static
TArray<FGuid>
BoardSequenceHelpers::GetAnimationBindings( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSubSection, iSequenceID );
    if( !result.mInnerSequence )
        return TArray<FGuid>();
    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return TArray<FGuid>();

    return ShotSequenceHelpers::GetAnimationBindings( iPlayer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
TArray<AOdysseyAnimationActor*>
BoardSequenceHelpers::GetAnimationSpawned( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSubSection, iSequenceID );
    if( !result.mInnerSequence )
        return TArray<AOdysseyAnimationActor*>();
    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return TArray<AOdysseyAnimationActor*>();

    return ShotSequenceHelpers::GetAnimationSpawned( iPlayer, result.mInnerSequence, result.mInnerSequenceId, iAnimationBinding );
}

//static
TArray<AOdysseyAnimationActor*>
BoardSequenceHelpers::GetAnimationSpawnedOrTemplate( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSubSection, iSequenceID );
    if( !result.mInnerSequence )
        return TArray<AOdysseyAnimationActor*>();
    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return TArray<AOdysseyAnimationActor*>();

    return ShotSequenceHelpers::GetAnimationSpawnedOrTemplate( iPlayer, result.mInnerSequence, result.mInnerSequenceId, iAnimationBinding );
}

//static
TArray<FGuid>
BoardSequenceHelpers::GetAnimationBindings( IMovieScenePlayer & iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FFrameNumber& iFrameNumber )
{
    check( !iSequence || iSequence->IsA<UBoardSequence>() );

    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSequence, iSequenceID, iFrameNumber );
    if( !result.mInnerSequence )
        return TArray<FGuid>();
    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return TArray<FGuid>();

    return ShotSequenceHelpers::GetAnimationBindings( iPlayer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
TArray<AOdysseyAnimationActor*>
BoardSequenceHelpers::GetAnimationSpawned( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FFrameNumber& iFrameNumber, FGuid iAnimationBinding )
{
    check( !iSequence || iSequence->IsA<UBoardSequence>() );

    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSequence, iSequenceID, iFrameNumber );
    if( !result.mInnerSequence )
        return TArray<AOdysseyAnimationActor*>();
    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return TArray<AOdysseyAnimationActor*>();

    return ShotSequenceHelpers::GetAnimationSpawned( iPlayer, result.mInnerSequence, result.mInnerSequenceId, iAnimationBinding );
}

//static
TArray<AOdysseyAnimationActor*>
BoardSequenceHelpers::GetAnimationSpawnedOrTemplate( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FFrameNumber& iFrameNumber, FGuid iAnimationBinding )
{
    check( !iSequence || iSequence->IsA<UBoardSequence>() );

    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSequence, iSequenceID, iFrameNumber );
    if( !result.mInnerSequence )
        return TArray<AOdysseyAnimationActor*>();
    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return TArray<AOdysseyAnimationActor*>();

    return ShotSequenceHelpers::GetAnimationSpawnedOrTemplate( iPlayer, result.mInnerSequence, result.mInnerSequenceId, iAnimationBinding );
}

//static
TArray<FGuid>
ShotSequenceHelpers::GetAnimationBindings( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    check( !iSequence || iSequence->IsA<UShotSequence>() );

    TArray<FGuid> bindings;

    UMovieScene* movieScene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !movieScene )
        return bindings;

    for( int i = 0; i < movieScene->GetPossessableCount(); i++ )
    {
        FMovieScenePossessable possessable = movieScene->GetPossessable( i );

        TArray<UObject*> objects = FindSpawnedObjectOrTemplate( iPlayer, iSequence, iSequenceID, possessable.GetGuid() );
        for( UObject* object : objects )
        {
            AOdysseyAnimationActor* animation_actor = Cast<AOdysseyAnimationActor>( object );
            if( !animation_actor )
                continue;

            bindings.AddUnique( possessable.GetGuid() );
        }
    }

    return bindings;
}

//static
TArray<AOdysseyAnimationActor*>
ShotSequenceHelpers::GetAnimationSpawned( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding )
{
    check( !iSequence || iSequence->IsA<UShotSequence>() );

    /* Warning:
    * iSequence is not used here
    * so GuessActorToSelect() will set it to nullptr
    * but it's because I'm lazy
    * GuessActorToSelect() should set it the right way ...
    */
    TArray<AOdysseyAnimationActor*> animation_actors;

    TArrayView<TWeakObjectPtr<>> weakObjects = iPlayer.FindBoundObjects( iAnimationBinding, iSequenceID );
    for( TWeakObjectPtr<> weakObject : weakObjects )
    {
        if( !weakObject.IsValid() )
            continue;

        AOdysseyAnimationActor* animation_actor = Cast<AOdysseyAnimationActor>( weakObject.Get() );
        if( !animation_actor )
            continue;

        animation_actors.Add( animation_actor );
    }

    return animation_actors;
}

//static
TArray<AOdysseyAnimationActor*>
ShotSequenceHelpers::GetAnimationSpawnedOrTemplate( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding )
{
    check( !iSequence || iSequence->IsA<UShotSequence>() );

    TArray<AOdysseyAnimationActor*> animation_actors;

    TArray<UObject*> objects = FindSpawnedObjectOrTemplate( iPlayer, iSequence, iSequenceID, iAnimationBinding );
    for( UObject* object : objects )
    {
        AOdysseyAnimationActor* animation_actor = Cast<AOdysseyAnimationActor>( object );
        if( !animation_actor )
            continue;

        animation_actors.AddUnique( animation_actor );
    }

    return animation_actors;
}

//static
int32
ShotSequenceHelpers::GetAttachedAnimations( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArray<AOdysseyAnimationActor*>* oAnimations, TArray<FGuid>* oAnimationBindings )
{
    if( oAnimations )
        oAnimations->Empty();
    if( oAnimationBindings )
        oAnimationBindings->Empty();

    UMovieScene* movieScene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !movieScene )
        return 0;

    TArray<AOdysseyAnimationActor*> animations;
    TArray<FGuid> animation_bindings;

    for( int i = 0; i < movieScene->GetPossessableCount(); i++ )
    {
        FMovieScenePossessable possessable = movieScene->GetPossessable( i );

        for( TWeakObjectPtr<> WeakObject : iPlayer.FindBoundObjects( possessable.GetGuid(), iSequenceID ) )
        {
            AOdysseyAnimationActor* animation = Cast<AOdysseyAnimationActor>( WeakObject.Get() );

            if( !animation )
                continue;

            USceneComponent* RootComp = animation->GetRootComponent();
            if( !RootComp || !RootComp->GetAttachParent() )
                continue;

            AActor* ParentActor = RootComp->GetAttachParent()->GetOwner();
            if( !ParentActor ) //TODO: confirm by comparing with the camera ? or is it enough as the animations are in the movie scene ?
                continue;

            animations.Add( animation );
            animation_bindings.Add( possessable.GetGuid() );
        }
    }

    if( oAnimations )
        *oAnimations = animations;
    if( oAnimationBindings )
        *oAnimationBindings = animation_bindings;

    return animations.Num();
}


//static
FQualifiedFrameTime
EposSequenceHelpers::GetIntermediateTime( IMovieScenePlayer& iPlayer, FQualifiedFrameTime iGlobalTime, FMovieSceneSequenceIDRef iToSequenceId )
{
    const FMovieSceneSequenceHierarchy* hierarchy = iPlayer.GetEvaluationTemplate().GetHierarchy();
    if( !hierarchy )
        return FQualifiedFrameTime();

    if( iToSequenceId == MovieSceneSequenceID::Root )
        return iGlobalTime;

    const FMovieSceneSubSequenceData* subdata = hierarchy->FindSubData( iToSequenceId );
    if( !subdata )
        return FQualifiedFrameTime();

    const FFrameTime root_time = ConvertFrameTime( iGlobalTime.Time, iGlobalTime.Rate, subdata->TickResolution );

    return FQualifiedFrameTime( root_time * subdata->RootToSequenceTransform, subdata->TickResolution );
}

//static
UEposMovieSceneSequence*
EposSequenceHelpers::GetRootEposSequence( IMovieScenePlayer& iPlayer, FMovieSceneSequenceIDRef iSequenceId )
{
    FMovieSceneSequenceID root_sequence_id;
    return GetRootEposSequence( iPlayer, iSequenceId, root_sequence_id );
}

//static
UEposMovieSceneSequence*
EposSequenceHelpers::GetRootEposSequence( IMovieScenePlayer& iPlayer, FMovieSceneSequenceIDRef iSequenceId, FMovieSceneSequenceID& oRootSequenceId )
{
    oRootSequenceId = MovieSceneSequenceID::Invalid;

    if( iSequenceId == MovieSceneSequenceID::Root )
    {
        oRootSequenceId = MovieSceneSequenceID::Root;
        return Cast<UEposMovieSceneSequence>( iPlayer.GetEvaluationTemplate().GetRootSequence() );
    }

    const FMovieSceneSequenceHierarchy* hierarchy = iPlayer.GetSharedPlaybackState()->GetHierarchy();
    if( !hierarchy )
        return nullptr;

    TArray<FMovieSceneSequenceID> sequence_ids;
    TArray<UEposMovieSceneSequence*> sequences;

    // If the given sequence is not an UEposMovieSceneSequence, just return
    UMovieSceneSequence* sequence = iPlayer.GetEvaluationTemplate().GetSequence( iSequenceId );
    if( !sequence || !sequence->IsA<UEposMovieSceneSequence>() )
        return nullptr;

    sequence_ids.Add( iSequenceId );
    sequences.Add( CastChecked<UEposMovieSceneSequence>( sequence ) );

    const FMovieSceneSequenceHierarchyNode* hierarchyNode = hierarchy->FindNode( iSequenceId );
    while( hierarchyNode && hierarchyNode->ParentID.IsValid() )
    {
        if( hierarchyNode->ParentID == MovieSceneSequenceID::Root )
        {
            UMovieSceneSequence* player_root_sequence = iPlayer.GetEvaluationTemplate().GetRootSequence();
            if( Cast<UEposMovieSceneSequence>( player_root_sequence ) )
            {
                sequence_ids.Add( MovieSceneSequenceID::Root );
                sequences.Add( CastChecked<UEposMovieSceneSequence>( player_root_sequence ) );
            }
        }
        else
        {
            UMovieSceneSequence* parent_sequence = iPlayer.GetEvaluationTemplate().GetSequence( hierarchyNode->ParentID );
            if( Cast<UEposMovieSceneSequence>( parent_sequence ) )
            {
                sequence_ids.Add( hierarchyNode->ParentID );
                sequences.Add( CastChecked<UEposMovieSceneSequence>( parent_sequence ) );
            }
        }
        hierarchyNode = hierarchy->FindNode( hierarchyNode->ParentID );
    }

    oRootSequenceId = sequence_ids.Num() ? sequence_ids.Last() : MovieSceneSequenceID::Invalid;

    return sequences.Num() ? sequences.Last() : nullptr;
}

//static
TArray<UStoryNote*>
EposSequenceHelpers::GetNotesRecursive( UMovieSceneSequence* iSequence, FFrameNumber iFrameNumber )
{
    TArray<UStoryNote*> notes;

    //ULevelSequence* level_sequence = iMoviePipeline->GetTargetSequence();

    //FFrameRate effective_framerate = iMoviePipeline->GetPipelineMasterConfig()->GetEffectiveFrameRate( iMoviePipeline->GetTargetSequence() );

    //FTimecode master_timecode = UMoviePipelineBlueprintLibrary::GetMasterTimecode( iMoviePipeline );
    //FFrameNumber master_current_frame_in_levelsequence = UMoviePipelineBlueprintLibrary::GetMasterFrameNumber( iMoviePipeline );
    //FFrameNumber master_current_frame = FFrameRate::TransformTime( master_current_frame_in_levelsequence, effective_framerate, iSequence->GetMovieScene()->GetTickResolution() ).GetFrame();

    ////FTimecode shot_timecode = UMoviePipelineBlueprintLibrary::GetCurrentShotTimecode( iMoviePipeline );
    //FFrameNumber shot_current_frame_in_levelsequence = UMoviePipelineBlueprintLibrary::GetCurrentShotFrameNumber( iMoviePipeline );
    //FFrameNumber shot_current_frame = FFrameRate::TransformTime( master_current_frame_in_levelsequence, effective_framerate, level_sequence->GetMovieScene()->GetTickResolution() ).GetFrame();

    FFrameNumber master_current_frame = iFrameNumber;

    //---

    using FTrackAndSectionInfo = TTuple<UMovieSceneTrack*, UMovieSceneSection*>;

    struct FSequenceNoteVisitor
        : UE::MovieScene::ISequenceVisitor
    {
        void SortInfo()
        {
                mTrackAndSectionInfo.StableSort( []( const FTrackAndSectionInfo& iA, const FTrackAndSectionInfo& iB )
                                                 {
                                                     UMovieSceneSection* sectionA = iA.Get<1>();
                                                     UMovieSceneSection* sectionB = iB.Get<1>();

                                                     TRangeBound<FFrameNumber> LowerBoundA = sectionA->GetRange().GetLowerBound();
                                                     return TRangeBound<FFrameNumber>::MinLower( LowerBoundA, sectionB->GetRange().GetLowerBound() ) == LowerBoundA;
                                                 } );
                mTrackAndSectionInfo.StableSort( []( const FTrackAndSectionInfo& iA, const FTrackAndSectionInfo& iB )
                                                 {
                                                     UMovieSceneSection* sectionA = iA.Get<1>();
                                                     UMovieSceneSection* sectionB = iB.Get<1>();

                                                     return sectionA->GetRowIndex() < sectionB->GetRowIndex();
                                                 } );

#if WITH_EDITORONLY_DATA
                mTrackAndSectionInfo.StableSort( []( const FTrackAndSectionInfo& iA, const FTrackAndSectionInfo& iB )
                                                 {
                                                     UMovieSceneTrack* trackA = iA.Get<0>();
                                                     UMovieSceneTrack* trackB = iB.Get<0>();

                                                     return trackA->GetSortingOrder() < trackB->GetSortingOrder();
                                                 } );
#endif
        }

        virtual void VisitSection( UMovieSceneTrack* iTrack, UMovieSceneSection* iSection, const FGuid& iGuid, const UE::MovieScene::FSubSequenceSpace& iLocalSpace )
        {
            UMovieSceneNoteSection* note_section = Cast<UMovieSceneNoteSection>( iSection );
            if( !note_section || !note_section->GetNote() )
                return;

            FFrameTime local_reference_time = mReferenceFrame * iLocalSpace.RootToSequenceTransform;
            if( !iSection->IsTimeWithinSection( local_reference_time.GetFrame() ) )
                return;

            FTrackAndSectionInfo track_and_section( iTrack, iSection );
            mTrackAndSectionInfo.Add( track_and_section );

            SortInfo();
        }

        FFrameNumber mReferenceFrame; // In tick resolution

        TSet<FTrackAndSectionInfo> mTrackAndSectionInfo;
    };


    UE::MovieScene::FSequenceVisitParams params;
    params.bVisitSections = true;
    params.bVisitRootTracks = true;
    params.bVisitSubSequences = true;

    FSequenceNoteVisitor note_visitor;
    note_visitor.mReferenceFrame = master_current_frame;

    // Visit all notes
    VisitSequence( iSequence, params, note_visitor );

    for( auto track_and_section : note_visitor.mTrackAndSectionInfo )
    {
        UMovieSceneNoteSection* note_section = Cast<UMovieSceneNoteSection>( track_and_section.Get<1>() );
        check( note_section && note_section->GetNote() );

        notes.Add( note_section->GetNote() );
    }

    return notes;
}

//static
TArray<TWeakObjectPtr<UMovieSceneNoteSection>>
EposSequenceHelpers::GetNotesRecursive( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    TArray<TWeakObjectPtr<UMovieSceneNoteSection>> note_sections = GetNotes( iPlayer, iSequence, iSequenceID, iFrameNumber );

    UMovieScene* movie_scene = iSequence->GetMovieScene();
    UMovieSceneCinematicBoardTrack* track = movie_scene->FindTrack<UMovieSceneCinematicBoardTrack>();
    if( !track )
        return note_sections;

    auto sections = track->GetAllSections();
    for( auto section : sections )
    {
        if( !section->IsTimeWithinSection( iFrameNumber ) || !section->IsActive() )
            continue;

        UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );

        BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( iPlayer, *subsection, iSequenceID );
        FFrameTime inner_time = iFrameNumber * subsection->OuterToInnerTransform();

        if( !result.mInnerSequence )
            continue;

        TArray<TWeakObjectPtr<UMovieSceneNoteSection>> note_sections_recursive = GetNotesRecursive( iPlayer, result.mInnerSequence, result.mInnerSequenceId, inner_time.GetFrame() );
        note_sections.Append( note_sections_recursive );
    }

    return note_sections;
}

//static
TArray<TWeakObjectPtr<UMovieSceneNoteSection>>
EposSequenceHelpers::GetNotes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TOptional<FFrameNumber> iFrameNumber )
{
    //TODO: certainly replace those 2 functions by the one with the sequencer visitor: UMoviePipelineStoryboardBlueprintLibrary::GetNotes()

    TArray<TWeakObjectPtr<UMovieSceneNoteSection>> note_sections;

    UMovieScene* movie_scene = iSequence->GetMovieScene();
    TArray<UMovieSceneTrack*> tracks = movie_scene->GetTracks();
#if WITH_EDITORONLY_DATA
    tracks.StableSort( []( const UMovieSceneTrack& iA, const UMovieSceneTrack& iB )
                       {
                           return iA.GetSortingOrder() < iB.GetSortingOrder();
                       } );
#endif

    for( auto track : tracks )
    {
        UMovieSceneNoteTrack* note_track = Cast<UMovieSceneNoteTrack>( track );
        if( !note_track )
            continue;

        TArray<UMovieSceneSection*> sections = note_track->GetAllSections();

        // It Should be MovieSceneHelpers::SortConsecutiveSections( sections ); but it doesn't use the stable sort
        sections.StableSort( []( const UMovieSceneSection& iA, const UMovieSceneSection& iB )
                             {
                                 TRangeBound<FFrameNumber> LowerBoundA = iA.GetRange().GetLowerBound();
                                 return TRangeBound<FFrameNumber>::MinLower( LowerBoundA, iB.GetRange().GetLowerBound() ) == LowerBoundA;
                             } );
        sections.StableSort( []( const UMovieSceneSection& iA, const UMovieSceneSection& iB )
                             {
                                 return iA.GetRowIndex() < iB.GetRowIndex();
                             } );


        for( auto section : sections )
        {
            if( iFrameNumber.IsSet() )
            {
                //MovieSceneHelpers::FindSectionAtTime()
                if( !section->IsTimeWithinSection( iFrameNumber.GetValue() ) || !section->IsActive() )
                    continue;
            }

            UMovieSceneNoteSection* note_section = Cast<UMovieSceneNoteSection>( section );
            if( !note_section )
                continue;

            UStoryNote* note = note_section->GetNote();
            if( !note )
                continue;

            note_sections.Add( note_section );
        }
    }

    return note_sections;
}


//static
ShotSequenceHelpers::FFindOrCreateAnimationVisibilityResult
ShotSequenceHelpers::FindAnimationVisibilityTrackAndSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding, TOptional<FFrameNumber> iFrameNumber )
{
    FFindOrCreateAnimationVisibilityResult result;

    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !moviescene )
        return result;

    result.mTrack = moviescene->FindTrack<UMovieSceneVisibilityTrack>( iAnimationBinding );
    if( !result.mTrack.IsValid() )
        return result;

    //---

    if( iFrameNumber.IsSet() )
    {
        for( auto section : result.mTrack->GetAllSections() )
        {
            if( section->IsTimeWithinSection( iFrameNumber.GetValue() ) )
            {
                result.mSections.Add( Cast<UMovieSceneBoolSection>( section ) );
            }
        }
    }
    else
    {
        for( auto section : result.mTrack->GetAllSections() )
            result.mSections.Add( Cast<UMovieSceneBoolSection>( section ) );
    }

    return result;
}

template<typename TrackClass>
static
FGuid
FindSingleChildBindingWithTrack( UMovieScene* iMovieScene, FGuid iParentBinding )
{
    TArray<FGuid> child_bindings;

    for( int32 possessableIndex = 0; possessableIndex < iMovieScene->GetPossessableCount(); ++possessableIndex )
    {
        const FMovieScenePossessable& possessable = iMovieScene->GetPossessable( possessableIndex );

        TrackClass* track = iMovieScene->FindTrack<TrackClass>( possessable.GetGuid() );
        if( !track )
            continue;

        //---

        auto CheckLineage = []( UMovieScene* iMovieScene, FGuid iChildBinding, FGuid iParentBinding ) -> bool
            {
                FGuid current_binding = iChildBinding;
                while( current_binding.IsValid() )
                {
                    if( current_binding == iParentBinding )
                        return true;

                    current_binding = iMovieScene->FindPossessable( current_binding )->GetParent();
                }

                return false;
            };

        if( CheckLineage( iMovieScene, possessable.GetGuid(), iParentBinding ) )
        {
            child_bindings.Add( possessable.GetGuid() );
        }
    }

    if( child_bindings.IsEmpty() )
        return FGuid();

    // Assume there is only ONE "TrackClass" in the hierarchy of iParentBinding
    check( child_bindings.Num() == 1 );

    return child_bindings[0];
}

//static
ShotSequenceHelpers::FFindOrCreateTimelineResult
ShotSequenceHelpers::FindTimelineTrackAndSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding, TOptional<FFrameNumber> iFrameNumber )
{
    FFindOrCreateTimelineResult result;

    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !moviescene )
        return result;

    FGuid child_binding_with_timeline = FindSingleChildBindingWithTrack<UOdysseyAnimationTimelineTrack>( moviescene, iAnimationBinding );
    if( !child_binding_with_timeline.IsValid() )
        return result;

    //---

    result.mTrack = moviescene->FindTrack<UOdysseyAnimationTimelineTrack>( child_binding_with_timeline );
    if( !ensure( result.mTrack.IsValid() ) )
        return result;

    //---

    if( iFrameNumber.IsSet() )
    {
        for( auto section : result.mTrack->GetAllSections() )
        {
            if( section->IsTimeWithinSection( iFrameNumber.GetValue() ) )
            {
                result.mSections.Add( Cast<UOdysseyAnimationTimelineSection>( section ) );
            }
        }
    }
    else
    {
        for( auto section : result.mTrack->GetAllSections() )
            result.mSections.Add( Cast<UOdysseyAnimationTimelineSection>( section ) );
    }

    return result;
}

//static
TArray<FFrameNumber>
BoardSequenceHelpers::GetAllAnimationCutTimes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FFrameNumber& iFrameNumber )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSequence, iSequenceID, iFrameNumber );
    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return TArray<FFrameNumber>();

    return ShotSequenceHelpers::GetAllAnimationCutTimes( iPlayer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
TArray<FFrameNumber>
ShotSequenceHelpers::GetAllAnimationCutTimes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    check( !iSequence || iSequence->IsA<UShotSequence>() );

    TArray<FFrameNumber> times;

    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !moviescene )
        return times;

    TArray<FGuid> animation_bindings = ShotSequenceHelpers::GetAnimationBindings( iPlayer, iSequence, iSequenceID );
    if( animation_bindings.IsEmpty() )
        return times;

    for( FGuid animation_binding : animation_bindings )
    {
        FGuid child_binding_with_timeline = FindSingleChildBindingWithTrack<UOdysseyAnimationTimelineTrack>( moviescene, animation_binding );
        if( !child_binding_with_timeline.IsValid() )
            continue;

        UOdysseyAnimationTimelineTrack* track = moviescene->FindTrack<UOdysseyAnimationTimelineTrack>( child_binding_with_timeline );
        if( !ensure(track) )
            continue;

        for( auto section : track->GetAllSections() )
        {
            UOdysseyAnimationTimelineSection* section_timeline = Cast<UOdysseyAnimationTimelineSection>( section );
            if( !ensure( section_timeline ) )
                continue;

            const FOdysseyAnimationCutChannel& channel = section_timeline->GetAnimationCutChannel();
            for( auto time : channel.GetData().GetTimes() )
                times.Add( time );
        }
    }

    times.Sort();

    return times;
}


bool
FKeyOpacity::Exists()
{
    return mKeyHandle != FKeyHandle::Invalid();
}

bool
FKeyOpacity::GetOpacity( float& oOpacity )
{
    if( !mChannel || mKeyHandle == FKeyHandle::Invalid() )
        return false;

    FMovieSceneFloatValue opacity;
    UE::MovieScene::GetKeyValue( mChannel, mKeyHandle, opacity );
    oOpacity = opacity.Value;

    return true;
}

void
FKeyOpacity::SetOpacity( float iOpacity )
{
    if( !mChannel || mKeyHandle == FKeyHandle::Invalid() )
        return;

    mSection->Modify();

    AssignValue( mChannel, mKeyHandle, FMath::Clamp( iOpacity, 0.f, 1.f ) );
}

//static
FKeyOpacity
ShotSequenceHelpers::ConvertToOpacityKey( TWeakObjectPtr<UMovieSceneSection> iSection, const FMovieSceneChannelHandle& iChannelHandle, FKeyHandle iKeyHandle )
{
    FKeyOpacity opacity_key;

    if( !iSection.IsValid()
        || iKeyHandle == FKeyHandle::Invalid() )
        return opacity_key;

    TMovieSceneChannelHandle<FMovieSceneFloatChannel> channel_handle = iChannelHandle.Cast<FMovieSceneFloatChannel>();
    FMovieSceneFloatChannel* float_channel = channel_handle.Get();
    if( !float_channel )
        return opacity_key;

    opacity_key.mSection = iSection;
    opacity_key.mKeyHandle = iKeyHandle;
    opacity_key.mChannel = float_channel;

    return opacity_key;
}

//static
ShotSequenceHelpers::FFindOrCreateMaterialParameterResult
ShotSequenceHelpers::FindMaterialParameterTrackAndSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iBinding, TOptional<FFrameNumber> iFrameNumber )
{
    FFindOrCreateMaterialParameterResult result;

    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !moviescene )
        return result;

    FGuid child_binding_with_material = FindSingleChildBindingWithTrack<UMovieSceneComponentMaterialTrack>( moviescene, iBinding );
    if( !child_binding_with_material.IsValid() )
        return result;

    //---

    result.mTrack = moviescene->FindTrack<UMovieSceneComponentMaterialTrack>( child_binding_with_material ); // Get only the material track of the first "material 0", should be ok as animation actor have only 1 material associated
    if( !result.mTrack.IsValid() )
        return result;

    //---

    if( iFrameNumber.IsSet() )
    {
        for( auto section : result.mTrack->GetAllSections() )
        {
            if( section->IsTimeWithinSection( iFrameNumber.GetValue() ) )
            {
                                                                                        // If we have an old parameter section already, manage to the old style section
                check( Cast<UMovieSceneComponentMaterialParameterSection>( section ) || Cast<UMovieSceneParameterSection>( section ) );
                result.mSections.Add( section );
            }
        }
    }
    else
    {
        for( auto section : result.mTrack->GetAllSections() )
        {
                                                                                    // If we have an old parameter section already, manage to the old style section
            check( Cast<UMovieSceneComponentMaterialParameterSection>( section ) || Cast<UMovieSceneParameterSection>( section ) );
            result.mSections.Add( section );
        }
    }

    return result;
}

//static
ShotSequenceHelpers::FFindOrCreateMaterialParameterResult
ShotSequenceHelpers::FindOrCreateMaterialParameterTrackAndSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iBinding, TOptional<FFrameNumber> iFrameNumber )
{
    FFindOrCreateMaterialParameterResult result = FindMaterialParameterTrackAndSections( iPlayer, iSequence, iSequenceID, iBinding, iFrameNumber );

    // Return if we get track AND sections (with optional iFrameNumber taken into account)
    if( result.mTrack.IsValid() && result.mSections.Num() )
        return result;

    // At this step, result.mSections is empty, but it doesn't necessarily mean that result.mTrack->GetAllSections() is also empty (if iFrameNumber is set but outside section(s) boundaries)
    // So, we considere to create a new section only if there is really no existing section (no matter of iFrameNumber)

    if( !result.mTrack.IsValid() )
    {
        // Many assumptions here:
        // - binding must be an actor (as this function should always be called on an actor binding)
        // - component binding must exist
        // - component binding must allow component material track
        TArray<UObject*> objects = FindSpawnedObjectOrTemplate( iPlayer, iSequence, iSequenceID, iBinding );
        AActor* actor = Cast<AActor>( objects[0] );
        UActorComponent* component = actor->FindComponentByClass<UOdysseyAnimationComponent>();
        if( !component )
            component = actor->GetRootComponent();
        if( !component )
            return result;

        FGuid child_binding_with_material = iPlayer.FindCachedObjectId( *component, iSequenceID );
        if( !child_binding_with_material.IsValid() )
            return result;

        //---

        result.mTrackCreated = true;

        UMovieSceneTrack* track = iSequence->GetMovieScene()->AddTrack<UMovieSceneComponentMaterialTrack>( child_binding_with_material );
        result.mTrack = Cast<UMovieSceneComponentMaterialTrack>( track );

        FComponentMaterialInfo material_info = { FName(), 0, EComponentMaterialType::IndexedMaterial }; //TODO: iMaterialTrackIndex;
        result.mTrack->SetMaterialInfo( material_info );
#if WITH_EDITORONLY_DATA
        result.mTrack->SetDisplayName( FText::Format( LOCTEXT( "IndexedMaterialSwitcherTrackName", "Material Element {0}" ), FText::AsNumber( result.mTrack->GetMaterialInfo().MaterialSlotIndex ) ) );
#endif
    }

    check( result.mTrack.IsValid() );

    //---

    // Use GetAllSections() to be sure to have the 'real' number of section inside the track
    // If we rely only on mSections and with a iFrameNumber set, we can create a section while there are ones but outside iFrameNumber
    if( !result.mTrack->GetAllSections().Num() )
    {
        result.mSectionsCreated = true;

        UMovieSceneSection* section = result.mTrack->CreateNewSection();
        check( result.mTrack->IsEmpty() );
        result.mTrack->AddSection( *section );

        section->SetRange( TRange<FFrameNumber>::All() );

        // The track UMovieSceneComponentMaterialTrack (now 5.4) always create a UMovieSceneComponentMaterialParameterSection
        check( Cast<UMovieSceneComponentMaterialParameterSection>( section ) );
        result.mSections.Add( section );
    }

    return result;
}

//static
ShotSequenceHelpers::FFindOrCreateParameterChannelResult
ShotSequenceHelpers::FindMaterialOpacityChannel( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iBinding, TWeakObjectPtr<UMovieSceneSection> iSection )
{
    FFindOrCreateParameterChannelResult result;

    if( UMovieSceneComponentMaterialParameterSection* component_material_parameter_section = Cast<UMovieSceneComponentMaterialParameterSection>( iSection ) )
    {
        TArray<FScalarMaterialParameterInfoAndCurve>& parameters = component_material_parameter_section->ScalarParameterInfosAndCurves;
        for( auto& parameter : parameters )
        {
            if( parameter.ParameterInfo.Name.IsEqual( TEXT( "DrawingOpacity" ) ) )
            {
                result.mChannel = &parameter.ParameterCurve;
                return result;
            }
        }
    }
    else if( UMovieSceneParameterSection* parameter_section = Cast<UMovieSceneParameterSection>( iSection ) )
    {
        TArray<FScalarParameterNameAndCurve>& parameters = parameter_section->GetScalarParameterNamesAndCurves();
        for( auto& parameter : parameters )
        {
            if( parameter.ParameterName.IsEqual( TEXT( "DrawingOpacity" ) ) )
            {
                result.mChannel = &parameter.ParameterCurve;
                return result;
            }
        }
    }

    return result;
}

//static
ShotSequenceHelpers::FFindOrCreateParameterChannelResult
ShotSequenceHelpers::FindOrCreateMaterialOpacityChannel( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iBinding, TWeakObjectPtr<UMovieSceneSection> iSection )
{
    FFindOrCreateParameterChannelResult result = FindMaterialOpacityChannel( iPlayer, iSequence, iSequenceID, iBinding, iSection );
    if( result.mChannel )
        return result;

    if( UMovieSceneComponentMaterialParameterSection* component_material_parameter_section = Cast<UMovieSceneComponentMaterialParameterSection>( iSection ) )
    {
        // This will create the channel named "DrawingOpacity" (as it not exists)
        // and add a (dummy) key to be able to recache internal stuff (ChannelProxy) of the parameter section
        // And once the parameter channel are created, next we remove the dummy key
        component_material_parameter_section->AddScalarParameterKey( FMaterialParameterInfo( TEXT( "DrawingOpacity" ) ), 0, 1.f, FString(), FString() );

        result.mChannelCreated = true;

        TArray<FScalarMaterialParameterInfoAndCurve>& parameters = component_material_parameter_section->ScalarParameterInfosAndCurves;
        for( auto& parameter : parameters )
        {
            if( parameter.ParameterInfo.Name.IsEqual( TEXT( "DrawingOpacity" ) ) )
            {
                TArray<FFrameNumber> key_times;
                TArray<FKeyHandle> key_handles;
                parameter.ParameterCurve.GetKeys( TRange<FFrameNumber>::All(), &key_times, &key_handles );
                check( key_handles.Num() == 1 );

                UE::MovieScene::SetChannelDefault( &parameter.ParameterCurve, 1.f );

                parameter.ParameterCurve.DeleteKeys( key_handles );

                result.mChannel = &parameter.ParameterCurve;
                return result;
            }
        }
    }
    else if( UMovieSceneParameterSection* parameter_section = Cast<UMovieSceneParameterSection>( iSection ) )
    {
        // This will create the channel named "DrawingOpacity" (as it not exists)
        // and add a (dummy) key to be able to recache internal stuff (ChannelProxy) of the parameter section
        // And once the parameter channel are created, next we remove the dummy key
        parameter_section->AddScalarParameterKey( TEXT( "DrawingOpacity" ), 0, 1.f );

        result.mChannelCreated = true;

        TArray<FScalarParameterNameAndCurve>& parameters = parameter_section->GetScalarParameterNamesAndCurves();
        for( auto& parameter : parameters )
        {
            if( parameter.ParameterName.IsEqual( TEXT( "DrawingOpacity" ) ) )
            {
                TArray<FFrameNumber> key_times;
                TArray<FKeyHandle> key_handles;
                parameter.ParameterCurve.GetKeys( TRange<FFrameNumber>::All(), &key_times, &key_handles );
                check( key_handles.Num() == 1 );

                UE::MovieScene::SetChannelDefault( &parameter.ParameterCurve, 1.f );

                parameter.ParameterCurve.DeleteKeys( key_handles );

                result.mChannel = &parameter.ParameterCurve;
                return result;
            }
        }
    }

    checkNoEntry();

    return result;
}

//static
FKeyOpacity
ShotSequenceHelpers::GetOpacityKey( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, FGuid iBinding )
{
    FKeyOpacity key_opacity;

    if( !iSequence->GetMovieScene()->GetPlaybackRange().Contains( iFrameNumber ) )
        return key_opacity;

    FFindOrCreateMaterialParameterResult result = FindMaterialParameterTrackAndSections( iPlayer, iSequence, iSequenceID, iBinding, iFrameNumber );
    if( !result.mTrack.IsValid() || !result.mSections.Num() )
        return key_opacity;

    FFindOrCreateParameterChannelResult channel_result = FindMaterialOpacityChannel( iPlayer, iSequence, iSequenceID, iBinding, result.mSections[0] ); // Only the first one, should be nearly always the case
    if( !channel_result.mChannel )
        return key_opacity;

    //---

    TArray<FKeyHandle> key_handles;
    channel_result.mChannel->GetKeys( TRange<FFrameNumber>( iFrameNumber ), nullptr, &key_handles );
    if( !key_handles.Num() )
        return key_opacity;

    key_opacity.mChannel = channel_result.mChannel;
    key_opacity.mSection = result.mSections[0];
    key_opacity.mKeyHandle = key_handles[0];

    return key_opacity;
}

//static
TArray<FFrameNumber>
ShotSequenceHelpers::GetCameraTransformTimes( UMovieSceneSequence* iSequence, TOptional<FFrameNumber>* oDefaultFrame )
{
    TArray<FFrameNumber> keys;

    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !moviescene )
        return keys;

    UMovieSceneTrack* cameracut_track = moviescene->GetCameraCutTrack();
    if( !cameracut_track )
        return keys;

    TArray<UMovieSceneSection*> cameracut_sections = cameracut_track->GetAllSections();
    if( !cameracut_sections.Num() )
        return keys;

    UMovieSceneSingleCameraCutSection* cameracut_section = Cast<UMovieSceneSingleCameraCutSection>( cameracut_sections[0] );
    if( !cameracut_section )
        return keys;

    if( !cameracut_section->GetCameraBindingID().IsValid() )
        return keys;

    UMovieSceneTrack* track = moviescene->FindTrack<UMovieScene3DTransformTrack>( cameracut_section->GetCameraBindingID().GetGuid() );
    if( !track )
        return keys;

    for( auto section : track->GetAllSections() )
    {
        TArrayView<FMovieSceneDoubleChannel*> channels = section->GetChannelProxy().GetChannels<FMovieSceneDoubleChannel>();
        for( int i = 0; i < 9; i++ )
        {
            TArrayView<const FFrameNumber> times = channels[i]->GetTimes();
            for( auto time : times )
            {
                TRange<FFrameNumber> range( cameracut_section->GetTrueRange() );
                range.SetUpperBound( TRangeBound<FFrameNumber>::FlipInclusion( range.GetUpperBound() ) ); // Special case when a key is on the frame just on the exclusive upper bound value to render it
                if( range.Contains( time ) )
                    keys.AddUnique( time );
            }
        }
    }

    if( keys.IsEmpty() && oDefaultFrame )
        *oDefaultFrame = cameracut_section->GetTrueRange().GetLowerBoundValue();

    return keys;
}

static
TArray<FFrameTime>
InnerToOuter( const UMovieSceneSubSection* iOuterSection, TArray<FFrameTime> iInnerKeys )
{
    TArray<FFrameTime> converted_keys;

    FMovieSceneInverseSequenceTransform localToRootTransform = iOuterSection->OuterToInnerTransform().Inverse();
    for( auto key : iInnerKeys )
    {
        TOptional<FFrameTime> converted_key = localToRootTransform.TryTransformTime( key );
        if( !converted_key )
            continue;

        converted_keys.Add( *converted_key );
    }

    return converted_keys;
}

//static
TArray<FFrameTime>
BoardSequenceHelpers::GetCameraTransformTimesRecursive( const UMovieSceneSubSection& iBoardSection, TArray<FFrameTime>& oDefaultFrameTimes )
{
    TArray<FFrameTime> keys;

    UMovieSceneSequence* innerMovieSceneSequence = iBoardSection.GetSequence();
    if( !innerMovieSceneSequence )
        return keys;

    // if we are on a shot subsequence
    if( innerMovieSceneSequence->IsA<UShotSequence>() )
    {
        TOptional<FFrameNumber> default_frame;
        TArray<FFrameNumber> subframes = ShotSequenceHelpers::GetCameraTransformTimes( innerMovieSceneSequence, &default_frame );
        if( default_frame )
        {
            TArray<FFrameTime> subkeys2( { *default_frame } );
            oDefaultFrameTimes.Append( InnerToOuter( &iBoardSection, subkeys2 ) );
        }

        TArray<FFrameTime> subkeys( subframes );
        keys = InnerToOuter( &iBoardSection, subkeys );

        return keys;
    }

    // if we are on a board subsequence
    if( innerMovieSceneSequence->IsA<UBoardSequence>() )
    {
        UMovieScene* innerMovieScene = innerMovieSceneSequence->GetMovieScene();
        if( !innerMovieScene )
            return keys;

        UMovieSceneCinematicBoardTrack* board_track = innerMovieScene->FindTrack<UMovieSceneCinematicBoardTrack>();
        if( !board_track )
            return keys;

        TArray<FFrameTime> subkeys;
        for( auto section : board_track->GetAllSections() )
        {
            UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );
            TArray<FFrameTime> section_keys;
            section_keys = GetCameraTransformTimesRecursive( *subsection, oDefaultFrameTimes );

            subkeys.Append( section_keys );
        }

        keys = InnerToOuter( &iBoardSection, subkeys );
        oDefaultFrameTimes = InnerToOuter( &iBoardSection, oDefaultFrameTimes );

        return keys;
    }

    return keys;
}

//---

//static
TArray<UMovieScene3DTransformSection*>
ShotSequenceHelpers::GetCameraTransformSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FGuid& iCameraBinding )
{
    TArray<UMovieScene3DTransformSection*> sections;

    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !moviescene )
        return sections;

    if( !iCameraBinding.IsValid() )
        return sections;

    UMovieScene3DTransformTrack* track = moviescene->FindTrack<UMovieScene3DTransformTrack>( iCameraBinding );
    if( !track )
        return sections;

    for( auto section : track->GetAllSections() )
        sections.Add( Cast<UMovieScene3DTransformSection>( section ) );

    return sections;
}

//static
TArray<UMovieScene3DTransformSection*>
ShotSequenceHelpers::GetAnimationTransformSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FGuid& iAnimationBinding )
{
    TArray<UMovieScene3DTransformSection*> sections;

    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !moviescene )
        return sections;

    if( !iAnimationBinding.IsValid() )
        return sections;

    UMovieSceneTrack* track = moviescene->FindTrack<UMovieScene3DTransformTrack>( iAnimationBinding );
    if( !track )
        return sections;

    for( auto section : track->GetAllSections() )
        sections.Add( Cast<UMovieScene3DTransformSection>( section ) );

    return sections;
}

//---

//static
FChannelProxyBySectionMap
BoardSequenceHelpers::BuildCameraTransformChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSubSection, iSequenceID );

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return FChannelProxyBySectionMap();

    return ShotSequenceHelpers::BuildCameraTransformChannelProxy( iPlayer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
FChannelProxyBySectionMap
ShotSequenceHelpers::BuildCameraTransformChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    FChannelProxyBySectionMap map;

    FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( iPlayer, iSequence, iSequenceID );

    //---

    TArray<UMovieScene3DTransformSection*> camera_transform_sections = ShotSequenceHelpers::GetCameraTransformSections( iPlayer, iSequence, iSequenceID, camera_binding );
    for( auto camera_transform_section : camera_transform_sections )
    {
        FMovieSceneChannelProxyData ChannelIndirection;

        const FMovieSceneChannelEntry* DoubleChannelEntry = camera_transform_section->GetChannelProxy().FindEntry( FMovieSceneDoubleChannel::StaticStruct()->GetFName() );
        if( DoubleChannelEntry )
        {
#if WITH_EDITOR
            TArrayView<FMovieSceneChannel* const>              DoubleChannels = DoubleChannelEntry->GetChannels();
            TArrayView<const FMovieSceneChannelMetaData>       MetaData = DoubleChannelEntry->GetMetaData();
            TArrayView<const TMovieSceneExternalValue<double>> MetaDataExt = DoubleChannelEntry->GetAllExtendedEditorData<FMovieSceneDoubleChannel>();

            for( int32 Index = 0; Index < DoubleChannels.Num(); ++Index )
            {
                ChannelIndirection.Add( *static_cast<FMovieSceneDoubleChannel*>( DoubleChannels[Index] ), MetaData[Index], MetaDataExt[Index] );
            }
#else
            TArrayView<FMovieSceneChannel* const>              DoubleChannels = DoubleChannelEntry->GetChannels();

            for( int32 Index = 0; Index < DoubleChannels.Num(); ++Index )
            {
                ChannelIndirection.Add( *static_cast<FMovieSceneDoubleChannel*>( DoubleChannels[Index] ) );
            }
#endif
        }

        TSharedPtr<FMovieSceneChannelProxy> ChannelProxy = MakeShared<FMovieSceneChannelProxy>( MoveTemp( ChannelIndirection ) );

        map.Add( camera_transform_section, ChannelProxy );

        // UDN: Hook into TransformSection::OnSignatureChangedEvent to invalidate this section's channel proxy if the transform is changed.
        // Set the delegate to the whole subsequence, then every changes (even removing section) will call the it
        //if( !camera_transform_section->OnSignatureChanged().IsBoundToObject( this ) )
        //    camera_transform_section->OnSignatureChanged().AddUObject( this, &UMovieSceneCinematicBoardSection::HandleInvalidateChannelProxy );
    }

    return map;
}

//static
TMap<FGuid, FChannelProxyBySectionMap>
BoardSequenceHelpers::BuildAnimationsTransformChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSubSection, iSequenceID );

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return TMap<FGuid, FChannelProxyBySectionMap>();

    return ShotSequenceHelpers::BuildAnimationsTransformChannelProxy( iPlayer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
TMap<FGuid, FChannelProxyBySectionMap>
ShotSequenceHelpers::BuildAnimationsTransformChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    TMap<FGuid, FChannelProxyBySectionMap> maps;

    TArray<FGuid> animation_bindings = ShotSequenceHelpers::GetAnimationBindings( iPlayer, iSequence, iSequenceID );

    for( auto animation_binding : animation_bindings )
    {
        FChannelProxyBySectionMap map;

        //---

        TArray<UMovieScene3DTransformSection*> animation_transform_sections = ShotSequenceHelpers::GetAnimationTransformSections( iPlayer, iSequence, iSequenceID, animation_binding );
        for( auto animation_transform_section : animation_transform_sections )
        {
            FMovieSceneChannelProxyData ChannelIndirection;

            const FMovieSceneChannelEntry* DoubleChannelEntry = animation_transform_section->GetChannelProxy().FindEntry( FMovieSceneDoubleChannel::StaticStruct()->GetFName() );
            if( DoubleChannelEntry )
            {
#if WITH_EDITOR
                TArrayView<FMovieSceneChannel* const>              DoubleChannels = DoubleChannelEntry->GetChannels();
                TArrayView<const FMovieSceneChannelMetaData>       MetaData = DoubleChannelEntry->GetMetaData();
                TArrayView<const TMovieSceneExternalValue<double>> MetaDataExt = DoubleChannelEntry->GetAllExtendedEditorData<FMovieSceneDoubleChannel>();

                for( int32 Index = 0; Index < DoubleChannels.Num(); ++Index )
                {
                    ChannelIndirection.Add( *static_cast<FMovieSceneDoubleChannel*>( DoubleChannels[Index] ), MetaData[Index], MetaDataExt[Index] );
                }
#else
                TArrayView<FMovieSceneChannel* const>              DoubleChannels = DoubleChannelEntry->GetChannels();

                for( int32 Index = 0; Index < DoubleChannels.Num(); ++Index )
                {
                    ChannelIndirection.Add( *static_cast<FMovieSceneDoubleChannel*>( DoubleChannels[Index] ) );
                }
#endif
            }

            TSharedPtr<FMovieSceneChannelProxy> ChannelProxy = MakeShared<FMovieSceneChannelProxy>( MoveTemp( ChannelIndirection ) );

            map.Add( animation_transform_section, ChannelProxy );

            // UDN: Hook into TransformSection::OnSignatureChangedEvent to invalidate this section's channel proxy if the transform is changed.
            // Set the delegate to the whole subsequence, then every changes (even removing section) will call the it
            //if( !camera_transform_section->OnSignatureChanged().IsBoundToObject( this ) )
            //    camera_transform_section->OnSignatureChanged().AddUObject( this, &UMovieSceneCinematicBoardSection::HandleInvalidateChannelProxy );
        }

        maps.Add( animation_binding, map );
    }

    return maps;
}

//static
TMap<FGuid, FChannelProxyBySectionMap>
BoardSequenceHelpers::BuildAnimationsTimelineChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSubSection, iSequenceID );

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return TMap<FGuid, FChannelProxyBySectionMap>();

    return ShotSequenceHelpers::BuildAnimationsTimelineChannelProxy( iPlayer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
TMap<FGuid, FChannelProxyBySectionMap>
ShotSequenceHelpers::BuildAnimationsTimelineChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    TMap<FGuid, FChannelProxyBySectionMap> maps;

    TArray<FGuid> animation_bindings = ShotSequenceHelpers::GetAnimationBindings( iPlayer, iSequence, iSequenceID );

    for( FGuid animation_binding : animation_bindings )
    {
        FChannelProxyBySectionMap map;

        //---

        FFindOrCreateTimelineResult result = FindTimelineTrackAndSections( iPlayer, iSequence, iSequenceID, animation_binding );

        for( TWeakObjectPtr<UOdysseyAnimationTimelineSection> animation_timeline_section : result.mSections )
        {
            FMovieSceneChannelProxyData ChannelIndirection;

            //const FMovieSceneChannelEntry* ObjectPathChannelEntry = animation_timeline_section->GetChannelProxy().FindEntry( FMovieSceneObjectPathChannel::StaticStruct()->GetFName() );
            //if( ObjectPathChannelEntry )
            //{
#if WITH_EDITOR
                ChannelIndirection.Add( animation_timeline_section->GetAnimationCutChannel(), FMovieSceneChannelMetaData() );
#else
                ChannelIndirection.Add( animation_timeline_section->GetAnimationCutChannel() );
#endif
            //}

            TSharedPtr<FMovieSceneChannelProxy> ChannelProxy = MakeShared<FMovieSceneChannelProxy>( MoveTemp( ChannelIndirection ) );

            map.Add( animation_timeline_section, ChannelProxy );

            // UDN: Hook into TransformSection::OnSignatureChangedEvent to invalidate this section's channel proxy if the transform is changed.
            // Set the delegate to the whole subsequence, then every changes (even removing section) will call the it
            //if( !camera_transform_section->OnSignatureChanged().IsBoundToObject( this ) )
            //    camera_transform_section->OnSignatureChanged().AddUObject( this, &UMovieSceneCinematicBoardSection::HandleInvalidateChannelProxy );
        }

        maps.Add( animation_binding, map );
    }

    return maps;
}

//static
TMap<FGuid, FChannelProxyBySectionMap>
BoardSequenceHelpers::BuildAnimationsOpacityChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSubSection, iSequenceID );

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return TMap<FGuid, FChannelProxyBySectionMap>();

    return ShotSequenceHelpers::BuildAnimationsOpacityChannelProxy( iPlayer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
TMap<FGuid, FChannelProxyBySectionMap>
ShotSequenceHelpers::BuildAnimationsOpacityChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    TMap<FGuid, FChannelProxyBySectionMap> maps;

    TArray<FGuid> animation_bindings = ShotSequenceHelpers::GetAnimationBindings( iPlayer, iSequence, iSequenceID );

    for( auto animation_binding : animation_bindings )
    {
        FChannelProxyBySectionMap map;

        //---

        FFindOrCreateMaterialParameterResult result = FindMaterialParameterTrackAndSections( iPlayer, iSequence, iSequenceID, animation_binding );

        for( auto animation_opacity_section : result.mSections )
        {
            FMovieSceneChannelProxyData ChannelIndirection;

            const FMovieSceneChannelEntry* FloatChannelEntry = animation_opacity_section->GetChannelProxy().FindEntry( FMovieSceneFloatChannel::StaticStruct()->GetFName() );
            if( FloatChannelEntry )
            {
#if WITH_EDITOR
                TArrayView<FMovieSceneChannel* const>                   FloatChannels = FloatChannelEntry->GetChannels();
                TArrayView<const FMovieSceneChannelMetaData>            MetaData = FloatChannelEntry->GetMetaData();
                TArrayView<const TMovieSceneExternalValue<float>>       MetaDataExt = FloatChannelEntry->GetAllExtendedEditorData<FMovieSceneFloatChannel>();

                for( int32 Index = 0; Index < FloatChannels.Num(); ++Index )
                {
                    if( MetaData[Index].Name.IsEqual( TEXT("DrawingOpacity") ) )
                        ChannelIndirection.Add( *static_cast<FMovieSceneFloatChannel*>( FloatChannels[Index] ), MetaData[Index], MetaDataExt[Index] );
                }
#else
                TArrayView<FMovieSceneChannel* const>                   FloatChannels = FloatChannelEntry->GetChannels();

                for( int32 Index = 0; Index < FloatChannels.Num(); ++Index )
                {
                    ChannelIndirection.Add( *static_cast<FMovieSceneFloatChannel*>( FloatChannels[Index] ) );
                }
#endif
            }

            TSharedPtr<FMovieSceneChannelProxy> ChannelProxy = MakeShared<FMovieSceneChannelProxy>( MoveTemp( ChannelIndirection ) );

            map.Add( animation_opacity_section, ChannelProxy );

            // UDN: Hook into TransformSection::OnSignatureChangedEvent to invalidate this section's channel proxy if the transform is changed.
            // Set the delegate to the whole subsequence, then every changes (even removing section) will call the it
            //if( !camera_transform_section->OnSignatureChanged().IsBoundToObject( this ) )
            //    camera_transform_section->OnSignatureChanged().AddUObject( this, &UMovieSceneCinematicBoardSection::HandleInvalidateChannelProxy );
        }

        maps.Add( animation_binding, map );
    }

    return maps;
}

//---

//static
TArray<UStoryNote*>
UMoviePipelineStoryboardBlueprintLibrary::GetNotes( const UMoviePipeline* iMoviePipeline )
{
    TArray<UStoryNote*> notes;

    ULevelSequence* level_sequence = iMoviePipeline->GetTargetSequence();

    FFrameRate effective_framerate = iMoviePipeline->GetPipelinePrimaryConfig()->GetEffectiveFrameRate( iMoviePipeline->GetTargetSequence() );

    //FTimecode master_timecode = UMoviePipelineBlueprintLibrary::GetMasterTimecode( iMoviePipeline );
    FFrameNumber master_current_frame_in_levelsequence = UMoviePipelineBlueprintLibrary::GetRootFrameNumber( iMoviePipeline );
    FFrameNumber master_current_frame = FFrameRate::TransformTime( master_current_frame_in_levelsequence, effective_framerate, level_sequence->GetMovieScene()->GetTickResolution() ).GetFrame();

    ////FTimecode shot_timecode = UMoviePipelineBlueprintLibrary::GetCurrentShotTimecode( iMoviePipeline );
    //FFrameNumber shot_current_frame_in_levelsequence = UMoviePipelineBlueprintLibrary::GetCurrentShotFrameNumber( iMoviePipeline );
    //FFrameNumber shot_current_frame = FFrameRate::TransformTime( master_current_frame_in_levelsequence, effective_framerate, level_sequence->GetMovieScene()->GetTickResolution() ).GetFrame();

    //---

    return EposSequenceHelpers::GetNotesRecursive( level_sequence, master_current_frame );
}

#undef LOCTEXT_NAMESPACE
