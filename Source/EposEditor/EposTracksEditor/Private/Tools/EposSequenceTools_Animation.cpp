// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/EposSequenceTools.h"

#include "Bindings/MovieSceneSpawnableActorBinding.h"
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
#include "LevelEditor.h"
#include "LevelEditorActions.h"
#include "LevelEditorSubsystem.h"
#include "LevelEditorViewport.h"
#include "LevelUtils.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieScene.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "Sections/MovieScene3DAttachSection.h"
#include "Sections/MovieSceneBoolSection.h"
#include "Sections/MovieSceneSubSection.h"
#include "Selection.h"
#include "SequencerUtilities.h"
#include "Tracks/MovieScene3DAttachTrack.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Sections/MovieScene3DTransformSection.h"

#include "ActorHelpers.h"
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

#if UE_BUILD_DEBUG
    if( UMaterialInstanceConstant* material = Cast<UMaterialInstanceConstant>( animation->GetAnimationComponent()->GetMaterial( 0 ) ) )
        material->SetScalarParameterValueEditorOnly( FMaterialParameterInfo( "Overlay" ), 1 );
#endif

    //https://forums.unrealengine.com/t/add-component-to-actor-in-c-the-final-word/646838/14

    // Using this will delete the component once the actor is renamed at the end of SpawnAndBindAnimation() -_-
    //UActorComponent* actor_component = animation->AddComponentByClass( UScalingComponent::StaticClass(), false, FTransform::Identity, false );

    // So create and attach/register it to the actor in 2 steps
    UScalingComponent* actor_component = NewObject<UScalingComponent>( animation, UScalingComponent::StaticClass(), FName( "Scaling" ), RF_Transactional /* ??? it's done in USubobjectDataSubsystem::AddNewSubobject()*/ );
    animation->FinishAddComponent( actor_component, false, FTransform::Identity );
    // This will add the component in details view of the actor
    animation->AddInstanceComponent( actor_component );
    // To update everything (done in USubobjectDataSubsystem::AddNewSubobject())
    FLevelEditorModule& LevelEditor = FModuleManager::LoadModuleChecked<FLevelEditorModule>( "LevelEditor" );
    LevelEditor.BroadcastComponentsEdited();

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

    FVector camera_view_size = ActorHelpers::ComputeSizeOfCameraView( iCamera, iFocusDistance );
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

    FVector camera_view_size = ActorHelpers::ComputeSizeOfCameraView( iCamera, focusDistance );
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

    if( !iAnimationArgs.mSpawnable )
        GEditor->ParentActors( iCamera, animation, NAME_None );

    UEposMovieSceneSequence* epos_sequence = Cast<UEposMovieSceneSequence>( iSequence );
    check( epos_sequence );

    FString animation_path;
    FString animation_name;
    NamingConvention::GenerateAnimationActorPathName( iSequencer, *epos_sequence, iSequenceID, !iAnimationArgs.mSpawnable, animation_path, animation_name );

    if( !iAnimationArgs.mName.IsEmpty() )
        animation_name = iAnimationArgs.mName;

    animation->SetFolderPath( *animation_path );
    FActorLabelUtilities::RenameExistingActor( animation, animation_name, false ); // The shot name is displayed in another column in the world outliner

    animation_name = NamingConvention::GenerateAnimationTrackName( iSequencer, *epos_sequence, iSequenceID, animation );

    UE::Sequencer::FCreateBindingParams BindingParams;
    BindingParams.BindingNameOverride = animation_name;
    BindingParams.bAllowCustomBinding = true;
    FGuid animationGuid = iSequencer.CreateBinding( *animation, BindingParams );

    //---

    if( iAnimationArgs.mSpawnable )
    {
        TSubclassOf<UMovieSceneCustomBinding> CustomBindingClass = UMovieSceneSpawnableActorBinding::StaticClass();

        const FMovieSceneBindingReferences* BindingReferences = iSequence->GetBindingReferences();

        if( BindingReferences )
        {
            for( const FMovieSceneBindingReference& Reference : BindingReferences->GetReferences( animationGuid ) )
            {
                for( const TSubclassOf<UMovieSceneCustomBinding>& SupportedCustomBindingType : iSequencer.GetSupportedCustomBindingTypes() )
                {
                    if( SupportedCustomBindingType && SupportedCustomBindingType->IsChildOf( CustomBindingClass ) &&
                        SupportedCustomBindingType->GetDefaultObject<UMovieSceneCustomBinding>()->SupportsConversionFromBinding( Reference, animation ) )
                    {
                        FMovieScenePossessable* NewPossessable = FSequencerUtilities::ConvertToCustomBinding( iSequencer.AsShared(), animationGuid, CustomBindingClass );

                        if( NewPossessable )
                        {
                            for( TWeakObjectPtr<> WeakObject : iSequencer.FindBoundObjects( NewPossessable->GetGuid(), iSequenceID ) )
                            {
                                AOdysseyAnimationActor* SpawnedActor = Cast<AOdysseyAnimationActor>( WeakObject.Get() );
                                if( SpawnedActor )
                                {
                                    animation = SpawnedActor;
                                }
                            }

                            animationGuid = NewPossessable->GetGuid();
                        }
                        break;
                    }
                }
            }
        }

        //---

        //check( iSequencer.GetFocusedTemplateID() == iSequenceID );
        FGuid newCameraGuid = iSequencer.FindObjectId( *iCamera, iSequenceID );
        FMovieSceneObjectBindingID attachBindingID = UE::MovieScene::FRelativeObjectBindingID( newCameraGuid );

        UMovieScene3DAttachTrack* attachTrack = iSequence->GetMovieScene()->AddTrack<UMovieScene3DAttachTrack>( animationGuid );

        // Add a placeholder range which will be modified right after to have an infinite range
        UMovieScene3DAttachSection* attachSection = Cast<UMovieScene3DAttachSection>( attachTrack->AddConstraint( 0, 1, NAME_None, NAME_None, attachBindingID ) );
        attachSection->SetRange( TRange<FFrameNumber>::All() );
        //attachSection->AttachmentLocationRule = EAttachmentRule::KeepWorld;
        //attachSection->AttachmentRotationRule = EAttachmentRule::KeepWorld;
        //attachSection->AttachmentScaleRule = EAttachmentRule::KeepWorld;
        //attachSection->DetachmentLocationRule = EDetachmentRule::KeepWorld;
        //attachSection->DetachmentRotationRule = EDetachmentRule::KeepWorld;
        //attachSection->DetachmentScaleRule = EDetachmentRule::KeepWorld;
        attachSection->Modify();

        attachTrack->Modify();

        // Update the default transform channel to take care of the attach track
        UMovieScene3DTransformTrack* transformTrack = Cast<UMovieScene3DTransformTrack>( iSequence->GetMovieScene()->FindTrack( UMovieScene3DTransformTrack::StaticClass(), animationGuid ) );
        if( transformTrack )
        {
            if( transformTrack->GetAllSections().Num() )
            {
                FTransform world_actor_transform = animation->GetActorTransform();
                FTransform relative_transform = world_actor_transform.GetRelativeTransform( iCamera->GetActorTransform() );

                UMovieScene3DTransformSection* TransformSection = Cast<UMovieScene3DTransformSection>( transformTrack->GetAllSections()[0] );

                TArrayView<FMovieSceneDoubleChannel*> DoubleChannels = TransformSection->GetChannelProxy().GetChannels<FMovieSceneDoubleChannel>();
                DoubleChannels[0]->SetDefault( relative_transform.GetLocation().X );
                DoubleChannels[1]->SetDefault( relative_transform.GetLocation().Y );
                DoubleChannels[2]->SetDefault( relative_transform.GetLocation().Z );

                DoubleChannels[3]->SetDefault( relative_transform.GetRotation().Euler().X );
                DoubleChannels[4]->SetDefault( relative_transform.GetRotation().Euler().Y );
                DoubleChannels[5]->SetDefault( relative_transform.GetRotation().Euler().Z );

                DoubleChannels[6]->SetDefault( relative_transform.GetScale3D().X );
                DoubleChannels[7]->SetDefault( relative_transform.GetScale3D().Y );
                DoubleChannels[8]->SetDefault( relative_transform.GetScale3D().Z );
            }
        }
    }

    //---

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

    FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( *iSequencer, result.mInnerSequence, result.mInnerSequenceId );
    if( !camera_binding.IsValid() )
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

    FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( *iSequencer, sequence, sequence_id );
    if( !camera_binding.IsValid() )
        return false;

    return true;
}

//static
void
ShotSequenceTools::CreateAnimation( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, const FAnimationArgs& iAnimationArgs )
{
    FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( iSequencer, iSequence, iSequenceID );
    ACineCameraActor* camera = ShotSequenceHelpers::GetCameraSpawned( iSequencer, iSequence, iSequenceID, camera_binding );
    if( !ensureMsgf(camera, TEXT( "In case of a spawnable, it means it is not spawned, the sequence is not the focused one by the sequencer" ) ) )
        return;

    ULevelEditorSubsystem* levelEditorSubsystem = GEditor->GetEditorSubsystem<ULevelEditorSubsystem>();
    if( FLevelUtils::IsLevelLocked( levelEditorSubsystem->GetCurrentLevel() ) )
    {
        FNotificationInfo Info( LOCTEXT( "cant-spawn-animation-in-locked-level", "The requested operation could not be completed because the level is locked." ) );
        Info.ExpireDuration = 5.0f;
        TSharedPtr<SNotificationItem> notification = FSlateNotificationManager::Get().AddNotification( Info );
        if (notification)
            notification->SetCompletionState( SNotificationItem::CS_Fail );
        return;
    }

    //---

    const FScopedTransaction transaction( LOCTEXT( "CreateStoryAnimationHere", "Create Storyboard Animation Here" ) );

    AOdysseyAnimationActor* animation_actor;
    TOptional<FFrameTime> start_sequence_in_storyboard;

    {
        cTemporarySwitchInner switch_to( iSequencer, iSequenceID );

        //---

        animation_actor = ShotSequenceTools::SpawnAndBindAnimation( iSequencer, iSequence, iSequenceID, camera_binding, camera, iFrameNumber, iAnimationArgs, nullptr );

        //---

        FMovieSceneInverseSequenceTransform localToRootTransform = iSequencer.GetFocusedMovieSceneSequenceTransform().Inverse();
        start_sequence_in_storyboard = localToRootTransform.TryTransformTime( 0 );

        iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
    }

    // Must be done after the inner/outer sequence switch
    // Otherwise it resets the selection if GEditor->SelectActor() is called inside SpawnAndBindAnimation()
    // Furthermore the hidden flag must also be set to true now
    iSequencer.EmptySelection();
    GEditor->SelectNone( true /*bNoteSelectionChange*/, true /*bDeselectBSPSurfs*/ );
    GEditor->SelectActor( animation_actor, true /*bInSelected*/, true /*bNotify*/, true /*bSelectEvenIfHidden*/ );

    if( start_sequence_in_storyboard )
        iSequencer.SetGlobalTime( *start_sequence_in_storyboard, true /* Evaluate */ );
}

//---

//static
int32
ShotSequenceTools::FilterSelectedAnimations( TArray<AOdysseyAnimationActor*> iAnimationActors, TArray<AOdysseyAnimationActor*>* oSelectedAnimations )
{
    if( oSelectedAnimations )
        oSelectedAnimations->Empty();

    int32 animation_count = iAnimationActors.Num();

    USelection* selection = GEditor->GetSelectedActors();

    int32 animation_selected_count = 0;
    for( int i = 0; i < animation_count; i++ )
    {
        if( !selection->IsSelected( iAnimationActors[i] ) )
            continue;

        if( oSelectedAnimations )
            oSelectedAnimations->Add( iAnimationActors[i] );

        animation_selected_count++;
    }

    return animation_selected_count;
}

//static
int32
ShotSequenceTools::FilterSelectedAnimationsOrAllAnimations( TArray<AOdysseyAnimationActor*> iAnimationActors, TArray<AOdysseyAnimationActor*>* oSelectedAnimations )
{
    if( oSelectedAnimations )
        oSelectedAnimations->Empty();

    int32 animation_selected_count = FilterSelectedAnimations( iAnimationActors, oSelectedAnimations );
    if( animation_selected_count )
        return animation_selected_count;

    if( oSelectedAnimations )
        *oSelectedAnimations = iAnimationActors;

    return iAnimationActors.Num();
}

//static
int32
ShotSequenceTools::FilterSelectedAnimations( TArray<AOdysseyAnimationActor*> iAnimationActors, TArray<FGuid> iAnimationBindings, TArray<AOdysseyAnimationActor*>* oSelectedAnimations, TArray<FGuid>* oSelectedAnimationBindings )
{
    check( iAnimationActors.Num() == iAnimationBindings.Num() );

    if( oSelectedAnimations )
        oSelectedAnimations->Empty();
    if( oSelectedAnimationBindings )
        oSelectedAnimationBindings->Empty();

    int32 animation_count = iAnimationActors.Num();

    USelection* selection = GEditor->GetSelectedActors();

    int32 animation_selected_count = 0;
    for( int i = 0; i < animation_count; i++ )
    {
        if( !selection->IsSelected( iAnimationActors[i] ) )
            continue;

        if( oSelectedAnimations )
            oSelectedAnimations->Add( iAnimationActors[i] );
        if( oSelectedAnimationBindings )
            oSelectedAnimationBindings->Add( iAnimationBindings[i] );

        animation_selected_count++;
    }

    return animation_selected_count;
}

//static
int32
ShotSequenceTools::FilterSelectedAnimationsOrAllAnimations( TArray<AOdysseyAnimationActor*> iAnimationActors, TArray<FGuid> iAnimationBindings, TArray<AOdysseyAnimationActor*>* oSelectedAnimations, TArray<FGuid>* oSelectedAnimationBindings )
{
    if( oSelectedAnimations )
        oSelectedAnimations->Empty();
    if( oSelectedAnimationBindings )
        oSelectedAnimationBindings->Empty();

    int32 animation_selected_count = FilterSelectedAnimations( iAnimationActors, iAnimationBindings, oSelectedAnimations, oSelectedAnimationBindings );
    if( animation_selected_count )
        return animation_selected_count;

    if( oSelectedAnimations )
        *oSelectedAnimations = iAnimationActors;
    if( oSelectedAnimationBindings )
        *oSelectedAnimationBindings = iAnimationBindings;

    return iAnimationActors.Num();
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
    TSet<ACineCameraActor*> cameras;
    for( FGuid animation_binding : iAnimationBindings )
    {
        ShotSequenceHelpers::FFindOrCreateAnimationAttachResult attach_result = ShotSequenceHelpers::FindAnimationAttachTrackAndSections( iSequencer, iSequence, iSequenceID, animation_binding );
        for( TWeakObjectPtr<UMovieScene3DAttachSection> weak_attach_section : attach_result.mSections )
        {
            const FMovieSceneObjectBindingID& attach_object_binding = weak_attach_section->GetConstraintBindingID();
            TArrayView<TWeakObjectPtr<>> weak_attached_objects = attach_object_binding.ResolveBoundObjects( iSequenceID, iSequencer );
            for( TWeakObjectPtr<> weak_attached_object : weak_attached_objects )
                cameras.Add( Cast<ACineCameraActor>( weak_attached_object ) );
        }
    }

    cameras.Remove( nullptr );

    if( cameras.Num() )
        return true;

    //---

    TSet<AOdysseyAnimationActor*> animations;
    for( FGuid animation_binding : iAnimationBindings )
    {
        for( TWeakObjectPtr<> object : iSequencer.FindBoundObjects( animation_binding, iSequenceID ) )
            animations.Add( Cast<AOdysseyAnimationActor>( object ) );
    }

    // It seems it can happen (FindBoundObjects() == nullptr) when actors are moved from one (sub)level to another (sub)level in the Levels panel
    // During the move (maybe):
    // - the actor is removed from a level
    // - then the binding is invalid
    // - a tick occurs (which calls all CanExecute() of every commands)
    // - then try to resolve the binding which are still invalid
    animations.Remove( nullptr );

    bool can_detach = false;
    for( AOdysseyAnimationActor* animation : animations )
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
    const FScopedTransaction transaction( LOCTEXT( "DetachAnimation", "Detach Animation" ) );

    TSet<FGuid> animation_bindings_to_transform;

    for( FGuid animation_binding : iAnimationBindings )
    {
        ShotSequenceHelpers::FFindOrCreateAnimationAttachResult attach_result = ShotSequenceHelpers::FindAnimationAttachTrackAndSections( iSequencer, iSequence, iSequenceID, animation_binding );
        if( attach_result.mTrack.IsValid() )
        {
            iSequence->GetMovieScene()->RemoveTrack( *attach_result.mTrack );

            animation_bindings_to_transform.Add( animation_binding );
        }
    }

    //---

    TSet<AOdysseyAnimationActor*> animations;

    for( FGuid animation_binding : iAnimationBindings )
    {
        for( TWeakObjectPtr<> object : iSequencer.FindBoundObjects( animation_binding, iSequenceID ) )
        {
            animations.Add( Cast<AOdysseyAnimationActor>( object ) );

            animation_bindings_to_transform.Add( animation_binding );
        }
    }

    GEditor->SelectNone( true, true );
    // It's certainly safe to not check if CanDetachAnimation() is ok (like CreateOpacity()/CreateAnimation)
    // as DetachSelectedActors() does the check
    for( AOdysseyAnimationActor* animation : animations )
        GEditor->SelectActor( animation, true /* bInSelected */, true /* bNotify */, true /* bSelectEvenIfHidden */ );

    GEditor->DetachSelectedActors();

    //---

    for( FGuid animation_binding : animation_bindings_to_transform )
    {
        //ACineCameraActor* camera = nullptr;
        //for( TWeakObjectPtr<UMovieScene3DAttachSection> weak_attach_section : attach_result.mSections )
        //{
        //    const FMovieSceneObjectBindingID& attach_object_binding = weak_attach_section->GetConstraintBindingID();
        //    TArrayView<TWeakObjectPtr<>> weak_attached_objects = attach_object_binding.ResolveBoundObjects( iSequenceID, iSequencer );
        //    for( TWeakObjectPtr<> weak_attached_object : weak_attached_objects )
        //        camera =  Cast<ACineCameraActor>( weak_attached_object );
        //}

        // Update the default transform channel to take care of the no more existing attach track
        UMovieScene3DTransformTrack* transformTrack = Cast<UMovieScene3DTransformTrack>( iSequence->GetMovieScene()->FindTrack( UMovieScene3DTransformTrack::StaticClass(), animation_binding ) );
        if( transformTrack )
        {
            TArray<AOdysseyAnimationActor*> animation_actors = ShotSequenceHelpers::GetAnimationSpawnedOrTemplate( iSequencer, iSequence, iSequenceID, animation_binding );
            AOdysseyAnimationActor* animation_actor = animation_actors[0];

            if( transformTrack->GetAllSections().Num() )
            {
                FTransform world_actor_transform = animation_actor->GetActorTransform();
                //FTransform relative_transform = world_actor_transform.GetRelativeTransform( camera->GetActorTransform() );

                UMovieScene3DTransformSection* TransformSection = Cast<UMovieScene3DTransformSection>( transformTrack->GetAllSections()[0] );

                TArrayView<FMovieSceneDoubleChannel*> DoubleChannels = TransformSection->GetChannelProxy().GetChannels<FMovieSceneDoubleChannel>();
                DoubleChannels[0]->SetDefault( world_actor_transform.GetLocation().X );
                DoubleChannels[1]->SetDefault( world_actor_transform.GetLocation().Y );
                DoubleChannels[2]->SetDefault( world_actor_transform.GetLocation().Z );

                DoubleChannels[3]->SetDefault( world_actor_transform.GetRotation().Euler().X );
                DoubleChannels[4]->SetDefault( world_actor_transform.GetRotation().Euler().Y );
                DoubleChannels[5]->SetDefault( world_actor_transform.GetRotation().Euler().Z );

                DoubleChannels[6]->SetDefault( world_actor_transform.GetScale3D().X );
                DoubleChannels[7]->SetDefault( world_actor_transform.GetScale3D().Y );
                DoubleChannels[8]->SetDefault( world_actor_transform.GetScale3D().Z );
            }
        }
    }

    //---

    //iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
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

    // Refresh all immediately to avoid delegate accessing no more valid bindings, ...
    // (But I don't know if it would be to the delegates to always check the validity of its (their) object(s) ? Or assuming that if the delegate exists via the widget, its object must be valid ?)
    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::RefreshAllImmediately );
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

    TArray<FGuid> animation_bindings = ShotSequenceHelpers::GetAnimationBindings( *iSequencer, result.mInnerSequence, result.mInnerSequenceId );

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
    TArray<AOdysseyAnimationActor*> animation_actors = ShotSequenceHelpers::GetAnimationSpawnedOrTemplate( iSequencer, iSequence, iSequenceID, iAnimationBinding );
    AOdysseyAnimationActor* animation_actor = animation_actors[0];

    return animation_actor->GetRootComponent()->IsVisible();
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
        TArray<AOdysseyAnimationActor*> animation_actors = ShotSequenceHelpers::GetAnimationSpawnedOrTemplate( iSequencer, iSequence, iSequenceID, animation_binding );
        AOdysseyAnimationActor* animation_actor = animation_actors[0];

        // Invert the visibility of the animation
        bool new_animation_visibility = !IsAnimationVisible( iSequencer, iSequence, iSequenceID, animation_binding );
        // But if there is a reference animation, invert it and use it for all animations
        if( iAnimationReference.IsSet() )
            new_animation_visibility = !reference_animation_visibility;

        // With this function, when the spawnable is destroyed and respawned (scrubbing outside the section), this value is not stored
        //animation_actor->GetRootComponent()->SetVisibility( new_animation_visibility );

        // When using this way, it works like when the property is clicked on the details panel
        // (But I don't know why)
        USceneComponent* component = animation_actor->GetRootComponent();
        if( component != nullptr )
        {
            FProperty* ChangedProperty = FindFProperty<FProperty>( USceneComponent::StaticClass(), component->GetVisiblePropertyName() );
            component->PreEditChange( ChangedProperty );

            component->SetVisibleFlag( new_animation_visibility );

            FPropertyChangedEvent PropertyChangedEvent( ChangedProperty );
            //component->PostEditChangeProperty( PropertyChangedEvent );
            FEditPropertyChain PropertyChain;
            PropertyChain.AddHead( ChangedProperty );
            FPropertyChangedChainEvent PropertyChainEvent( PropertyChain, PropertyChangedEvent );
            component->PostEditChangeChainProperty( PropertyChainEvent );
        }
    }
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
    // Section selection must be done first, to call our callback in toolkit helpers which change the current frame (when section selection changed)
    // otherwise spawnable won't be loaded and so not found

    // To unselect section(s)
    iSequencer.EmptySelection();
    // And then select the current one
    iSequencer.SelectSection( iParentSection );

    //---

    auto objects = iSequencer.FindBoundObjects( iAnimationBinding, iSequenceID );

    // Get the animation(s) corresponding to the one(s) on the current clicked row
    // It's an array, but generally it should always be only 1 entry
    TArray<AOdysseyAnimationActor*> animations;
    for( auto object : objects )
        animations.Add( Cast<AOdysseyAnimationActor>( object ) );

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
    // Selection must done before empty it
    // Otherwise the filter will always return empty arrays

    // Get all the currently selected animations inside the whole current shot
    TSet<AOdysseyAnimationActor*> animation_actors_selected;
    TArray<FGuid> animation_bindings = ShotSequenceHelpers::GetAnimationBindings( iSequencer, iSequence, iSequenceID );
    for( FGuid animation_binding : animation_bindings )
    {
        TArray<AOdysseyAnimationActor*> animation_actors = ShotSequenceHelpers::GetAnimationSpawned( iSequencer, iSequence, iSequenceID, animation_binding );

        TArray<AOdysseyAnimationActor*> current_animation_actors_selected;
        ShotSequenceTools::FilterSelectedAnimations( animation_actors, &current_animation_actors_selected );
        animation_actors_selected.Append( current_animation_actors_selected );
    }

    //---

    // Section selection must be done first, to call our callback in toolkit helpers which change the current frame (when section selection changed)
    // otherwise spawnable won't be loaded and so not found

    // To unselect section(s)
    iSequencer.EmptySelection();
    // And then select the current one
    iSequencer.SelectSection( iParentSection );

    //---

    TArrayView<TWeakObjectPtr<>> objects = iSequencer.FindBoundObjects( iAnimationBinding, iSequenceID );

    // Get the animation(s) corresponding to the one(s) on the current clicked row
    // It's an array, but generally it should always be only 1 entry
    TArray<AOdysseyAnimationActor*> animations_self;
    for( TWeakObjectPtr<> object : objects )
        animations_self.Add( Cast<AOdysseyAnimationActor>( object ) );

    // Add the self animation to the selection
    // or remove it from the selection if it was already selected
    for( AOdysseyAnimationActor* animation_self : animations_self )
    {
        if( animation_actors_selected.Contains( animation_self ) )
            animation_actors_selected.Remove( animation_self );
        else
            animation_actors_selected.Add( animation_self );
    }

    // To unselect all actors
    GEditor->SelectNone( true, true );
    // And then select the current one(s)
    for( AOdysseyAnimationActor* animation_selected : animation_actors_selected )
        GEditor->SelectActor( animation_selected, true /* bInSelected */, true /* bNotify */, true /* bSelectEvenIfHidden */ );
}

//---

#undef LOCTEXT_NAMESPACE
