// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

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
#include "LevelEditorViewport.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieScene.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"

#include "Board/BoardSequence.h"
#include "EposSequenceHelpers.h"
#include "PlaneActor.h"
#include "Tools/ResourceAssetTools.h"

#define LOCTEXT_NAMESPACE "EposSequenceTools_Plane"

//static
FVector
ShotSequenceTools::ComputePlaneScale( const ACineCameraActor* iCamera, float iDistance ) // From FDrawFrustumSceneProxy::GetDynamicMeshElements()
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
FVector
FindNextFreePlaneLocation( UWorld* iWorld, FVector iPlaneLocation, FVector iCameraLocation )
{
    FVector next_location = iPlaneLocation;

    TArray<AActor*> existing_planes;
    UGameplayStatics::GetAllActorsOfClass( iWorld, APlaneActor::StaticClass(), existing_planes );

    auto ExistingPlaneOnLocation = [&existing_planes]( FVector iPlaneLocation )
    {
        for( auto existing_plane : existing_planes )
        {
            if( existing_plane->GetActorLocation().Equals( iPlaneLocation ) )
                return true;
        }

        return false;
    };

    while( true )
    {
        bool used_location = ExistingPlaneOnLocation( next_location );
        if( !used_location )
            break;

        FVector direction = ( iCameraLocation - next_location ).GetSafeNormal();
        next_location += direction * 0.01f;
    }

    return next_location;
}

//static
APlaneActor*
ShotSequenceTools::SpawnPlane( UWorld* iWorld, ACineCameraActor* iCamera, UMaterialInstanceConstant* iMaterial )
{
    FTransform camera_transform = iCamera->GetRootComponent()->GetComponentTransform();

    FVector const CamLocation = camera_transform.GetLocation();
    FVector const CamDir = camera_transform.GetRotation().Vector();
    FRotator const CamRot = camera_transform.Rotator();

    //-

    // Make a function GuessPlaneLocation(...)
    float FocusDistance = 200;
    FVector plane_location = CamLocation + CamDir * FocusDistance;
    plane_location = FindNextFreePlaneLocation( iWorld, plane_location, CamLocation );

    FVector plane_scale = ShotSequenceTools::ComputePlaneScale( iCamera, FocusDistance );

    FRotator plane_rotator = CamRot;

    //---

    FActorSpawnParameters SpawnParams;
    APlaneActor* plane = iWorld->SpawnActor<APlaneActor>( SpawnParams );
    if( !plane )
        return nullptr;

    plane->GetStaticMeshComponent()->SetMaterial( 0, iMaterial );

    plane->SetActorScale3D( plane_scale );
    plane->SetActorLocation( plane_location );
    plane->SetActorRotation( FRotator( 0.f, 90.f, 90.f ) );
    plane->AddActorWorldRotation( plane_rotator );

    //plane->AttachToActor( iCamera, FAttachmentTransformRules::KeepRelativeTransform ); // Done in the editor with GEditor->ParentActors();

    return plane;
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
ShotSequenceTools::SpawnAndBindPlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FGuid iCameraGuid, ACineCameraActor* iCamera, FFrameNumber iFrameNumber )
{
    if( !GCurrentLevelEditingViewportClient )
        return;

    UMaterialInstanceConstant* new_material = ProjectAssetTools::CreateMaterialAndTexture( iSequence, iCamera, iSequencer.GetRootMovieSceneSequence() );
    if( !new_material )
        return;

    //---

    UWorld* world = GCurrentLevelEditingViewportClient->GetWorld();

    APlaneActor* plane = ShotSequenceTools::SpawnPlane( world, iCamera, new_material );

    //---

    GEditor->ParentActors( iCamera, plane, NAME_None );

    plane->SetFolderPath( *FPaths::GetBaseFilename( iSequencer.GetRootMovieSceneSequence()->GetPathName() ) );
    FActorLabelUtilities::RenameExistingActor( plane, TEXT( "Plane_1" ), true ); // The shot name is displayed in another column in the world outliner
    //SetPlaneLabelUnique( plane, TEXT("Plane_01_") + iSequence->GetDisplayName().ToString() );

    FGuid planeGuid = iSequencer.CreateBinding( *plane, plane->GetActorLabel() );

    iSequencer.OnActorAddedToSequencer().Broadcast( plane, planeGuid );
}

//---

//static
void
BoardSequenceTools::CreatePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::CreatePlane( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame() );
}

//static
bool
BoardSequenceTools::CanCreatePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber )
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
ShotSequenceTools::CreatePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    CreatePlane( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
}

//static
bool
ShotSequenceTools::CanCreatePlane( ISequencer* iSequencer, FFrameNumber iFrameNumber )
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
ShotSequenceTools::CreatePlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( iSequencer, iSequence, iSequenceID, &camera_guid );

    if( !camera )
        return;

    //---

    const FScopedTransaction transaction( LOCTEXT( "CreateStoryPlaneHere", "Create Storyboard Plane Here" ) );

    cTemporarySwitchInner switch_to( iSequencer, iSequenceID );

    //---

    ShotSequenceTools::SpawnAndBindPlane( iSequencer, iSequence, camera_guid, camera, iFrameNumber );

    //---

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
}

//---

//static
int32
BoardSequenceTools::GetAllPlanes( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<APlaneActor*>* oPlanes, TArray<FGuid>* oPlaneBindings )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return 0;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return 0;

    return ShotSequenceHelpers::GetAllPlanes( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kSelectedOrAll, oPlanes, oPlaneBindings );
}

//static
int32
ShotSequenceTools::GetAllPlanes( ISequencer* iSequencer, TArray<APlaneActor*>* oPlanes, TArray<FGuid>* oPlaneBindings )
{
    return ShotSequenceHelpers::GetAllPlanes( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), EGetPlane::kSelectedOrAll, oPlanes, oPlaneBindings );
}

//---
//---
//---

//static
void
BoardSequenceTools::DetachPlane( ISequencer* iSequencer, FFrameNumber iFrameNumber, APlaneActor* iPlane )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return;

    ShotSequenceTools::DetachPlane( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iPlane );
}

//static
void
ShotSequenceTools::DetachPlane( ISequencer* iSequencer, APlaneActor* iPlane )
{
    DetachPlane( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iPlane );
}

//static
void
ShotSequenceTools::DetachPlane( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, APlaneActor* iPlane )
{
    //FGuid camera_guid;
    //ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( iSequencer, iSequence, iSequenceID, &camera_guid );

    //if( !camera )
    //    return;

    //---

    //const FScopedTransaction transaction( LOCTEXT( "CreateStoryPlaneHere", "Create Storyboard Plane Here" ) );

    //cTemporarySwitchInner switch_to( iSequencer, iSequenceID );

    //---

    GEditor->SelectActor( iPlane, true, true );

    GEditor->DetachSelectedActors();

    //---

    //iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
}

//---

//static
int32
BoardSequenceTools::GetAttachedPlanes( ISequencer* iSequencer, FFrameNumber iFrameNumber, TArray<APlaneActor*>* oPlanes, TArray<FGuid>* oPlaneBindings )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber );
    if( !result.mInnerSequence )
        return 0;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return 0;

    return ShotSequenceHelpers::GetAttachedPlanes( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kSelectedOrAll, oPlanes, oPlaneBindings );
}

//static
int32
ShotSequenceTools::GetAttachedPlanes( ISequencer* iSequencer, TArray<APlaneActor*>* oPlanes, TArray<FGuid>* oPlaneBindings )
{
    return ShotSequenceHelpers::GetAttachedPlanes( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), EGetPlane::kSelectedOrAll, oPlanes, oPlaneBindings );
}


#undef LOCTEXT_NAMESPACE
