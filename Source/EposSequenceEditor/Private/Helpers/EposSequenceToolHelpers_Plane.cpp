// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Helpers/EposSequenceToolHelpers.h"

#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
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
#include "Materials/MaterialInstanceConstant.h"
#include "MovieScene.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"

#include "Board/BoardSequence.h"
#include "Board/BoardSequenceHelpers.h"
#include "Shot/ShotSequenceHelpers.h"

#define LOCTEXT_NAMESPACE "EposSequenceToolHelpers_Plane"

//static
FVector
ShotSequenceToolHelpers::ComputePlaneScale( const ACineCameraActor* iCamera, float iDistance ) // From FDrawFrustumSceneProxy::GetDynamicMeshElements()
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

//static
AStaticMeshActor*
ShotSequenceToolHelpers::SpawnPlane( UWorld* iWorld, ACineCameraActor* iCamera, UMaterialInstanceConstant* iMaterial )
{
    FTransform camera_transform = iCamera->GetRootComponent()->GetComponentTransform();

    FVector const CamLocation = camera_transform.GetLocation();
    FVector const CamDir = camera_transform.GetRotation().Vector();
    FRotator const CamRot = camera_transform.Rotator();

    //-

    // Make a function ComputePlaneLocation(...)
    float FocusDistance = 200;
    FVector plane_location = CamLocation + CamDir * FocusDistance;

    FVector plane_scale = ShotSequenceToolHelpers::ComputePlaneScale( iCamera, FocusDistance );

    FRotator plane_rotator = CamRot;

    //---

    FActorSpawnParameters SpawnParams;
    AStaticMeshActor* plane = iWorld->SpawnActor<AStaticMeshActor>( SpawnParams );
    if( !plane )
        return nullptr;

    UStaticMesh* plane_mesh = LoadObject<UStaticMesh>( nullptr, TEXT( "/Epos/S_1_Unit_Plane.S_1_Unit_Plane" ) );
    check( plane_mesh );

    plane->GetStaticMeshComponent()->SetStaticMesh( plane_mesh );
    plane->GetStaticMeshComponent()->SetMaterial( 0, iMaterial );
    plane->SetMobility( EComponentMobility::Movable );
    plane->SetActorHiddenInGame( true );

    plane->SetActorScale3D( plane_scale );
    plane->SetActorLocation( plane_location );
    plane->SetActorRotation( FRotator( 0.f, 90.f, 90.f ) );
    plane->AddActorWorldRotation( plane_rotator );

    //plane->AttachToActor( iCamera, FAttachmentTransformRules::KeepRelativeTransform ); // Done in the editor with GEditor->ParentActors();

    return plane;
}

//static
UMaterialInstanceConstant*
ShotSequenceToolHelpers::CreateMaterialInstanceConstantAsset( UMovieSceneSequence* iSequence, UMovieSceneSequence* iRootSequence, FString& oPackageName, FString& oAssetName )
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

        // For information, as they were replaced by a float parameter to switch between grid or not
        //FStaticParameterSet static_params;
        //material_master->GetStaticParameterValues( static_params );
        //for( auto& parameter : static_params.StaticSwitchParameters )
        //    parameter.bOverride = true;
        //material_master->UpdateStaticPermutation( static_params );

        //// Needed to compute all cases during creation, to have all shaders computed
        //for( int combination = 0; combination < FMath::Pow( 2, static_params.StaticSwitchParameters.Num() ); combination++ )
        //{
        //    //UE_LOG( LogTemp, Warning, TEXT( "combination: %d" ), combination );
        //    for( int i = 0; i < static_params.StaticSwitchParameters.Num(); i++ )
        //    {
        //        static_params.StaticSwitchParameters[i].Value = combination & ( 1 << i );
        //        //UE_LOG( LogTemp, Warning, TEXT( "i: %d - value: %d" ), i, static_params.StaticSwitchParameters[i].Value );
        //        material_master->UpdateStaticPermutation( static_params );
        //    }
        //}
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

//static
UTexture2D*
ShotSequenceToolHelpers::CreateTexture2DAsset( UMovieSceneSequence* iSequence, UMaterialInterface* iMaterial, FString& oPackageName, FString& oAssetName )
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
ShotSequenceToolHelpers::SpawnAndBindPlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid iCameraGuid, ACineCameraActor* iCamera, FFrameNumber iFrameNumber )
{
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

    UWorld* world = GCurrentLevelEditingViewportClient->GetWorld();

    AStaticMeshActor* plane = ShotSequenceToolHelpers::SpawnPlane( world, iCamera, new_material );

    //---

    GEditor->ParentActors( iCamera, plane, NAME_None );

    plane->SetFolderPath( *FPaths::GetBaseFilename( iSequencer.GetRootMovieSceneSequence()->GetPathName() ) );
    FActorLabelUtilities::SetActorLabelUnique( plane, TEXT( "Plane_1" ) ); // The shot name is displayed in another column in the world outliner
    //SetPlaneLabelUnique( plane, TEXT("Plane_01_") + iSequence->GetDisplayName().ToString() );

    FGuid planeGuid = iSequencer.CreateBinding( *plane, plane->GetActorLabel() );

    iSequencer.OnActorAddedToSequencer().Broadcast( plane, planeGuid );
}

//---

//static
void
BoardSequenceToolHelpers::CreatePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceToolHelpers::CreatePlane( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame() );
}

//static
void
ShotSequenceToolHelpers::CreatePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    CreatePlane( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
}

//static
void
ShotSequenceToolHelpers::CreatePlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( iSequencer, iSequence, iSequenceID, &camera_guid );

    if( !camera )
        return;

    //---

    const FScopedTransaction transaction( LOCTEXT( "CreateStoryPlaneHere", "Create Storyboard Plane Here" ) );

    cTemporarySwitchInner switch_to( iSequencer, iSequenceID );

    //---

    ShotSequenceToolHelpers::SpawnAndBindPlane( iSequencer, iSequence, camera_guid, camera, iFrameNumber );

    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
}

//---
//---
//---

//static
int32
BoardSequenceToolHelpers::GetPlanes( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<AStaticMeshActor*>* oPlanes, TArray<FGuid>* oPlaneBindings )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return 0;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return 0;

    return ShotSequenceHelpers::GetPlanes( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kSelectedOrAll, oPlanes, oPlaneBindings );
}

//static
int32
ShotSequenceToolHelpers::GetPlanes( ISequencer* iSequencer, TArray<AStaticMeshActor*>* oPlanes, TArray<FGuid>* oPlaneBindings )
{
    return ShotSequenceHelpers::GetPlanes( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), EGetPlane::kSelectedOrAll, oPlanes, oPlaneBindings );
}


#undef LOCTEXT_NAMESPACE
