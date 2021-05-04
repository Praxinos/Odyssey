// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Shot/ShotSequenceHelpers.h"

//#include "AssetRegistryModule.h"
#include "Channels/MovieSceneChannelProxy.h"
//#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "Evaluation/MovieSceneSequenceHierarchy.h"
//#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Materials/MaterialInstanceConstant.h"
#include "IMovieScenePlayer.h"
#include "MovieScene.h"
#include "MovieSceneCommonHelpers.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "Sections/MovieSceneSubSection.h"
#include "Sections/MovieScene3DTransformSection.h"
#include "Sections/MovieScenePrimitiveMaterialSection.h"
#include "Tracks/MovieScene3DTransformTrack.h"
//#include "Tracks/MovieSceneCinematicShotTrack.h"
#include "Tracks/MovieScenePrimitiveMaterialTrack.h"

//#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
//#include "Shot/ShotSequence.h"
//#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"

#define LOCTEXT_NAMESPACE "ShotSequenceHelpers"

//static
ACineCameraActor*
ShotSequenceHelpers::GetCamera( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid* oCameraBinding )
{
    UMovieScene* movieScene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !movieScene )
        return nullptr;

    ACineCameraActor* ExistingCamera = nullptr;
    for( int i = 0; i < movieScene->GetPossessableCount(); i++ )
    {
        FMovieScenePossessable possessable = movieScene->GetPossessable( i );

        for( TWeakObjectPtr<> WeakObject : iPlayer.FindBoundObjects( possessable.GetGuid(), iSequenceID ) )
        {
            ExistingCamera = Cast<ACineCameraActor>( WeakObject.Get() );

            if( ExistingCamera )
            {
                if( oCameraBinding )
                    *oCameraBinding = possessable.GetGuid();

                return ExistingCamera;
            }
        }
    }

    return nullptr;
}

//static
int32
ShotSequenceHelpers::GetPlanes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, EGetPlane iSelection, TArray<AStaticMeshActor*>* oPlanes, TArray<FGuid>* oPlaneBindings )
{
    if( oPlanes )
        oPlanes->Empty();
    if( oPlaneBindings )
        oPlaneBindings->Empty();

    UMovieScene* movieScene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !movieScene )
        return 0;

    TArray<AStaticMeshActor*> planes;
    TArray<FGuid> plane_bindings;

    TArray<AStaticMeshActor*> planes_selected;
    TArray<FGuid> plane_bindings_selected;

    TArray<AStaticMeshActor*> planes_not_selected;
    TArray<FGuid> plane_bindings_not_selected;

    for( int i = 0; i < movieScene->GetPossessableCount(); i++ )
    {
        FMovieScenePossessable possessable = movieScene->GetPossessable( i );

        for( TWeakObjectPtr<> WeakObject : iPlayer.FindBoundObjects( possessable.GetGuid(), iSequenceID ) )
        {
            AStaticMeshActor* plane = Cast<AStaticMeshActor>( WeakObject.Get() );

            if( plane )
            {
                switch( iSelection )
                {
                    case EGetPlane::kAlwaysAll:
                        planes.Add( plane );
                        plane_bindings.Add( possessable.GetGuid() );
                        break;
                    case EGetPlane::kSelectedOnly:
                        if( plane->IsSelected() )
                        {
                            planes.Add( plane );
                            plane_bindings.Add( possessable.GetGuid() );
                        }
                        break;
                    default:
                    case EGetPlane::kSelectedOrAll:
                        if( plane->IsSelected() )
                        {
                            planes_selected.Add( plane );
                            plane_bindings_selected.Add( possessable.GetGuid() );
                        }
                        else
                        {
                            planes_not_selected.Add( plane );
                            plane_bindings_not_selected.Add( possessable.GetGuid() );
                        }
                        break;
                }
            }
        }
    }

    if( planes.Num() )
    {
        if( oPlanes )
            oPlanes->Append( planes );
        if( oPlaneBindings )
            oPlaneBindings->Append( plane_bindings );

        return planes.Num();
    }
    else if( planes_selected.Num() )
    {
        if( oPlanes )
            oPlanes->Append( planes_selected );
        if( oPlaneBindings )
            oPlaneBindings->Append( plane_bindings_selected );

        return planes_selected.Num();
    }
    else
    {
        if( oPlanes )
            oPlanes->Append( planes_not_selected );
        if( oPlaneBindings )
            oPlaneBindings->Append( plane_bindings_not_selected );

        return planes_not_selected.Num();
    }
}

//static
int32
ShotSequenceHelpers::GetDrawingIndex( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, FGuid iPlaneBinding, FDrawingData* oData )
{
    if( oData )
    {
        oData->mChannel = nullptr;
        oData->mTrack = nullptr;
        oData->mSection = nullptr;
    }

    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !moviescene )
        return INDEX_NONE;

    TArrayView<TWeakObjectPtr<>> objects = iPlayer.FindBoundObjects( iPlaneBinding, iSequenceID );
    if( objects.Num() != 1 )
        return INDEX_NONE;
    AStaticMeshActor* plane = Cast<AStaticMeshActor>( objects[0] );
    if( !plane )
        return INDEX_NONE;

    FGuid plane_component = iPlayer.FindObjectId( *plane->GetRootComponent(), iSequenceID );

    UMovieScenePrimitiveMaterialTrack* track = moviescene->FindTrack<UMovieScenePrimitiveMaterialTrack>( plane_component );
    if( !track )
        return INDEX_NONE;

    UMovieScenePrimitiveMaterialSection* section = Cast<UMovieScenePrimitiveMaterialSection>( MovieSceneHelpers::FindSectionAtTime( track->GetAllSections(), iFrameNumber ) );
    if( !section )
        return INDEX_NONE;

    if( !moviescene->GetPlaybackRange().Contains( iFrameNumber ) )
        return INDEX_NONE;

    TArrayView<FMovieSceneObjectPathChannel*> channels = section->GetChannelProxy().GetChannels<FMovieSceneObjectPathChannel>();
    check( channels.Num() == 1 );
    int32 key_index = channels[0]->GetData().FindKey( iFrameNumber );

    if( oData )
    {
        oData->mChannel = channels[0];
        oData->mTrack = track;
        oData->mSection = section;
    }

    return key_index;
}

//static
TArray<FFrameNumber>
ShotSequenceHelpers::GetAllMaterialTimes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    TArray<FFrameNumber> times;

    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !moviescene )
        return times;

    TArray<AStaticMeshActor*> planes;
    TArray<FGuid> guids;
    int32 nb_plane = GetPlanes( iPlayer, iSequence, iSequenceID, EGetPlane::kSelectedOrAll, &planes, &guids );
    if( !nb_plane )
        return times;

    for( int i = 0; i < planes.Num(); i++ )
    {
        AStaticMeshActor* plane = planes[i];
        FGuid guid = guids[i];

        FGuid plane_component = iPlayer.FindObjectId( *plane->GetRootComponent(), iSequenceID );

        UMovieScenePrimitiveMaterialTrack* track = moviescene->FindTrack<UMovieScenePrimitiveMaterialTrack>( plane_component );
        if( !track )
            continue;

        for( auto section : track->GetAllSections() )
        {
            UMovieScenePrimitiveMaterialSection* section_material = Cast<UMovieScenePrimitiveMaterialSection>( section );
            if( !section_material )
                continue;

            TArrayView<FMovieSceneObjectPathChannel*> channels = section_material->GetChannelProxy().GetChannels<FMovieSceneObjectPathChannel>();
            check( channels.Num() == 1 );
            for( auto time : channels[0]->GetData().GetTimes() )
                times.Add( time );
        }
    }

    times.Sort();

    return times;
}

//static
TArray<FFrameNumber>
ShotSequenceHelpers::GetCameraTransformTimes( UMovieSceneSequence* iSequence )
{
    TArray<FFrameNumber> keys;

    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !moviescene )
        return keys;

    UMovieSceneTrack* cameracut_track = moviescene->GetCameraCutTrack();
    if( !cameracut_track )
        return keys;

    TArray<UMovieSceneSection*> cameracut_sections = cameracut_track->GetAllSections();
    if( !cameracut_sections.Num() )
        return keys;

    UMovieSceneSingleCameraCutSection* cameracut_section = Cast<UMovieSceneSingleCameraCutSection>( cameracut_sections[0] );
    if( !cameracut_section )
        return keys;

    UMovieSceneTrack* track = moviescene->FindTrack<UMovieScene3DTransformTrack>( cameracut_section->GetCameraBindingID().GetGuid() );
    if( !track )
        return keys;

    for( auto section : track->GetAllSections() )
    {
        TArrayView<FMovieSceneFloatChannel*> channels = section->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>();
        for( int i = 0; i < 9; i++ )
        {
            TArrayView<const FFrameNumber> times = channels[i]->GetTimes();
            for( auto time : times )
            {
                TRange<FFrameNumber> range( cameracut_section->GetTrueRange() );
                range.SetUpperBound( TRangeBound<FFrameNumber>::FlipInclusion( range.GetUpperBound() ) ); // Special case when a key is on the frame just on the exclusive upper bound value to render it
                if( range.Contains( time ) )
                    keys.AddUnique( time );
            }
        }
    }

    return keys;
}

//---

//static
TArray<UMovieScene3DTransformSection*>
ShotSequenceHelpers::GetCameraTransformSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FGuid& iCameraBinding )
{
    TArray<UMovieScene3DTransformSection*> sections;

    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !moviescene )
        return sections;

    if( !iCameraBinding.IsValid() )
        return sections;

    UMovieScene3DTransformTrack* track = moviescene->FindTrack<UMovieScene3DTransformTrack>( iCameraBinding );
    if( !track )
        return sections;

    for( auto section : track->GetAllSections() )
        sections.Add( Cast<UMovieScene3DTransformSection>( section ) );

    return sections;
}

//static
TArray<UMovieScene3DTransformSection*>
ShotSequenceHelpers::GetPlaneTransformSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FGuid& iPlaneBinding )
{
    TArray<UMovieScene3DTransformSection*> sections;

    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !moviescene )
        return sections;

    if( !iPlaneBinding.IsValid() )
        return sections;

    UMovieSceneTrack* track = moviescene->FindTrack<UMovieScene3DTransformTrack>( iPlaneBinding );
    if( !track )
        return sections;

    for( auto section : track->GetAllSections() )
        sections.Add( Cast<UMovieScene3DTransformSection>( section ) );

    return sections;
}

//static
TArray<UMovieScenePrimitiveMaterialSection*>
ShotSequenceHelpers::GetPlaneMaterialSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FGuid& iPlaneBinding )
{
    TArray<UMovieScenePrimitiveMaterialSection*> sections;

    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !moviescene )
        return sections;

    if( !iPlaneBinding.IsValid() )
        return sections;

    TArrayView<TWeakObjectPtr<>> objects = iPlayer.FindBoundObjects( iPlaneBinding, iSequenceID );
    if( objects.Num() != 1 )
        return sections;
    AStaticMeshActor* plane = Cast<AStaticMeshActor>( objects[0] );
    if( !plane )
        return sections;

    FGuid plane_component = iPlayer.FindObjectId( *plane->GetRootComponent(), iSequenceID );

    UMovieSceneTrack* track = moviescene->FindTrack<UMovieScenePrimitiveMaterialTrack>( plane_component );
    if( !track )
        return sections;

    for( auto section : track->GetAllSections() )
        sections.Add( Cast<UMovieScenePrimitiveMaterialSection>( section ) );

    return sections;
}

#undef LOCTEXT_NAMESPACE
