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

#include "ActorHelpers.h"
#include "Board/BoardSequence.h"
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

TArray<TWeakObjectPtr<AActor>> BoardSequenceTools::mDirectActorsSelectedHistory;

//static
void
BoardSequenceTools::AddSelectedActorToHistory( AActor* iActor )
{
    if( !iActor )
        return;

    mDirectActorsSelectedHistory.Remove( iActor );
    mDirectActorsSelectedHistory.Add( iActor );
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

    AActor* actor_to_select = BoardSequenceTools::GuessActorToSelect( iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, actor_selected );
    // If already selected, nothing to do
    //if( actor_selected.Contains( actor_to_select ) )
    //    return nullptr;

    return actor_to_select;
}

//static
AActor*
BoardSequenceTools::GuessActorToSelect( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceId, const FFrameNumber& iFrameNumber, const TArray<AActor*>& /*iLastSelectedActors*/ )
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
    mDirectActorsSelectedHistory.RemoveAll( []( TWeakObjectPtr<AActor> iActor )
                                            {
                                                return !iActor.IsValid();
                                            } );

    // Auto-select camera if it's the last actor type directly selected by the user
    if( mDirectActorsSelectedHistory.Num()
        && mDirectActorsSelectedHistory.Last()->IsA<ACineCameraActor>() )
    //if( actors.Num() )
    {
        UMovieSceneSequence* sequence = nullptr;
        FMovieSceneSequenceID sequenceId = MovieSceneSequenceID::Invalid;
        ACineCameraActor* camera = BoardSequenceHelpers::GetCameraRecursive( *iSequencer, iSequence, iSequenceId, iFrameNumber, nullptr, &sequence, &sequenceId );

        return camera;
    }

    //---

    // Otherwise *always* return an animation (if there is one at the current frame)

    //actors = iLastSelectedActors.FilterByPredicate( []( const AActor* iActor )
    //                                                {
    //                                                    return iActor->IsA<AOdysseyAnimationActor>();
    //                                                } );
    //if( mDirectActorsSelectedHistory.Num()
    //    && mDirectActorsSelectedHistory.Last()->IsA<AOdysseyAnimationActor>() )
    //if( actors.Num() )
    {
        TArray<AOdysseyAnimationActor*> animations;
        TArray<FGuid> unordered_bindings;
        UMovieSceneSequence* sequence = nullptr;
        FMovieSceneSequenceID sequenceId = MovieSceneSequenceID::Invalid;
        BoardSequenceHelpers::GetAllAnimationsRecursive( *iSequencer, iSequence, iSequenceId, EGetAnimation::kAll, iFrameNumber, &animations, &unordered_bindings, &sequence, &sequenceId );

        // Animation not found AND no sequence (shot) found, so nothing can be guess
        if( !sequence )
            return nullptr;

        TArray<AOdysseyAnimationActor*> ordered_animations;
        TArray<FGuid> ordered_bindings;
        ShotSequenceTools::SortBindings( animations, unordered_bindings, sequence->GetMovieScene(), &ordered_animations, &ordered_bindings );

        if( ordered_animations.Num() )
        {
            int32 max_preferred_index = INDEX_NONE;
            for( AActor* animation : ordered_animations )
            {
                int32 current_index;
                if( mDirectActorsSelectedHistory.FindLast( animation, current_index ) )
                {
                    if( current_index > max_preferred_index )
                        max_preferred_index = current_index;
                }
            }

            AActor* preferred_animation = nullptr;
            if( mDirectActorsSelectedHistory.IsValidIndex( max_preferred_index ) )
                preferred_animation = mDirectActorsSelectedHistory[max_preferred_index].Get();

            if( !preferred_animation )
            {
                preferred_animation = ordered_animations[0];
                mDirectActorsSelectedHistory.Add( preferred_animation );
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
    Algo::StableSort( binding_and_animation_actors, []( const FBindingAndAnimationActor& iA, const FBindingAndAnimationActor& iB )
                      {
                          // If at least one of the binding was not already sorted (by drag'n drop in shot), use the name to sort both
                          if( iA.Binding->GetSortingOrder() == -1 || iB.Binding->GetSortingOrder() == -1 )
                              return iA.Binding->GetName() < iB.Binding->GetName();
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

    if( !FVector::Parallel( camera_lookat, animation_lookat ) )
        return false;

    FVector camera_to_animation( iActor->GetActorLocation() - iCamera->GetActorLocation() );
    camera_to_animation.Normalize();

    if( !FVector::Coplanar( iCamera->GetActorLocation(), camera_lookat, iCamera->GetActorLocation(), camera_to_animation ) )
        return false;

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
