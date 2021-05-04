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
TSharedPtr<FMovieSceneChannelProxy>
CinematicBoardSectionKeysHelpers::BuildCameraTransformChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID )
{
    FMovieSceneChannelProxyData ChannelIndirection;

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( iPlayer, iSubSection, iSequenceID );

    FGuid camera_binding;
    /*ACineCameraActor* camera =*/ ShotSequenceHelpers::GetCamera( iPlayer, result.mInnerSequence, result.mInnerSequenceId, &camera_binding );

    //---

    TArray<UMovieScene3DTransformSection*> camera_transform_sections = ShotSequenceHelpers::GetCameraTransformSections( iPlayer, result.mInnerSequence, result.mInnerSequenceId, camera_binding );
    for( auto camera_transform_section : camera_transform_sections )
    {
        const FMovieSceneChannelEntry* FloatChannelEntry = camera_transform_section->GetChannelProxy().FindEntry( FMovieSceneFloatChannel::StaticStruct()->GetFName() );
        if( FloatChannelEntry )
        {
            TArrayView<FMovieSceneChannel* const>             FloatChannels = FloatChannelEntry->GetChannels();
            TArrayView<const FMovieSceneChannelMetaData>      MetaData = FloatChannelEntry->GetMetaData();
            TArrayView<const TMovieSceneExternalValue<float>> MetaDataExt = FloatChannelEntry->GetAllExtendedEditorData<FMovieSceneFloatChannel>();

            for( int32 Index = 0; Index < FloatChannels.Num(); ++Index )
            {
                ChannelIndirection.Add( *static_cast<FMovieSceneFloatChannel*>( FloatChannels[Index] ), MetaData[Index], MetaDataExt[Index] );
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
TSharedPtr<FMetaFloatChannel>
CinematicBoardSectionKeysHelpers::BuildCameraTransformMetaChannel( const TSharedPtr<FMovieSceneChannelProxy> iChannelProxy, const FFrameNumber& iMergeTolerance )
{
    TSharedPtr<FMetaFloatChannel> meta_channel = MakeShared<FMetaFloatChannel>( iMergeTolerance );

    meta_channel->Build( iChannelProxy );

    return meta_channel;
}

//---

//static
TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>>
CinematicBoardSectionKeysHelpers::BuildPlanesTransformChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID )
{
    TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> proxies;

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( iPlayer, iSubSection, iSequenceID );

    TArray<AStaticMeshActor*> planes;
    TArray<FGuid> bindings;
    /*int plane_count =*/ ShotSequenceHelpers::GetPlanes( iPlayer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kAlwaysAll, &planes, &bindings );

    for( auto binding : bindings )
    {
        FMovieSceneChannelProxyData ChannelIndirection;

        //---

        TArray<UMovieScene3DTransformSection*> plane_transform_sections = ShotSequenceHelpers::GetPlaneTransformSections( iPlayer, result.mInnerSequence, result.mInnerSequenceId, binding );
        for( auto plane_transform_section : plane_transform_sections )
        {
            const FMovieSceneChannelEntry* FloatChannelEntry = plane_transform_section->GetChannelProxy().FindEntry( FMovieSceneFloatChannel::StaticStruct()->GetFName() );
            if( FloatChannelEntry )
            {
                TArrayView<FMovieSceneChannel* const>             FloatChannels = FloatChannelEntry->GetChannels();
                TArrayView<const FMovieSceneChannelMetaData>      MetaData = FloatChannelEntry->GetMetaData();
                TArrayView<const TMovieSceneExternalValue<float>> MetaDataExt = FloatChannelEntry->GetAllExtendedEditorData<FMovieSceneFloatChannel>();

                for( int32 Index = 0; Index < FloatChannels.Num(); ++Index )
                {
                    ChannelIndirection.Add( *static_cast<FMovieSceneFloatChannel*>( FloatChannels[Index] ), MetaData[Index], MetaDataExt[Index] );
                }
            }

            // UDN: Hook into TransformSection::OnSignatureChangedEvent to invalidate this section's channel proxy if the transform is changed.
            // Set the delegate to the whole subsequence, then every changes (even removing section) will call the it
            //if( !camera_transform_section->OnSignatureChanged().IsBoundToObject( this ) )
            //    camera_transform_section->OnSignatureChanged().AddUObject( this, &UMovieSceneCinematicBoardSection::HandleInvalidateChannelProxy );
        }

        TSharedPtr<FMovieSceneChannelProxy> ChannelProxy = MakeShared<FMovieSceneChannelProxy>( MoveTemp( ChannelIndirection ) );

        proxies.Add( binding, ChannelProxy );
    }

    return proxies;
}

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
TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>>
CinematicBoardSectionKeysHelpers::BuildPlanesMaterialChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID )
{
    TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> proxies;

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( iPlayer, iSubSection, iSequenceID );

    TArray<AStaticMeshActor*> planes;
    TArray<FGuid> bindings;
    /*int plane_count =*/ ShotSequenceHelpers::GetPlanes( iPlayer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kAlwaysAll, &planes, &bindings );

    for( auto binding : bindings )
    {
        FMovieSceneChannelProxyData ChannelIndirection;

        //---

        TArray<UMovieScenePrimitiveMaterialSection*> plane_material_sections = ShotSequenceHelpers::GetPlaneMaterialSections( iPlayer, result.mInnerSequence, result.mInnerSequenceId, binding );
        for( auto plane_material_section : plane_material_sections )
        {
            const FMovieSceneChannelEntry* ObjectPathChannelEntry = plane_material_section->GetChannelProxy().FindEntry( FMovieSceneObjectPathChannel::StaticStruct()->GetFName() );
            if( ObjectPathChannelEntry )
            {
                TArrayView<FMovieSceneChannel* const>                   ObjectPathChannels = ObjectPathChannelEntry->GetChannels();
                TArrayView<const FMovieSceneChannelMetaData>            MetaData = ObjectPathChannelEntry->GetMetaData();
                TArrayView<const TMovieSceneExternalValue<UObject*>>    MetaDataExt = ObjectPathChannelEntry->GetAllExtendedEditorData<FMovieSceneObjectPathChannel>();

                for( int32 Index = 0; Index < ObjectPathChannels.Num(); ++Index )
                {
                    ChannelIndirection.Add( *static_cast<FMovieSceneObjectPathChannel*>( ObjectPathChannels[Index] ), MetaData[Index], MetaDataExt[Index] );
                }
            }

            // UDN: Hook into TransformSection::OnSignatureChangedEvent to invalidate this section's channel proxy if the transform is changed.
            // Set the delegate to the whole subsequence, then every changes (even removing section) will call the it
            //if( !camera_transform_section->OnSignatureChanged().IsBoundToObject( this ) )
            //    camera_transform_section->OnSignatureChanged().AddUObject( this, &UMovieSceneCinematicBoardSection::HandleInvalidateChannelProxy );
        }

        TSharedPtr<FMovieSceneChannelProxy> ChannelProxy = MakeShared<FMovieSceneChannelProxy>( MoveTemp( ChannelIndirection ) );

        proxies.Add( binding, ChannelProxy );
    }

    return proxies;
}

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
