// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardTrack/CinematicBoardSectionHelpers.h"

#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "ScopedTransaction.h"
#include "MovieSceneTrack.h"
#include "MovieScene.h"
#include "MovieSceneToolHelpers.h"
#include "MovieSceneTimeHelpers.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "CommonMovieSceneTools.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Engine/StaticMeshActor.h"
#include "CineCameraActor.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/CinematicBoardTrackEditor.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposMovieSceneSequence.h"
#include "Helpers/SectionsHelpersConvert.h"
#include "Helpers/SectionsHelpersResize.h"
#include "Shot/ShotSequence.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutHelpers.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"

//---

FInnerSequenceData::FInnerSequenceData()
    : mInnerSequenceID()
    , mHierarchy( nullptr )
    , mPlayer( nullptr )
    , mNode( nullptr )
    , mSubData( nullptr )
    , mInnerMovieSceneSequence( nullptr )
    , mInnerMovieScene( nullptr )
{
}

FInnerSequenceData::FInnerSequenceData( const FMovieSceneSequenceIDRef iID, const FMovieSceneSequenceHierarchy* iHierarchy, IMovieScenePlayer* ioPlayer )
    : mInnerSequenceID( iID )
    , mHierarchy( iHierarchy )
    , mPlayer( ioPlayer )
    , mNode( nullptr )
    , mSubData( nullptr )
    , mInnerMovieSceneSequence( nullptr )
    , mInnerMovieScene( nullptr )
{
}

bool
FInnerSequenceData::IsInitialized() const
{
    return mInnerSequenceID.IsValid();
}

bool
FInnerSequenceData::IsFilled() const
{
    return !!mInnerMovieScene;
}

FMovieSceneSequenceID
FInnerSequenceData::GetInnerSequenceID() const
{
    return mInnerSequenceID;
}

UMovieSceneSequence*
FInnerSequenceData::GetInnerSequence() const
{
    return mInnerMovieSceneSequence;
}

UMovieScene*
FInnerSequenceData::GetInnerMovieScene() const
{
    return mInnerMovieScene;
}

void
FInnerSequenceData::InitializeFromSubSection( IMovieScenePlayer* iPlayer, FMovieSceneSequenceIDRef iSequenceID, const UMovieSceneSubSection& iSubSection )
{
    mInnerSequenceID = FMovieSceneSequenceID();
    mHierarchy = nullptr;
    mPlayer = nullptr;
    mNode = nullptr;
    mSubData = nullptr;
    mInnerMovieSceneSequence = nullptr;
    mInnerMovieScene = nullptr;

    if( !iPlayer )
        return;

    const UMovieSceneSubSection&            sectionObject = iSubSection;
    const FMovieSceneSequenceID             thisSequenceID = iSequenceID;
    const FMovieSceneSequenceID             targetSequenceID = sectionObject.GetSequenceID();
    const FMovieSceneSequenceHierarchy*     hierarchy = iPlayer->GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( iPlayer->GetEvaluationTemplate().GetCompiledDataID() );

    if( !hierarchy )
        return;

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

    if( !innerSequenceID )
        return;

    mInnerSequenceID = *innerSequenceID;
    mHierarchy = hierarchy;
    mPlayer = iPlayer;
}

void
FInnerSequenceData::Fill()
{
    if( !IsInitialized() )
        return;

    const FMovieSceneSequenceHierarchyNode* Node = mHierarchy->FindNode( mInnerSequenceID );
    const FMovieSceneSubSequenceData*       SubData = mHierarchy->FindSubData( mInnerSequenceID );
    if( !ensure( SubData && Node ) )
        return;

    UMovieSceneSequence* InnerSequence = SubData->GetSequence();
    UMovieScene*         InnerMovieScene = InnerSequence ? InnerSequence->GetMovieScene() : nullptr;
    if( !InnerMovieScene )
        return;

    mNode = Node;
    mSubData = SubData;
    mInnerMovieSceneSequence = InnerSequence;
    mInnerMovieScene = InnerMovieScene;
}

//---
//---
//---

//static
int
CinematicBoardSectionBindingHelpers::GetMaxPlaneBindings( const UMovieSceneTrack& iTrack, ISequencer& iSequencer )
{
    int count = 0;
    for( auto section : iTrack.GetAllSections() )
    {
        UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );
        TArray<FMovieScenePossessable> bindings = GetPlaneBindings( *subsection, iSequencer );

        count = FMath::Max( count, bindings.Num() );
    }

    return count;
}

//static
TArray<FMovieScenePossessable>
CinematicBoardSectionBindingHelpers::GetPlaneBindings( const UMovieSceneSubSection& iSection, ISequencer& iSequencer )
{
    TArray<FMovieScenePossessable> bindings;

    FInnerSequenceData result;
    result.InitializeFromSubSection( &iSequencer, iSequencer.GetFocusedTemplateID(), iSection );
    if( !result.IsInitialized() )
        return bindings;

    result.Fill();
    if( !result.IsFilled() )
        return bindings;

    FMovieSceneSequenceID inner_sequence_id = result.GetInnerSequenceID();
    UMovieScene* inner_movie_scene = result.GetInnerMovieScene();

    AStaticMeshActor* plane = nullptr;
    for( int i = 0; i < inner_movie_scene->GetPossessableCount(); i++ )
    {
        FMovieScenePossessable possessable = inner_movie_scene->GetPossessable( i );

        for( auto Object : iSequencer.FindBoundObjects( possessable.GetGuid(), inner_sequence_id ) )
        {
            plane = Cast<AStaticMeshActor>( Object.Get() );
            if( plane )
                bindings.Add( possessable );
        }
    }

    return bindings;
}

//static
FMovieScenePossessable
CinematicBoardSectionBindingHelpers::GetCameraBinding( const UMovieSceneSubSection& iSection, ISequencer& iSequencer )
{
    FMovieScenePossessable binding;

    FInnerSequenceData result;
    result.InitializeFromSubSection( &iSequencer, iSequencer.GetFocusedTemplateID(), iSection );
    if( !result.IsInitialized() )
        return binding;

    result.Fill();
    if( !result.IsFilled() )
        return binding;

    FMovieSceneSequenceID inner_sequence_id = result.GetInnerSequenceID();
    UMovieScene* inner_movie_scene = result.GetInnerMovieScene();

    ACineCameraActor* camera = nullptr;
    for( int i = 0; i < inner_movie_scene->GetPossessableCount(); i++ )
    {
        FMovieScenePossessable possessable = inner_movie_scene->GetPossessable( i );

        for( auto Object : iSequencer.FindBoundObjects( possessable.GetGuid(), inner_sequence_id ) )
        {
            camera = Cast<ACineCameraActor>( Object.Get() );
            if( camera )
                return possessable;
        }
    }

    return binding;
}

//static
TArray<UMovieScene3DTransformSection*>
CinematicBoardSectionBindingHelpers::GetCameraTransformSections( UMovieSceneSequence* iInnerSequence, ISequencer& iSequencer )
{
    TArray<UMovieScene3DTransformSection*> sections;

    UMovieScene* inner_moviescene = iInnerSequence ? iInnerSequence->GetMovieScene() : nullptr;
    if( !inner_moviescene )
        return sections;

    FMovieScenePossessable possessable_camera;
    for( int i = 0; i < inner_moviescene->GetPossessableCount(); i++ )
    {
        FMovieScenePossessable possessable = inner_moviescene->GetPossessable( i );

        for( auto Object : iInnerSequence->LocateBoundObjects( possessable.GetGuid(), iSequencer.GetPlaybackContext() ) ) //TODO: is it correct or use iSequencer.FindBoundObjects(...) instead ?
        {
            ACineCameraActor* camera = Cast<ACineCameraActor>( Object );
            if( camera )
                possessable_camera = possessable;
        }

        if( possessable_camera.GetGuid().IsValid() )
            break;
    }

    if( !possessable_camera.GetGuid().IsValid() )
        return sections;

    UMovieSceneTrack* track = inner_moviescene->FindTrack<UMovieScene3DTransformTrack>( possessable_camera.GetGuid() );
    if( !track )
        return sections;

    for( auto section : track->GetAllSections() )
        sections.Add( Cast<UMovieScene3DTransformSection>( section ) );

    return sections;
}

//---
//---
//---

//static
TArray<FFrameTime>
CinematicBoardSectionKeysHelpers::FindCameraTransformKeysRecursive( const UMovieSceneSubSection& iBoardSection )
{
    TArray<FFrameTime> keys;

    UMovieSceneSequence* innerMovieSceneSequence = iBoardSection.GetSequence();
    if( !innerMovieSceneSequence )
        return keys;

    // if we are on a shot subsequence
    if( innerMovieSceneSequence->IsA<UShotSequence>() )
    {
        TArray<FFrameTime> subkeys = MovieSceneSingleCameraCutHelpers::GetCameraTransformKeys( innerMovieSceneSequence );

        keys = SectionsHelpersConvert::InnerToOuter( &iBoardSection, subkeys );

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
            section_keys = FindCameraTransformKeysRecursive( *subsection );

            subkeys.Append( section_keys );
        }

        keys = SectionsHelpersConvert::InnerToOuter( &iBoardSection, subkeys );

        return keys;
    }

    return keys;
}

//static
TArray<double>
CinematicBoardSectionKeysHelpers::BuildThumbnailKeys( const UMovieSceneSubSection& iSubSection )
{
    TArray<FFrameTime> keys_as_frame = FindCameraTransformKeysRecursive( iSubSection );
    return SectionsHelpersConvert::FrameToSecond( &iSubSection, keys_as_frame );
}

//static
TSharedPtr<FMovieSceneChannelProxy>
CinematicBoardSectionKeysHelpers::BuildCameraTransformChannelProxy( const UMovieSceneSubSection& iSubSection, ISequencer& iSequencer )
{
    FMovieSceneChannelProxyData ChannelIndirection;

    UMovieSceneSequence* inner_sequence = iSubSection.GetSequence();

    //---

    int sort = 0;

    TArray<UMovieScene3DTransformSection*> camera_transform_sections = CinematicBoardSectionBindingHelpers::GetCameraTransformSections( inner_sequence, iSequencer );
    int section_index = -1;
    for( auto camera_transform_section : camera_transform_sections )
    {
        section_index++;

        const FMovieSceneChannelEntry* FloatChannelEntry = camera_transform_section->GetChannelProxy().FindEntry( FMovieSceneFloatChannel::StaticStruct()->GetFName() );
        if( FloatChannelEntry )
        {
            TArrayView<FMovieSceneChannel* const>        FloatChannels = FloatChannelEntry->GetChannels();
            TArrayView<const FMovieSceneChannelMetaData>      MetaData = FloatChannelEntry->GetMetaData();
            TArrayView<const TMovieSceneExternalValue<float>> MetaDataExt = FloatChannelEntry->GetAllExtendedEditorData<FMovieSceneFloatChannel>();

            for( int32 Index = 0; Index < FloatChannels.Num(); ++Index )
            {
                FMovieSceneChannelMetaData MetaDataEntry = MetaData[Index];
                MetaDataEntry.bCanCollapseToTrack = false;
                MetaDataEntry.SortOrder = sort++;
                MetaDataEntry.DisplayText = FText::Format( FText::FromString( "{0} - {1}" ), MetaDataEntry.Group, MetaDataEntry.DisplayText, sort );
                MetaDataEntry.Group = FText::Format( FText::FromString( "Camera.Section{0}" ), section_index ); // Must be after MetaDataEntry.DisplayText because it used the old MetaDataEntry.Group
                //MetaDataEntry.Color = FLinearColor( 0, 0, 1, 0.8 );

                //UMovieScene3DTransformTrack* track = camera_transform_section->GetTypedOuter<UMovieScene3DTransformTrack>();
                //FGuid id = track->FindObjectBindingGuid();
                //FMovieSceneBinding* binding = inner_sequence->GetMovieScene()->FindBinding( id );
                //UE_LOG( LogTemp, Warning, TEXT( "%s: group: %s - label: %s - index: %d - sort: %d" ), *binding->GetName(), *MetaDataEntry.Group.ToString(), *MetaDataEntry.Name.ToString(), Index, sort );

                ChannelIndirection.Add( *static_cast<FMovieSceneFloatChannel*>( FloatChannels[Index] ), MetaDataEntry, MetaDataExt[Index] );
            }
        }

        // UDN: Hook into TransformSection::OnSignatureChangedEvent to invalidate this section's channel proxy if the transform is changed.
        // Set the delegate to the whole subsequence, then every changes (even removing section) will call the it
        //if( !camera_transform_section->OnSignatureChanged().IsBoundToObject( this ) )
        //    camera_transform_section->OnSignatureChanged().AddUObject( this, &UMovieSceneCinematicBoardSection::HandleInvalidateChannelProxy );
    }

    TSharedPtr<FMovieSceneChannelProxy> ChannelProxy = MakeShared<FMovieSceneChannelProxy>( MoveTemp( ChannelIndirection ) );
    return ChannelProxy;
}

//static
TArray<FFrameTime>
CinematicBoardSectionKeysHelpers::FindPlaneTransformKeys( const UMovieSceneSubSection& iBoardSection, FMovieScenePossessable iPossessable )
{
    TArray<FFrameTime> keys;

    UMovieSceneSequence* innerMovieSceneSequence = iBoardSection.GetSequence();
    if( !innerMovieSceneSequence )
        return keys;

    // if we are on a shot subsequence
    if( innerMovieSceneSequence->IsA<UShotSequence>() )
    {
        TArray<FFrameTime> subkeys = MovieSceneSingleCameraCutHelpers::GetPlaneTransformKeys( innerMovieSceneSequence, iPossessable );

        keys = SectionsHelpersConvert::InnerToOuter( &iBoardSection, subkeys );

        return keys;
    }

    return keys;
}

//static
TMap<FGuid, TArray<double>>
CinematicBoardSectionKeysHelpers::BuildPlaneTransformsKeys( const UMovieSceneSubSection& iSubSection, ISequencer& iSequencer )
{
    TMap<FGuid, TArray<double>> keys;

    TArray<FMovieScenePossessable> possessables = CinematicBoardSectionBindingHelpers::GetPlaneBindings( iSubSection, iSequencer );
    for( auto possessable : possessables )
    {
        TArray<FFrameTime> keys_as_frame = FindPlaneTransformKeys( iSubSection, possessable );
        keys.Add( possessable.GetGuid() ) = SectionsHelpersConvert::FrameToSecond( &iSubSection, keys_as_frame );
    }

    return keys;
}

//static
TArray<FFrameTime>
CinematicBoardSectionKeysHelpers::FindPlaneMaterialKeys( const UMovieSceneSubSection& iBoardSection, FMovieScenePossessable iPossessable )
{
    TArray<FFrameTime> keys;

    UMovieSceneSequence* innerMovieSceneSequence = iBoardSection.GetSequence();
    if( !innerMovieSceneSequence )
        return keys;

    // if we are on a shot subsequence
    if( innerMovieSceneSequence->IsA<UShotSequence>() )
    {
        TArray<FFrameTime> subkeys = MovieSceneSingleCameraCutHelpers::GetPlaneMaterialKeys( innerMovieSceneSequence, iPossessable );

        keys = SectionsHelpersConvert::InnerToOuter( &iBoardSection, subkeys );

        return keys;
    }

    return keys;
}

//static
TMap<FGuid, TArray<double>>
CinematicBoardSectionKeysHelpers::BuildPlaneMaterialsKeys( const UMovieSceneSubSection& iSubSection, ISequencer& iSequencer )
{
    TMap<FGuid, TArray<double>> keys;

    TArray<FMovieScenePossessable> possessables = CinematicBoardSectionBindingHelpers::GetPlaneBindings( iSubSection, iSequencer );
    for( auto possessable : possessables )
    {
        TArray<FFrameTime> keys_as_frame = FindPlaneMaterialKeys( iSubSection, possessable );
        keys.Add( possessable.GetGuid() ) = SectionsHelpersConvert::FrameToSecond( &iSubSection, keys_as_frame );
    }

    return keys;
}
