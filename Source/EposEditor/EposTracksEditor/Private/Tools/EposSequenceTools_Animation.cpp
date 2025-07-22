// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
#include "Framework/Notifications/NotificationManager.h"
#include "ISequencer.h"
#include "Kismet/GameplayStatics.h"
#include "LevelEditorActions.h"
#include "LevelEditorSubsystem.h"
#include "LevelEditorViewport.h"
#include "LevelUtils.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieScene.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "Sections/MovieSceneBoolSection.h"
#include "Sections/MovieSceneSubSection.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "Board/BoardSequence.h"
#include "EposSequenceHelpers.h"
#include "NamingConvention.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationActor.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationTimelineSection.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerStack.h"
#include "ScalingComponent.h"
#include "Settings/EposTracksEditorSettings.h"
#include "Tools/ResourceAssetTools.h"

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
ShotSequenceTools::SpawnAnimation( UWorld* iWorld, ACineCameraActor* iCamera, float iFocusDistance, float iSafeMargin, FVector2D iRelativeScaling )
{
    check( !FLevelUtils::IsLevelLocked( iWorld->GetCurrentLevel() ) );

    FActorSpawnParameters SpawnParams;
    AOdysseyAnimationActor* animation = iWorld->SpawnActor<AOdysseyAnimationActor>( SpawnParams );
    if( !animation )
        return nullptr;

    animation->SetActorHiddenInGame( true );

#if UE_BUILD_DEBUG
    if( UMaterialInstanceConstant* material = Cast<UMaterialInstanceConstant>( animation->GetAnimationComponent()->GetMaterial( 0 ) ) )
        material->SetScalarParameterValueEditorOnly( FMaterialParameterInfo( "Overlay" ), 1 );
#endif

    // Using this will delete the component once the actor is renamed at the end of SpawnAndBindAnimation() -_-
    //UActorComponent* actor_component = animation->AddComponentByClass( UScalingComponent::StaticClass(), false, FTransform::Identity, false );
    // So create and attach/register it to the actor in 2 steps
    UScalingComponent* actor_component = NewObject<UScalingComponent>( animation, UScalingComponent::StaticClass() );
    animation->FinishAddComponent( actor_component, false, FTransform::Identity );

    check( actor_component );
    UScalingComponent* scaling_component = Cast<UScalingComponent>( actor_component );
    check( scaling_component );

    scaling_component = animation->FindComponentByClass<UScalingComponent>();
    if( !scaling_component )
        return nullptr;

    scaling_component->SetSafeMargin( iSafeMargin );
    scaling_component->SetRelativeScaling( iRelativeScaling );

    //---

    FTransform camera_transform = iCamera->GetRootComponent()->GetComponentTransform();

    FVector const CamLocation = camera_transform.GetLocation();
    FVector const CamDir = camera_transform.GetRotation().Vector();
    FRotator const CamRot = camera_transform.Rotator();

    //-

    FVector animation_location = CamLocation + CamDir * iFocusDistance;
    animation_location = FindNextFreeAnimationLocation( iWorld, animation_location, CamLocation );

    FVector camera_view_size = scaling_component->ComputeSizeOfCameraView( iCamera, iFocusDistance );
    FVector animation_scale = scaling_component->ComputeScaleWithScaleAndMargin( camera_view_size );

    FRotator animation_rotator = CamRot;

    //---

    animation->SetActorScale3D( animation_scale );
    animation->SetActorLocation( animation_location );
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

    check( !FLevelUtils::IsLevelLocked( world->GetCurrentLevel() ) );

    GEditor->SelectNone( true, true );

    const UEposTracksEditorSettings* settings = GetDefault<UEposTracksEditorSettings>();
    float margin = settings->AnimationActorSettings.SafeMargin;
    FVector2D relative_scaling = settings->AnimationActorSettings.RelativeScaling;

    if( iAnimationArgs.mMargin.IsSet() )
        margin = iAnimationArgs.mMargin.GetValue();

    float focusDistance = 200;
    AOdysseyAnimationActor* animation = ShotSequenceTools::SpawnAnimation( world, iCamera, focusDistance, margin, relative_scaling );

    UScalingComponent* scaling_component = animation->FindComponentByClass<UScalingComponent>();
    check( scaling_component );

    //---

    FVector camera_view_size = scaling_component->ComputeSizeOfCameraView( iCamera, focusDistance );
    FVector camera_view_size_with_scaling = scaling_component->ComputeScaleWithScaleAndMargin( camera_view_size );
    FIntPoint texture_size = scaling_component->ComputeTextureSize( camera_view_size_with_scaling, settings->AnimationSettings.Height );

    //UMaterialInstanceConstant* new_animation = iAnimationArgs.mAnimation.IsValid()
    //                                           ? ProjectAssetTools::CreateAnimation( iSequencer, iSequence, iSequenceID, iAnimationArgs.mAnimation.Get() )
    //                                           : ProjectAssetTools::CreateAnimation( iSequencer, iSequence, iSequenceID, texture_size );
    TOptional<FLinearColor> background_layer_color;
    if( settings->AnimationSettings.AddLayerBackground )
        background_layer_color = settings->AnimationSettings.LayerBackgroundColor;

    UOdysseyAnimation* new_animation = iAnimationArgs.mAnimation.IsValid()
                                       ? iAnimationArgs.mAnimation.Get()
                                       : ProjectAssetTools::CreateAnimation( iSequencer, iSequence, iSequenceID, texture_size, settings->AnimationSettings.Format, settings->AnimationSettings.FrameRate, settings->AnimationSettings.DefaultLayerClass, background_layer_color );
    if( !new_animation )
        return nullptr;

    FTransform transform = animation->GetTransform();
    animation->GetAnimationComponent()->InitializeFromAnimation( new_animation );
    animation->SetActorTransform( transform );

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

    //iSequencer.OnActorAddedToSequencer().Broadcast( animation, animationGuid ); // Already called when creating the binding for animation

    // Set the post behavior to hold
    UOdysseyLayerStack* layer_stack = new_animation->GetLayerStack();
    for( UOdysseyLayer* layer : layer_stack->GetLayers() )
    {
        layer->SetPostBehaviour( EOdysseyLayerImagePostBehaviour::Hold );
    }

    // Set the size of the new section to playbackrange by default
    ShotSequenceHelpers::FFindOrCreateTimelineResult result = ShotSequenceHelpers::FindTimelineTrackAndSections( iSequencer, iSequence, iSequenceID, animationGuid );
    for( TWeakObjectPtr<UOdysseyAnimationTimelineSection> section : result.mSections )
    {
        section->SetPreBehaviour( EOdysseyAnimationPlayerPostBehaviour::Hold );
        section->SetPostBehaviour( EOdysseyAnimationPlayerPostBehaviour::Hold );
        section->SetRange( iSequence->GetMovieScene()->GetPlaybackRange() );
        section->SetAnimation( section->GetAnimation() ); // To rebuild the channel (as the section is created at the current frame by default)
    }

    if( oGuid )
        *oGuid = animationGuid;

    return animation;
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

    ULevelEditorSubsystem* levelEditorSubsystem = GEditor->GetEditorSubsystem<ULevelEditorSubsystem>();
    if( FLevelUtils::IsLevelLocked( levelEditorSubsystem->GetCurrentLevel() ) )
    {
        FNotificationInfo Info( LOCTEXT( "cant-spawn-animation-in-locked-level", "The requested operation could not be completed because the level is locked." ) );
        Info.ExpireDuration = 5.0f;
        FSlateNotificationManager::Get().AddNotification( Info )->SetCompletionState( SNotificationItem::CS_Fail );
        return;
    }

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

//static
int32
BoardSequenceTools::GetAllAnimations( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<AOdysseyAnimationActor*>* oAnimations, TArray<FGuid>* oAnimationBindings )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return 0;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return 0;

    return ShotSequenceHelpers::GetAllAnimations( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, EGetAnimation::kSelectedOrAll, oAnimations, oAnimationBindings );
}

//static
int32
ShotSequenceTools::GetAllAnimations( ISequencer* iSequencer, TArray<AOdysseyAnimationActor*>* oAnimations, TArray<FGuid>* oAnimationBindings )
{
    return ShotSequenceHelpers::GetAllAnimations( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), EGetAnimation::kSelectedOrAll, oAnimations, oAnimationBindings );
}

//---
//---
//---

//static
bool
BoardSequenceTools::CanDetachAnimation( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArray<FGuid> iAnimationBindings )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return false;

    return ShotSequenceTools::CanDetachAnimation( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iAnimationBindings );
}

//static
bool
BoardSequenceTools::CanDetachAnimation( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iAnimationBinding )
{
    return BoardSequenceTools::CanDetachAnimation( iSequencer, iSubSection, TArray<FGuid>( { iAnimationBinding } ) );
}

//static
bool
ShotSequenceTools::CanDetachAnimation( ISequencer* iSequencer, FGuid iAnimationBinding )
{
    return ShotSequenceTools::CanDetachAnimation( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), TArray<FGuid>( { iAnimationBinding } ) );
}

//static
bool
ShotSequenceTools::CanDetachAnimation( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArray<FGuid> iAnimationBindings )
{
    TArray<AOdysseyAnimationActor*> animations;
    for( auto animation_binding : iAnimationBindings )
    {
        for( auto object : iSequencer.FindBoundObjects( animation_binding, iSequenceID ) )
            animations.Add( Cast<AOdysseyAnimationActor>( object ) );
    }

    bool can_detach = false;
    for( auto animation : animations )
    {
        USceneComponent* RootComp = animation->GetRootComponent();
        if( !RootComp || !RootComp->GetAttachParent() )
            continue;

        AActor* ParentActor = RootComp->GetAttachParent()->GetOwner();
        if( !ParentActor ) //TODO: confirm by comparing with the camera ? or is it enough as the animations are in the movie scene ?
            continue;

        can_detach = true;
    }

    return can_detach;
}

//-

//static
void
BoardSequenceTools::DetachAnimation( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iAnimationBindings )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::DetachAnimation( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iAnimationBindings );
}

//static
void
BoardSequenceTools::DetachAnimation( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iAnimationBinding )
{
    BoardSequenceTools::DetachAnimation( iSequencer, iFrameNumber, TArray<FGuid>( { iAnimationBinding } ) );
}

//static
void
BoardSequenceTools::DetachAnimation( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArray<FGuid> iAnimationBindings )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::DetachAnimation( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iAnimationBindings );
}

//static
void
BoardSequenceTools::DetachAnimation( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iAnimationBinding )
{
    BoardSequenceTools::DetachAnimation( iSequencer, iSubSection, TArray<FGuid>( { iAnimationBinding } ) );
}

//static
void
ShotSequenceTools::DetachAnimation( ISequencer* iSequencer, TArray<FGuid> iAnimationBindings )
{
    DetachAnimation( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iAnimationBindings );
}

//static
void
ShotSequenceTools::DetachAnimation( ISequencer* iSequencer, FGuid iAnimationBinding )
{
    DetachAnimation( iSequencer, TArray<FGuid>( { iAnimationBinding } ) );
}

//static
void
ShotSequenceTools::DetachAnimation( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArray<FGuid> iAnimationBindings )
{
    TArray<AOdysseyAnimationActor*> animations;

    for( auto animation_binding : iAnimationBindings )
    {
        for( auto object : iSequencer.FindBoundObjects( animation_binding, iSequenceID ) )
            animations.Add( Cast<AOdysseyAnimationActor>( object ) );
    }

    //---

    const FScopedTransaction transaction( LOCTEXT( "DetachAnimation", "Detach Animation" ) );

    //---

    GEditor->SelectNone( true, true );
    // It's certainly safe to not check if CanDetachAnimation() is ok (like CreateOpacity()/CreateAnimation)
    // as DetachSelectedActors() does the check
    for( auto animation : animations )
        GEditor->SelectActor( animation, true /* bInSelected */, true /* bNotify */, true /* bSelectEvenIfHidden */ );

    GEditor->DetachSelectedActors();

    //---

    //iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
}

//---

//static
int32
BoardSequenceTools::GetAttachedAnimations( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<AOdysseyAnimationActor*>* oAnimations, TArray<FGuid>* oAnimationBindings )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return 0;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return 0;

    return ShotSequenceHelpers::GetAttachedAnimations( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, EGetAnimation::kSelectedOrAll, oAnimations, oAnimationBindings );
}

//static
int32
ShotSequenceTools::GetAttachedAnimations( ISequencer* iSequencer, TArray<AOdysseyAnimationActor*>* oAnimations, TArray<FGuid>* oAnimationBindings )
{
    return ShotSequenceHelpers::GetAttachedAnimations( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), EGetAnimation::kSelectedOrAll, oAnimations, oAnimationBindings );
}

//---

//static
void
BoardSequenceTools::DeleteAnimation( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<FGuid> iAnimationBindings )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::DeleteAnimation( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iAnimationBindings );
}

//static
void
BoardSequenceTools::DeleteAnimation( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iAnimationBinding )
{
    BoardSequenceTools::DeleteAnimation( iSequencer, iFrameNumber, TArray<FGuid>( { iAnimationBinding } ) );
}

//static
void
BoardSequenceTools::DeleteAnimation( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArray<FGuid> iAnimationBindings )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::DeleteAnimation( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iAnimationBindings );
}

//static
void
BoardSequenceTools::DeleteAnimation( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iAnimationBinding )
{
    BoardSequenceTools::DeleteAnimation( iSequencer, iSubSection, TArray<FGuid>( { iAnimationBinding } ) );
}

//static
void
ShotSequenceTools::DeleteAnimation( ISequencer* iSequencer, TArray<FGuid> iAnimationBindings )
{
    ShotSequenceTools::DeleteAnimation( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iAnimationBindings );
}

//static
void
ShotSequenceTools::DeleteAnimation( ISequencer* iSequencer, FGuid iAnimationBinding )
{
    ShotSequenceTools::DeleteAnimation( iSequencer, TArray<FGuid>( { iAnimationBinding } ) );
}

namespace
{
static
TArray<FGuid>
GetSubBindings( UMovieScene* iMovieScene, FGuid iAnimationBinding )
{
    // This will make a level traversal order: https://towardsdatascience.com/4-types-of-tree-traversal-algorithms-d56328450846#ce5c
    // And then, reverse this order to start by the children
    //
    // This part may need to checked
    // It works but for the moment, animation has only 1 component, and the component can't have other subcomponents, so it's a simple parent-child relation for animation

    TQueue<FGuid> queue;
    TArray<FGuid> bindings;

    queue.Enqueue( iAnimationBinding );

    while( !queue.IsEmpty() )
    {
        FGuid binding;
        queue.Dequeue( binding );

        bindings.Add( binding );

        // Enqueue all the children of the current binding
        for( int32 PossessableIndex = 0; PossessableIndex < iMovieScene->GetPossessableCount(); ++PossessableIndex )
        {
            const FMovieScenePossessable& Possessable = iMovieScene->GetPossessable( PossessableIndex );

            if( Possessable.GetParent() == binding )
                queue.Enqueue( Possessable.GetGuid() );
        }
    }

    Algo::Reverse( bindings ); // To start with children first

    return bindings;
}
}

//static
void
ShotSequenceTools::DeleteAnimation( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArray<FGuid> iAnimationBindings )
{
    UMovieScene* movieScene = iSequence->GetMovieScene();

    TArray<AOdysseyAnimationActor*> animations;
    TArray<FGuid> bindings;

    for( auto animation_binding : iAnimationBindings )
    {
        for( auto object : iSequencer.FindBoundObjects( animation_binding, iSequenceID ) )
            animations.Add( Cast<AOdysseyAnimationActor>( object ) );

        bindings = GetSubBindings( movieScene, animation_binding );
    }

    //---

    bindings = TSet<FGuid>( bindings ).Array(); // To make them unique

    //---

    const FScopedTransaction transaction( LOCTEXT( "DeleteAnimation", "Delete Animation" ) );

    movieScene->Modify();
    iSequence->Modify();

    //--- Delete bindings

    for( auto binding : bindings )
    {
        movieScene->RemovePossessable( binding );
        iSequence->UnbindPossessableObjects( binding );
    }

    iSequencer.RestorePreAnimatedState();

    //--- Delete actors

    GEditor->SelectNone( true, true );
    for( auto animation : animations )
        GEditor->SelectActor( animation, true /* bInSelected */, true /* bNotify */, true /* bSelectEvenIfHidden */ );

    FLevelEditorActionCallbacks::ExecuteExecCommand( FString( TEXT( "DELETE" ) ) ); // In LevelEditor.cpp

    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemRemoved );
}

//---

//static
bool
BoardSequenceTools::IsAnimationInEditionMode( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    //---

    bool is_edited = false;

    TArray<FGuid> animation_bindings;
    ShotSequenceHelpers::GetAllAnimations( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, EGetAnimation::kAll, nullptr, &animation_bindings );

    for( FGuid animation_binding : animation_bindings )
    {
        ShotSequenceHelpers::FFindOrCreateTimelineResult result_shot = ShotSequenceHelpers::FindTimelineTrackAndSections( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, animation_binding );
        for( TWeakObjectPtr<UOdysseyAnimationTimelineSection> section : result_shot.mSections )
        {
            is_edited |= ShotSequenceTools::IsAnimationInEditionMode( iSequencer, result.mInnerSequence, result.mInnerSequenceId, section->GetAnimation() );
        }
    }

    return is_edited;
}

//static
bool
ShotSequenceTools::IsAnimationInEditionMode( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UOdysseyAnimation* iAnimation )
{
    UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();

    TArray<IAssetEditorInstance*> opened_editors = AssetEditorSubsystem->FindEditorsForAsset( iAnimation );
    //FName name = opened_editors.Num() ? opened_editors[0]->GetEditorName() : NAME_None;

    return !!opened_editors.Num();
}

//---

//static
bool
BoardSequenceTools::IsAnimationVisible( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iAnimationBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    return ShotSequenceTools::IsAnimationVisible( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iAnimationBinding );
}

//static
bool
BoardSequenceTools::IsAnimationVisible( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iAnimationBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return false;

    return ShotSequenceTools::IsAnimationVisible( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iAnimationBinding );
}

//static
bool
ShotSequenceTools::IsAnimationVisible( ISequencer* iSequencer, FGuid iAnimationBinding )
{
    return ShotSequenceTools::IsAnimationVisible( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iAnimationBinding );
}

//static
bool
ShotSequenceTools::IsAnimationVisible( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding )
{
    ShotSequenceHelpers::FFindOrCreateAnimationVisibilityResult animation_visibility_result = ShotSequenceHelpers::FindAnimationVisibilityTrackAndSections( iSequencer, iSequence, iSequenceID, iAnimationBinding );

    if( !animation_visibility_result.mTrack.IsValid() )
        return false;

    if( animation_visibility_result.mSections.Num() == 0 )
        return false;

    for( auto section : animation_visibility_result.mSections )
        return section->GetChannel().GetDefault().Get( false );

    return false;
}

//---

//static
void
BoardSequenceTools::ToggleAnimationVisibility( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid iAnimationBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::ToggleAnimationVisibility( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, TArray<FGuid>( { iAnimationBinding } ) );
}

//static
void
BoardSequenceTools::ToggleAnimationVisibility( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, TArray<FGuid> iAnimationBindings, TOptional<FGuid> iAnimationReference )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::ToggleAnimationVisibility( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iAnimationBindings, iAnimationReference );
}

//static
void
BoardSequenceTools::ToggleAnimationVisibility( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iAnimationBinding )
{
    BoardSequenceTools::ToggleAnimationVisibility( iSequencer, iSubSection, TArray<FGuid>( { iAnimationBinding } ) );
}

//static
void
ShotSequenceTools::ToggleAnimationVisibility( ISequencer* iSequencer, TArray<FGuid> iAnimationBindings, TOptional<FGuid> iAnimationReference )
{
    ShotSequenceTools::ToggleAnimationVisibility( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iAnimationBindings, iAnimationReference );
}

//static
void
ShotSequenceTools::ToggleAnimationVisibility( ISequencer* iSequencer, FGuid iAnimationBinding )
{
    ShotSequenceTools::ToggleAnimationVisibility( iSequencer, TArray<FGuid>( { iAnimationBinding } ) );
}

//static
void
ShotSequenceTools::ToggleAnimationVisibility( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArray<FGuid> iAnimationBindings, TOptional<FGuid> iAnimationReference )
{
    const FScopedTransaction transaction( LOCTEXT( "ToggleAnimationVisibility", "Toggle Animation Visibility" ) );

    //---

    bool reference_animation_visibility = false;
    if( iAnimationReference.IsSet() )
        reference_animation_visibility = IsAnimationVisible( iSequencer, iSequence, iSequenceID, iAnimationReference.GetValue() );

    for( auto animation_binding : iAnimationBindings )
    {
        ShotSequenceHelpers::FFindOrCreateAnimationVisibilityResult animation_visibility_result = ShotSequenceHelpers::FindAnimationVisibilityTrackAndSections( iSequencer, iSequence, iSequenceID, animation_binding );

        if( !animation_visibility_result.mTrack.IsValid() )
            continue;

        if( animation_visibility_result.mSections.Num() == 0 )
            continue;

        //---

        // Invert the visibility of the animation
        bool new_animation_visibility = !IsAnimationVisible( iSequencer, iSequence, iSequenceID, animation_binding );
        // But if there is a reference animation, invert it and use it for all animations
        if( iAnimationReference.IsSet() )
            new_animation_visibility = !reference_animation_visibility;

        for( auto section : animation_visibility_result.mSections )
        {
            section->Modify();

            section->GetChannel().SetDefault( new_animation_visibility );
        }
    }

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

//---

//static
void
BoardSequenceTools::SelectSingleAnimation( ISequencer* iSequencer, UMovieSceneSubSection* iSubSection, FGuid iAnimationBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, *iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::SelectSingleAnimation( *iSequencer, iSubSection, result.mInnerSequence, result.mInnerSequenceId, iAnimationBinding );
}

//static
void
ShotSequenceTools::SelectSingleAnimation( ISequencer& iSequencer, UMovieSceneSubSection* iParentSection, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding )
{
    auto objects = iSequencer.FindBoundObjects( iAnimationBinding, iSequenceID );

    // Get the animation(s) corresponding to the one(s) on the current clicked row
    // It's an array, but generally it should always be only 1 entry
    TArray<AOdysseyAnimationActor*> animations;
    for( auto object : objects )
        animations.Add( Cast<AOdysseyAnimationActor>( object ) );

    // To unselect section(s)
    iSequencer.EmptySelection();
    // And then select the current one
    iSequencer.SelectSection( iParentSection );

    // To unselect all actors
    GEditor->SelectNone( true, true );
    // And then select the current one(s)
    for( auto animation : animations )
        GEditor->SelectActor( animation, true /* bInSelected */, true /* bNotify */, true /* bSelectEvenIfHidden */ );
}

//---

//static
void
BoardSequenceTools::SelectMultiAnimation( ISequencer* iSequencer, UMovieSceneSubSection* iSubSection, FGuid iAnimationBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, *iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::SelectMultiAnimation( *iSequencer, iSubSection, result.mInnerSequence, result.mInnerSequenceId, iAnimationBinding );
}

//static
void
ShotSequenceTools::SelectMultiAnimation( ISequencer& iSequencer, UMovieSceneSubSection* iParentSection, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding )
{
    auto objects = iSequencer.FindBoundObjects( iAnimationBinding, iSequenceID );

    // Get the animation(s) corresponding to the one(s) on the current clicked row
    // It's an array, but generally it should always be only 1 entry
    TArray<AOdysseyAnimationActor*> animations_self;
    for( auto object : objects )
        animations_self.Add( Cast<AOdysseyAnimationActor>( object ) );

    // Get all the currently selected animations inside the whole current shot
    TArray<AOdysseyAnimationActor*> animations_selected;
    ShotSequenceHelpers::GetAllAnimations( iSequencer, iSequence, iSequenceID, EGetAnimation::kSelectedOnly, &animations_selected, nullptr );

    // Add the self animation to the selection
    // or remove it from the selection if it was already selected
    for( auto animation_self : animations_self )
    {
        if( animations_selected.Contains( animation_self ) )
            animations_selected.Remove( animation_self );
        else
            animations_selected.Add( animation_self );
    }

    // To unselect section(s)
    iSequencer.EmptySelection();
    // And then select the current one
    iSequencer.SelectSection( iParentSection );

    // To unselect all actors
    GEditor->SelectNone( true, true );
    // And then select the current one(s)
    for( auto animation_selected : animations_selected )
        GEditor->SelectActor( animation_selected, true /* bInSelected */, true /* bNotify */, true /* bSelectEvenIfHidden */ );
}

//---

#undef LOCTEXT_NAMESPACE
