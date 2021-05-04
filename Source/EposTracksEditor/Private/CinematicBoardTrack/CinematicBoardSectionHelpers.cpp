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
#include "Sections/MovieScenePrimitiveMaterialSection.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Tracks/MovieScenePrimitiveMaterialTrack.h"
#include "Engine/StaticMeshActor.h"
#include "CineCameraActor.h"

#include "Board/BoardSequence.h"
#include "Board/BoardSequenceHelpers.h"
#include "CinematicBoardTrack/CinematicBoardTrackEditor.h"
#include "CinematicBoardTrack/MetaChannelProxy.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposMovieSceneSequence.h"
#include "Helpers/SectionHelpersConvert.h"
#include "Shot/ShotSequence.h"
#include "Shot/ShotSequenceHelpers.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"

//---

//static
TSharedPtr<FMetaFloatChannel>
CinematicBoardSectionKeysHelpers::BuildCameraTransformMetaChannel( const TSharedPtr<FMovieSceneChannelProxy> iChannelProxy, const FFrameNumber& iMergeTolerance )
{
    TSharedPtr<FMetaFloatChannel> meta_channel = MakeShared<FMetaFloatChannel>( iMergeTolerance );

    meta_channel->Build( iChannelProxy );

    return meta_channel;
}

//---

//static
TMap<FGuid, TSharedPtr<FMetaFloatChannel>>
CinematicBoardSectionKeysHelpers::BuildPlanesTransformMetaChannel( const TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> iChannelProxies, const FFrameNumber& iMergeTolerance )
{
    TMap<FGuid, TSharedPtr<FMetaFloatChannel>> meta_channels;

    for( const auto& pair : iChannelProxies )
    {
        FGuid guid = pair.Key;
        TSharedPtr<FMovieSceneChannelProxy> proxy = pair.Value;

        TSharedPtr<FMetaFloatChannel> meta_channel = MakeShared<FMetaFloatChannel>( iMergeTolerance );
        meta_channel->Build( proxy );

        meta_channels.Add( guid, meta_channel );
    }

    return meta_channels;
}

//---

//static
TMap<FGuid, TSharedPtr<FMetaMaterialChannel>>
CinematicBoardSectionKeysHelpers::BuildPlanesMaterialMetaChannel( const TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> iChannelProxies, const FFrameNumber& iMergeTolerance )
{
    TMap<FGuid, TSharedPtr<FMetaMaterialChannel>> meta_channels;

    for( const auto& pair : iChannelProxies )
    {
        FGuid guid = pair.Key;
        TSharedPtr<FMovieSceneChannelProxy> proxy = pair.Value;

        TSharedPtr<FMetaMaterialChannel> meta_channel = MakeShared<FMetaMaterialChannel>( iMergeTolerance );
        meta_channel->Build( proxy );

        meta_channels.Add( guid, meta_channel );
    }

    return meta_channels;
}
