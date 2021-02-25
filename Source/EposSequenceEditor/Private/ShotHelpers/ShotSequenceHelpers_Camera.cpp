// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "ShotSequenceHelpers.h"

#include "AssetToolsModule.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Factories/Texture2dFactoryNew.h"
#include "ISequencer.h"
#include "LevelEditorActions.h"
#include "LevelEditorViewport.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieScene.h"
#include "MovieSceneSequence.h"
#include "MovieSceneToolHelpers.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Tracks/MovieSceneCinematicShotTrack.h"

#define LOCTEXT_NAMESPACE "ShotSequenceHelpers_Camera"

//static
ACineCameraActor*
ShotSequenceHelpers::GetCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid* oGuid )
{
    UMovieScene* movieScene = iSequence->GetMovieScene();
    if( !movieScene )
        return nullptr;

    ACineCameraActor* ExistingCamera = nullptr;
    for( int i = 0; i < movieScene->GetPossessableCount(); i++ )
    {
        FMovieScenePossessable possessable = movieScene->GetPossessable( i );

        for( TWeakObjectPtr<> WeakObject : iSequencer.FindBoundObjects( possessable.GetGuid(), iSequenceID ) )
        {
            ExistingCamera = Cast<ACineCameraActor>( WeakObject.Get() );

            if( ExistingCamera )
            {
                if( oGuid )
                    *oGuid = possessable.GetGuid();

                return ExistingCamera;
            }
        }
    }

    return nullptr;
}

//---

ShotSequenceHelpers::cTemporarySwitchInner::cTemporarySwitchInner( ISequencer& iSequencer, FMovieSceneSequenceIDRef iInnerID )
    : mSequencer( iSequencer )
    , mOriginalId()
{
    check( iInnerID != MovieSceneSequenceID::Root );

    mOriginalId = mSequencer.GetFocusedTemplateID();
    if( iInnerID == mOriginalId )
        return;

    UMovieSceneSubSection* subsection = mSequencer.FindSubSection( iInnerID );
    check( subsection );
    mSequencer.FocusSequenceInstance( *subsection );
}

ShotSequenceHelpers::cTemporarySwitchInner::~cTemporarySwitchInner()
{
    FMovieSceneSequenceID focused_id = mSequencer.GetFocusedTemplateID();
    if( focused_id == mOriginalId )
        return;

    if( mOriginalId == MovieSceneSequenceID::Root )
    {
        mSequencer.ResetToNewRootSequence( *mSequencer.GetRootMovieSceneSequence() );
    }
    else
    {
        UMovieSceneSubSection* subsection = mSequencer.FindSubSection( mOriginalId );
        check( subsection );
        mSequencer.FocusSequenceInstance( *subsection );
    }
}

//---

//static
void
ShotSequenceHelpers::CreateCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    UMovieScene* movieScene = iSequence->GetMovieScene();
    if( !movieScene )
        return;

    if( movieScene->IsReadOnly() )
    {
        //ShowReadOnlyError();
        return;
    }

    FGuid CameraGuid;
    ACineCameraActor* ExistingCamera = ShotSequenceHelpers::GetCamera( iSequencer, iSequence, iSequenceID, &CameraGuid );
    if( ExistingCamera )
        return;

    //---

    const FScopedTransaction transaction( LOCTEXT( "CreateStoryCameraHere", "Create Storyboard Camera Here" ) );

    cTemporarySwitchInner switch_to( iSequencer, iSequenceID );

    //---

    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::SpawnAndBindCamera( iSequencer, &camera_guid );

    ShotSequenceHelpers::CameraAdded( iSequencer, iSequence, camera_guid, camera, iSequencer.GetLocalTime().Time.FloorToFrame() );

    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
}

//static
ACineCameraActor*
ShotSequenceHelpers::SpawnAndBindCamera( ISequencer& iSequencer, FGuid* oGuid ) // From FSequencer::CreateCamera()
{
    UWorld* World = GCurrentLevelEditingViewportClient->GetWorld();

    // Set new camera to match viewport
    FActorSpawnParameters SpawnParams;
    ACineCameraActor* NewCamera = World->SpawnActor<ACineCameraActor>( SpawnParams );
    if( !NewCamera )
        return nullptr;

    NewCamera->SetActorLocation( GCurrentLevelEditingViewportClient->GetViewLocation(), false );
    NewCamera->SetActorRotation( GCurrentLevelEditingViewportClient->GetViewRotation() );
    //pNewCamera->CameraComponent->FieldOfView = ViewportClient->ViewFOV; //@todo set the focal length from this field of view

    FGuid CameraGuid = iSequencer.CreateBinding( *NewCamera, NewCamera->GetActorLabel() );
    if( !CameraGuid.IsValid() )
        return nullptr;

    iSequencer.OnActorAddedToSequencer().Broadcast( NewCamera, CameraGuid );

    //---
    // From FSequencer::NewCameraAdded( CameraGuid, NewCamera )

    //iSequencer.SetPerspectiveViewportCameraCutEnabled( false );

    // Lock the viewport to this camera
    if( NewCamera && NewCamera->GetLevel() )
    {
        // an option ?

        //GCurrentLevelEditingViewportClient->SetMatineeActorLock( nullptr );
        //GCurrentLevelEditingViewportClient->SetActorLock( NewCamera );
        //GCurrentLevelEditingViewportClient->bLockedCameraView = true;
        //GCurrentLevelEditingViewportClient->UpdateViewForLockedActor();
        GCurrentLevelEditingViewportClient->Invalidate();
    }

    *oGuid = CameraGuid;
    return NewCamera;
}

//static
void
ShotSequenceHelpers::CameraAdded( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid CameraGuid, const ACineCameraActor* iCamera, FFrameNumber FrameNumber)
{
    CreateCameraCut( iSequencer, iSequence, CameraGuid, FrameNumber );

    SpawnAndBindPlane( iSequencer, iSequence, CameraGuid, iCamera, FrameNumber );
}

//static
void
ShotSequenceHelpers::CreateCameraCut( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid iCameraGuid, FFrameNumber /*iFrameNumber*/ ) // From MovieSceneToolHelpers::CameraAdded()
{
    UMovieScene* movieScene = iSequence->GetMovieScene();

    //---

    // If there's a cinematic shot track, no need to set this camera to a shot
    UMovieSceneTrack* CinematicShotTrack = movieScene->FindMasterTrack( UMovieSceneCinematicShotTrack::StaticClass() );
    if( CinematicShotTrack )
        return;

    UMovieSceneTrack* CameraCutTrack = movieScene->GetCameraCutTrack();

    // If there's a camera cut track with at least one section, no need to change the section
    if( CameraCutTrack && CameraCutTrack->GetAllSections().Num() > 0 )
    {
        UMovieSceneSingleCameraCutSection* CameraCutSection = Cast<UMovieSceneSingleCameraCutSection>( CameraCutTrack->GetAllSections()[0] );

        CameraCutSection->Modify();
        CameraCutSection->SetCameraGuid( iCameraGuid );

        return;
    }

    if( !CameraCutTrack )
    {
        CameraCutTrack = movieScene->AddCameraCutTrack( UMovieSceneSingleCameraCutTrack::StaticClass() );
    }

    if( CameraCutTrack )
    {
        UMovieSceneSection* Section = MovieSceneHelpers::FindSectionAtTime( CameraCutTrack->GetAllSections(), 0 /*iFrameNumber*/ );
        UMovieSceneSingleCameraCutSection* CameraCutSection = Cast<UMovieSceneSingleCameraCutSection>( Section );

        if( CameraCutSection )
        {
            CameraCutSection->Modify();
            CameraCutSection->SetCameraGuid( iCameraGuid );
        }
        else
        {
            UMovieSceneSingleCameraCutTrack* single_cameracut_track = Cast<UMovieSceneSingleCameraCutTrack>( CameraCutTrack );
            FMovieSceneObjectBindingID binding_id( iCameraGuid, MovieSceneSequenceID::Root, EMovieSceneObjectBindingSpace::Local ); // Like in UMovieSceneSingleCameraCutSection::SetCameraGuid()

            single_cameracut_track->AddNewSingleCameraCut( binding_id, 0 /*iFrameNumber*/ );

            //CameraCutTrack->Modify();

            //UMovieSceneSingleCameraCutSection* NewSection = Cast<UMovieSceneSingleCameraCutSection>( CameraCutTrack->CreateNewSection() );
            //NewSection->SetRange( movieScene->GetPlaybackRange() );
            //NewSection->SetCameraGuid( iCameraGuid );
            //CameraCutTrack->AddSection( *NewSection );
        }
    }
}

//static
FVector
ShotSequenceHelpers::ComputePlaneScale( const ACineCameraActor* iCamera, float iDistance ) // From FDrawFrustumSceneProxy::GetDynamicMeshElements()
{
    float FrustumAngle = iCamera->GetCineCameraComponent()->GetHorizontalFieldOfView();
    float FrustumAspectRatio = iCamera->GetCineCameraComponent()->AspectRatio;
    float FrustumEndDist = iDistance;

    //---

    FVector Direction( 1, 0, 0 );
    FVector LeftVector( 0, 1, 0 );
    FVector UpVector( 0, 0, 1 );

    FVector Verts[8];

    // FOVAngle controls the horizontal angle.
    const float HozHalfAngleInRadians = FMath::DegreesToRadians( FrustumAngle * 0.5f );

    float HozLength = 0.0f;
    float VertLength = 0.0f;

    //if( FrustumAngle > 0.0f )
    //{
    //    HozLength = FrustumStartDist * FMath::Tan( HozHalfAngleInRadians );
    //    VertLength = HozLength / FrustumAspectRatio;
    //}
    //else
    //{
    //    const float OrthoWidth = ( FrustumAngle == 0.0f ) ? 1000.0f : -FrustumAngle;
    //    HozLength = OrthoWidth * 0.5f;
    //    VertLength = HozLength / FrustumAspectRatio;
    //}

    //// near plane verts
    //Verts[0] = ( Direction * FrustumStartDist ) + ( UpVector * VertLength ) + ( LeftVector * HozLength );
    //Verts[1] = ( Direction * FrustumStartDist ) + ( UpVector * VertLength ) - ( LeftVector * HozLength );
    //Verts[2] = ( Direction * FrustumStartDist ) - ( UpVector * VertLength ) - ( LeftVector * HozLength );
    //Verts[3] = ( Direction * FrustumStartDist ) - ( UpVector * VertLength ) + ( LeftVector * HozLength );

    if( FrustumAngle > 0.0f )
    {
        HozLength = FrustumEndDist * FMath::Tan( HozHalfAngleInRadians );
        VertLength = HozLength / FrustumAspectRatio;
    }

    // far plane verts
    Verts[4] = ( Direction * FrustumEndDist ) + ( UpVector * VertLength ) + ( LeftVector * HozLength );
    Verts[5] = ( Direction * FrustumEndDist ) + ( UpVector * VertLength ) - ( LeftVector * HozLength );
    Verts[6] = ( Direction * FrustumEndDist ) - ( UpVector * VertLength ) - ( LeftVector * HozLength );
    Verts[7] = ( Direction * FrustumEndDist ) - ( UpVector * VertLength ) + ( LeftVector * HozLength );

    //for( int32 X = 0; X < 8; ++X )
    //{
    //    Verts[X] = GetLocalToWorld().TransformPosition( Verts[X] );
    //}

    float norm_x = FVector::Distance( Verts[4], Verts[5] );
    float norm_y = FVector::Distance( Verts[4], Verts[7] );

    //---

    return FVector( norm_x, norm_y, 1.f );
    //return FVector( 1.5f, 1.f, 1.f );
}

static
UMaterialInstanceConstant*
CreateMaterialInstanceConstantAsset( UMovieSceneSequence* iSequence, FString& oPackageName, FString& oAssetName )
{
    UMaterial* material = LoadObject<UMaterial>( nullptr, TEXT( "/Epos/M_Plane_Basic.M_Plane_Basic" ) );
    if( !material )
        return nullptr;

    UPackage* package = iSequence->GetPackage();
    FString package_name = package->GetName(); // ie. /Game/MyStoryboard2/shot0001_01

    FAssetToolsModule& Module = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    Module.Get().CreateUniqueAssetName( package_name, "_Inst", oPackageName, oAssetName );

    UMaterialInstanceConstantFactoryNew* factory = NewObject<UMaterialInstanceConstantFactoryNew>();
    factory->InitialParent = material;

    FString package_path = FPackageName::GetLongPackagePath( oPackageName );
    UObject* new_object = Module.Get().CreateAsset( oAssetName, package_path, UMaterialInstanceConstant::StaticClass(), factory );

    return Cast<UMaterialInstanceConstant>( new_object );
}

static
UTexture2D*
CreateTexture2DAsset( UMovieSceneSequence* iSequence, UMaterialInterface* iMaterial, FString& oPackageName, FString& oAssetName )
{
    UTexture2D* texture_transparent = LoadObject<UTexture2D>( nullptr, TEXT( "/Epos/T_Transparent" ) );
    if( !texture_transparent )
        return nullptr;

    UPackage* package = iMaterial->GetPackage();
    FString package_name = package->GetName(); // ie. /Game/MyStoryboard2/M_Plane_Basic_Inst

    FAssetToolsModule& Module = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    Module.Get().CreateUniqueAssetName( package_name, "_Texture", oPackageName, oAssetName );

    FString package_path = FPackageName::GetLongPackagePath( oPackageName );
    UObject* new_object = Module.Get().DuplicateAsset( oAssetName, package_path, texture_transparent );

    return Cast<UTexture2D>( new_object );
}

//static
void
ShotSequenceHelpers::SpawnAndBindPlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid iCameraGuid, const ACineCameraActor* iCamera, FFrameNumber iFrameNumber )
{
    FTransform camera_transform = iCamera->GetRootComponent()->GetComponentTransform();

    //---

    FVector const CamLocation = camera_transform.GetLocation();
    FVector const CamDir = camera_transform.GetRotation().Vector();
    FRotator const CamRot = camera_transform.Rotator();

    //---

    // Make a function ComputePlaneLocation(...)
    float FocusDistance = 200;
    FVector plane_location = CamLocation + CamDir * FocusDistance;

    FVector plane_scale = ShotSequenceHelpers::ComputePlaneScale( iCamera, FocusDistance );

    //---

    UStaticMesh* plane_mesh = LoadObject<UStaticMesh>( nullptr, TEXT( "/Epos/S_1_Unit_Plane.S_1_Unit_Plane" ) );
    check( plane_mesh );

    //---

    FString new_material_package_name;
    FString new_material_asset_name;
    UMaterialInstanceConstant* new_material = CreateMaterialInstanceConstantAsset( iSequence, new_material_package_name, new_material_asset_name );
    if( !new_material )
        return;

    FString new_texture_package_name;
    FString new_texture_asset_name;
    UTexture2D* new_texture = CreateTexture2DAsset( iSequence, new_material, new_texture_package_name, new_texture_asset_name );
    if( !new_texture )
        return;

    new_material->SetTextureParameterValueEditorOnly( TEXT( "DrawingTexture" ), new_texture );

    FStaticParameterSet static_params;
    new_material->GetStaticParameterValues( static_params );
    for( auto& parameter : static_params.StaticSwitchParameters )
        parameter.bOverride = true;
    new_material->UpdateStaticPermutation( static_params );

    // Needed to compute all cases during creation, to have all shaders computed
    for( int combination = 0; combination < FMath::Pow( 2, static_params.StaticSwitchParameters.Num() ); combination++ )
    {
        //UE_LOG( LogTemp, Warning, TEXT( "combination: %d" ), combination );
        for( int i = 0; i < static_params.StaticSwitchParameters.Num(); i++ )
        {
            static_params.StaticSwitchParameters[i].Value = combination & ( 1 << i );
            //UE_LOG( LogTemp, Warning, TEXT( "i: %d - value: %d" ), i, static_params.StaticSwitchParameters[i].Value );
            new_material->UpdateStaticPermutation( static_params );
        }
    }

    //---

    UWorld* World = GCurrentLevelEditingViewportClient->GetWorld();

    FActorSpawnParameters SpawnParams;
    AStaticMeshActor* plane = World->SpawnActor<AStaticMeshActor>( SpawnParams );

    plane->GetStaticMeshComponent()->SetStaticMesh( plane_mesh );
    plane->GetStaticMeshComponent()->SetMaterial( 0, new_material );

    //---

    plane->SetActorScale3D( plane_scale );
    plane->SetActorLocation( plane_location );
    plane->SetActorRotation( FRotator( 0.f, 90.f, 90.f ) );
    plane->AddActorWorldRotation( CamRot );

    //---

    FGuid planeGuid = iSequencer.CreateBinding( *plane, plane->GetActorLabel() );

    iSequencer.OnActorAddedToSequencer().Broadcast( plane, planeGuid );
}

//---

//static
void
ShotSequenceHelpers::SnapCameraToViewport( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    UMovieScene* movieScene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !movieScene )
        return;

    if( movieScene->IsReadOnly() )
    {
        //ShowReadOnlyError();
        return;
    }

    FGuid CameraGuid;
    ACineCameraActor* ExistingCamera = ShotSequenceHelpers::GetCamera( iSequencer, iSequence, iSequenceID, &CameraGuid );
    if( !ExistingCamera )
        return;

    UMovieSceneTrack* track = movieScene->FindTrack( UMovieScene3DTransformTrack::StaticClass(), CameraGuid );
    UMovieScene3DTransformTrack* transform_track = Cast<UMovieScene3DTransformTrack>( track );

    if( !transform_track || !transform_track->GetAllSections().Num() )
        return;

    UMovieSceneSection* section = transform_track->GetAllSections()[0];
    UMovieScene3DTransformSection* transform_section = Cast<UMovieScene3DTransformSection>( section );

    if( !transform_section )
        return;

    //---

    const FScopedTransaction transaction( LOCTEXT( "SnapStoryCameraToViewport", "Snap Storyboard Camera To Viewport" ) );

    cTemporarySwitchInner switch_to( iSequencer, iSequenceID );

    //---

    FVector new_location = GCurrentLevelEditingViewportClient->GetViewLocation();
    FRotator new_rotation = GCurrentLevelEditingViewportClient->GetViewRotation();
    //FVector Scale = iActor->GetActorScale();

    ExistingCamera->SetActorLocation( new_location, false );
    ExistingCamera->SetActorRotation( new_rotation );


//TODO: set all (?) planes ?


    //---

    FFrameNumber sequence_frame = iSequencer.GetLocalTime().Time.GetFrame();

    transform_section->Modify();

    TArrayView<FMovieSceneFloatChannel*> FloatChannels = transform_section->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>();

    //if( FloatChannels[0]->GetNumKeys() == 1 &&
    //    FloatChannels[1]->GetNumKeys() == 1 &&
    //    FloatChannels[2]->GetNumKeys() == 1 &&
    //    FloatChannels[3]->GetNumKeys() == 1 &&
    //    FloatChannels[4]->GetNumKeys() == 1 &&
    //    FloatChannels[5]->GetNumKeys() == 1 )
    //{
    //    AddKeyToChannel( FloatChannels[0], FloatChannels[0]->GetTimes()[0], ExistingCamera->GetActorLocation().X, iSequencer->GetKeyInterpolation() );
    //    AddKeyToChannel( FloatChannels[1], FloatChannels[1]->GetTimes()[0], ExistingCamera->GetActorLocation().Y, iSequencer->GetKeyInterpolation() );
    //    AddKeyToChannel( FloatChannels[2], FloatChannels[2]->GetTimes()[0], ExistingCamera->GetActorLocation().Z, iSequencer->GetKeyInterpolation() );

    //    AddKeyToChannel( FloatChannels[3], FloatChannels[3]->GetTimes()[0], ExistingCamera->GetActorRotation().Euler().X, iSequencer->GetKeyInterpolation() );
    //    AddKeyToChannel( FloatChannels[4], FloatChannels[4]->GetTimes()[0], ExistingCamera->GetActorRotation().Euler().Y, iSequencer->GetKeyInterpolation() );
    //    AddKeyToChannel( FloatChannels[5], FloatChannels[5]->GetTimes()[0], ExistingCamera->GetActorRotation().Euler().Z, iSequencer->GetKeyInterpolation() );
    //}
    //else
    {
        AddKeyToChannel( FloatChannels[0], sequence_frame, ExistingCamera->GetActorLocation().X, iSequencer.GetKeyInterpolation() );
        AddKeyToChannel( FloatChannels[1], sequence_frame, ExistingCamera->GetActorLocation().Y, iSequencer.GetKeyInterpolation() );
        AddKeyToChannel( FloatChannels[2], sequence_frame, ExistingCamera->GetActorLocation().Z, iSequencer.GetKeyInterpolation() );

        AddKeyToChannel( FloatChannels[3], sequence_frame, ExistingCamera->GetActorRotation().Euler().X, iSequencer.GetKeyInterpolation() );
        AddKeyToChannel( FloatChannels[4], sequence_frame, ExistingCamera->GetActorRotation().Euler().Y, iSequencer.GetKeyInterpolation() );
        AddKeyToChannel( FloatChannels[5], sequence_frame, ExistingCamera->GetActorRotation().Euler().Z, iSequencer.GetKeyInterpolation() );
    }

    //AddKeyToChannel( FloatChannels[6], 0, Scale.X, iSequencer->GetKeyInterpolation() );
    //AddKeyToChannel( FloatChannels[7], 0, Scale.Y, iSequencer->GetKeyInterpolation() );
    //AddKeyToChannel( FloatChannels[8], 0, Scale.Z, iSequencer->GetKeyInterpolation() );

    //FloatChannels[0]->SetDefault( new_location.X );
    //FloatChannels[1]->SetDefault( new_location.Y );
    //FloatChannels[2]->SetDefault( new_location.Z );

    //FloatChannels[3]->SetDefault( new_rotation.Euler().X );
    //FloatChannels[4]->SetDefault( new_rotation.Euler().Y );
    //FloatChannels[5]->SetDefault( new_rotation.Euler().Z );

    ////FloatChannels[6]->SetDefault( Scale.X );
    ////FloatChannels[7]->SetDefault( Scale.Y );
    ////FloatChannels[8]->SetDefault( Scale.Z );


//TODO: set all (?) key planes ?


    //---
    // From FSequencer::NewCameraAdded( CameraGuid, NewCamera )

    //iSequencer->SetPerspectiveViewportCameraCutEnabled( false );

    // Lock the viewport to this camera
    if( ExistingCamera && ExistingCamera->GetLevel() )
    {
        //GCurrentLevelEditingViewportClient->SetMatineeActorLock( nullptr );
        //GCurrentLevelEditingViewportClient->SetActorLock( ExistingCamera );
        //GCurrentLevelEditingViewportClient->bLockedCameraView = true;
        //GCurrentLevelEditingViewportClient->UpdateViewForLockedActor();
        GCurrentLevelEditingViewportClient->Invalidate();
    }
    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

//---

//static
void
ShotSequenceHelpers::CreatePlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( iSequencer, iSequence, iSequenceID, &camera_guid );

    if( !camera )
        return;

    //---

    const FScopedTransaction transaction( LOCTEXT( "CreateStoryPlaneHere", "Create Storyboard Plane Here" ) );

    cTemporarySwitchInner switch_to( iSequencer, iSequenceID );

    //---

    ShotSequenceHelpers::SpawnAndBindPlane( iSequencer, iSequence, camera_guid, camera, iSequencer.GetLocalTime().Time.GetFrame() );

    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
}

#undef LOCTEXT_NAMESPACE
