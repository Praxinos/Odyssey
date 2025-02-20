// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "ToolkitHelpers.h"

#include "Animation/SkeletalMeshActor.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "CineCameraActor.h"
#include "Components/StaticMeshComponent.h"
#include "Containers/ArrayBuilder.h"
#include "GameFramework/Actor.h"
#include "IMovieScenePlaybackClient.h"
#include "ISequencer.h"
#include "LevelEditor.h"
#include "Materials/MaterialInterface.h"
#include "MovieScene.h"
#include "MovieSceneSequence.h"
#include "MovieSceneToolHelpers.h"
#include "NiagaraActor.h"
#include "PaperFlipbookActor.h"
#include "Sections/MovieScene3DTransformSection.h"
#include "Sections/MovieSceneBoolSection.h"
#include "Sections/MovieScenePrimitiveMaterialSection.h"
#include "SLevelViewport.h"
#include "Tracks/IMovieSceneTransformOrigin.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Tracks/MovieSceneCinematicShotTrack.h"
#include "Tracks/MovieScenePrimitiveMaterialTrack.h"
#include "Tracks/MovieSceneSkeletalAnimationTrack.h"
#include "Tracks/MovieSceneVisibilityTrack.h"

#include "EposSequenceEditorCommands.h"
#include "EposSequenceHelpers.h"
#include "EposSequenceModule.h"
#include "OdysseyAnimationActor.h"
#include "PlaneActor.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "ToolkitHelpers"

FTransform GetTransformOrigin( TSharedPtr<ISequencer> Sequencer )
{
    FTransform TransformOrigin;

    const IMovieScenePlaybackClient* Client = Sequencer->GetPlaybackClient();
    const UObject* InstanceData = Client ? Client->GetInstanceData() : nullptr;
    const IMovieSceneTransformOrigin* RawInterface = Cast<const IMovieSceneTransformOrigin>( InstanceData );

    const bool bHasInterface = RawInterface || ( InstanceData && InstanceData->GetClass()->ImplementsInterface( UMovieSceneTransformOrigin::StaticClass() ) );
    if( bHasInterface )
    {
        // Retrieve the current origin
        TransformOrigin = RawInterface ? RawInterface->GetTransformOrigin() : IMovieSceneTransformOrigin::Execute_BP_GetTransformOrigin( InstanceData );
    }

    return TransformOrigin;
}

//static
UMovieSceneTrack*
ToolkitHelpers::CreateTrack( ISequencer* iSequencer, AActor* iActor, const FGuid& iBinding, UClass* iClass, FComponentMaterialInfo iMaterialTrackInfo )
{
    if( !iBinding.IsValid() )
        return nullptr;

    // get focused movie scene
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    if( sequence == nullptr )
        return nullptr;

    UMovieScene* movieScene = sequence->GetMovieScene();
    if( movieScene == nullptr )
        return nullptr;

    UMovieSceneTrack* NewTrack = movieScene->FindTrack( iClass, iBinding );
    check( iMaterialTrackInfo.MaterialType == EComponentMaterialType::Empty || iMaterialTrackInfo.MaterialType == EComponentMaterialType::IndexedMaterial );
    // For material track, multiple new tracks (of the same type) may be needed
    if( !NewTrack || iMaterialTrackInfo.MaterialSlotIndex > 0 )
        NewTrack = movieScene->AddTrack( iClass, iBinding );

    bool bCreateDefaultSection = false;
#if WITH_EDITORONLY_DATA
    bCreateDefaultSection = NewTrack->SupportsDefaultSections();
#endif

    if( bCreateDefaultSection )
    {
        UMovieSceneSection* NewSection;
        if( NewTrack->GetAllSections().Num() > 0 )
        {
            NewSection = NewTrack->GetAllSections()[0];
        }
        else
        {
            NewSection = NewTrack->CreateNewSection();
            check( NewTrack->IsEmpty() );
            NewTrack->AddSection( *NewSection );
        }

        // @todo sequencer: hack: setting defaults for transform tracks
        if( NewTrack->IsA( UMovieScene3DTransformTrack::StaticClass() ) && iSequencer->GetAutoSetTrackDefaults() )
        {
            auto TransformSection = Cast<UMovieScene3DTransformSection>( NewSection );

            FTransform Transform = iActor->GetTransform();

            if( USceneComponent* SceneComponent = Cast<USceneComponent>( iActor->GetRootComponent() ) )
            {
                Transform = iActor->GetRootComponent()->GetRelativeTransform();

                if( !SceneComponent->GetAttachParent() )
                {
                    Transform *= GetTransformOrigin( iSequencer->AsShared() ).Inverse();
                }
            }

            FVector Location = Transform.GetTranslation();
            FRotator Rotation = Transform.GetRotation().Rotator();
            FVector Scale = Transform.GetScale3D();

            TArrayView<FMovieSceneDoubleChannel*> DoubleChannels = TransformSection->GetChannelProxy().GetChannels<FMovieSceneDoubleChannel>();

            //AddKeyToChannel( DoubleChannels[0], 0, Location.X, iSequencer->GetKeyInterpolation() );
            //AddKeyToChannel( DoubleChannels[1], 0, Location.Y, iSequencer->GetKeyInterpolation() );
            //AddKeyToChannel( DoubleChannels[2], 0, Location.Z, iSequencer->GetKeyInterpolation() );

            //AddKeyToChannel( DoubleChannels[3], 0, Rotation.Euler().X, iSequencer->GetKeyInterpolation() );
            //AddKeyToChannel( DoubleChannels[4], 0, Rotation.Euler().Y, iSequencer->GetKeyInterpolation() );
            //AddKeyToChannel( DoubleChannels[5], 0, Rotation.Euler().Z, iSequencer->GetKeyInterpolation() );

            //AddKeyToChannel( DoubleChannels[6], 0, Scale.X, iSequencer->GetKeyInterpolation() );
            //AddKeyToChannel( DoubleChannels[7], 0, Scale.Y, iSequencer->GetKeyInterpolation() );
            //AddKeyToChannel( DoubleChannels[8], 0, Scale.Z, iSequencer->GetKeyInterpolation() );

            DoubleChannels[0]->SetDefault( Location.X );
            DoubleChannels[1]->SetDefault( Location.Y );
            DoubleChannels[2]->SetDefault( Location.Z );

            DoubleChannels[3]->SetDefault( Rotation.Euler().X );
            DoubleChannels[4]->SetDefault( Rotation.Euler().Y );
            DoubleChannels[5]->SetDefault( Rotation.Euler().Z );

            DoubleChannels[6]->SetDefault( Scale.X );
            DoubleChannels[7]->SetDefault( Scale.Y );
            DoubleChannels[8]->SetDefault( Scale.Z );
        }

        if( NewTrack->IsA<UMovieScenePrimitiveMaterialTrack>() && iSequencer->GetAutoSetTrackDefaults() )
        {
            UMovieScenePrimitiveMaterialTrack* material_track = Cast< UMovieScenePrimitiveMaterialTrack >( NewTrack );

            UMovieScenePrimitiveMaterialSection* material_section = Cast<UMovieScenePrimitiveMaterialSection>( NewSection );

            material_track->SetMaterialInfo( iMaterialTrackInfo );

            FText trackDisplayName = !iMaterialTrackInfo.MaterialSlotName.IsNone()
                ? FText::Format( LOCTEXT( "SlotMaterialSwitcherTrackName", "Material Slot: {0}" ), FText::FromName( iMaterialTrackInfo.MaterialSlotName ) )
                : FText::Format( LOCTEXT( "IndexedMaterialSwitcherTrackName", "Material Element {0}" ), FText::AsNumber( iMaterialTrackInfo.MaterialSlotIndex ) );
            material_track->SetDisplayName( trackDisplayName );

            //---

            APlaneActor* actor = Cast< APlaneActor >( iActor );
            FMovieSceneObjectPathChannelKeyValue material_objectpath;
            if( actor )
            {
                UMaterialInterface* material = actor->GetStaticMeshComponent()->GetMaterial( material_track->GetMaterialInfo().MaterialSlotIndex );
                material_objectpath = material;
            }

            TArrayView<FMovieSceneObjectPathChannel*> MaterialChannels = material_section->GetChannelProxy().GetChannels<FMovieSceneObjectPathChannel>();
            check( MaterialChannels.Num() == 1 );

            UE::MovieScene::AddKeyToChannel( MaterialChannels[0], 0, material_objectpath, iSequencer->GetKeyInterpolation() );
        }

        if( iSequencer->GetInfiniteKeyAreas() )
        {
            NewSection->SetRange( TRange<FFrameNumber>::All() );
        }
    }

    return NewTrack;
}

//static
FGuid
ToolkitHelpers::CreateComponentTrack( ISequencer* iSequencer, AActor* iActor, UActorComponent* iComponent )
{
    return iSequencer->GetHandleToObject( iComponent );
}

////static
//FGuid
//ToolkitHelpers::CreateComponentTrack( ISequencer* iSequencer, AActor* iActor, const FString& iComponentName )
//{
//    for( UActorComponent* Component : iActor->GetComponents() )
//    {
//        if( !Component )
//            continue;
//
//        if( !Component->GetName().Contains( iComponentName ) )
//            continue;
//
//        FGuid binding = iSequencer->GetHandleToObject( Component );
//        if( binding.IsValid() )
//        {
//            return binding; // Get only the first component matching the name
//        }
//    }
//
//    return FGuid();
//}

//static
void
ToolkitHelpers::CreatePropertyTrack( ISequencer* iSequencer, AActor* iActor, const FGuid& iBinding, UClass* iClass, const FString& iComponentPath, const FString& iPropertyPath )
{
    TSharedRef<FPropertyPath> PropertyPath = FPropertyPath::CreateEmpty();
    UObject* PropertyOwner = iActor;

    // determine object hierarchy
    TArray<FString> ComponentNames;
    iComponentPath.ParseIntoArray( ComponentNames, TEXT( "." ) );

    for( const FString& ComponentName : ComponentNames )
    {
        PropertyOwner = FindObjectFast<UObject>( PropertyOwner, *ComponentName );
        if( PropertyOwner == nullptr )
            return;
    }

    UStruct* PropertyOwnerClass = PropertyOwner->GetClass();

    // determine property path
    TArray<FString> PropertyNames;
    iPropertyPath.ParseIntoArray( PropertyNames, TEXT( "." ) );

    //TODO: added in 5.2
    //bool bReplaceWithTransformTrack = false;
    for( const FString& PropertyName : PropertyNames )
    {
        FProperty* Property = PropertyOwnerClass->FindPropertyByName( *PropertyName );

        if( Property != nullptr )
        {
            PropertyPath->AddProperty( FPropertyInfo( Property ) );

            //TODO: added in 5.2
            //// Transform tracks are a special case and must be handled separately.
            //if( PropertyOwner->IsA( USceneComponent::StaticClass() ) &&
            //    ( PropertyName == TEXT( "RelativeLocation" ) || PropertyName == TEXT( "RelativeRotation" ) || PropertyName == TEXT( "RelativeScale3D" ) ) )
            //{
            //    bReplaceWithTransformTrack = true;
            //    break;
            //}
        }

        FStructProperty* StructProperty = CastField<FStructProperty>( Property );

        if( StructProperty != nullptr )
        {
            PropertyOwnerClass = StructProperty->Struct;
            continue;
        }

        FObjectProperty* ObjectProperty = CastField<FObjectProperty>( Property );

        if( ObjectProperty != nullptr )
        {
            PropertyOwnerClass = ObjectProperty->PropertyClass;
            continue;
        }

        break;
    }

    //FGuid ComponentBinding = iSequencer->GetHandleToObject( PropertyOwner );

    //TODO: added in 5.2
    //if( bReplaceWithTransformTrack )
    //{
    //    UClass* TrackClass = UMovieScene3DTransformTrack::StaticClass();
    //    UMovieSceneTrack* NewTrack = MovieScene->FindTrack( TrackClass, ComponentBinding );
    //    if( !NewTrack )
    //    {
    //        NewTrack = MovieScene->AddTrack( TrackClass, ComponentBinding );
    //        CreateDefaultTrackSection( NewTrack, PropertyOwner );
    //    }
    //    return;
    //}

    //TODO: added in 5.5
    //bool bFoundPropertyTrack = false;
    //if( ComponentBinding.IsValid() )
    //{
    //    TArray<UMovieSceneTrack*> Tracks = MovieScene->FindTracks( UMovieScenePropertyTrack::StaticClass(), ComponentBinding, NAME_None );
    //    for( UMovieSceneTrack* Track : Tracks )
    //    {
    //        UMovieScenePropertyTrack* PropertyTrack = Cast<UMovieScenePropertyTrack>( Track );
    //        if( PropertyTrack )
    //        {
    //            if( PropertyTrack->GetPropertyPath() == PropertyTrackSettings.PropertyPath )
    //            {
    //                bFoundPropertyTrack = true;
    //                break;
    //            }
    //        }
    //    }
    //}

    //if( bFoundPropertyTrack )
    //{
    //    continue;
    //}

    if( !iSequencer->CanKeyProperty( FCanKeyPropertyParams( PropertyOwner->GetClass(), *PropertyPath ) ) )
        return;

    // key property
    FKeyPropertyParams KeyPropertyParams( TArrayBuilder<UObject*>().Add( PropertyOwner ), *PropertyPath, ESequencerKeyMode::ManualKey );

    iSequencer->KeyProperty( KeyPropertyParams );

    //---

    if( !iClass )
        return;

    if( !iBinding.IsValid() )
        return;

    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    if( sequence == nullptr )
        return;

    UMovieScene* movieScene = sequence->GetMovieScene();
    if( movieScene == nullptr )
        return;

    UMovieSceneTrack* NewTrack = movieScene->FindTrack( iClass, iBinding );
    if( !NewTrack )
        return;

    if( NewTrack->IsA<UMovieSceneVisibilityTrack>() )
    {
        UMovieSceneVisibilityTrack* track = Cast<UMovieSceneVisibilityTrack>( NewTrack );
        check( track );

        auto sections = track->GetAllSections();
        if( !sections.Num() )
            return;

        UMovieSceneBoolSection* section = Cast<UMovieSceneBoolSection>( sections[0] );
        section->SetRange( movieScene->GetPlaybackRange() );

        FMovieSceneBoolChannel& channel = section->GetChannel();
        channel.SetDefault( true );
    }
}

//static
void
ToolkitHelpers::CreateDefaultTracksForActor( ISequencer* iSequencer, AActor* iActor, const FGuid iBinding )
{
    // For binding which has been removed when dropped actor is not supported
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    if( !sequence )
        return;
    UMovieScene* movieScene = sequence->GetMovieScene();
    if( !movieScene )
        return;
    if( !movieScene->FindBinding( iBinding ) )
        return;

    // TODO-lchabant: add default tracks (re-use level sequence toolkit code).

    // For cinecamera actor
    // - '3DTransform' track
    // - 'CameraComponent' binding (automatically when adding property)
    //     - 'CurrentFocalLength' property
    //     - 'FocusSettings.ManualFocusDistance' property
    //     - 'CurrentAperture' property
    if( iActor->IsA<ACineCameraActor>() )
    {
        CreateTrack( iSequencer, iActor, iBinding, UMovieScene3DTransformTrack::StaticClass() );

        //CreatePropertyTrack( iSequencer, iActor, "CameraComponent", "CurrentFocalLength" );
        //CreatePropertyTrack( iSequencer, iActor, "CameraComponent", "FocusSettings.ManualFocusDistance" );
        //CreatePropertyTrack( iSequencer, iActor, "CameraComponent", "CurrentAperture" );

        return;
    }
    // For planes (static mesh actor)
    // - ('3DTransform' track)
    // - 'Visibility' track
    // - 'StaticMeshComponent' binding
    //     - 'Material Switcher' track
    else if( iActor->IsA<APlaneActor>() )
    {
        //CreateTrack( iSequencer, iActor, iBinding, UMovieScene3DTransformTrack::StaticClass() );

        CreatePropertyTrack( iSequencer, iActor, iBinding, UMovieSceneVisibilityTrack::StaticClass(), "", "bHidden" );

        FGuid component_binding = CreateComponentTrack( iSequencer, iActor, iActor->GetRootComponent() );

        //---

        // From D:\work\UnrealEngine\Engine\Source\Editor\MovieSceneTools\Private\TrackEditors\PrimitiveMaterialTrackEditor.cpp
        UObject* object = iSequencer->FindSpawnedObjectOrTemplate( component_binding );
        USceneComponent* sceneComponent = Cast<USceneComponent>( object );
        UPrimitiveComponent* primitiveComponent = Cast<UPrimitiveComponent>( sceneComponent );

        if( primitiveComponent )
        {
            int32 numMaterials = primitiveComponent->GetNumMaterials();
            TArray<FName> materialSlotNames = primitiveComponent->GetMaterialSlotNames();

            for( int32 materialIndex = 0; materialIndex < numMaterials; materialIndex++ )
            {
                FName materialSlotName = materialSlotNames.IsValidIndex( materialIndex ) ? materialSlotNames[materialIndex] : FName();
                FComponentMaterialInfo materialInfo{ materialSlotName, materialIndex, EComponentMaterialType::IndexedMaterial };

                CreateTrack( iSequencer, iActor, component_binding, UMovieScenePrimitiveMaterialTrack::StaticClass(), materialInfo );
            }
        }
    }
    else if( iActor->IsA<AOdysseyAnimationActor>() )
    {
        CreatePropertyTrack( iSequencer, iActor, iBinding, UMovieSceneVisibilityTrack::StaticClass(), "", "bHidden" );
    }
    // For skeletal mesh actor
    // - '3DTransform' track
    // - 'SkeletalAnimation' track
    else if( iActor->IsA<ASkeletalMeshActor>() )
    {
        CreateTrack( iSequencer, iActor, iBinding, UMovieScene3DTransformTrack::StaticClass() );
        CreateTrack( iSequencer, iActor, iBinding, UMovieSceneSkeletalAnimationTrack::StaticClass() );
    }
    // For niagara actor
    // - '3DTransform' track
    else if( iActor->IsA<ANiagaraActor>() )
    {
        CreateTrack( iSequencer, iActor, iBinding, UMovieScene3DTransformTrack::StaticClass() );
    }
    // For flipbook actor
    // - '3DTransform' track
    else if( iActor->IsA<APaperFlipbookActor>() )
    {
        CreateTrack( iSequencer, iActor, iBinding, UMovieScene3DTransformTrack::StaticClass() );
    }
    // For empty actor
    else if( ExactCast<AActor>( iActor ) )
    {
    }

    // callback to set up default tracks via code
    FEposSequenceModule& eposSequenceModule = FModuleManager::LoadModuleChecked<FEposSequenceModule>( "EposSequence" );
    eposSequenceModule.OnNewActorTrackAdded().Broadcast( *iActor, iBinding, iSequencer->AsShared() );
}

void
ToolkitHelpers::FixCameraBindingOnCameraCut( ISequencer* iSequencer, AActor* iActor, const FGuid iBinding )
{
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    if( !sequence )
        return;
    UMovieScene* movieScene = sequence->GetMovieScene();
    if( !movieScene )
        return;

    //---

    // Not a camera, nothing to do
    if( !iActor->IsA<ACineCameraActor>() )
        return;

    // No single cameracut track, nothing to do
    UMovieSceneTrack* track = movieScene->GetCameraCutTrack();
    UMovieSceneSingleCameraCutTrack* cameracut_track = Cast<UMovieSceneSingleCameraCutTrack>( track );
    if( !cameracut_track )
        return;

    if( !cameracut_track->GetAllSections().Num() )
        return;

    UMovieSceneSection* section = cameracut_track->GetAllSections()[0];
    UMovieSceneSingleCameraCutSection* cameracut_section = Cast<UMovieSceneSingleCameraCutSection>( section );

    cameracut_section->Modify();
    cameracut_section->SetCameraGuid( iBinding );
}

void
ToolkitHelpers::PatchStandardCameraCutTrack( ISequencer* iSequencer, AActor* iActor, const FGuid iBinding )
{
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    FMovieSceneSequenceID sequence_id = iSequencer->GetFocusedTemplateID();
    if( !sequence )
        return;
    UMovieScene* movieScene = sequence->GetMovieScene();
    if( !movieScene )
        return;

    //---

    // No cameracut track, nothing to do
    UMovieSceneTrack* track = movieScene->GetCameraCutTrack();
    if( !track )
        return;

    // Already a single cameracut track, nothing to do
    UMovieSceneSingleCameraCutTrack* cameracut_track = Cast<UMovieSceneSingleCameraCutTrack>( track );
    if( cameracut_track )
        return;

    // Transform the existing cameracut (not a single one) to a single one
    movieScene->RemoveCameraCutTrack();

    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, sequence, sequence_id, &camera_guid );

    //TODO: maybe get the size of the existing section
    // But as it should only be called in FSequencer::AddActors(), after an auto track creation, it should be ok to replace without taking care to of the existing section

    ShotSequenceTools::CreateCameraCut( *iSequencer, sequence, camera_guid, iSequencer->GetLocalTime().Time.FloorToFrame() );
}

//---

//static
void
ToolkitHelpers::SetStoryboardViewport()
{
    auto GetFirstPerspectiveClient = []() -> SLevelViewport*
    {
        FLevelEditorViewportClient* levelVC = nullptr;

        for( FLevelEditorViewportClient* viewportClient : GEditor->GetLevelViewportClients() )
        {
            if( viewportClient
                && viewportClient->GetViewMode() != VMI_Unknown
                && viewportClient->AllowsCinematicControl()
                && viewportClient->IsPerspective() )
                //TODO: improve by getting an already "storyboard viewport" if exists
            {
                levelVC = viewportClient;
                break;
            }
        }

        if( !levelVC )
        {
            //TODO: improve by setting to perspective if no one find
            //viewport->GetLevelViewportClient().SetViewportType(ELevelViewportType::LVT_Perspective); // Need to be called first

            FLevelEditorModule& levelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>( "LevelEditor" );
            return levelEditorModule.GetFirstActiveLevelViewport().Get();
        }

        TSharedPtr<SEditorViewport> viewport_widget = levelVC->GetEditorViewportWidget();
        return StaticCastSharedPtr<SLevelViewport>( viewport_widget ).Get();
    };

    // Do not use a TSharedPtr<> !!!
    // https://udn.unrealengine.com/s/question/0D54z00007bITs8CAG/changing-the-viewport-layout-type-makes-the-viewport-unfocused-and-gcurrentleveleditingviewportclient-nullptr
    SLevelViewport* viewport = GetFirstPerspectiveClient();
    if( viewport )
    {
        viewport->GetCommandList()->ExecuteAction( FEposSequenceEditorCommands::Get().ToggleStoryboardViewportCommand.ToSharedRef() );
        viewport = nullptr; // viewport is no more valid after changing viewport layout type

        // Only for 5.0.1
        viewport = GetFirstPerspectiveClient();
        viewport->GetLevelViewportClient().SetCurrentViewport();
    }
}

//---

//static
void
ToolkitHelpers::HandleActorAddedToSequencer( AActor* iActor, const FGuid iBinding, ISequencer* iSequencer )
{
    ToolkitHelpers::CreateDefaultTracksForActor( iSequencer, iActor, iBinding );

    ToolkitHelpers::FixCameraBindingOnCameraCut( iSequencer, iActor, iBinding );

    //PATCH: replace standard cameracut track (if exists) by our single cameracut track
    ToolkitHelpers::PatchStandardCameraCutTrack( iSequencer, iActor, iBinding );
}

//static
void
ToolkitHelpers::HandleOnActivateSequence( FMovieSceneSequenceIDRef iSequenceID, ISequencer* iSequencer )
{
    check( iSequenceID == iSequencer->GetFocusedTemplateID() );

    auto playback_range = iSequencer->GetFocusedMovieSceneSequence()->GetMovieScene()->GetPlaybackRange();
    FQualifiedFrameTime time = iSequencer->GetLocalTime();

    if( playback_range.Contains( time.Time.GetFrame() ) )
        return;

    iSequencer->SetLocalTime( playback_range.GetLowerBoundValue() );
}

//static
void
ToolkitHelpers::HandleOnSelectionChangedSections( TArray<UMovieSceneSection*> iSections, ISequencer* iSequencer )
{
    if( !iSections.Num() )
        return;

    //PATCH: sometimes the given section is not inside the current sequence
    // first down (of the double click) on a section in the root sequence
    //     [2021.07.27 - 08.40.02:647][497]LogTemp : Warning : GetSelectionChangedSections : local time 102000
    //     [2021.07.27 - 08.40.02:647][497]LogTemp : Warning : GetSelectionChangedSections : 1 sections
    //     [2021.07.27 - 08.40.02:647][497]LogTemp : Warning : GetSelectionChangedSections : xxxboard0010_01xxx
    // unselect the section before changing the focused sequence
    //     [2021.07.27 - 08.40.02:715][505]LogTemp : Warning : GetSelectionChangedSections : local time 0
    //     [2021.07.27 - 08.40.02:716][505]LogTemp : Warning : GetSelectionChangedSections : 0 sections
    // change the focused sequence to make the subsequence the focused one
    //     [2021.07.27 - 08.40.02:716][505]LogTemp : Warning : OnActivateSequence
    // !!!
    // the section inside the root sequence (previous focused sequence) is still set as selected (but not everytime)
    // and this makes the "set local time" wrong due to the lower bound value of the section which is related to the root sequence and not the new focused one
    // !!!
    //     [2021.07.27 - 08.40.02:793][514]LogTemp : Warning : GetSelectionChangedSections : local time 0
    //     [2021.07.27 - 08.40.02:793][514]LogTemp : Warning : GetSelectionChangedSections : 1 sections
    //     [2021.07.27 - 08.40.02:793][514]LogTemp : Warning : GetSelectionChangedSections : xxxboard0010_01xxx
    // so we check the given section is one of the section in the focused sequence
    if( !iSequencer->GetFocusedMovieSceneSequence()->GetMovieScene()->GetAllSections().Contains( iSections.Last() ) )
        return;

    FQualifiedFrameTime time = iSequencer->GetLocalTime();
    if( iSections.Last()->GetTrueRange().Contains( time.Time.GetFrame() ) )
        return;

    iSequencer->SetLocalTime( iSections.Last()->GetTrueRange().GetLowerBoundValue() );
}

#undef LOCTEXT_NAMESPACE
