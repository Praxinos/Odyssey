// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "ToolkitHelpers.h"

#include "Animation/SkeletalMeshActor.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "CineCameraActor.h"
#include "Containers/ArrayBuilder.h"
#include "GameFramework/Actor.h"
#include "ISequencer.h"
#include "MovieScene.h"
#include "MovieSceneSequence.h"
#include "MovieSceneToolHelpers.h"
#include "Sections/MovieScene3DTransformSection.h"
#include "Sections/MovieSceneBoolSection.h"
#include "Sections/MovieScenePrimitiveMaterialSection.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Tracks/MovieSceneCinematicShotTrack.h"
#include "Tracks/MovieScenePrimitiveMaterialTrack.h"
#include "Tracks/MovieSceneSkeletalAnimationTrack.h"
#include "Tracks/MovieSceneVisibilityTrack.h"

#include "EposSequenceHelpers.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "ToolkitHelpers"

//static
UMovieSceneTrack*
ToolkitHelpers::CreateTrack( ISequencer* iSequencer, AActor* iActor, const FGuid& iBinding, UClass* iClass, int iMaterialTrackIndex )
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
    // For material track, multiple new tracks (of the same type) may be needed
    if( !NewTrack || iMaterialTrackIndex > 0 )
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

            FVector Location = iActor->GetActorLocation();
            FRotator Rotation = iActor->GetActorRotation();
            FVector Scale = iActor->GetActorScale();

            if( iActor->GetRootComponent() )
            {
                FTransform ActorRelativeTransform = iActor->GetRootComponent()->GetRelativeTransform();

                Location = ActorRelativeTransform.GetTranslation();
                Rotation = ActorRelativeTransform.GetRotation().Rotator();
                Scale = ActorRelativeTransform.GetScale3D();
            }

            TArrayView<FMovieSceneDoubleChannel*> DoubleChannels = TransformSection->GetChannelProxy().GetChannels<FMovieSceneDoubleChannel>();

            AddKeyToChannel( DoubleChannels[0], 0, Location.X, iSequencer->GetKeyInterpolation() );
            AddKeyToChannel( DoubleChannels[1], 0, Location.Y, iSequencer->GetKeyInterpolation() );
            AddKeyToChannel( DoubleChannels[2], 0, Location.Z, iSequencer->GetKeyInterpolation() );

            AddKeyToChannel( DoubleChannels[3], 0, Rotation.Euler().X, iSequencer->GetKeyInterpolation() );
            AddKeyToChannel( DoubleChannels[4], 0, Rotation.Euler().Y, iSequencer->GetKeyInterpolation() );
            AddKeyToChannel( DoubleChannels[5], 0, Rotation.Euler().Z, iSequencer->GetKeyInterpolation() );

            AddKeyToChannel( DoubleChannels[6], 0, Scale.X, iSequencer->GetKeyInterpolation() );
            AddKeyToChannel( DoubleChannels[7], 0, Scale.Y, iSequencer->GetKeyInterpolation() );
            AddKeyToChannel( DoubleChannels[8], 0, Scale.Z, iSequencer->GetKeyInterpolation() );

            //DoubleChannels[0]->SetDefault( Location.X );
            //DoubleChannels[1]->SetDefault( Location.Y );
            //DoubleChannels[2]->SetDefault( Location.Z );

            //DoubleChannels[3]->SetDefault( Rotation.Euler().X );
            //DoubleChannels[4]->SetDefault( Rotation.Euler().Y );
            //DoubleChannels[5]->SetDefault( Rotation.Euler().Z );

            //DoubleChannels[6]->SetDefault( Scale.X );
            //DoubleChannels[7]->SetDefault( Scale.Y );
            //DoubleChannels[8]->SetDefault( Scale.Z );
        }

        if( NewTrack->IsA<UMovieScenePrimitiveMaterialTrack>() && iSequencer->GetAutoSetTrackDefaults() )
        {
            UMovieScenePrimitiveMaterialTrack* material_track = Cast< UMovieScenePrimitiveMaterialTrack >( NewTrack );

            auto material_section = Cast<UMovieScenePrimitiveMaterialSection>( NewSection );

            material_track->MaterialIndex = iMaterialTrackIndex;
            material_track->SetDisplayName( FText::Format( LOCTEXT( "MaterialTrackName_Format", "Material Element {0}" ), FText::AsNumber( material_track->MaterialIndex ) ) );

            //---

            APlaneActor* actor = Cast< APlaneActor >( iActor );
            FMovieSceneObjectPathChannelKeyValue material_objectpath;
            if( actor )
            {
                UMaterialInterface* material = actor->GetStaticMeshComponent()->GetMaterial( material_track->MaterialIndex );
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

    for( const FString& PropertyName : PropertyNames )
    {
        FProperty* Property = PropertyOwnerClass->FindPropertyByName( *PropertyName );

        if( Property != nullptr )
        {
            PropertyPath->AddProperty( FPropertyInfo( Property ) );
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
        int32 minNumMaterials = TNumericLimits<int32>::Max();
        for( TWeakObjectPtr<> weakObject : iSequencer->FindObjectsInCurrentSequence( component_binding ) )
        {
            UPrimitiveComponent* primitiveComponent = Cast<UPrimitiveComponent>( weakObject.Get() );
            if( !primitiveComponent )
                continue;

            minNumMaterials = FMath::Min( minNumMaterials, primitiveComponent->GetNumMaterials() );
        }

        if( minNumMaterials == TNumericLimits<int32>::Max() )
            minNumMaterials = 0;

        for( int material_index = 0; material_index < minNumMaterials; material_index++ )
            CreateTrack( iSequencer, iActor, component_binding, UMovieScenePrimitiveMaterialTrack::StaticClass(), material_index );
    }
    // For skeletal mesh actor
    // - '3DTransform' track
    // - 'SkeletalAnimation' track
    else if( iActor->IsA<ASkeletalMeshActor>() )
    {
        CreateTrack( iSequencer, iActor, iBinding, UMovieScene3DTransformTrack::StaticClass() );
        CreateTrack( iSequencer, iActor, iBinding, UMovieSceneSkeletalAnimationTrack::StaticClass() );
    }
    // For empty actor
    else if( ExactCast<AActor>( iActor ) )
    {
    }
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

#undef LOCTEXT_NAMESPACE
