// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Board/BoardSequenceHelpers.h"

#include "Compilation/MovieSceneCompiledDataManager.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "Evaluation/MovieSceneSequenceHierarchy.h"
#include "IMovieScenePlayer.h"
#include "MovieScene.h"
#include "MovieSceneCommonHelpers.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "Sections/MovieSceneSubSection.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "Shot/ShotSequence.h"
#include "Shot/ShotSequenceHelpers.h"

#define LOCTEXT_NAMESPACE "BoardSequenceHelpers"

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
TSharedPtr<FMovieSceneChannelProxy>
BoardSequenceHelpers::BuildCameraTransformChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSubSection, iSequenceID );

    return ShotSequenceHelpers::BuildCameraTransformChannelProxy( iPlayer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>>
BoardSequenceHelpers::BuildPlanesTransformChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSubSection, iSequenceID );

    return ShotSequenceHelpers::BuildPlanesTransformChannelProxy( iPlayer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>>
BoardSequenceHelpers::BuildPlanesMaterialChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID )
{
    FInnerSequenceResult result = GetInnerSequence( iPlayer, iSubSection, iSequenceID );

    return ShotSequenceHelpers::BuildPlanesMaterialChannelProxy( iPlayer, result.mInnerSequence, result.mInnerSequenceId );
}

#undef LOCTEXT_NAMESPACE
