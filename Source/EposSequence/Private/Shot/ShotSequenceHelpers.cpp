// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Shot/ShotSequenceHelpers.h"

//#include "AssetRegistryModule.h"
//#include "AssetToolsModule.h"
//#include "Channels/MovieSceneChannelProxy.h"
//#include "Channels/MovieSceneFloatChannel.h"
#include "CineCameraActor.h"
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

#define LOCTEXT_NAMESPACE "ShotSequenceHelpers"


//static
ACineCameraActor*
ShotSequenceHelpers::GetCamera( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid* oCameraBinding )
{
    UMovieScene* movieScene = iSequence->GetMovieScene();
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


#undef LOCTEXT_NAMESPACE
