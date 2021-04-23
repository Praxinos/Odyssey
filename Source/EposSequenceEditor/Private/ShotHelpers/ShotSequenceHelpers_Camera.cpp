// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "ShotSequenceHelpers.h"

#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Factories/Texture2dFactoryNew.h"
#include "ISequencer.h"
#include "KeyframeTrackEditor.h"
#include "LevelEditorActions.h"
#include "LevelEditorViewport.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieScene.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "MovieSceneToolHelpers.h"
#include "ObjectEditorUtils.h"
#include "Sections/MovieScenePrimitiveMaterialSection.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Tracks/MovieSceneCinematicShotTrack.h"
#include "Tracks/MovieScenePrimitiveMaterialTrack.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "Settings/EposSequenceEditorSettings.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"

#define LOCTEXT_NAMESPACE "ShotSequenceHelpers_Camera"

BoardSequenceHelpers::FInnerSequenceResult
BoardSequenceHelpers::GetInnerSequence( ISequencer& iSequencer, const FFrameNumber& iFrameNumber )
{
    FInnerSequenceResult result = { nullptr, MovieSceneSequenceID::Invalid, 0 };

    UMovieSceneSequence* sequence = iSequencer.GetFocusedMovieSceneSequence();
    UMovieScene* moviescene = sequence ? sequence->GetMovieScene() : nullptr;
    UMovieSceneCinematicBoardTrack* board_track = moviescene ? moviescene->FindMasterTrack<UMovieSceneCinematicBoardTrack>() : nullptr;
    UMovieSceneSection* section = moviescene ? MovieSceneHelpers::FindSectionAtTime( board_track->GetAllSections(), iFrameNumber ) : nullptr;
    UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );

    result.mInnerSequence = subsection ? subsection->GetSequence() : nullptr;
    //result.mInnerSequenceId = subsection ? subsection->GetSequenceID() : FMovieSceneSequenceID();

    //---

    if( subsection )
    {
        const FMovieSceneSequenceID             thisSequenceID = iSequencer.GetFocusedTemplateID();
        const FMovieSceneSequenceID             targetSequenceID = subsection->GetSequenceID();
        const FMovieSceneSequenceHierarchy*     hierarchy = iSequencer.GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( iSequencer.GetEvaluationTemplate().GetCompiledDataID() );

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

        const FMovieSceneSubSequenceData* subData = hierarchy->FindSubData( result.mInnerSequenceId );
        result.mInnerTime = iFrameNumber * subsection->OuterToInnerTransform();
    }

    return result;
}

//static
bool
BoardSequenceHelpers::CanCreateCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    FInnerSequenceResult result = GetInnerSequence( *iSequencer, iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    return !ShotSequenceHelpers::GetCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
ACineCameraActor*
BoardSequenceHelpers::GetCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber, FGuid* oGuid )
{
    FInnerSequenceResult result = GetInnerSequence( *iSequencer, iFrameNumber );
    if( !result.mInnerSequence )
        return nullptr;

    return ShotSequenceHelpers::GetCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, oGuid );
}

//---

//static
ACineCameraActor*
ShotSequenceHelpers::GetCamera( ISequencer* iSequencer, FGuid* oGuid )
{
    return GetCamera( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), oGuid );
}

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
//---
//---

ShotSequenceHelpers::cTemporarySwitchInner::cTemporarySwitchInner( ISequencer& iSequencer, FMovieSceneSequenceIDRef iInnerID )
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

    FFrameRate display_rate = mSequencer.GetFocusedDisplayRate();
    FFrameRate tick_resolution = mSequencer.GetFocusedTickResolution();
    mSequencer.SetGlobalTime( ConvertFrameTime( mOriginalGlobalTime, display_rate, tick_resolution ) );
}

//---
//---
//---

//static
void
BoardSequenceHelpers::CreateCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    FInnerSequenceResult result = GetInnerSequence( *iSequencer, iFrameNumber );
    if( !result.mInnerSequence )
        return;

    return ShotSequenceHelpers::CreateCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
void
ShotSequenceHelpers::CreateCamera( ISequencer* iSequencer )
{
    CreateCamera( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID() );
}

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
    ACineCameraActor* camera = ShotSequenceHelpers::SpawnAndBindCamera( iSequencer, iSequence, &camera_guid );

    ShotSequenceHelpers::CameraAdded( iSequencer, iSequence, camera_guid, camera, iSequencer.GetLocalTime().Time.FloorToFrame() );

    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
}

//static
ACineCameraActor*
ShotSequenceHelpers::SpawnAndBindCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid* oGuid ) // From FSequencer::CreateCamera()
{
    UWorld* World = GCurrentLevelEditingViewportClient->GetWorld();

    // Set new camera to match viewport
    FActorSpawnParameters SpawnParams;
    ACineCameraActor* NewCamera = World->SpawnActor<ACineCameraActor>( SpawnParams );
    if( !NewCamera )
        return nullptr;

    NewCamera->SetFolderPath( *FPaths::GetBaseFilename( iSequencer.GetRootMovieSceneSequence()->GetPathName() ) );
    FActorLabelUtilities::SetActorLabelUnique( NewCamera, TEXT( "Camera_1" ) ); // The shot name is displayed in another column in the world outliner

    NewCamera->SetActorLocation( GCurrentLevelEditingViewportClient->GetViewLocation(), false );
    NewCamera->SetActorRotation( GCurrentLevelEditingViewportClient->GetViewRotation() );
    //pNewCamera->CameraComponent->FieldOfView = ViewportClient->ViewFOV; //@todo set the focal length from this field of view
    const UEposSequenceEditorSettings* settings = GetDefault<UEposSequenceEditorSettings>();

    // https://udn.unrealengine.com/s/question/0D54z00006uhl34CAA/plugin-cuproperty-how-to-change-uproperty-and-trigger-prepostedit-

    //UCineCameraComponent* CameraComponent = NewCamera->GetCineCameraComponent();
    //if( CameraComponent != NULL )
    //{
    //    {
    //        FProperty* ChangedProperty = FindFProperty<FProperty>( UCineCameraComponent::StaticClass(), "LensSettings" );
    //        CameraComponent->PreEditChange( ChangedProperty );

    //        CameraComponent->LensSettings = settings->CameraSettings.LensSettings;

    //        FPropertyChangedEvent PropertyChangedEvent( ChangedProperty );
    //        //CameraComponent->PostEditChangeProperty( PropertyChangedEvent );
    //        FEditPropertyChain PropertyChain;
    //        PropertyChain.AddHead( ChangedProperty );
    //        FPropertyChangedChainEvent PropertyChainEvent( PropertyChain, PropertyChangedEvent );
    //        CameraComponent->PostEditChangeChainProperty( PropertyChainEvent );
    //    }
    //    ... do it for all properties to change
    //}

    NewCamera->GetCineCameraComponent()->LensSettings = settings->CameraSettings.LensSettings;
    NewCamera->GetCineCameraComponent()->Filmback = settings->CameraSettings.Filmback;
    NewCamera->GetCineCameraComponent()->CurrentAperture = settings->CameraSettings.CurrentAperture;
    NewCamera->GetCineCameraComponent()->SetCurrentFocalLength( settings->CameraSettings.CurrentFocalLength ); // Use setter to trigger RecalcDerivedData(), so no need to call Pre/PostChange() and its huge syntax

    FGuid CameraGuid = iSequencer.CreateBinding( *NewCamera, NewCamera->GetActorLabel() );
    if( !CameraGuid.IsValid() )
        return nullptr;

    iSequencer.OnActorAddedToSequencer().Broadcast( NewCamera, CameraGuid );

    //---
    // From FSequencer::NewCameraAdded( CameraGuid, NewCamera )

    // an option ?

    iSequencer.SetPerspectiveViewportCameraCutEnabled( false );

    // Lock the viewport to this camera
    if( NewCamera && NewCamera->GetLevel() )
    {
        GCurrentLevelEditingViewportClient->SetMatineeActorLock( nullptr );
        GCurrentLevelEditingViewportClient->SetActorLock( NewCamera );
        GCurrentLevelEditingViewportClient->bLockedCameraView = true;
        GCurrentLevelEditingViewportClient->UpdateViewForLockedActor();
        GCurrentLevelEditingViewportClient->Invalidate();
    }

    *oGuid = CameraGuid;
    return NewCamera;
}

//static
void
ShotSequenceHelpers::CameraAdded( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid CameraGuid, ACineCameraActor* iCamera, FFrameNumber FrameNumber)
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

//---
//---
//---

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
CreateMaterialInstanceConstantAsset( UMovieSceneSequence* iSequence, UMovieSceneSequence* iRootSequence, FString& oPackageName, FString& oAssetName )
{
    UMaterial* material_root = LoadObject<UMaterial>( nullptr, TEXT( "/Epos/M_Plane_Root.M_Plane_Root" ) );
    if( !material_root )
        return nullptr;

    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( TEXT( "AssetRegistry" ) );

    UMaterialInstanceConstant* material_master = nullptr;

    UObject* root_asset = iRootSequence->GetMovieScene()->GetOuter();
    UPackage* root_package = root_asset->GetPackage();
    FString root_package_name = root_package->GetName() / "MI_Plane_Master"; // ie. /Game/MyStoryboard2

    TArray<FAssetData> asset_datas;
    assetRegistryModule.Get().GetAssetsByPackageName( *root_package_name, asset_datas );
    if( !asset_datas.Num() )
    {
        FString package_name;
        FString asset_name;
        assetToolsModule.Get().CreateUniqueAssetName( root_package_name, "", package_name, asset_name );

        UMaterialInstanceConstantFactoryNew* factory = NewObject<UMaterialInstanceConstantFactoryNew>();
        factory->InitialParent = material_root;

        FString package_path = FPackageName::GetLongPackagePath( package_name );
        UObject* new_object = assetToolsModule.Get().CreateAsset( asset_name, package_path, UMaterialInstanceConstant::StaticClass(), factory );

        material_master = Cast<UMaterialInstanceConstant>( new_object );

        //---

        FStaticParameterSet static_params;
        material_master->GetStaticParameterValues( static_params );
        for( auto& parameter : static_params.StaticSwitchParameters )
            parameter.bOverride = true;
        material_master->UpdateStaticPermutation( static_params );

        // Needed to compute all cases during creation, to have all shaders computed
        for( int combination = 0; combination < FMath::Pow( 2, static_params.StaticSwitchParameters.Num() ); combination++ )
        {
            //UE_LOG( LogTemp, Warning, TEXT( "combination: %d" ), combination );
            for( int i = 0; i < static_params.StaticSwitchParameters.Num(); i++ )
            {
                static_params.StaticSwitchParameters[i].Value = combination & ( 1 << i );
                //UE_LOG( LogTemp, Warning, TEXT( "i: %d - value: %d" ), i, static_params.StaticSwitchParameters[i].Value );
                material_master->UpdateStaticPermutation( static_params );
            }
        }
    }
    else
    {
        material_master = Cast<UMaterialInstanceConstant>( asset_datas[0].GetAsset() );
    }

    if( !material_master )
        return nullptr;

    //---

    UPackage* package = iSequence->GetPackage();
    FString package_name = package->GetName(); // ie. /Game/MyStoryboard2/shot0001_01

    assetToolsModule.Get().CreateUniqueAssetName( package_name, "_MI_01", oPackageName, oAssetName );

    UMaterialInstanceConstantFactoryNew* factory = NewObject<UMaterialInstanceConstantFactoryNew>();
    factory->InitialParent = material_master;

    FString package_path = FPackageName::GetLongPackagePath( oPackageName );
    UObject* new_object = assetToolsModule.Get().CreateAsset( oAssetName, package_path, UMaterialInstanceConstant::StaticClass(), factory );

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
    Module.Get().CreateUniqueAssetName( package_name, "_T_01", oPackageName, oAssetName );

    FString package_path = FPackageName::GetLongPackagePath( oPackageName );
    UObject* new_object = Module.Get().DuplicateAsset( oAssetName, package_path, texture_transparent );

    return Cast<UTexture2D>( new_object );
}

/*
static
int
SplitActorLabel( const FString iPrefix, FString& ioLabel, int32& oIndex )
{
    ioLabel.RemoveFromStart( iPrefix );

    // Look at the label and see if it begins in a number and separate them
    FString index;
    const TArray<TCHAR>& LabelCharArray = ioLabel.GetCharArray();
    for( int32 CharIdx = 0; CharIdx < LabelCharArray.Num(); CharIdx++ )
    {
        if( !FChar::IsDigit( LabelCharArray[CharIdx] ) )
            break;

        index += LabelCharArray[CharIdx];
    }

    if( !index.Len() )
        return 0;

    ioLabel.RemoveFromStart( index );
    oIndex = FCString::Atoi( *index );

    return index.Len();
}

static
void
SetPlaneLabelUnique( AActor* Actor, const FString& NewActorLabel )
{
    check( Actor );

    FString suffix = NewActorLabel;
    FString ModifiedActorLabel = NewActorLabel;
    int32   index = 0;

    FCachedActorLabels ActorLabels;
    TSet<AActor*> IgnoreActors;
    IgnoreActors.Add( Actor );
    ActorLabels.Populate( Actor->GetWorld(), IgnoreActors );

    if( ActorLabels.Contains( ModifiedActorLabel ) )
    {
        // See if the current label begins in a number, and try to create a new label based on that
        int index_length = SplitActorLabel( TEXT( "Plane_" ), suffix, index );
        if( index_length == 0 )
        {
            // If there wasn't a number on there, append a number, starting from 2 (1 before incrementing below)
            index = 1;
        }

        // Update the actor label until we find one that doesn't already exist
        while( ActorLabels.Contains( ModifiedActorLabel ) )
        {
            ++index;

            FString format = FString::Format( TEXT( "%0{0}d" ), { index_length } );
            TCHAR format2[10] = { 0 };
            for( int i = 0; i < FMath::Min( format.Len(), 10 ); i++ )
                format2[i] = format[i];
            FString index_string = FString::Printf( format2, index );
            ModifiedActorLabel = TEXT( "Plane_" ) + index_string + suffix;
        }
    }

    Actor->SetActorLabel( ModifiedActorLabel );
}
*/

//static
void
ShotSequenceHelpers::SpawnAndBindPlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid iCameraGuid, ACineCameraActor* iCamera, FFrameNumber iFrameNumber )
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
    UMaterialInstanceConstant* new_material = CreateMaterialInstanceConstantAsset( iSequence, iSequencer.GetRootMovieSceneSequence(), new_material_package_name, new_material_asset_name );
    if( !new_material )
        return;

    FString new_texture_package_name;
    FString new_texture_asset_name;
    UTexture2D* new_texture = CreateTexture2DAsset( iSequence, new_material, new_texture_package_name, new_texture_asset_name );
    if( !new_texture )
        return;

    new_material->SetTextureParameterValueEditorOnly( TEXT( "DrawingTexture" ), new_texture );

    //---

    UWorld* World = GCurrentLevelEditingViewportClient->GetWorld();

    FActorSpawnParameters SpawnParams;
    AStaticMeshActor* plane = World->SpawnActor<AStaticMeshActor>( SpawnParams );

    plane->GetStaticMeshComponent()->SetStaticMesh( plane_mesh );
    plane->GetStaticMeshComponent()->SetMaterial( 0, new_material );
    plane->SetMobility( EComponentMobility::Movable );
    plane->SetActorHiddenInGame( true );

    plane->SetFolderPath( *FPaths::GetBaseFilename( iSequencer.GetRootMovieSceneSequence()->GetPathName() ) );
    FActorLabelUtilities::SetActorLabelUnique( plane, TEXT("Plane_1") ); // The shot name is displayed in another column in the world outliner
    //SetPlaneLabelUnique( plane, TEXT("Plane_01_") + iSequence->GetDisplayName().ToString() );

    //---

    plane->SetActorScale3D( plane_scale );
    plane->SetActorLocation( plane_location );
    plane->SetActorRotation( FRotator( 0.f, 90.f, 90.f ) );
    plane->AddActorWorldRotation( CamRot );

    //plane->AttachToActor( iCamera, FAttachmentTransformRules::KeepRelativeTransform );
    GEditor->ParentActors( iCamera, plane, NAME_None );

    //---

    FGuid planeGuid = iSequencer.CreateBinding( *plane, plane->GetActorLabel() );

    iSequencer.OnActorAddedToSequencer().Broadcast( plane, planeGuid );
}

//---

//static
void
BoardSequenceHelpers::CreatePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    FInnerSequenceResult result = GetInnerSequence( *iSequencer, iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceHelpers::CreatePlane( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iFrameNumber );
}

//static
void
ShotSequenceHelpers::CreatePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    CreatePlane( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
}

//static
void
ShotSequenceHelpers::CreatePlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( iSequencer, iSequence, iSequenceID, &camera_guid );

    if( !camera )
        return;

    //---

    const FScopedTransaction transaction( LOCTEXT( "CreateStoryPlaneHere", "Create Storyboard Plane Here" ) );

    cTemporarySwitchInner switch_to( iSequencer, iSequenceID );

    //---

    ShotSequenceHelpers::SpawnAndBindPlane( iSequencer, iSequence, camera_guid, camera, iFrameNumber );

    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
}

//---
//---
//---

static
AStaticMeshActor*
GetPlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid* oPlaneBinding )
{
    UMovieScene* movieScene = iSequence->GetMovieScene();
    if( !movieScene )
        return nullptr;

    AStaticMeshActor* plane = nullptr;
    for( int i = 0; i < movieScene->GetPossessableCount(); i++ )
    {
        FMovieScenePossessable possessable = movieScene->GetPossessable( i );

        for( TWeakObjectPtr<> WeakObject : iSequencer.FindBoundObjects( possessable.GetGuid(), iSequenceID ) )
        {
            plane = Cast<AStaticMeshActor>( WeakObject.Get() );

            if( plane )
            {
                if( oPlaneBinding )
                    *oPlaneBinding = possessable.GetGuid();

                return plane;
            }
        }
    }

    return nullptr;
}

//---

//static
bool
BoardSequenceHelpers::CanCreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    FInnerSequenceResult result = GetInnerSequence( *iSequencer, iFrameNumber );
    if( !result.mInnerSequence )
        return false;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return false;

    return ShotSequenceHelpers::CanCreateDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iFrameNumber );
}

//static
bool
ShotSequenceHelpers::CanCreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    return CanCreateDrawing( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
}

//static
bool
ShotSequenceHelpers::CanCreateDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    UMovieScene* moviescene = iSequence->GetMovieScene();
    if( !moviescene )
        return false;

    FGuid plane_binding;
    AStaticMeshActor* plane = GetPlane( iSequencer, iSequence, iSequenceID, &plane_binding );
    if( !plane )
        return false;

    FGuid plane_component = iSequencer.FindObjectId( *plane->GetRootComponent(), iSequenceID );

    UMovieScenePrimitiveMaterialTrack* track = moviescene->FindTrack<UMovieScenePrimitiveMaterialTrack>( plane_component );
    if( !track )
        return false;

    UMovieScenePrimitiveMaterialSection* section = Cast<UMovieScenePrimitiveMaterialSection>( MovieSceneHelpers::FindSectionAtTime( track->GetAllSections(), iFrameNumber ) );
    if( !section )
        return false;

    TArrayView<FMovieSceneObjectPathChannel*> channels = section->GetChannelProxy().GetChannels<FMovieSceneObjectPathChannel>();
    check( channels.Num() == 1 );
    int32 key_index = channels[0]->GetData().FindKey( iFrameNumber );
    return key_index == INDEX_NONE;
}

//---

//static
void
BoardSequenceHelpers::CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    FInnerSequenceResult result = GetInnerSequence( *iSequencer, iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceHelpers::CreateDrawing( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iFrameNumber );
}

//static
void
ShotSequenceHelpers::CreateDrawing( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    CreateDrawing( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
}

//static
void
ShotSequenceHelpers::CreateDrawing( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    UMovieScene* moviescene = iSequence->GetMovieScene();
    if( !moviescene )
        return;

    FGuid plane_binding;
    AStaticMeshActor* plane = GetPlane( iSequencer, iSequence, iSequenceID, &plane_binding );
    if( !plane )
        return;

    FGuid plane_component = iSequencer.FindObjectId( *plane->GetRootComponent(), iSequenceID );

    UMovieScenePrimitiveMaterialTrack* track = moviescene->FindTrack<UMovieScenePrimitiveMaterialTrack>( plane_component );
    if( !track )
        return;

    UMovieScenePrimitiveMaterialSection* section = Cast<UMovieScenePrimitiveMaterialSection>( MovieSceneHelpers::FindSectionAtTime( track->GetAllSections(), iFrameNumber ) );
    if( !section )
        return;

    TArrayView<FMovieSceneObjectPathChannel*> channels = section->GetChannelProxy().GetChannels<FMovieSceneObjectPathChannel>();
    check( channels.Num() == 1 );
    int32 key_index = channels[0]->GetData().FindKey( iFrameNumber );
    if( key_index != INDEX_NONE )
        return;

    //---

    const FScopedTransaction transaction( LOCTEXT( "CreateDrawing", "Create a new drawing" ) );

    section->Modify();

    //---

    FString new_material_package_name;
    FString new_material_asset_name;
    UMaterialInstanceConstant* new_material = CreateMaterialInstanceConstantAsset( iSequence, iSequencer.GetRootMovieSceneSequence(), new_material_package_name, new_material_asset_name );
    if( !new_material )
        return;

    FString new_texture_package_name;
    FString new_texture_asset_name;
    UTexture2D* new_texture = CreateTexture2DAsset( iSequence, new_material, new_texture_package_name, new_texture_asset_name );
    if( !new_texture )
        return;

    new_material->SetTextureParameterValueEditorOnly( TEXT( "DrawingTexture" ), new_texture );

    //---

    FMovieSceneObjectPathChannelKeyValue material_objectpath( new_material );

    UE::MovieScene::AddKeyToChannel( channels[0], iFrameNumber, material_objectpath, iSequencer.GetKeyInterpolation() );

    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

//---
//---
//---

//static
void
BoardSequenceHelpers::SnapCameraToViewport( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    FInnerSequenceResult result = GetInnerSequence( *iSequencer, iFrameNumber );
    if( !result.mInnerSequence )
        return;

    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, &camera_guid );
    if( !camera )
        return;

    ShotSequenceHelpers::SnapCameraToViewport( *iSequencer, result.mInnerSequence, camera, camera_guid, result.mInnerTime.GetFrame() );
}

//static
void
ShotSequenceHelpers::SnapCameraToViewport( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    FMovieSceneSequenceID sequence_id = iSequencer->GetFocusedTemplateID();
    if( !sequence )
        return;

    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, sequence, sequence_id, &camera_guid );
    if( !camera )
        return;

    ShotSequenceHelpers::SnapCameraToViewport( *iSequencer, sequence, camera, camera_guid, iFrameNumber );
}

//static
void
ShotSequenceHelpers::SnapCameraToViewport( ISequencer& iSequencer, UMovieSceneSequence* iSequence, ACineCameraActor* ioCamera, FGuid iCameraGuid, FFrameNumber iFrameNumber )
{
    UMovieScene* movieScene = iSequence->GetMovieScene();
    if( !movieScene || movieScene->IsReadOnly() )
    {
        //ShowReadOnlyError();
        return;
    }

    UMovieSceneTrack* track = movieScene->FindTrack<UMovieScene3DTransformTrack>( iCameraGuid );
    UMovieSceneSection* section = track ? MovieSceneHelpers::FindSectionAtTime( track->GetAllSections(), iFrameNumber ) : nullptr;
    UMovieScene3DTransformSection* transform_section = Cast<UMovieScene3DTransformSection>( section );

    if( !transform_section )
        return;

    //---

    const FScopedTransaction transaction( LOCTEXT( "SnapStoryCameraToViewport", "Snap Storyboard Camera To Viewport" ) );

    //---

    FVector new_location = GCurrentLevelEditingViewportClient->GetViewLocation();
    FRotator new_rotation = GCurrentLevelEditingViewportClient->GetViewRotation();
    //FVector Scale = iActor->GetActorScale();

    ioCamera->SetActorLocation( new_location, false );
    ioCamera->SetActorRotation( new_rotation );

//TODO: set all (?) planes ?

    //---

    transform_section->Modify();

    FMovieSceneChannelProxy& proxy = section->GetChannelProxy();
    TArrayView<FMovieSceneFloatChannel*> FloatChannels = proxy.GetChannels<FMovieSceneFloatChannel>();

//TODO: maybe use the same object as StopPilotingCamera() ???

    AddKeyToChannel( FloatChannels[0], iFrameNumber, ioCamera->GetActorLocation().X, iSequencer.GetKeyInterpolation() );
    AddKeyToChannel( FloatChannels[1], iFrameNumber, ioCamera->GetActorLocation().Y, iSequencer.GetKeyInterpolation() );
    AddKeyToChannel( FloatChannels[2], iFrameNumber, ioCamera->GetActorLocation().Z, iSequencer.GetKeyInterpolation() );

    AddKeyToChannel( FloatChannels[3], iFrameNumber, ioCamera->GetActorRotation().Euler().X, iSequencer.GetKeyInterpolation() );
    AddKeyToChannel( FloatChannels[4], iFrameNumber, ioCamera->GetActorRotation().Euler().Y, iSequencer.GetKeyInterpolation() );
    AddKeyToChannel( FloatChannels[5], iFrameNumber, ioCamera->GetActorRotation().Euler().Z, iSequencer.GetKeyInterpolation() );

    //AddKeyToChannel( FloatChannels[6], 0, Scale.X, iSequencer->GetKeyInterpolation() );
    //AddKeyToChannel( FloatChannels[7], 0, Scale.Y, iSequencer->GetKeyInterpolation() );
    //AddKeyToChannel( FloatChannels[8], 0, Scale.Z, iSequencer->GetKeyInterpolation() );

    if( FloatChannels[0]->GetNumKeys() <= 1 )
        FloatChannels[0]->SetDefault( new_location.X );
    if( FloatChannels[1]->GetNumKeys() <= 1 )
        FloatChannels[1]->SetDefault( new_location.Y );
    if( FloatChannels[2]->GetNumKeys() <= 1 )
        FloatChannels[2]->SetDefault( new_location.Z );

    if( FloatChannels[3]->GetNumKeys() <= 1 )
        FloatChannels[3]->SetDefault( new_rotation.Euler().X );
    if( FloatChannels[4]->GetNumKeys() <= 1 )
        FloatChannels[4]->SetDefault( new_rotation.Euler().Y );
    if( FloatChannels[5]->GetNumKeys() <= 1 )
        FloatChannels[5]->SetDefault( new_rotation.Euler().Z );

    //FloatChannels[6]->SetDefault( Scale.X );
    //FloatChannels[7]->SetDefault( Scale.Y );
    //FloatChannels[8]->SetDefault( Scale.Z );

//TODO: set all (?) key planes ?

    //---
    // From FSequencer::NewCameraAdded( CameraGuid, NewCamera )

    //iSequencer->SetPerspectiveViewportCameraCutEnabled( false );

    // Lock the viewport to this camera
    if( ioCamera && ioCamera->GetLevel() )
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
//---
//---

static
float
UnwindChannel( const float& OldValue, float NewValue )
{
    while( NewValue - OldValue > 180.0f )
    {
        NewValue -= 360.0f;
    }
    while( NewValue - OldValue < -180.0f )
    {
        NewValue += 360.0f;
    }
    return NewValue;
}
static
FRotator
UnwindRotator( const FRotator& InOld, const FRotator& InNew )
{
    FRotator Result;
    Result.Pitch = UnwindChannel( InOld.Pitch, InNew.Pitch );
    Result.Yaw = UnwindChannel( InOld.Yaw, InNew.Yaw );
    Result.Roll = UnwindChannel( InOld.Roll, InNew.Roll );
    return Result;
}

// From ...\UE_4.26\Engine\Source\Editor\MovieSceneTools\Private\TrackEditors\TransformTrackEditor.cpp
static
void
GetTransformKeys( ISequencer& iSequencer, const TOptional<FTransformData>& LastTransform, const FTransformData& CurrentTransform, EMovieSceneTransformChannel ChannelsToKey, UObject* Object, UMovieSceneSection* Section, FGeneratedTrackKeys& OutGeneratedKeys )
{
    using namespace UE::MovieScene;

    bool bLastVectorIsValid = LastTransform.IsSet();

    // If key all is enabled, for a key on all the channels
    if( iSequencer.GetKeyGroupMode() == EKeyGroupMode::KeyAll )
    {
        bLastVectorIsValid = false;
        ChannelsToKey = EMovieSceneTransformChannel::All;
    }

    //FBuiltInComponentTypes* BuiltInComponents = FBuiltInComponentTypes::Get();

    //FTransformData RecomposedTransform = RecomposeTransform( CurrentTransform, Object, Section );

    // Set translation keys/defaults
    {
        bool bKeyX = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::TranslationX );
        bool bKeyY = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::TranslationY );
        bool bKeyZ = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::TranslationZ );

        if( bLastVectorIsValid )
        {
            bKeyX &= !FMath::IsNearlyEqual( LastTransform->Translation.X, CurrentTransform.Translation.X );
            bKeyY &= !FMath::IsNearlyEqual( LastTransform->Translation.Y, CurrentTransform.Translation.Y );
            bKeyZ &= !FMath::IsNearlyEqual( LastTransform->Translation.Z, CurrentTransform.Translation.Z );
        }

        if( iSequencer.GetKeyGroupMode() == EKeyGroupMode::KeyGroup && ( bKeyX || bKeyY || bKeyZ ) )
        {
            bKeyX = bKeyY = bKeyZ = true;
        }

        FVector KeyVector = CurrentTransform.Translation;
        //FVector KeyVector = RecomposedTransform.Translation;

        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneFloatChannel>( 0, KeyVector.X, bKeyX ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneFloatChannel>( 1, KeyVector.Y, bKeyY ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneFloatChannel>( 2, KeyVector.Z, bKeyZ ) );
    }

    // Set rotation keys/defaults
    {
        bool bKeyX = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::RotationX );
        bool bKeyY = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::RotationY );
        bool bKeyZ = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::RotationZ );

        FRotator KeyRotator = CurrentTransform.Rotation;
        if( bLastVectorIsValid )
        {
            KeyRotator = UnwindRotator( LastTransform->Rotation, CurrentTransform.Rotation );

            bKeyX &= !FMath::IsNearlyEqual( LastTransform->Rotation.Roll, KeyRotator.Roll );
            bKeyY &= !FMath::IsNearlyEqual( LastTransform->Rotation.Pitch, KeyRotator.Pitch );
            bKeyZ &= !FMath::IsNearlyEqual( LastTransform->Rotation.Yaw, KeyRotator.Yaw );
        }

        if( iSequencer.GetKeyGroupMode() == EKeyGroupMode::KeyGroup && ( bKeyX || bKeyY || bKeyZ ) )
        {
            bKeyX = bKeyY = bKeyZ = true;
        }

        // Do we need to unwind re-composed rotations?
        //KeyRotator = UnwindRotator( CurrentTransform.Rotation, RecomposedTransform.Rotation );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneFloatChannel>( 3, KeyRotator.Roll, bKeyX ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneFloatChannel>( 4, KeyRotator.Pitch, bKeyY ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneFloatChannel>( 5, KeyRotator.Yaw, bKeyZ ) );

    }

    // Set scale keys/defaults
    {
        bool bKeyX = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::ScaleX );
        bool bKeyY = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::ScaleY );
        bool bKeyZ = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::ScaleZ );

        if( bLastVectorIsValid )
        {
            bKeyX &= !FMath::IsNearlyEqual( LastTransform->Scale.X, CurrentTransform.Scale.X );
            bKeyY &= !FMath::IsNearlyEqual( LastTransform->Scale.Y, CurrentTransform.Scale.Y );
            bKeyZ &= !FMath::IsNearlyEqual( LastTransform->Scale.Z, CurrentTransform.Scale.Z );
        }

        if( iSequencer.GetKeyGroupMode() == EKeyGroupMode::KeyGroup && ( bKeyX || bKeyY || bKeyZ ) )
        {
            bKeyX = bKeyY = bKeyZ = true;
        }

        FVector KeyVector = CurrentTransform.Scale;
        //FVector KeyVector = RecomposedTransform.Scale;
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneFloatChannel>( 6, KeyVector.X, bKeyX ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneFloatChannel>( 7, KeyVector.Y, bKeyY ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneFloatChannel>( 8, KeyVector.Z, bKeyZ ) );
    }
}

// From ...\UE_4.26\Engine\Source\Editor\MovieSceneTools\Public\KeyframeTrackEditor.h
static
void
AddKeysToSection( ISequencer& iSequencer, UMovieSceneSection* Section, FFrameNumber KeyTime, const FGeneratedTrackKeys& Keys, ESequencerKeyMode KeyMode )
{
    EAutoChangeMode AutoChangeMode = iSequencer.GetAutoChangeMode();

    FMovieSceneChannelProxy& Proxy = Section->GetChannelProxy();

    const bool bSetDefaults = iSequencer.GetAutoSetTrackDefaults();

    if( KeyMode != ESequencerKeyMode::AutoKey || AutoChangeMode == EAutoChangeMode::AutoKey || AutoChangeMode == EAutoChangeMode::All )
    {
        EMovieSceneKeyInterpolation InterpolationMode = iSequencer.GetKeyInterpolation();

        const bool bKeyEvenIfUnchanged =
            KeyMode == ESequencerKeyMode::ManualKeyForced ||
            iSequencer.GetKeyGroupMode() == EKeyGroupMode::KeyAll ||
            iSequencer.GetKeyGroupMode() == EKeyGroupMode::KeyGroup;

        const bool bKeyEvenIfEmpty =
            ( KeyMode == ESequencerKeyMode::AutoKey && AutoChangeMode == EAutoChangeMode::All ) ||
            KeyMode == ESequencerKeyMode::ManualKeyForced;

        for( const FMovieSceneChannelValueSetter& GeneratedKey : Keys )
        {
            GeneratedKey->Apply( Section, Proxy, KeyTime, InterpolationMode, bKeyEvenIfUnchanged, bKeyEvenIfEmpty );
        }
    }

    if( bSetDefaults )
    {
        for( const FMovieSceneChannelValueSetter& GeneratedKey : Keys )
        {
            GeneratedKey->ApplyDefault( Section, Proxy );
        }
    }
}

//static
void
BoardSequenceHelpers::StopPilotingCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber, ACineCameraActor* iCamera, const TOptional<FTransformData>& iPreviousTransform, const FTransformData& iNewTransform )
{
    FInnerSequenceResult result = GetInnerSequence( *iSequencer, iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceHelpers::StopPilotingCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame(), iCamera, iPreviousTransform, iNewTransform );
}

//static
void
ShotSequenceHelpers::StopPilotingCamera( ISequencer* iSequencer, FFrameNumber iFrameNumber, ACineCameraActor* iCamera, const TOptional<FTransformData>& iPreviousTransform, const FTransformData& iNewTransform )
{
    StopPilotingCamera( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, iCamera, iPreviousTransform, iNewTransform );
}

//static
void
ShotSequenceHelpers::StopPilotingCamera( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, ACineCameraActor* iCamera, const TOptional<FTransformData>& iPreviousTransform, const FTransformData& iNewTransform )
{
    const FScopedTransaction transaction( LOCTEXT( "StopPilotingCamera", "Stop Piloting Storyboard Camera" ) );

    FGuid Binding = iSequencer.FindObjectId( *iCamera, iSequenceID );
    if( !Binding.IsValid() )
        return;

    UMovieScene3DTransformTrack* transform_track = iSequence->GetMovieScene()->FindTrack<UMovieScene3DTransformTrack>( Binding );

    //---

    UMovieSceneSection* section = MovieSceneHelpers::FindSectionAtTime( transform_track->GetAllSections(), iFrameNumber );
    FGeneratedTrackKeys generated_keys;
    GetTransformKeys( iSequencer, iPreviousTransform, iNewTransform, EMovieSceneTransformChannel::All, iCamera, section, generated_keys );

    //---

//TODO: set all (?) planes ?

    //---

    AddKeysToSection( iSequencer, section, iFrameNumber, generated_keys, ESequencerKeyMode::AutoKey );

//TODO: set all (?) key planes ?

    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}


#undef LOCTEXT_NAMESPACE
