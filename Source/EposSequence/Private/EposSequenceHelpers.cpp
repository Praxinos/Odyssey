// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "EposSequenceHelpers.h"

#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "Evaluation/MovieSceneSequenceHierarchy.h"
#include "Materials/MaterialInstanceConstant.h"
#include "IMovieScenePlayer.h"
#include "LevelSequence.h"
#include "MoviePipeline.h"
#include "MoviePipelineBlueprintLibrary.h"
#include "MoviePipelineMasterConfig.h"
#include "MovieScene.h"
#include "MovieSceneCommonHelpers.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "MovieSceneSequenceVisitor.h"
#include "Sections/MovieSceneSubSection.h"
#include "Sections/MovieScene3DTransformSection.h"
#include "Sections/MovieSceneBoolSection.h"
#include "Sections/MovieSceneParameterSection.h"
#include "Sections/MovieScenePrimitiveMaterialSection.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Tracks/MovieSceneMaterialTrack.h"
#include "Tracks/MovieScenePrimitiveMaterialTrack.h"
#include "Tracks/MovieSceneVisibilityTrack.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "NoteTrack/MovieSceneNoteTrack.h"
#include "NoteTrack/MovieSceneNoteSection.h"
#include "PlaneActor.h"
#include "Shot/ShotSequence.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"
#include "StoryNote.h"

#define LOCTEXT_NAMESPACE "EposSequenceHelpers"

BoardSequenceHelpers::FInnerSequenceResult
BoardSequenceHelpers::GetInnerSequence( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceId )
{
    FInnerSequenceResult result;

    result.mInnerSequence = iSubSection.GetSequence();
    result.mInnerMovieScene = result.mInnerSequence ? result.mInnerSequence->GetMovieScene() : nullptr;

    //---

    const FMovieSceneSequenceID             thisSequenceID = iSequenceId;
    const FMovieSceneSequenceID             targetSequenceID = iSubSection.GetSequenceID();
    const FMovieSceneSequenceHierarchy*     hierarchy = iPlayer.GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( iPlayer.GetEvaluationTemplate().GetCompiledDataID() );

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

    return result;
}

BoardSequenceHelpers::FInnerSequenceResult
BoardSequenceHelpers::GetInnerSequence( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceId, const FFrameNumber& iFrameNumber )
{
    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    UMovieSceneCinematicBoardTrack* board_track = moviescene ? moviescene->FindMasterTrack<UMovieSceneCinematicBoardTrack>() : nullptr;
    UMovieSceneSection* section = board_track ? MovieSceneHelpers::FindSectionAtTime( board_track->GetAllSections(), iFrameNumber ) : nullptr;
    UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );

    if( !subsection )
        return FInnerSequenceResult();

    FInnerSequenceResult result = GetInnerSequence( iPlayer, *subsection, iSequenceId );
    result.mInnerTime = iFrameNumber * subsection->OuterToInnerTransform();

    return result;
}

//---
//---
//---

//static
ACineCameraActor*
BoardSequenceHelpers::GetCamera( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID, FGuid* oCameraBinding )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSubSection, iSequenceID );

    return ShotSequenceHelpers::GetCamera( iPlayer, result.mInnerSequence, result.mInnerSequenceId, oCameraBinding );
}

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
ShotSequenceHelpers::GetAllPlanes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, EGetPlane iPlaneSelection, TArray<APlaneActor*>* oPlanes, TArray<FGuid>* oPlaneBindings )
{
    if( oPlanes )
        oPlanes->Empty();
    if( oPlaneBindings )
        oPlaneBindings->Empty();

    UMovieScene* movieScene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !movieScene )
        return 0;

    TArray<APlaneActor*> planes;
    TArray<FGuid> plane_bindings;

    TArray<APlaneActor*> planes_selected;
    TArray<FGuid> plane_bindings_selected;

    TArray<APlaneActor*> planes_not_selected;
    TArray<FGuid> plane_bindings_not_selected;

    for( int i = 0; i < movieScene->GetPossessableCount(); i++ )
    {
        FMovieScenePossessable possessable = movieScene->GetPossessable( i );

        for( TWeakObjectPtr<> WeakObject : iPlayer.FindBoundObjects( possessable.GetGuid(), iSequenceID ) )
        {
            APlaneActor* plane = Cast<APlaneActor>( WeakObject.Get() );

            if( !plane )
                continue;

            switch( iPlaneSelection )
            {
                case EGetPlane::kAll:
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
ShotSequenceHelpers::GetAttachedPlanes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, EGetPlane iPlaneSelection, TArray<APlaneActor*>* oPlanes, TArray<FGuid>* oPlaneBindings )
{
    if( oPlanes )
        oPlanes->Empty();
    if( oPlaneBindings )
        oPlaneBindings->Empty();

    UMovieScene* movieScene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !movieScene )
        return 0;

    TArray<APlaneActor*> planes;
    TArray<FGuid> plane_bindings;

    TArray<APlaneActor*> planes_selected;
    TArray<FGuid> plane_bindings_selected;

    TArray<APlaneActor*> planes_not_selected;
    TArray<FGuid> plane_bindings_not_selected;

    for( int i = 0; i < movieScene->GetPossessableCount(); i++ )
    {
        FMovieScenePossessable possessable = movieScene->GetPossessable( i );

        for( TWeakObjectPtr<> WeakObject : iPlayer.FindBoundObjects( possessable.GetGuid(), iSequenceID ) )
        {
            APlaneActor* plane = Cast<APlaneActor>( WeakObject.Get() );

            if( !plane )
                continue;

            USceneComponent* RootComp = plane->GetRootComponent();
            if( !RootComp || !RootComp->GetAttachParent() )
                continue;

            AActor* ParentActor = RootComp->GetAttachParent()->GetOwner();
            if( !ParentActor ) //TODO: confirm by comparing with the camera ? or is it enough as the planes are in the movie scene ?
                continue;

            switch( iPlaneSelection )
            {
                case EGetPlane::kAll:
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
TArray<TWeakObjectPtr<UMovieSceneNoteSection>>
EposSequenceHelpers::GetNotesRecursive( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber )
{
    TArray<TWeakObjectPtr<UMovieSceneNoteSection>> note_sections = GetNotes( iPlayer, iSequence, iSequenceID, iFrameNumber );

    UMovieScene* movie_scene = iSequence->GetMovieScene();
    UMovieSceneCinematicBoardTrack* track = movie_scene->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
    if( !track )
        return note_sections;

    auto sections = track->GetAllSections();
    for( auto section : sections )
    {
        if( !section->IsTimeWithinSection( iFrameNumber ) || !section->IsActive() )
            continue;

        UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );

        BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( iPlayer, *subsection, iSequenceID );
        FFrameTime inner_time = iFrameNumber * subsection->OuterToInnerTransform();

        if( !result.mInnerSequence )
            continue;

        TArray<TWeakObjectPtr<UMovieSceneNoteSection>> note_sections_recursive = GetNotesRecursive( iPlayer, result.mInnerSequence, result.mInnerSequenceId, inner_time.GetFrame() );
        note_sections.Append( note_sections_recursive );
    }

    return note_sections;
}

//static
TArray<TWeakObjectPtr<UMovieSceneNoteSection>>
EposSequenceHelpers::GetNotes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TOptional<FFrameNumber> iFrameNumber )
{
    //TODO: certainly replace those 2 functions by the one with the sequencer visitor: UMoviePipelineStoryboardBlueprintLibrary::GetNotes()

    TArray<TWeakObjectPtr<UMovieSceneNoteSection>> note_sections;

    UMovieScene* movie_scene = iSequence->GetMovieScene();
    TArray<UMovieSceneTrack*> tracks = movie_scene->GetMasterTracks();
#if WITH_EDITORONLY_DATA
    tracks.StableSort( []( const UMovieSceneTrack& iA, const UMovieSceneTrack& iB )
                       {
                           return iA.GetSortingOrder() < iB.GetSortingOrder();
                       } );
#endif

    for( auto track : tracks )
    {
        UMovieSceneNoteTrack* note_track = Cast<UMovieSceneNoteTrack>( track );
        if( !note_track )
            continue;

        TArray<UMovieSceneSection*> sections = note_track->GetAllSections();

        // It Should be MovieSceneHelpers::SortConsecutiveSections( sections ); but it doesn't use the stable sort
        sections.StableSort( []( const UMovieSceneSection& iA, const UMovieSceneSection& iB )
                             {
                                 TRangeBound<FFrameNumber> LowerBoundA = iA.GetRange().GetLowerBound();
                                 return TRangeBound<FFrameNumber>::MinLower( LowerBoundA, iB.GetRange().GetLowerBound() ) == LowerBoundA;
                             } );
        sections.StableSort( []( const UMovieSceneSection& iA, const UMovieSceneSection& iB )
                             {
                                 return iA.GetRowIndex() < iB.GetRowIndex();
                             } );


        for( auto section : sections )
        {
            if( iFrameNumber.IsSet() )
            {
                //MovieSceneHelpers::FindSectionAtTime()
                if( !section->IsTimeWithinSection( iFrameNumber.GetValue() ) || !section->IsActive() )
                    continue;
            }

            UMovieSceneNoteSection* note_section = Cast<UMovieSceneNoteSection>( section );
            if( !note_section )
                continue;

            UStoryNote* note = note_section->GetNote();
            if( !note )
                continue;

            note_sections.Add( note_section );
        }
    }

    return note_sections;
}


//static
ShotSequenceHelpers::FFindOrCreatePlaneVisibilityResult
ShotSequenceHelpers::FindPlaneVisibilityTrackAndSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding, TOptional<FFrameNumber> iFrameNumber )
{
    FFindOrCreatePlaneVisibilityResult result;

    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !moviescene )
        return result;

    FMovieSceneBinding* binding = moviescene->FindBinding( iPlaneBinding );
    if( !binding )
        return result;

    const TArray<UMovieSceneTrack*>& tracks = binding->GetTracks();
    for( auto track : tracks )
    {
        result.mTrack = Cast<UMovieSceneVisibilityTrack>( track );
        if( result.mTrack.IsValid() )
            break;
    }

    if( !result.mTrack.IsValid() )
        return result;

    //---

    if( iFrameNumber.IsSet() )
    {
        for( auto section : result.mTrack->GetAllSections() )
        {
            if( section->IsTimeWithinSection( iFrameNumber.GetValue() ) )
            {
                result.mSections.Add( Cast<UMovieSceneBoolSection>( section ) );
            }
        }
    }
    else
    {
        for( auto section : result.mTrack->GetAllSections() )
            result.mSections.Add( Cast<UMovieSceneBoolSection>( section ) );
    }

    return result;
}


bool
FDrawing::Exists()
{
    return mKeyHandle != FKeyHandle::Invalid();
}

UMaterialInstance*
FDrawing::GetMaterial() const
{
    if( !mChannel || mKeyHandle == FKeyHandle::Invalid() )
        return nullptr;

    FMovieSceneObjectPathChannelKeyValue value;
    UE::MovieScene::GetKeyValue( mChannel, mKeyHandle, value );
    UMaterialInstance* material = Cast<UMaterialInstance>( value.Get() );

    return material;
}

void
FDrawing::SetMaterial( UMaterialInstance* iMaterial )
{
    if( !mChannel || mKeyHandle == FKeyHandle::Invalid() )
        return;

    mSection->Modify();

    FMovieSceneObjectPathChannelKeyValue new_value( iMaterial );
    UE::MovieScene::AssignValue( mChannel, mKeyHandle, new_value );
}

//friend
bool
operator==( const FDrawing& iLhs, const FDrawing& iRhs )
{
    return iLhs.mChannel == iRhs.mChannel
        && iLhs.mSection == iRhs.mSection
        && iLhs.mKeyHandle == iRhs.mKeyHandle
        && iLhs.GetMaterial() == iRhs.GetMaterial();
}

//static
FDrawing
ShotSequenceHelpers::ConvertToDrawing( TWeakObjectPtr<UMovieSceneSection> iSection, const FMovieSceneChannelHandle& iChannelHandle, FKeyHandle iKeyHandle )
{
    FDrawing drawing;

    if( !iSection.IsValid()
        || iKeyHandle == FKeyHandle::Invalid() )
        return drawing;

    TMovieSceneChannelHandle<FMovieSceneObjectPathChannel> channel_handle = iChannelHandle.Cast<FMovieSceneObjectPathChannel>();
    FMovieSceneObjectPathChannel* object_channel = channel_handle.Get();
    if( !object_channel )
        return drawing;

    drawing.mSection = iSection;
    drawing.mKeyHandle = iKeyHandle;
    drawing.mChannel = object_channel;

    return drawing;
}

//static
ShotSequenceHelpers::FFindOrCreateMaterialDrawingResult
ShotSequenceHelpers::FindMaterialDrawingTrackAndSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding, TOptional<FFrameNumber> iFrameNumber )
{
    FFindOrCreateMaterialDrawingResult result;

    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !moviescene )
        return result;

    TArrayView<TWeakObjectPtr<>> objects = iPlayer.FindBoundObjects( iPlaneBinding, iSequenceID );
    if( objects.Num() != 1 )
        return result;
    APlaneActor* plane = Cast<APlaneActor>( objects[0] );
    if( !plane )
        return result;

    FGuid plane_component = iPlayer.FindCachedObjectId( *plane->GetRootComponent(), iSequenceID );
    if( !plane_component.IsValid() )
        return result;

    //---

    result.mPlaneComponentBinding = plane_component;

    result.mTrack = moviescene->FindTrack<UMovieScenePrimitiveMaterialTrack>( result.mPlaneComponentBinding ); // Get only the material track of the first "material 0", should be ok as plane actor have only 1 material associated
    if( !result.mTrack.IsValid() )
        return result;

    //---

    if( iFrameNumber.IsSet() )
    {
        for( auto section : result.mTrack->GetAllSections() )
        {
            if( section->IsTimeWithinSection( iFrameNumber.GetValue() ) )
            {
                result.mSections.Add( Cast<UMovieScenePrimitiveMaterialSection>( section ) );
            }
        }
    }
    else
    {
        for( auto section : result.mTrack->GetAllSections() )
            result.mSections.Add( Cast<UMovieScenePrimitiveMaterialSection>( section ) );
    }

    return result;
}

//static
ShotSequenceHelpers::FFindOrCreateMaterialDrawingResult
ShotSequenceHelpers::FindOrCreateMaterialDrawingTrackAndSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding, TOptional<FFrameNumber> iFrameNumber )
{
    FFindOrCreateMaterialDrawingResult result = FindMaterialDrawingTrackAndSections( iPlayer, iSequence, iSequenceID, iPlaneBinding, iFrameNumber );

    // Return if we get track AND sections (with optional iFrameNumber taken into account)
    if( result.mTrack.IsValid() && result.mSections.Num() )
        return result;

    // At this step, result.mSections is empty, but it doesn't necessarily mean that result.mTrack->GetAllSections() is also empty (if iFrameNumber is set but outside section(s) boundaries)
    // So, we considere to create a new section only if there is really no existing section (no matter of iFrameNumber)

    if( !result.mTrack.IsValid() )
    {
        result.mTrackCreated = true;

        UMovieSceneTrack* track = iSequence->GetMovieScene()->AddTrack( UMovieScenePrimitiveMaterialTrack::StaticClass(), result.mPlaneComponentBinding );
        result.mTrack = Cast<UMovieScenePrimitiveMaterialTrack>( track );

        result.mTrack->MaterialIndex = 0; //TODO: iMaterialTrackIndex;
#if WITH_EDITORONLY_DATA
        result.mTrack->SetDisplayName( FText::Format( LOCTEXT( "MaterialTrackName_Format", "Material Element {0}" ), FText::AsNumber( result.mTrack->MaterialIndex ) ) );
#endif
    }

    check( result.mTrack.IsValid() );

    //---

    // Use GetAllSections() to be sure to have the 'real' number of section inside the track
    // If we rely only on mSections and with a iFrameNumber set, we can create a section while there are ones but outside iFrameNumber
    if( !result.mTrack->GetAllSections().Num() )
    {
        result.mSectionsCreated = true;

        UMovieSceneSection* section = result.mTrack->CreateNewSection();
        check( result.mTrack->IsEmpty() );
        result.mTrack->AddSection( *section );

        section->SetRange( TRange<FFrameNumber>::All() );

        result.mSections.Add( Cast<UMovieScenePrimitiveMaterialSection>( section ) );
    }

    return result;
}

//static
FDrawing
ShotSequenceHelpers::GetDrawing( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    FDrawing drawing;

    if( !iSequence->GetMovieScene()->GetPlaybackRange().Contains( iFrameNumber ) )
        return drawing;

    FFindOrCreateMaterialDrawingResult result = FindMaterialDrawingTrackAndSections( iPlayer, iSequence, iSequenceID, iPlaneBinding, iFrameNumber );
    if( !result.mTrack.IsValid() || !result.mSections.Num() )
        return drawing;

    TWeakObjectPtr<UMovieScenePrimitiveMaterialSection> section = result.mSections[0];
    FMovieSceneObjectPathChannel* channel = &section->MaterialChannel;

    //---

    TArray<FKeyHandle> key_handles;
    channel->GetKeys( TRange<FFrameNumber>( iFrameNumber ), nullptr, &key_handles );
    if( !key_handles.Num() )
        return drawing;

    drawing.mChannel = channel;
    drawing.mSection = section;
    drawing.mKeyHandle = key_handles[0];

    return drawing;
}

//static
TArray<FDrawing>
ShotSequenceHelpers::GetAllDrawings( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding )
{
    TArray<FDrawing> drawings;

    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !moviescene )
        return drawings;

    TArrayView<TWeakObjectPtr<>> objects = iPlayer.FindBoundObjects( iPlaneBinding, iSequenceID );
    if( objects.Num() != 1 )
        return drawings;
    APlaneActor* plane = Cast<APlaneActor>( objects[0] );
    if( !plane )
        return drawings;

    FGuid plane_component = iPlayer.FindCachedObjectId( *plane->GetRootComponent(), iSequenceID );
    if( !plane_component.IsValid() )
        return drawings;

    UMovieScenePrimitiveMaterialTrack* track = moviescene->FindTrack<UMovieScenePrimitiveMaterialTrack>( plane_component );
    if( !track )
        return drawings;

    for( auto section : track->GetAllSections() )
    {
        UMovieScenePrimitiveMaterialSection* section_material = Cast<UMovieScenePrimitiveMaterialSection>( section );
        if( !section_material )
            continue;

        TArrayView<FMovieSceneObjectPathChannel*> channels = section_material->GetChannelProxy().GetChannels<FMovieSceneObjectPathChannel>();
        check( channels.Num() == 1 );
        for( int k = 0; k < channels[0]->GetNumKeys(); k++ )
        {
            FDrawing drawing = { channels[0], section, channels[0]->GetData().GetHandle( k ) };
            drawings.Add( drawing );
        }
    }

    return drawings;
}

//static
TArray<FFrameNumber>
ShotSequenceHelpers::GetAllDrawingTimes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, EGetPlane iPlaneSelection )
{
    TArray<FFrameNumber> times;

    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !moviescene )
        return times;

    TArray<APlaneActor*> planes;
    TArray<FGuid> guids;
    int32 nb_plane = GetAllPlanes( iPlayer, iSequence, iSequenceID, iPlaneSelection, &planes, &guids );
    if( !nb_plane )
        return times;

    for( int i = 0; i < planes.Num(); i++ )
    {
        APlaneActor* plane = planes[i];
        FGuid guid = guids[i];

        FGuid plane_component = iPlayer.FindCachedObjectId( *plane->GetRootComponent(), iSequenceID );
        if( !plane_component.IsValid() )
            continue;

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

bool
FKeyOpacity::Exists()
{
    return mKeyHandle != FKeyHandle::Invalid();
}

bool
FKeyOpacity::GetOpacity( float& oOpacity )
{
    if( !mChannel || mKeyHandle == FKeyHandle::Invalid() )
        return false;

    FMovieSceneFloatValue opacity;
    UE::MovieScene::GetKeyValue( mChannel, mKeyHandle, opacity );
    oOpacity = opacity.Value;

    return true;
}

void
FKeyOpacity::SetOpacity( float iOpacity )
{
    if( !mChannel || mKeyHandle == FKeyHandle::Invalid() )
        return;

    mSection->Modify();

    AssignValue( mChannel, mKeyHandle, FMath::Clamp( iOpacity, 0.f, 1.f ) );
}

//static
FKeyOpacity
ShotSequenceHelpers::ConvertToOpacityKey( TWeakObjectPtr<UMovieSceneSection> iSection, const FMovieSceneChannelHandle& iChannelHandle, FKeyHandle iKeyHandle )
{
    FKeyOpacity opacity_key;

    if( !iSection.IsValid()
        || iKeyHandle == FKeyHandle::Invalid() )
        return opacity_key;

    TMovieSceneChannelHandle<FMovieSceneFloatChannel> channel_handle = iChannelHandle.Cast<FMovieSceneFloatChannel>();
    FMovieSceneFloatChannel* float_channel = channel_handle.Get();
    if( !float_channel )
        return opacity_key;

    opacity_key.mSection = iSection;
    opacity_key.mKeyHandle = iKeyHandle;
    opacity_key.mChannel = float_channel;

    return opacity_key;
}

//static
ShotSequenceHelpers::FFindOrCreateMaterialParameterResult
ShotSequenceHelpers::FindMaterialParameterTrackAndSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding, TOptional<FFrameNumber> iFrameNumber )
{
    FFindOrCreateMaterialParameterResult result;

    UMovieScene* moviescene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !moviescene )
        return result;

    TArrayView<TWeakObjectPtr<>> objects = iPlayer.FindBoundObjects( iPlaneBinding, iSequenceID );
    if( objects.Num() != 1 )
        return result;
    APlaneActor* plane = Cast<APlaneActor>( objects[0] );
    if( !plane )
        return result;

    FGuid plane_component = iPlayer.FindCachedObjectId( *plane->GetRootComponent(), iSequenceID );
    if( !plane_component.IsValid() )
        return result;

    //---

    result.mPlaneComponentBinding = plane_component;

    result.mTrack = moviescene->FindTrack<UMovieSceneComponentMaterialTrack>( result.mPlaneComponentBinding ); // Get only the material track of the first "material 0", should be ok as plane actor have only 1 material associated
    if( !result.mTrack.IsValid() )
        return result;

    //---

    if( iFrameNumber.IsSet() )
    {
        for( auto section : result.mTrack->GetAllSections() )
        {
            if( section->IsTimeWithinSection( iFrameNumber.GetValue() ) )
            {
                result.mSections.Add( Cast<UMovieSceneParameterSection>( section ) );
            }
        }
    }
    else
    {
        for( auto section : result.mTrack->GetAllSections() )
            result.mSections.Add( Cast<UMovieSceneParameterSection>( section ) );
    }

    return result;
}

//static
ShotSequenceHelpers::FFindOrCreateMaterialParameterResult
ShotSequenceHelpers::FindOrCreateMaterialParameterTrackAndSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding, TOptional<FFrameNumber> iFrameNumber )
{
    FFindOrCreateMaterialParameterResult result = FindMaterialParameterTrackAndSections( iPlayer, iSequence, iSequenceID, iPlaneBinding, iFrameNumber );

    // Return if we get track AND sections (with optional iFrameNumber taken into account)
    if( result.mTrack.IsValid() && result.mSections.Num() )
        return result;

    // At this step, result.mSections is empty, but it doesn't necessarily mean that result.mTrack->GetAllSections() is also empty (if iFrameNumber is set but outside section(s) boundaries)
    // So, we considere to create a new section only if there is really no existing section (no matter of iFrameNumber)

    if( !result.mTrack.IsValid() )
    {
        result.mTrackCreated = true;

        UMovieSceneTrack* track = iSequence->GetMovieScene()->AddTrack( UMovieSceneComponentMaterialTrack::StaticClass(), result.mPlaneComponentBinding );
        result.mTrack = Cast<UMovieSceneComponentMaterialTrack>( track );

        result.mTrack->SetMaterialIndex( 0 ); //TODO: iMaterialTrackIndex;
#if WITH_EDITORONLY_DATA
        result.mTrack->SetDisplayName( FText::Format( LOCTEXT( "MaterialTrackName_Format", "Material Element {0}" ), FText::AsNumber( result.mTrack->GetMaterialIndex() ) ) );
#endif
    }

    check( result.mTrack.IsValid() );

    //---

    // Use GetAllSections() to be sure to have the 'real' number of section inside the track
    // If we rely only on mSections and with a iFrameNumber set, we can create a section while there are ones but outside iFrameNumber
    if( !result.mTrack->GetAllSections().Num() )
    {
        result.mSectionsCreated = true;

        UMovieSceneSection* section = result.mTrack->CreateNewSection();
        check( result.mTrack->IsEmpty() );
        result.mTrack->AddSection( *section );

        section->SetRange( TRange<FFrameNumber>::All() );

        result.mSections.Add( Cast<UMovieSceneParameterSection>( section ) );
    }

    return result;
}

//static
ShotSequenceHelpers::FFindOrCreateParameterChannelResult
ShotSequenceHelpers::FindMaterialOpacityChannel( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding, TWeakObjectPtr<UMovieSceneParameterSection> iSection )
{
    FFindOrCreateParameterChannelResult result;

    TArray<FScalarParameterNameAndCurve>& parameters = iSection->GetScalarParameterNamesAndCurves();
    for( auto& parameter : parameters )
    {
        if( parameter.ParameterName.IsEqual( TEXT( "DrawingOpacity" ) ) )
        {
            result.mChannel = &parameter.ParameterCurve;
            return result;
        }
    }

    return result;
}

//static
ShotSequenceHelpers::FFindOrCreateParameterChannelResult
ShotSequenceHelpers::FindOrCreateMaterialOpacityChannel( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding, TWeakObjectPtr<UMovieSceneParameterSection> iSection )
{
    FFindOrCreateParameterChannelResult result = FindMaterialOpacityChannel( iPlayer, iSequence, iSequenceID, iPlaneBinding, iSection );
    if( result.mChannel )
        return result;

    // This will create the channel named "DrawingOpacity" (as it not exists)
    // and add a (dummy) key to be able to recache internal stuff (ChannelProxy) of the parameter section
    // And once the parameter channel are created, next we remove the dummy key
    iSection->AddScalarParameterKey( TEXT( "DrawingOpacity" ), 0, 1.f );

    result.mChannelCreated = true;

    TArray<FScalarParameterNameAndCurve>& parameters = iSection->GetScalarParameterNamesAndCurves();
    for( auto& parameter : parameters )
    {
        if( parameter.ParameterName.IsEqual( TEXT( "DrawingOpacity" ) ) )
        {
            TArray<FFrameNumber> key_times;
            TArray<FKeyHandle> key_handles;
            parameter.ParameterCurve.GetKeys( TRange<FFrameNumber>::All(), &key_times, &key_handles );
            check( key_handles.Num() == 1 );

            UE::MovieScene::SetChannelDefault( &parameter.ParameterCurve, 1.f );

            parameter.ParameterCurve.DeleteKeys( key_handles );

            result.mChannel = &parameter.ParameterCurve;
            return result;
        }
    }

    checkNoEntry();

    return result;
}

//static
FKeyOpacity
ShotSequenceHelpers::GetOpacityKey( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, FGuid iPlaneBinding )
{
    FKeyOpacity key_opacity;

    if( !iSequence->GetMovieScene()->GetPlaybackRange().Contains( iFrameNumber ) )
        return key_opacity;

    FFindOrCreateMaterialParameterResult result = FindMaterialParameterTrackAndSections( iPlayer, iSequence, iSequenceID, iPlaneBinding, iFrameNumber );
    if( !result.mTrack.IsValid() || !result.mSections.Num() )
        return key_opacity;

    FFindOrCreateParameterChannelResult channel_result = FindMaterialOpacityChannel( iPlayer, iSequence, iSequenceID, iPlaneBinding, result.mSections[0] ); // Only the first one, should be nearly always the case
    if( !channel_result.mChannel )
        return key_opacity;

    //---

    TArray<FKeyHandle> key_handles;
    channel_result.mChannel->GetKeys( TRange<FFrameNumber>( iFrameNumber ), nullptr, &key_handles );
    if( !key_handles.Num() )
        return key_opacity;

    key_opacity.mChannel = channel_result.mChannel;
    key_opacity.mSection = result.mSections[0];
    key_opacity.mKeyHandle = key_handles[0];

    return key_opacity;
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
        TArrayView<FMovieSceneDoubleChannel*> channels = section->GetChannelProxy().GetChannels<FMovieSceneDoubleChannel>();
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

static
TArray<FFrameTime>
InnerToOuter( const UMovieSceneSubSection* iOuterSection, TArray<FFrameTime> iInnerKeys )
{
    TArray<FFrameTime> converted_keys;

    const FMovieSceneSequenceTransform InnerToOuterTransform = iOuterSection->OuterToInnerTransform().InverseLinearOnly();
    for( auto key : iInnerKeys )
    {
        const FFrameTime converted_key = key * InnerToOuterTransform;
        converted_keys.Add( converted_key );
    }

    return converted_keys;
}

//static
TArray<FFrameTime>
BoardSequenceHelpers::GetCameraTransformTimesRecursive( const UMovieSceneSubSection& iBoardSection )
{
    TArray<FFrameTime> keys;

    UMovieSceneSequence* innerMovieSceneSequence = iBoardSection.GetSequence();
    if( !innerMovieSceneSequence )
        return keys;

    // if we are on a shot subsequence
    if( innerMovieSceneSequence->IsA<UShotSequence>() )
    {
        TArray<FFrameNumber> subframes = ShotSequenceHelpers::GetCameraTransformTimes( innerMovieSceneSequence );

        TArray<FFrameTime> subkeys( subframes );
        keys = InnerToOuter( &iBoardSection, subkeys );

        return keys;
    }

    // if we are on a board subsequence
    if( innerMovieSceneSequence->IsA<UBoardSequence>() )
    {
        UMovieScene* innerMovieScene = innerMovieSceneSequence->GetMovieScene();
        if( !innerMovieScene )
            return keys;

        UMovieSceneCinematicBoardTrack* board_track = innerMovieScene->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
        if( !board_track )
            return keys;

        TArray<FFrameTime> subkeys;
        for( auto section : board_track->GetAllSections() )
        {
            UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );
            TArray<FFrameTime> section_keys;
            section_keys = GetCameraTransformTimesRecursive( *subsection );

            subkeys.Append( section_keys );
        }

        keys = InnerToOuter( &iBoardSection, subkeys );

        return keys;
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

//---

//static
FChannelProxyBySectionMap
BoardSequenceHelpers::BuildCameraTransformChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSubSection, iSequenceID );

    return ShotSequenceHelpers::BuildCameraTransformChannelProxy( iPlayer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
FChannelProxyBySectionMap
ShotSequenceHelpers::BuildCameraTransformChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    FChannelProxyBySectionMap map;

    FGuid camera_binding;
    /*ACineCameraActor* camera =*/ ShotSequenceHelpers::GetCamera( iPlayer, iSequence, iSequenceID, &camera_binding );

    //---

    TArray<UMovieScene3DTransformSection*> camera_transform_sections = ShotSequenceHelpers::GetCameraTransformSections( iPlayer, iSequence, iSequenceID, camera_binding );
    for( auto camera_transform_section : camera_transform_sections )
    {
        FMovieSceneChannelProxyData ChannelIndirection;

        const FMovieSceneChannelEntry* DoubleChannelEntry = camera_transform_section->GetChannelProxy().FindEntry( FMovieSceneDoubleChannel::StaticStruct()->GetFName() );
        if( DoubleChannelEntry )
        {
#if WITH_EDITOR
            TArrayView<FMovieSceneChannel* const>              DoubleChannels = DoubleChannelEntry->GetChannels();
            TArrayView<const FMovieSceneChannelMetaData>       MetaData = DoubleChannelEntry->GetMetaData();
            TArrayView<const TMovieSceneExternalValue<double>> MetaDataExt = DoubleChannelEntry->GetAllExtendedEditorData<FMovieSceneDoubleChannel>();

            for( int32 Index = 0; Index < DoubleChannels.Num(); ++Index )
            {
                ChannelIndirection.Add( *static_cast<FMovieSceneDoubleChannel*>( DoubleChannels[Index] ), MetaData[Index], MetaDataExt[Index] );
            }
#else
            TArrayView<FMovieSceneChannel* const>              DoubleChannels = DoubleChannelEntry->GetChannels();

            for( int32 Index = 0; Index < DoubleChannels.Num(); ++Index )
            {
                ChannelIndirection.Add( *static_cast<FMovieSceneDoubleChannel*>( DoubleChannels[Index] ) );
            }
#endif
        }

        TSharedPtr<FMovieSceneChannelProxy> ChannelProxy = MakeShared<FMovieSceneChannelProxy>( MoveTemp( ChannelIndirection ) );

        map.Add( camera_transform_section, ChannelProxy );

        // UDN: Hook into TransformSection::OnSignatureChangedEvent to invalidate this section's channel proxy if the transform is changed.
        // Set the delegate to the whole subsequence, then every changes (even removing section) will call the it
        //if( !camera_transform_section->OnSignatureChanged().IsBoundToObject( this ) )
        //    camera_transform_section->OnSignatureChanged().AddUObject( this, &UMovieSceneCinematicBoardSection::HandleInvalidateChannelProxy );
    }

    return map;
}

//static
TMap<FGuid, FChannelProxyBySectionMap>
BoardSequenceHelpers::BuildPlanesTransformChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSubSection, iSequenceID );

    return ShotSequenceHelpers::BuildPlanesTransformChannelProxy( iPlayer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
TMap<FGuid, FChannelProxyBySectionMap>
ShotSequenceHelpers::BuildPlanesTransformChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    TMap<FGuid, FChannelProxyBySectionMap> maps;

    TArray<APlaneActor*> planes;
    TArray<FGuid> bindings;
    /*int plane_count =*/ ShotSequenceHelpers::GetAllPlanes( iPlayer, iSequence, iSequenceID, EGetPlane::kAll, &planes, &bindings );

    for( auto binding : bindings )
    {
        FChannelProxyBySectionMap map;

        //---

        TArray<UMovieScene3DTransformSection*> plane_transform_sections = ShotSequenceHelpers::GetPlaneTransformSections( iPlayer, iSequence, iSequenceID, binding );
        for( auto plane_transform_section : plane_transform_sections )
        {
            FMovieSceneChannelProxyData ChannelIndirection;

            const FMovieSceneChannelEntry* DoubleChannelEntry = plane_transform_section->GetChannelProxy().FindEntry( FMovieSceneDoubleChannel::StaticStruct()->GetFName() );
            if( DoubleChannelEntry )
            {
#if WITH_EDITOR
                TArrayView<FMovieSceneChannel* const>              DoubleChannels = DoubleChannelEntry->GetChannels();
                TArrayView<const FMovieSceneChannelMetaData>       MetaData = DoubleChannelEntry->GetMetaData();
                TArrayView<const TMovieSceneExternalValue<double>> MetaDataExt = DoubleChannelEntry->GetAllExtendedEditorData<FMovieSceneDoubleChannel>();

                for( int32 Index = 0; Index < DoubleChannels.Num(); ++Index )
                {
                    ChannelIndirection.Add( *static_cast<FMovieSceneDoubleChannel*>( DoubleChannels[Index] ), MetaData[Index], MetaDataExt[Index] );
                }
#else
                TArrayView<FMovieSceneChannel* const>              DoubleChannels = DoubleChannelEntry->GetChannels();

                for( int32 Index = 0; Index < DoubleChannels.Num(); ++Index )
                {
                    ChannelIndirection.Add( *static_cast<FMovieSceneDoubleChannel*>( DoubleChannels[Index] ) );
                }
#endif
            }

            TSharedPtr<FMovieSceneChannelProxy> ChannelProxy = MakeShared<FMovieSceneChannelProxy>( MoveTemp( ChannelIndirection ) );

            map.Add( plane_transform_section, ChannelProxy );

            // UDN: Hook into TransformSection::OnSignatureChangedEvent to invalidate this section's channel proxy if the transform is changed.
            // Set the delegate to the whole subsequence, then every changes (even removing section) will call the it
            //if( !camera_transform_section->OnSignatureChanged().IsBoundToObject( this ) )
            //    camera_transform_section->OnSignatureChanged().AddUObject( this, &UMovieSceneCinematicBoardSection::HandleInvalidateChannelProxy );
        }

        maps.Add( binding, map );
    }

    return maps;
}

//static
TMap<FGuid, FChannelProxyBySectionMap>
BoardSequenceHelpers::BuildPlanesMaterialChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSubSection, iSequenceID );

    return ShotSequenceHelpers::BuildPlanesMaterialChannelProxy( iPlayer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
TMap<FGuid, FChannelProxyBySectionMap>
ShotSequenceHelpers::BuildPlanesMaterialChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    TMap<FGuid, FChannelProxyBySectionMap> maps;

    TArray<APlaneActor*> planes;
    TArray<FGuid> bindings;
    /*int plane_count =*/ ShotSequenceHelpers::GetAllPlanes( iPlayer, iSequence, iSequenceID, EGetPlane::kAll, &planes, &bindings );

    for( auto binding : bindings )
    {
        FChannelProxyBySectionMap map;

        //---

        FFindOrCreateMaterialDrawingResult result = FindMaterialDrawingTrackAndSections( iPlayer, iSequence, iSequenceID, binding );

        for( auto plane_material_section : result.mSections )
        {
            FMovieSceneChannelProxyData ChannelIndirection;

            const FMovieSceneChannelEntry* ObjectPathChannelEntry = plane_material_section->GetChannelProxy().FindEntry( FMovieSceneObjectPathChannel::StaticStruct()->GetFName() );
            if( ObjectPathChannelEntry )
            {
#if WITH_EDITOR
                TArrayView<FMovieSceneChannel* const>                   ObjectPathChannels = ObjectPathChannelEntry->GetChannels();
                TArrayView<const FMovieSceneChannelMetaData>            MetaData = ObjectPathChannelEntry->GetMetaData();
                TArrayView<const TMovieSceneExternalValue<UObject*>>    MetaDataExt = ObjectPathChannelEntry->GetAllExtendedEditorData<FMovieSceneObjectPathChannel>();

                for( int32 Index = 0; Index < ObjectPathChannels.Num(); ++Index )
                {
                    ChannelIndirection.Add( *static_cast<FMovieSceneObjectPathChannel*>( ObjectPathChannels[Index] ), MetaData[Index], MetaDataExt[Index] );
                }
#else
                TArrayView<FMovieSceneChannel* const>                   ObjectPathChannels = ObjectPathChannelEntry->GetChannels();

                for( int32 Index = 0; Index < ObjectPathChannels.Num(); ++Index )
                {
                    ChannelIndirection.Add( *static_cast<FMovieSceneObjectPathChannel*>( ObjectPathChannels[Index] ) );
                }
#endif
            }

            TSharedPtr<FMovieSceneChannelProxy> ChannelProxy = MakeShared<FMovieSceneChannelProxy>( MoveTemp( ChannelIndirection ) );

            map.Add( plane_material_section, ChannelProxy );

            // UDN: Hook into TransformSection::OnSignatureChangedEvent to invalidate this section's channel proxy if the transform is changed.
            // Set the delegate to the whole subsequence, then every changes (even removing section) will call the it
            //if( !camera_transform_section->OnSignatureChanged().IsBoundToObject( this ) )
            //    camera_transform_section->OnSignatureChanged().AddUObject( this, &UMovieSceneCinematicBoardSection::HandleInvalidateChannelProxy );
        }

        maps.Add( binding, map );
    }

    return maps;
}

//static
TMap<FGuid, FChannelProxyBySectionMap>
BoardSequenceHelpers::BuildPlanesOpacityChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSubSection, iSequenceID );

    return ShotSequenceHelpers::BuildPlanesOpacityChannelProxy( iPlayer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
TMap<FGuid, FChannelProxyBySectionMap>
ShotSequenceHelpers::BuildPlanesOpacityChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    TMap<FGuid, FChannelProxyBySectionMap> maps;

    TArray<APlaneActor*> planes;
    TArray<FGuid> bindings;
    /*int plane_count =*/ ShotSequenceHelpers::GetAllPlanes( iPlayer, iSequence, iSequenceID, EGetPlane::kAll, &planes, &bindings );

    for( auto binding : bindings )
    {
        FChannelProxyBySectionMap map;

        //---

        FFindOrCreateMaterialParameterResult result = FindMaterialParameterTrackAndSections( iPlayer, iSequence, iSequenceID, binding );

        for( auto plane_opacity_section : result.mSections )
        {
            FMovieSceneChannelProxyData ChannelIndirection;

            const FMovieSceneChannelEntry* FloatChannelEntry = plane_opacity_section->GetChannelProxy().FindEntry( FMovieSceneFloatChannel::StaticStruct()->GetFName() );
            if( FloatChannelEntry )
            {
#if WITH_EDITOR
                TArrayView<FMovieSceneChannel* const>                   FloatChannels = FloatChannelEntry->GetChannels();
                TArrayView<const FMovieSceneChannelMetaData>            MetaData = FloatChannelEntry->GetMetaData();
                TArrayView<const TMovieSceneExternalValue<float>>       MetaDataExt = FloatChannelEntry->GetAllExtendedEditorData<FMovieSceneFloatChannel>();

                for( int32 Index = 0; Index < FloatChannels.Num(); ++Index )
                {
                    if( MetaData[Index].Name.IsEqual( TEXT("DrawingOpacity") ) )
                        ChannelIndirection.Add( *static_cast<FMovieSceneFloatChannel*>( FloatChannels[Index] ), MetaData[Index], MetaDataExt[Index] );
                }
#else
                TArrayView<FMovieSceneChannel* const>                   FloatChannels = FloatChannelEntry->GetChannels();

                for( int32 Index = 0; Index < FloatChannels.Num(); ++Index )
                {
                    ChannelIndirection.Add( *static_cast<FMovieSceneFloatChannel*>( FloatChannels[Index] ) );
                }
#endif
            }

            TSharedPtr<FMovieSceneChannelProxy> ChannelProxy = MakeShared<FMovieSceneChannelProxy>( MoveTemp( ChannelIndirection ) );

            map.Add( plane_opacity_section, ChannelProxy );

            // UDN: Hook into TransformSection::OnSignatureChangedEvent to invalidate this section's channel proxy if the transform is changed.
            // Set the delegate to the whole subsequence, then every changes (even removing section) will call the it
            //if( !camera_transform_section->OnSignatureChanged().IsBoundToObject( this ) )
            //    camera_transform_section->OnSignatureChanged().AddUObject( this, &UMovieSceneCinematicBoardSection::HandleInvalidateChannelProxy );
        }

        maps.Add( binding, map );
    }

    return maps;
}

//---

//static
TArray<UStoryNote*>
UMoviePipelineStoryboardBlueprintLibrary::GetNotes( const UMoviePipeline* iMoviePipeline )
{
    TArray<UStoryNote*> notes;

    ULevelSequence* level_sequence = iMoviePipeline->GetTargetSequence();

    FFrameRate effective_framerate = iMoviePipeline->GetPipelineMasterConfig()->GetEffectiveFrameRate( iMoviePipeline->GetTargetSequence() );

    //FTimecode master_timecode = UMoviePipelineBlueprintLibrary::GetMasterTimecode( iMoviePipeline );
    FFrameNumber master_current_frame_in_levelsequence = UMoviePipelineBlueprintLibrary::GetMasterFrameNumber( iMoviePipeline );
    FFrameNumber master_current_frame = FFrameRate::TransformTime( master_current_frame_in_levelsequence, effective_framerate, level_sequence->GetMovieScene()->GetTickResolution() ).GetFrame();

    ////FTimecode shot_timecode = UMoviePipelineBlueprintLibrary::GetCurrentShotTimecode( iMoviePipeline );
    //FFrameNumber shot_current_frame_in_levelsequence = UMoviePipelineBlueprintLibrary::GetCurrentShotFrameNumber( iMoviePipeline );
    //FFrameNumber shot_current_frame = FFrameRate::TransformTime( master_current_frame_in_levelsequence, effective_framerate, level_sequence->GetMovieScene()->GetTickResolution() ).GetFrame();

    //---

    using FTrackAndSectionInfo = TTuple<UMovieSceneTrack*, UMovieSceneSection*>;
    using FSequenceFrameInfo = TTuple<FMovieSceneSequenceID, TArray<FTrackAndSectionInfo>>;

    struct FSequenceNoteVisitor
        : UE::MovieScene::ISequenceVisitor
    {
        void SortInfo()
        {
            for( auto& info : mInfo )
            {
                TArrayView<FTrackAndSectionInfo> track_and_section_view( info.Get<1>() );

                track_and_section_view.StableSort( []( const FTrackAndSectionInfo& iA, const FTrackAndSectionInfo& iB )
                                                   {
                                                       UMovieSceneSection* sectionA = iA.Get<1>();
                                                       UMovieSceneSection* sectionB = iB.Get<1>();

                                                       TRangeBound<FFrameNumber> LowerBoundA = sectionA->GetRange().GetLowerBound();
                                                       return TRangeBound<FFrameNumber>::MinLower( LowerBoundA, sectionB->GetRange().GetLowerBound() ) == LowerBoundA;
                                                   } );
                track_and_section_view.StableSort( []( const FTrackAndSectionInfo& iA, const FTrackAndSectionInfo& iB )
                                                   {
                                                       UMovieSceneSection* sectionA = iA.Get<1>();
                                                       UMovieSceneSection* sectionB = iB.Get<1>();

                                                       return sectionA->GetRowIndex() < sectionB->GetRowIndex();
                                                   } );

#if WITH_EDITORONLY_DATA
                track_and_section_view.StableSort( []( const FTrackAndSectionInfo& iA, const FTrackAndSectionInfo& iB )
                                                   {
                                                       UMovieSceneTrack* trackA = iA.Get<0>();
                                                       UMovieSceneTrack* trackB = iB.Get<0>();

                                                       return trackA->GetSortingOrder() < trackB->GetSortingOrder();
                                                   } );
#endif
            }

        }

        virtual void VisitSection( UMovieSceneTrack* iTrack, UMovieSceneSection* iSection, const FGuid& iGuid, const UE::MovieScene::FSubSequenceSpace& iLocalSpace )
        {
            UMovieSceneNoteSection* note_section = Cast<UMovieSceneNoteSection>( iSection );
            if( !note_section || !note_section->GetNote() )
                return;

            FFrameTime local_reference_time = mReferenceFrame * iLocalSpace.RootToSequenceTransform;
            if( !iSection->IsTimeWithinSection( local_reference_time.GetFrame() ) )
                return;

            FMovieSceneSequenceID sequence_id_to_insert = iLocalSpace.SequenceID;
            FSequenceFrameInfo* sequence_frame_info = mInfo.FindByPredicate( [sequence_id_to_insert]( const FSequenceFrameInfo& iInfo ) { return iInfo.Get<0>() == sequence_id_to_insert; } );
            if( sequence_frame_info )
            {
                FTrackAndSectionInfo track_and_section( iTrack, iSection );
                sequence_frame_info->Get<1>().Add( track_and_section );
            }
            else
            {
                FTrackAndSectionInfo track_and_section( iTrack, iSection );
                FSequenceFrameInfo info( sequence_id_to_insert, TArray<FTrackAndSectionInfo>( { track_and_section } ) );
                mInfo.Insert( info, 0 );
            }

            SortInfo();
        }

        FFrameNumber mReferenceFrame; // In tick resolution

        TArray<FSequenceFrameInfo> mInfo;
    };


    UE::MovieScene::FSequenceVisitParams params;
    params.bVisitSections = true;
    params.bVisitMasterTracks = true;
    params.bVisitSubSequences = true;

    FSequenceNoteVisitor note_visitor;
    note_visitor.mReferenceFrame = master_current_frame;

    // Visit all notes
    VisitSequence( level_sequence, params, note_visitor );

    for( auto info : note_visitor.mInfo )
    {
        for( auto track_and_section : info.Get<1>() )
        {
            UMovieSceneNoteSection* note_section = Cast<UMovieSceneNoteSection>( track_and_section.Get<1>() );
            check( note_section && note_section->GetNote() );

            notes.Add( note_section->GetNote() );
        }
    }

    return notes;
}

#undef LOCTEXT_NAMESPACE
