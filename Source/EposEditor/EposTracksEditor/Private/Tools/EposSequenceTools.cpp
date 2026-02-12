// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/EposSequenceTools.h"

#include "AnimatedRange.h"
#include "CineCameraActor.h"
#include "Engine/Selection.h"
#include "ISequencer.h"
#include "LevelEditorViewport.h"
#include "MovieScene.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "MovieSceneTimeHelpers.h"
#include "Sections/MovieSceneSubSection.h"

#include "ActorHelpers.h"
#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposSequenceHelpers.h"
#include "OdysseyAnimationActor.h"
#include "ScalingComponent.h"
#include "Shot/ShotSequence.h"

#define LOCTEXT_NAMESPACE "EposSequenceTools"

//---

void
EjectAnyActor()
{
    if( GCurrentLevelEditingViewportClient && GCurrentLevelEditingViewportClient->IsAnyActorLocked() )
    {
        if( GCurrentLevelEditingViewportClient && GCurrentLevelEditingViewportClient->GetViewMode() != VMI_Unknown && GCurrentLevelEditingViewportClient->AllowsCinematicControl() )
        {
            GCurrentLevelEditingViewportClient->SetCinematicActorLock( nullptr );
            GCurrentLevelEditingViewportClient->SetActorLock( nullptr );
            GCurrentLevelEditingViewportClient->bLockedCameraView = false;
            GCurrentLevelEditingViewportClient->ViewFOV = GCurrentLevelEditingViewportClient->FOVAngle;
            GCurrentLevelEditingViewportClient->RemoveCameraRoll();
            GCurrentLevelEditingViewportClient->UpdateViewForLockedActor();
            GCurrentLevelEditingViewportClient->Invalidate();
        }
    }
}

//---

void BoardSequenceTools::UpdateViewRange( ISequencer* iSequencer, TRange<FFrameNumber> iNewRange )
{
    if( iNewRange.IsEmpty() )
        return;

    TRange<double> working_range = iSequencer->GetFocusedMovieSceneSequence()->GetMovieScene()->GetEditorData().GetWorkingRange(); // iSequencer->GetClampRange() is only inside FSequencer and not ISequencer
    working_range = UE::MovieScene::DilateRange( working_range, 0., ( iNewRange / iSequencer->GetFocusedTickResolution() ).Size<double>() );
    iSequencer->SetClampRange( working_range );

    TRange<double> view_range = iSequencer->GetViewRange();
    view_range = UE::MovieScene::DilateRange( view_range, 0., ( iNewRange / iSequencer->GetFocusedTickResolution() ).Size<double>() );
    iSequencer->SetViewRange( view_range );
}

//---

ShotSequenceTools::cTemporarySwitchInner::cTemporarySwitchInner( ISequencer& iSequencer, FMovieSceneSequenceIDRef iInnerID )
    : mSequencer( iSequencer )
    , mOriginalId()
    , mOriginalGlobalTime()
{
    mOriginalId = mSequencer.GetFocusedTemplateID();
    if( iInnerID == mOriginalId )
        return;

    FFrameRate display_rate = mSequencer.GetFocusedDisplayRate();
    FFrameRate tick_resolution = mSequencer.GetFocusedTickResolution();
    mOriginalGlobalTime = ConvertFrameTime( mSequencer.GetGlobalTime().Time, tick_resolution, display_rate );

    UMovieSceneSubSection* subsection = mSequencer.FindSubSection( iInnerID );
    check( subsection );
    mSequencer.FocusSequenceInstance( *subsection );
}

ShotSequenceTools::cTemporarySwitchInner::~cTemporarySwitchInner()
{
    FMovieSceneSequenceID focused_id = mSequencer.GetFocusedTemplateID();
    if( focused_id == mOriginalId )
        return;

    //mSequencer.PopToSequenceInstance( mOriginalId );

    if( mOriginalId == MovieSceneSequenceID::Root )
    {
        //mSequencer.ResetToNewRootSequence( *mSequencer.GetRootMovieSceneSequence() );
        mSequencer.PopToSequenceInstance( mOriginalId );
    }
    else
    {
        UMovieSceneSubSection* subsection = mSequencer.FindSubSection( mOriginalId );
        check( subsection );
        mSequencer.FocusSequenceInstance( *subsection );
    }

    FFrameRate display_rate = mSequencer.GetFocusedDisplayRate();
    FFrameRate tick_resolution = mSequencer.GetFocusedTickResolution();
    mSequencer.SetGlobalTime( ConvertFrameTime( mOriginalGlobalTime, display_rate, tick_resolution ) );
}

//---

//static
void
BoardSequenceTools::RenameBinding( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iBinding, FString iNewLabel )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UBoardSequence>() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return;

    ShotSequenceTools::RenameBinding( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iBinding, iNewLabel );
}

//static
void
ShotSequenceTools::RenameBinding( ISequencer* iSequencer, FGuid iBinding, FString iNewLabel )
{
    ShotSequenceTools::RenameBinding( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iBinding, iNewLabel );
}

//static
void
ShotSequenceTools::RenameBinding( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iBinding, FString iNewLabel )
{
    check( iSequence->IsA<UShotSequence>() );

    UMovieScene* movie_scene = iSequence->GetMovieScene();
    FMovieScenePossessable* possessable = movie_scene ? movie_scene->FindPossessable( iBinding ) : nullptr;
    if( !possessable )
        return;

    auto objects = iSequencer.FindBoundObjects( iBinding, iSequenceID );
    AActor* actor = nullptr;
    if( objects.Num() )
        actor = Cast<AActor>( objects[0] );

    //---

    const FScopedTransaction transaction( LOCTEXT( "SetTrackName", "Set Track Name" ) );

    FMovieScenePossessable new_possessable( *possessable );
    new_possessable.SetName( iNewLabel );
    movie_scene->ReplacePossessable( iBinding, new_possessable );

    //---

    if( actor )
        FActorLabelUtilities::RenameExistingActor( actor, new_possessable.GetName() );

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::RefreshAllImmediately );
}

//---

TArray<ShotSequenceTools::FBindingAndActorClass> ShotSequenceTools::mDirectBindingsSelectedHistory;

//static
void
BoardSequenceTools::AddSelectedActorToHistory( ISequencer* iSequencer, AActor* iActor )
{
    ShotSequenceTools::AddSelectedActorToHistory( iSequencer, iActor );
}

//static
void
ShotSequenceTools::AddSelectedActorToHistory( ISequencer* iSequencer, AActor* iActor )
{
    if( !iActor )
        return;

    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    FMovieSceneSequenceID sequenceId = iSequencer->GetFocusedTemplateID();

    if( sequence->IsA<UBoardSequence>() )
    {
        UMovieScene* moviescene = sequence->GetMovieScene();
        UMovieSceneCinematicBoardTrack* board_track = moviescene ? moviescene->FindTrack<UMovieSceneCinematicBoardTrack>() : nullptr;
        TArray<UMovieSceneSection*> sections = board_track ? board_track->GetAllSections() : TArray<UMovieSceneSection*>();
        for( UMovieSceneSection* section : sections )
        {
            UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );
            if( !subsection )
                continue;

            FGuid binding = iSequencer->FindCachedObjectId( *iActor, subsection->GetSequenceID() );
            if( binding.IsValid() )
            {
                FBindingAndActorClass entry = { binding, subsection->GetSequenceID(), iActor->GetClass() };
                mDirectBindingsSelectedHistory.RemoveAll( [entry]( const FBindingAndActorClass& iEntry )
                                                          {
                                                              return iEntry.Guid == entry.Guid;
                                                          } );
                mDirectBindingsSelectedHistory.Add( entry );
            }
        }
    }
    else if( sequence->IsA<UShotSequence>() )
    {
        FGuid binding = iSequencer->FindCachedObjectId( *iActor, sequenceId );
        if( binding.IsValid() )
        {
            FBindingAndActorClass entry = { binding, sequenceId, iActor->GetClass() };
            mDirectBindingsSelectedHistory.RemoveAll( [entry]( const FBindingAndActorClass& iEntry )
                                                      {
                                                          return iEntry.Guid == entry.Guid;
                                                      } );
            mDirectBindingsSelectedHistory.Add( entry );
        }
    }

}

//static
AActor*
BoardSequenceTools::GuessActorToSelect( ISequencer* iSequencer, const FFrameNumber& iFrameNumber )
{
    // This is no more used as there are some times where this selection is auto-empty by the engine
    // so we can't rely on it to always guess the animation/camera to auto-select
    //
    TArray<AActor*> actor_selected;
    //// Store the currently selected actor to guess the type of actor (camera or animation) to auto-select
    //USelection* selection = GEditor->GetSelectedActors();
    //TArray<AActor*> actor_selected;
    //selection->GetSelectedObjects<AActor>( actor_selected );

    AActor* actor_to_select = ShotSequenceTools::GuessActorToSelect( iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, actor_selected );
    // If already selected, nothing to do
    //if( actor_selected.Contains( actor_to_select ) )
    //    return nullptr;

    return actor_to_select;
}

//static
AActor*
ShotSequenceTools::GuessActorToSelect( ISequencer* iSequencer, const FFrameNumber& iFrameNumber )
{
    // This is no more used as there are some times where this selection is auto-empty by the engine
    // so we can't rely on it to always guess the animation/camera to auto-select
    //
    TArray<AActor*> actor_selected;
    //// Store the currently selected actor to guess the type of actor (camera or animation) to auto-select
    //USelection* selection = GEditor->GetSelectedActors();
    //TArray<AActor*> actor_selected;
    //selection->GetSelectedObjects<AActor>( actor_selected );

    AActor* actor_to_select = ShotSequenceTools::GuessActorToSelect( iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, actor_selected );
    // If already selected, nothing to do
    //if( actor_selected.Contains( actor_to_select ) )
    //    return nullptr;

    return actor_to_select;
}

//static
AActor*
ShotSequenceTools::GuessActorToSelect( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceId, const FFrameNumber& iFrameNumber, const TArray<AActor*>& /*iLastSelectedActors*/ )
{
    // See comment above in GuessActorToSelect() to know why iLastSelectedActors is no more used
    //TArray<AActor*> actors = iLastSelectedActors.FilterByPredicate( []( const AActor* iActor )
    //                                                                {
    //                                                                    return iActor->IsA<ACineCameraActor>();
    //                                                                } );

    // Actor may be destroyed, for example:
    // - drag on some shots (to select some animations)
    // - open another map
    // - actors in the cache are all invalid
    mDirectBindingsSelectedHistory.RemoveAll( []( FBindingAndActorClass iEntry )
                                              {
                                                  return !iEntry.Guid.IsValid();
                                              } );

    // Auto-select camera if it's the last actor type directly selected by the user
    if( mDirectBindingsSelectedHistory.Num()
        && mDirectBindingsSelectedHistory.Last().ActorClass == ACineCameraActor::StaticClass() )
    //if( actors.Num() )
    {
        if( iSequence->IsA<UBoardSequence>() )
        {
            UMovieSceneSequence* sequence = nullptr;
            FMovieSceneSequenceID sequenceId = MovieSceneSequenceID::Invalid;
            FGuid camera_binding = BoardSequenceHelpers::GetCameraBindingRecursive( *iSequencer, iSequence, iSequenceId, iFrameNumber, &sequence, &sequenceId );
            ACineCameraActor* camera = ShotSequenceHelpers::GetCameraSpawned( *iSequencer, sequence, sequenceId, camera_binding );

            return camera;
        }
        else if( iSequence->IsA<UShotSequence>() )
        {
            FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( *iSequencer, iSequence, iSequenceId );
            ACineCameraActor* camera = ShotSequenceHelpers::GetCameraSpawned( *iSequencer, iSequence, iSequenceId, camera_binding );

            return camera;
        }
    }

    //---

    // Otherwise *always* return an animation (if there is one at the current frame)

    //actors = iLastSelectedActors.FilterByPredicate( []( const AActor* iActor )
    //                                                {
    //                                                    return iActor->IsA<AOdysseyAnimationActor>();
    //                                                } );
    //if( ShotSequenceTools::mDirectActorsSelectedHistory.Num()
    //    && ShotSequenceTools::mDirectActorsSelectedHistory.Last()->IsA<AOdysseyAnimationActor>() )
    //if( actors.Num() )
    {
        TArray<AOdysseyAnimationActor*> animations;
        TArray<FGuid> unordered_bindings;
        UMovieSceneSequence* sequence = nullptr;
        FMovieSceneSequenceID sequenceId = MovieSceneSequenceID::Invalid;
        if( iSequence->IsA<UBoardSequence>() )
        {
            BoardSequenceHelpers::GetAllAnimationsRecursive( *iSequencer, iSequence, iSequenceId, EGetAnimation::kAll, iFrameNumber, &animations, &unordered_bindings, &sequence, &sequenceId );
        }
        else if( iSequence->IsA<UShotSequence>() )
        {
            sequence = iSequence;
            sequenceId = iSequenceId;
            ShotSequenceHelpers::GetAllAnimations( *iSequencer, iSequence, iSequenceId, EGetAnimation::kAll, &animations, &unordered_bindings );
        }

        // Animation not found AND no sequence (shot) found, so nothing can be guess
        if( !sequence )
            return nullptr;

        TArray<AOdysseyAnimationActor*> ordered_animations;
        TArray<FGuid> ordered_bindings;
        ShotSequenceTools::SortBindings( animations, unordered_bindings, sequence->GetMovieScene(), &ordered_animations, &ordered_bindings );

        if( ordered_bindings.Num() )
        {
            int32 max_preferred_index = INDEX_NONE;
            for( FGuid binding : ordered_bindings )
            {
                int32 current_index = mDirectBindingsSelectedHistory.FindLastByPredicate( [binding]( const FBindingAndActorClass& iEntry )
                                                                                          {
                                                                                              return iEntry.Guid == binding;
                                                                                          } );
                if( current_index != INDEX_NONE )
                {
                    if( current_index > max_preferred_index )
                        max_preferred_index = current_index;
                }
            }

            AActor* preferred_animation = nullptr;
            if( mDirectBindingsSelectedHistory.IsValidIndex( max_preferred_index ) )
            {
                TArrayView<TWeakObjectPtr<>> weakObjects = iSequencer->FindBoundObjects( mDirectBindingsSelectedHistory[max_preferred_index].Guid, mDirectBindingsSelectedHistory[max_preferred_index].SequenceId );
                if( weakObjects.Num() )
                    preferred_animation = Cast<AActor>( weakObjects[0] );
            }

            if( !preferred_animation )
            {
                preferred_animation = ordered_animations[0];

                if( preferred_animation )
                {
                    FBindingAndActorClass entry = { ordered_bindings[0], sequenceId, preferred_animation->GetClass() };
                    mDirectBindingsSelectedHistory.Add( entry );
                }
            }

            return preferred_animation;
        }
    }

    //---

    return nullptr;
}

//---

//static
void
ShotSequenceTools::SortBindings( TArray<AOdysseyAnimationActor*> iAnimationActors, TArray<FGuid> iBindings, UMovieScene* iMovieScene, TArray<AOdysseyAnimationActor*>* oOrderedAnimationActors, TArray<FGuid>* oOrderedBindings )
{
    check( iAnimationActors.Num() == iBindings.Num() );

    struct FBindingAndAnimationActor
    {
        FMovieSceneBinding* Binding;
        AOdysseyAnimationActor* AnimationActor;
    };

    // Find their corresponding scene binding
    TArray<FBindingAndAnimationActor> binding_and_animation_actors;
    for( int i = 0; i < iBindings.Num(); i++ )
    {
        binding_and_animation_actors.Add( { iMovieScene->FindBinding( iBindings[i] ), iAnimationActors[i] } );
    }

    // Sort scene bindings by their sorting order/name
    // (This should match the native sorting of tracks inside shot)
    Algo::StableSort( binding_and_animation_actors, [iMovieScene]( const FBindingAndAnimationActor& iA, const FBindingAndAnimationActor& iB )
                      {
                          // If at least one of the binding was not already sorted (by drag'n drop in shot), use the name to sort both
                          if( iA.Binding->GetSortingOrder() == -1 || iB.Binding->GetSortingOrder() == -1 )
                          {
                              FString nameA = iMovieScene->GetObjectDisplayName( iA.Binding->GetObjectGuid() ).ToString();
                              FString nameB = iMovieScene->GetObjectDisplayName( iB.Binding->GetObjectGuid() ).ToString();

                              return nameA < nameB;
                          }
                          // Otherwise just use the set sorting order
                          else
                              return iA.Binding->GetSortingOrder() < iB.Binding->GetSortingOrder();
                      } );

    // Get all animations in the gui order
    for( auto ordered_binding_and_animation_actor : binding_and_animation_actors )
    {
        oOrderedBindings->Add( ordered_binding_and_animation_actor.Binding->GetObjectGuid() );
        oOrderedAnimationActors->Add( ordered_binding_and_animation_actor.AnimationActor );
    }
}

//---

//static
bool
ShotSequenceTools::CanMoveAndScaleActor( const AActor* iActor, const ACineCameraActor* iCamera )
{
    if( !iActor || !iCamera )
        return false;

    // Already normalized
    FVector camera_lookat = iCamera->GetActorForwardVector();
    FVector animation_lookat = iActor->GetActorUpVector();

    // Check if the animation (as an infinit mathematical plane) is orthogonal to the camera
    // but the center of the animation is not necessary facing the camera
    if( !FVector::Parallel( camera_lookat, animation_lookat, FMath::Cos( FMath::DegreesToRadians( 0.1f ) ) /* tolerance is less than 0.1° */ ) )
        return false;

    FVector camera_to_animation( iActor->GetActorLocation() - iCamera->GetActorLocation() );
    camera_to_animation.Normalize();

    // Now check if the center of the animation is facing the camera
    if( !camera_lookat.Equals( camera_to_animation ) && !camera_lookat.Equals( -camera_to_animation ) )
        return false;
    // The tolerance between 2 parallels is the cosinus of angle
    // and by default, 2 parallels are considered parallel if the angle is less than 1°
    // and the angle can't be reduce too much due to precision
    // so, just compare the 2 normalized vectors with the default tolerance
    // (in addition, as it is the same base point, coplanar just check parallelism)
    //if( !FVector::Coplanar( iCamera->GetActorLocation(), camera_lookat, iCamera->GetActorLocation(), camera_to_animation, FMath::Cos( FMath::DegreesToRadians( 0.1f ) ) ) )
    //    return false;

    return true;
};

//static
bool
ShotSequenceTools::MoveAndScaleActor( AActor* ioActor, const ACineCameraActor* iCamera, float iNewDistance, EScaleActor iScaleType )
{
    if( !ShotSequenceTools::CanMoveAndScaleActor( ioActor, iCamera ) )
        return false;

    if( FMath::IsNearlyZero( iNewDistance ) )
        return false;

    struct FParameterCache
    {
        float mDistanceFromCamera;
        FVector mActorScale;
        FVector mCameraViewSize;
    };

    FParameterCache old_parameter;
    old_parameter.mActorScale = ioActor->GetActorScale3D();
    old_parameter.mDistanceFromCamera = FVector::Distance( iCamera->GetActorLocation(), ioActor->GetActorLocation() );
    old_parameter.mCameraViewSize = ActorHelpers::ComputeSizeOfCameraView( iCamera, old_parameter.mDistanceFromCamera );

    FVector new_camera_view_size = ActorHelpers::ComputeSizeOfCameraView( iCamera, iNewDistance );

    FVector new_animation_location = iCamera->GetActorLocation() + ( ioActor->GetActorLocation() - iCamera->GetActorLocation() ).GetSafeNormal() * iNewDistance;
    ioActor->SetActorLocation( new_animation_location );

    switch( iScaleType )
    {
        // Until EScaleActor::kFitToCamera will be removed
        PRAGMA_DISABLE_DEPRECATION_WARNINGS
        case EScaleActor::kFitToCamera:
        PRAGMA_ENABLE_DEPRECATION_WARNINGS
        case EScaleActor::kRelativeScale:
        {
            FVector ratio = new_camera_view_size / old_parameter.mCameraViewSize;
            FVector new_actor_scale = old_parameter.mActorScale * ratio;

            ioActor->SetActorScale3D( new_actor_scale );
        }
        break;

        case EScaleActor::kNo:
            // nothing to do
            break;

        default: checkNoEntry();
    }

    return true;
}

//static
bool
ShotSequenceTools::CanFitActorToCameraView( const AActor* iActor, const ACineCameraActor* iCamera )
{
    return CanMoveAndScaleActor( iActor, iCamera );
};

//static
bool
ShotSequenceTools::FitActorToCameraView( AActor* ioActor, const ACineCameraActor* iCamera )
{
    if( !CanFitActorToCameraView( ioActor, iCamera ) )
        return false;

    float distance = FVector::Distance( iCamera->GetActorLocation(), ioActor->GetActorLocation() );
    FVector new_camera_view_size = ActorHelpers::ComputeSizeOfCameraView( iCamera, distance );

    FVector scale = new_camera_view_size;
    UScalingComponent* scaling_component = ioActor->FindComponentByClass<UScalingComponent>();
    if( scaling_component )
        scale = scaling_component->ComputeScaleWithScaleAndMargin( new_camera_view_size );

    ioActor->SetActorScale3D( scale );

    return true;
}

#undef LOCTEXT_NAMESPACE
