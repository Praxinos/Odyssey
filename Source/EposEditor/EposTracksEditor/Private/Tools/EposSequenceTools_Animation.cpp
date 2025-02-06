// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/EposSequenceTools.h"

#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "Editor.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Factories/Texture2dFactoryNew.h"
#include "ISequencer.h"
#include "Kismet/GameplayStatics.h"
#include "LevelEditorActions.h"
#include "LevelEditorViewport.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieScene.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "Sections/MovieSceneBoolSection.h"
#include "Sections/MovieSceneSubSection.h"

#include "Board/BoardSequence.h"
#include "EposSequenceHelpers.h"
#include "NamingConvention.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationActor.h"
#include "OdysseyAnimationComponent.h"
#include "Settings/EposTracksEditorSettings.h"
#include "Tools/ResourceAssetTools.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "EposSequenceTools_Animation"

static
FVector
FindNextFreeAnimationLocation( UWorld* iWorld, FVector iAnimationLocation, FVector iCameraLocation )
{
    FVector next_location = iAnimationLocation;

    TArray<AActor*> existing_animations;
    UGameplayStatics::GetAllActorsOfClass( iWorld, AOdysseyAnimationActor::StaticClass(), existing_animations );

    auto ExistingAnimationOnLocation = [&existing_animations]( FVector iAnimationLocation )
    {
        for( auto existing_animation : existing_animations )
        {
            if( existing_animation->GetActorLocation().Equals( iAnimationLocation ) )
                return true;
        }

        return false;
    };

    while( true )
    {
        bool used_location = ExistingAnimationOnLocation( next_location );
        if( !used_location )
            break;

        FVector direction = ( iCameraLocation - next_location ).GetSafeNormal();
        next_location += direction * 0.01f;
    }

    return next_location;
}

//static
AOdysseyAnimationActor*
ShotSequenceTools::SpawnAnimation( UWorld* iWorld, ACineCameraActor* iCamera, float iSafeMargin, FVector2D iRelativeScaling )
{
    FActorSpawnParameters SpawnParams;
    AOdysseyAnimationActor* animation = iWorld->SpawnActor<AOdysseyAnimationActor>( SpawnParams );
    if( !animation )
        return nullptr;

    animation->SafeMargin = iSafeMargin;
    animation->RelativeScaling = iRelativeScaling;

    //---

    FTransform camera_transform = iCamera->GetRootComponent()->GetComponentTransform();

    FVector const CamLocation = camera_transform.GetLocation();
    FVector const CamDir = camera_transform.GetRotation().Vector();
    FRotator const CamRot = camera_transform.Rotator();

    //-

    // Make a function GuessAnimationLocation(...)
    float FocusDistance = 200;
    FVector animation_location = CamLocation + CamDir * FocusDistance;
    animation_location = FindNextFreeAnimationLocation( iWorld, animation_location, CamLocation );

    FVector animation_scale = animation->ComputeAnimationScaleWithScaleAndMargin( iCamera, FocusDistance );

    FRotator animation_rotator = CamRot;

    //---

    animation->SetActorScale3D( animation_scale );
    animation->SetActorLocation( animation_location );
    animation->SetActorRotation( FRotator( 0.f, 90.f, 90.f ) );
    animation->AddActorWorldRotation( animation_rotator );

    //animation->AttachToActor( iCamera, FAttachmentTransformRules::KeepRelativeTransform ); // Done in the editor with GEditor->ParentActors();

    return animation;
}

//static
AOdysseyAnimationActor*
ShotSequenceTools::SpawnAndBindAnimation( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iCameraGuid, ACineCameraActor* iCamera, FFrameNumber iFrameNumber, const FAnimationArgs& iAnimationArgs, FGuid* oGuid )
{
    if( !GCurrentLevelEditingViewportClient )
        return nullptr;

    //---

    UWorld* world = GCurrentLevelEditingViewportClient->GetWorld();

    GEditor->SelectNone( true, true );

    const UEposTracksEditorSettings* settings = GetDefault<UEposTracksEditorSettings>();
    float margin = settings->AnimationActorSettings.SafeMargin;
    FVector2D relative_scaling = settings->AnimationActorSettings.RelativeScaling;

    if( iAnimationArgs.mMargin.IsSet() )
        margin = iAnimationArgs.mMargin.GetValue();

    AOdysseyAnimationActor* animation = ShotSequenceTools::SpawnAnimation( world, iCamera, margin, relative_scaling );

    //---

    FIntPoint texture_size = animation->ComputeTextureSize( iCamera, settings->AnimationSettings.Height );

    //UMaterialInstanceConstant* new_animation = iAnimationArgs.mAnimation.IsValid()
    //                                           ? ProjectAssetTools::CreateAnimation( iSequencer, iSequence, iSequenceID, iAnimationArgs.mAnimation.Get() )
    //                                           : ProjectAssetTools::CreateAnimation( iSequencer, iSequence, iSequenceID, texture_size );
    TOptional<FLinearColor> background_layer_color;
    if( settings->AnimationSettings.AddLayerBackground )
        background_layer_color = settings->AnimationSettings.LayerBackgroundColor;

    UOdysseyAnimation* new_animation = ProjectAssetTools::CreateAnimation( iSequencer, iSequence, iSequenceID, texture_size, settings->AnimationSettings.Format, settings->AnimationSettings.FrameRate, settings->AnimationSettings.DefaultLayerClass, background_layer_color );
    if( !new_animation )
        return nullptr;

    animation->GetAnimationComponent()->SetAnimation( new_animation );

    //-

    GEditor->ParentActors( iCamera, animation, NAME_None );

    UEposMovieSceneSequence* epos_sequence = Cast<UEposMovieSceneSequence>( iSequence );
    check( epos_sequence );

    FString animation_path;
    FString animation_name;
    NamingConvention::GenerateAnimationActorPathName( iSequencer, *epos_sequence, iSequenceID, animation_path, animation_name );

    if( !iAnimationArgs.mName.IsEmpty() )
        animation_name = iAnimationArgs.mName;

    animation->SetFolderPath( *animation_path );
    FActorLabelUtilities::RenameExistingActor( animation, animation_name, false ); // The shot name is displayed in another column in the world outliner

    animation_name = NamingConvention::GenerateAnimationTrackName( iSequencer, *epos_sequence, iSequenceID, animation );

    FGuid animationGuid = iSequencer.CreateBinding( *animation, animation_name );

    //iSequencer.OnActorAddedToSequencer().Broadcast( animation, animationGuid );

    if( oGuid )
        *oGuid = animationGuid;

    return animation;
}

//---

//static
bool
ShotSequenceTools::CanMoveAndScaleAnimation( const AOdysseyAnimationActor* iAnimation, const ACineCameraActor* iCamera )
{
    if( !iAnimation || !iCamera )
        return false;

    // Already normalized
    FVector camera_lookat = iCamera->GetActorForwardVector();
    FVector animation_lookat = iAnimation->GetActorUpVector();

    if( !FVector::Parallel( camera_lookat, animation_lookat ) )
        return false;

    FVector camera_to_animation( iAnimation->GetActorLocation() - iCamera->GetActorLocation() );
    camera_to_animation.Normalize();

    if( !FVector::Coplanar( iCamera->GetActorLocation(), camera_lookat, iCamera->GetActorLocation(), camera_to_animation ) )
        return false;

    return true;
};

//static
bool
ShotSequenceTools::MoveAndScaleAnimation( AOdysseyAnimationActor* ioAnimation, const ACineCameraActor* iCamera, float iNewDistance, EScaleAnimation iScaleType )
{
    if( !ShotSequenceTools::CanMoveAndScaleAnimation( ioAnimation, iCamera ) )
        return false;

    if( FMath::IsNearlyZero( iNewDistance ) )
        return false;

    float old_distance = FVector::Distance( iCamera->GetActorLocation(), ioAnimation->GetActorLocation() );
    FVector old_scale = ioAnimation->GetActorScale3D();
    FVector old_scale_camera100 = ioAnimation->ComputeAnimationScaleWithScaleAndMargin( iCamera, old_distance );

    FVector new_animation_location = iCamera->GetActorLocation() + ( ioAnimation->GetActorLocation() - iCamera->GetActorLocation() ).GetSafeNormal() * iNewDistance;

    ioAnimation->SetActorLocation( new_animation_location );

    switch( iScaleType )
    {
        case EScaleAnimation::kFitToCamera:
            {
                FVector scale = ioAnimation->ComputeAnimationScaleWithScaleAndMargin( iCamera, iNewDistance );
                ioAnimation->SetActorScale3D( scale );
            }
            break;

        case EScaleAnimation::kRelativeScale:
            {
                FVector new_scale_camera100 = ioAnimation->ComputeAnimationScaleWithScaleAndMargin( iCamera, iNewDistance );
                FVector ratio = new_scale_camera100 / old_scale_camera100;
                FVector new_scale = old_scale * ratio;

                ioAnimation->SetActorScale3D( new_scale );
            }
            break;

        case EScaleAnimation::kNo:
            // nothing to do
            break;

        default: checkNoEntry();
    }

    return true;
}

//---

//static
void
BoardSequenceTools::CreateAnimation( ISequencer* iSequencer, FFrameNumber iFrameNumber, const FAnimationArgs& iAnimationArgs )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::CreateAnimation( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame(), iAnimationArgs );
}

//static
void
BoardSequenceTools::CreateAnimation( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FFrameNumber iFrameNumber, const FAnimationArgs& iAnimationArgs )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    if( !iSubSection.GetTrueRange().Contains( iFrameNumber ) )
        return;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return;

    ShotSequenceTools::CreateAnimation( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame(), iAnimationArgs );
}

//static
bool
BoardSequenceTools::CanCreateAnimation( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId );
    if( !camera )
        return false;

    return true;
}

//static
void
ShotSequenceTools::CreateAnimation( ISequencer* iSequencer, FFrameNumber iFrameNumber, const FAnimationArgs& iAnimationArgs )
{
    CreateAnimation( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, iAnimationArgs );
}

//static
bool
ShotSequenceTools::CanCreateAnimation( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    FMovieSceneSequenceID sequence_id = iSequencer->GetFocusedTemplateID();
    if( !sequence )
        return false;

    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, sequence, sequence_id );
    if( !camera )
        return false;

    return true;
}

//static
void
ShotSequenceTools::CreateAnimation( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, const FAnimationArgs& iAnimationArgs )
{
    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( iSequencer, iSequence, iSequenceID, &camera_guid );

    if( !camera )
        return;

    //---

    const FScopedTransaction transaction( LOCTEXT( "CreateStoryAnimationHere", "Create Storyboard Animation Here" ) );

    AOdysseyAnimationActor* animation_actor;

    {
        cTemporarySwitchInner switch_to( iSequencer, iSequenceID );

        //---

        animation_actor = ShotSequenceTools::SpawnAndBindAnimation( iSequencer, iSequence, iSequenceID, camera_guid, camera, iFrameNumber, iAnimationArgs, nullptr );

        //---

        iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
    }

    // Must be done after the inner/outer sequence switch
    // Otherwise it resets the selection if GEditor->SelectActor() is called inside SpawnAndBindAnimation()
    // Furthermore the hidden flag must also be set to true now
    GEditor->SelectActor( animation_actor, true /*bInSelected*/, true /*bNotify*/, true /*bSelectEvenIfHidden*/ );
}

//---
//
////static
//int32
//BoardSequenceTools::GetAllPlanes( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<APlaneActor*>* oPlanes, TArray<FGuid>* oPlaneBindings )
//{
//    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
//    if( !result.mInnerSequence )
//        return 0;
//
//    if( result.mInnerSequence->IsA<UBoardSequence>() )
//        return 0;
//
//    return ShotSequenceHelpers::GetAllPlanes( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kSelectedOrAll, oPlanes, oPlaneBindings );
//}
//
////static
//int32
//ShotSequenceTools::GetAllPlanes( ISequencer* iSequencer, TArray<APlaneActor*>* oPlanes, TArray<FGuid>* oPlaneBindings )
//{
//    return ShotSequenceHelpers::GetAllPlanes( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), EGetPlane::kSelectedOrAll, oPlanes, oPlaneBindings );
//}
//
////---
////---
////---
//
////static
//bool
//BoardSequenceTools::CanDetachPlane( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArray<FGuid> iPlaneBindings )
//{
//    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
//    if( !result.mInnerSequence )
//        return false;
//
//    return ShotSequenceTools::CanDetachPlane( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iPlaneBindings );
//}
//
////static
//bool
//BoardSequenceTools::CanDetachPlane( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iPlaneBinding )
//{
//    return BoardSequenceTools::CanDetachPlane( iSequencer, iSubSection, TArray<FGuid>( { iPlaneBinding } ) );
//}
//
////static
//bool
//ShotSequenceTools::CanDetachPlane( ISequencer* iSequencer, FGuid iPlaneBinding )
//{
//    return ShotSequenceTools::CanDetachPlane( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), TArray<FGuid>( { iPlaneBinding } ) );
//}
//
////static
//bool
//ShotSequenceTools::CanDetachPlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArray<FGuid> iPlaneBindings )
//{
//    TArray<APlaneActor*> planes;
//    for( auto plane_binding : iPlaneBindings )
//    {
//        for( auto object : iSequencer.FindBoundObjects( plane_binding, iSequenceID ) )
//            planes.Add( Cast<APlaneActor>( object ) );
//    }
//
//    bool can_detach = false;
//    for( auto plane : planes )
//    {
//        USceneComponent* RootComp = plane->GetRootComponent();
//        if( !RootComp || !RootComp->GetAttachParent() )
//            continue;
//
//        AActor* ParentActor = RootComp->GetAttachParent()->GetOwner();
//        if( !ParentActor ) //TODO: confirm by comparing with the camera ? or is it enough as the planes are in the movie scene ?
//            continue;
//
//        can_detach = true;
//    }
//
//    return can_detach;
//}
//
////-
//
////static
//void
//BoardSequenceTools::DetachPlane( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iPlaneBindings )
//{
//    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
//    if( !result.mInnerSequence )
//        return;
//
//    ShotSequenceTools::DetachPlane( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iPlaneBindings );
//}
//
////static
//void
//BoardSequenceTools::DetachPlane( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
//{
//    BoardSequenceTools::DetachPlane( iSequencer, iFrameNumber, TArray<FGuid>( { iPlaneBinding } ) );
//}
//
////static
//void
//BoardSequenceTools::DetachPlane( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArray<FGuid> iPlaneBindings )
//{
//    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
//    if( !result.mInnerSequence )
//        return;
//
//    ShotSequenceTools::DetachPlane( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iPlaneBindings );
//}
//
////static
//void
//BoardSequenceTools::DetachPlane( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iPlaneBinding )
//{
//    BoardSequenceTools::DetachPlane( iSequencer, iSubSection, TArray<FGuid>( { iPlaneBinding } ) );
//}
//
////static
//void
//ShotSequenceTools::DetachPlane( ISequencer* iSequencer, TArray<FGuid> iPlaneBindings )
//{
//    DetachPlane( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iPlaneBindings );
//}
//
////static
//void
//ShotSequenceTools::DetachPlane( ISequencer* iSequencer, FGuid iPlaneBinding )
//{
//    DetachPlane( iSequencer, TArray<FGuid>( { iPlaneBinding } ) );
//}
//
////static
//void
//ShotSequenceTools::DetachPlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArray<FGuid> iPlaneBindings )
//{
//    TArray<APlaneActor*> planes;
//
//    for( auto plane_binding : iPlaneBindings )
//    {
//        for( auto object : iSequencer.FindBoundObjects( plane_binding, iSequenceID ) )
//            planes.Add( Cast<APlaneActor>( object ) );
//    }
//
//    //---
//
//    const FScopedTransaction transaction( LOCTEXT( "DetachPlane", "Detach Plane" ) );
//
//    //---
//
//    GEditor->SelectNone( true, true );
//    // It's certainly safe to not check if CanDetachPlane() is ok (like CreateOpacity()/CreateDrawing)
//    // as DetachSelectedActors() does the check
//    for( auto plane : planes )
//        GEditor->SelectActor( plane, true /* bInSelected */, true /* bNotify */, true /* bSelectEvenIfHidden */ );
//
//    GEditor->DetachSelectedActors();
//
//    //---
//
//    //iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
//}
//
////---
//
////static
//int32
//BoardSequenceTools::GetAttachedPlanes( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<APlaneActor*>* oPlanes, TArray<FGuid>* oPlaneBindings )
//{
//    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
//    if( !result.mInnerSequence )
//        return 0;
//
//    if( result.mInnerSequence->IsA<UBoardSequence>() )
//        return 0;
//
//    return ShotSequenceHelpers::GetAttachedPlanes( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kSelectedOrAll, oPlanes, oPlaneBindings );
//}
//
////static
//int32
//ShotSequenceTools::GetAttachedPlanes( ISequencer* iSequencer, TArray<APlaneActor*>* oPlanes, TArray<FGuid>* oPlaneBindings )
//{
//    return ShotSequenceHelpers::GetAttachedPlanes( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), EGetPlane::kSelectedOrAll, oPlanes, oPlaneBindings );
//}
//
////---
//
////static
//void
//BoardSequenceTools::DeletePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iPlaneBindings )
//{
//    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
//    if( !result.mInnerSequence )
//        return;
//
//    ShotSequenceTools::DeletePlane( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iPlaneBindings );
//}
//
////static
//void
//BoardSequenceTools::DeletePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
//{
//    BoardSequenceTools::DeletePlane( iSequencer, iFrameNumber, TArray<FGuid>( { iPlaneBinding } ) );
//}
//
////static
//void
//BoardSequenceTools::DeletePlane( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArray<FGuid> iPlaneBindings )
//{
//    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
//    if( !result.mInnerSequence )
//        return;
//
//    ShotSequenceTools::DeletePlane( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iPlaneBindings );
//}
//
////static
//void
//BoardSequenceTools::DeletePlane( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iPlaneBinding )
//{
//    BoardSequenceTools::DeletePlane( iSequencer, iSubSection, TArray<FGuid>( { iPlaneBinding } ) );
//}
//
////static
//void
//ShotSequenceTools::DeletePlane( ISequencer* iSequencer, TArray<FGuid> iPlaneBindings )
//{
//    ShotSequenceTools::DeletePlane( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iPlaneBindings );
//}
//
////static
//void
//ShotSequenceTools::DeletePlane( ISequencer* iSequencer, FGuid iPlaneBinding )
//{
//    ShotSequenceTools::DeletePlane( iSequencer, TArray<FGuid>( { iPlaneBinding } ) );
//}
//
//namespace
//{
//static
//TArray<FGuid>
//GetSubBindings( UMovieScene* iMovieScene, FGuid iPlaneBinding )
//{
//    // This will make a level traversal order: https://towardsdatascience.com/4-types-of-tree-traversal-algorithms-d56328450846#ce5c
//    // And then, reverse this order to start by the children
//    //
//    // This part may need to checked
//    // It works but for the moment, plane has only 1 component, and the component can't have other subcomponents, so it's a simple parent-child relation for plane
//
//    TQueue<FGuid> queue;
//    TArray<FGuid> bindings;
//
//    queue.Enqueue( iPlaneBinding );
//
//    while( !queue.IsEmpty() )
//    {
//        FGuid binding;
//        queue.Dequeue( binding );
//
//        bindings.Add( binding );
//
//        // Enqueue all the children of the current binding
//        for( int32 PossessableIndex = 0; PossessableIndex < iMovieScene->GetPossessableCount(); ++PossessableIndex )
//        {
//            const FMovieScenePossessable& Possessable = iMovieScene->GetPossessable( PossessableIndex );
//
//            if( Possessable.GetParent() == binding )
//                queue.Enqueue( Possessable.GetGuid() );
//        }
//    }
//
//    Algo::Reverse( bindings ); // To start with children first
//
//    return bindings;
//}
//}
//
////static
//void
//ShotSequenceTools::DeletePlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArray<FGuid> iPlaneBindings )
//{
//    UMovieScene* movieScene = iSequence->GetMovieScene();
//
//    TArray<APlaneActor*> planes;
//    TArray<FGuid> bindings;
//
//    for( auto plane_binding : iPlaneBindings )
//    {
//        for( auto object : iSequencer.FindBoundObjects( plane_binding, iSequenceID ) )
//            planes.Add( Cast<APlaneActor>( object ) );
//
//        bindings = GetSubBindings( movieScene, plane_binding );
//    }
//
//    //---
//
//    bindings = TSet<FGuid>( bindings ).Array(); // To make them unique
//
//    //---
//
//    const FScopedTransaction transaction( LOCTEXT( "DeletePlane", "Delete Plane" ) );
//
//    movieScene->Modify();
//    iSequence->Modify();
//
//    //--- Delete bindings
//
//    for( auto binding : bindings )
//    {
//        movieScene->RemovePossessable( binding );
//        iSequence->UnbindPossessableObjects( binding );
//    }
//
//    iSequencer.RestorePreAnimatedState();
//
//    //--- Delete actors
//
//    GEditor->SelectNone( true, true );
//    for( auto plane : planes )
//        GEditor->SelectActor( plane, true /* bInSelected */, true /* bNotify */, true /* bSelectEvenIfHidden */ );
//
//    FLevelEditorActionCallbacks::ExecuteExecCommand( FString( TEXT( "DELETE" ) ) ); // In LevelEditor.cpp
//
//    //---
//
//    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemRemoved );
//}
//
////---
//
////static
//bool
//BoardSequenceTools::IsPlaneVisible( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
//{
//    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
//    if( !result.mInnerSequence )
//        return false;
//
//    return ShotSequenceTools::IsPlaneVisible( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iPlaneBinding );
//}
//
////static
//bool
//BoardSequenceTools::IsPlaneVisible( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iPlaneBinding )
//{
//    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
//    if( !result.mInnerSequence )
//        return false;
//
//    return ShotSequenceTools::IsPlaneVisible( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iPlaneBinding );
//}
//
////static
//bool
//ShotSequenceTools::IsPlaneVisible( ISequencer* iSequencer, FGuid iPlaneBinding )
//{
//    return ShotSequenceTools::IsPlaneVisible( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iPlaneBinding );
//}
//
////static
//bool
//ShotSequenceTools::IsPlaneVisible( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding )
//{
//    ShotSequenceHelpers::FFindOrCreatePlaneVisibilityResult plane_visibility_result = ShotSequenceHelpers::FindPlaneVisibilityTrackAndSections( iSequencer, iSequence, iSequenceID, iPlaneBinding );
//
//    if( !plane_visibility_result.mTrack.IsValid() )
//        return false;
//
//    if( plane_visibility_result.mSections.Num() == 0 )
//        return false;
//
//    for( auto section : plane_visibility_result.mSections )
//        return section->GetChannel().GetDefault().Get( false );
//
//    return false;
//}
//
////---
//
////static
//void
//BoardSequenceTools::TogglePlaneVisibility( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
//{
//    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
//    if( !result.mInnerSequence )
//        return;
//
//    ShotSequenceTools::TogglePlaneVisibility( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, TArray<FGuid>( { iPlaneBinding } ) );
//}
//
////static
//void
//BoardSequenceTools::TogglePlaneVisibility( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArray<FGuid> iPlaneBindings, TOptional<FGuid> iPlaneReference )
//{
//    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
//    if( !result.mInnerSequence )
//        return;
//
//    ShotSequenceTools::TogglePlaneVisibility( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iPlaneBindings, iPlaneReference );
//}
//
////static
//void
//BoardSequenceTools::TogglePlaneVisibility( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iPlaneBinding )
//{
//    BoardSequenceTools::TogglePlaneVisibility( iSequencer, iSubSection, TArray<FGuid>( { iPlaneBinding } ) );
//}
//
////static
//void
//ShotSequenceTools::TogglePlaneVisibility( ISequencer* iSequencer, TArray<FGuid> iPlaneBindings, TOptional<FGuid> iPlaneReference )
//{
//    ShotSequenceTools::TogglePlaneVisibility( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iPlaneBindings, iPlaneReference );
//}
//
////static
//void
//ShotSequenceTools::TogglePlaneVisibility( ISequencer* iSequencer, FGuid iPlaneBinding )
//{
//    ShotSequenceTools::TogglePlaneVisibility( iSequencer, TArray<FGuid>( { iPlaneBinding } ) );
//}
//
////static
//void
//ShotSequenceTools::TogglePlaneVisibility( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArray<FGuid> iPlaneBindings, TOptional<FGuid> iPlaneReference )
//{
//    const FScopedTransaction transaction( LOCTEXT( "TogglePlaneVisibility", "Toggle Plane Visibility" ) );
//
//    //---
//
//    bool reference_plane_visibility = false;
//    if( iPlaneReference.IsSet() )
//        reference_plane_visibility = IsPlaneVisible( iSequencer, iSequence, iSequenceID, iPlaneReference.GetValue() );
//
//    for( auto plane_binding : iPlaneBindings )
//    {
//        ShotSequenceHelpers::FFindOrCreatePlaneVisibilityResult plane_visibility_result = ShotSequenceHelpers::FindPlaneVisibilityTrackAndSections( iSequencer, iSequence, iSequenceID, plane_binding );
//
//        if( !plane_visibility_result.mTrack.IsValid() )
//            continue;
//
//        if( plane_visibility_result.mSections.Num() == 0 )
//            continue;
//
//        //---
//
//        // Invert the visibility of the plane
//        bool new_plane_visibility = !IsPlaneVisible( iSequencer, iSequence, iSequenceID, plane_binding );
//        // But if there is a reference plane, invert it and use it for all planes
//        if( iPlaneReference.IsSet() )
//            new_plane_visibility = !reference_plane_visibility;
//
//        for( auto section : plane_visibility_result.mSections )
//        {
//            section->Modify();
//
//            section->GetChannel().SetDefault( new_plane_visibility );
//        }
//    }
//
//    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
//}
//
////---
//
////static
//void
//BoardSequenceTools::SelectSinglePlane( ISequencer* iSequencer, UMovieSceneSubSection* iSubSection, FGuid iPlaneBinding )
//{
//    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, *iSubSection, iSequencer->GetFocusedTemplateID() );
//    if( !result.mInnerSequence )
//        return;
//
//    ShotSequenceTools::SelectSinglePlane( *iSequencer, iSubSection, result.mInnerSequence, result.mInnerSequenceId, iPlaneBinding );
//}
//
////static
//void
//ShotSequenceTools::SelectSinglePlane( ISequencer& iSequencer, UMovieSceneSubSection* iParentSection, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding )
//{
//    auto objects = iSequencer.FindBoundObjects( iPlaneBinding, iSequenceID );
//
//    // Get the plane(s) corresponding to the one(s) on the current clicked row
//    // It's an array, but generally it should always be only 1 entry
//    TArray<APlaneActor*> planes;
//    for( auto object : objects )
//        planes.Add( Cast<APlaneActor>( object ) );
//
//    // To unselect section(s)
//    iSequencer.EmptySelection();
//    // And then select the current one
//    iSequencer.SelectSection( iParentSection );
//
//    // To unselect all actors
//    GEditor->SelectNone( true, true );
//    // And then select the current one(s)
//    for( auto plane : planes )
//        GEditor->SelectActor( plane, true /* bInSelected */, true /* bNotify */, true /* bSelectEvenIfHidden */ );
//}
//
////---
//
////static
//void
//BoardSequenceTools::SelectMultiPlane( ISequencer* iSequencer, UMovieSceneSubSection* iSubSection, FGuid iPlaneBinding )
//{
//    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, *iSubSection, iSequencer->GetFocusedTemplateID() );
//    if( !result.mInnerSequence )
//        return;
//
//    ShotSequenceTools::SelectMultiPlane( *iSequencer, iSubSection, result.mInnerSequence, result.mInnerSequenceId, iPlaneBinding );
//}
//
////static
//void
//ShotSequenceTools::SelectMultiPlane( ISequencer& iSequencer, UMovieSceneSubSection* iParentSection, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding )
//{
//    auto objects = iSequencer.FindBoundObjects( iPlaneBinding, iSequenceID );
//
//    // Get the plane(s) corresponding to the one(s) on the current clicked row
//    // It's an array, but generally it should always be only 1 entry
//    TArray<APlaneActor*> planes_self;
//    for( auto object : objects )
//        planes_self.Add( Cast<APlaneActor>( object ) );
//
//    // Get all the currently selected planes inside the whole current shot
//    TArray<APlaneActor*> planes_selected;
//    ShotSequenceHelpers::GetAllPlanes( iSequencer, iSequence, iSequenceID, EGetPlane::kSelectedOnly, &planes_selected, nullptr );
//
//    // Add the self plane to the selection
//    // or remove it from the selection if it was already selected
//    for( auto plane_self : planes_self )
//    {
//        if( planes_selected.Contains( plane_self ) )
//            planes_selected.Remove( plane_self );
//        else
//            planes_selected.Add( plane_self );
//    }
//
//    // To unselect section(s)
//    iSequencer.EmptySelection();
//    // And then select the current one
//    iSequencer.SelectSection( iParentSection );
//
//    // To unselect all actors
//    GEditor->SelectNone( true, true );
//    // And then select the current one(s)
//    for( auto plane_selected : planes_selected )
//        GEditor->SelectActor( plane_selected, true /* bInSelected */, true /* bNotify */, true /* bSelectEvenIfHidden */ );
//}

//---

#undef LOCTEXT_NAMESPACE
