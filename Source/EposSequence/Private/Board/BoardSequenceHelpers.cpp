// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Board/BoardSequenceHelpers.h"

//#include "AssetRegistryModule.h"
//#include "AssetToolsModule.h"
//#include "Channels/MovieSceneChannelProxy.h"
//#include "Channels/MovieSceneFloatChannel.h"
//#include "CineCameraActor.h"
//#include "CineCameraComponent.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "Evaluation/MovieSceneSequenceHierarchy.h"
//#include "Engine/StaticMesh.h"
//#include "Engine/StaticMeshActor.h"
//#include "Materials/MaterialInstanceConstant.h"
#include "IMovieScenePlayer.h"
#include "MovieScene.h"
#include "MovieSceneCommonHelpers.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "Sections/MovieSceneSubSection.h"
//#include "Sections/MovieScenePrimitiveMaterialSection.h"
//#include "Tracks/MovieScene3DTransformTrack.h"
//#include "Tracks/MovieSceneCinematicShotTrack.h"
//#include "Tracks/MovieScenePrimitiveMaterialTrack.h"

//#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
//#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
//#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"

#define LOCTEXT_NAMESPACE "BoardSequenceHelpers"

BoardSequenceHelpers::FInnerSequenceResult
BoardSequenceHelpers::GetInnerSequence( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceId )
{
    FInnerSequenceResult result = { nullptr, MovieSceneSequenceID::Invalid, nullptr, 0 };

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
    UMovieSceneSubSection* subsection = CastChecked<UMovieSceneSubSection>( section );

    FInnerSequenceResult result = GetInnerSequence( iPlayer, *subsection, iSequenceId );
    result.mInnerTime = iFrameNumber * subsection->OuterToInnerTransform();

    return result;
}

#undef LOCTEXT_NAMESPACE
