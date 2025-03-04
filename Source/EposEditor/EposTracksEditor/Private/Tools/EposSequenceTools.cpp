// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/EposSequenceTools.h"

#include "AnimatedRange.h"
#include "CineCameraActor.h"
#include "ISequencer.h"
#include "MovieScene.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "MovieSceneTimeHelpers.h"
#include "LevelEditorViewport.h"

#include "Board/BoardSequence.h"
#include "EposSequenceHelpers.h"
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

    UScalingComponent* scaling_component = ioActor->FindComponentByClass<UScalingComponent>();
    if( !scaling_component )
        return false;

    float old_distance = FVector::Distance( iCamera->GetActorLocation(), ioActor->GetActorLocation() );
    FVector old_scale = ioActor->GetActorScale3D();
    FVector old_camera_view_size = scaling_component->ComputeSizeOfCameraView( iCamera, old_distance );
    FVector old_scale_camera100 = scaling_component->ComputeScaleWithScaleAndMargin( old_camera_view_size );

    FVector new_camera_view_size = scaling_component->ComputeSizeOfCameraView( iCamera, iNewDistance );

    FVector new_animation_location = iCamera->GetActorLocation() + ( ioActor->GetActorLocation() - iCamera->GetActorLocation() ).GetSafeNormal() * iNewDistance;

    ioActor->SetActorLocation( new_animation_location );

    switch( iScaleType )
    {
        case EScaleActor::kFitToCamera:
        {
            FVector scale = scaling_component->ComputeScaleWithScaleAndMargin( new_camera_view_size );
            ioActor->SetActorScale3D( scale );
        }
        break;

        case EScaleActor::kRelativeScale:
        {
            FVector new_scale_camera100 = scaling_component->ComputeScaleWithScaleAndMargin( new_camera_view_size );
            FVector ratio = new_scale_camera100 / old_scale_camera100;
            FVector new_scale = old_scale * ratio;

            ioActor->SetActorScale3D( new_scale );
        }
        break;

        case EScaleActor::kNo:
            // nothing to do
            break;

        default: checkNoEntry();
    }

    return true;
}


#undef LOCTEXT_NAMESPACE
