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

//static
int
CinematicBoardSectionBindingHelpers::GetMaxPlaneBindings( IMovieScenePlayer& iPlayer, const UMovieSceneTrack& iTrack, FMovieSceneSequenceIDRef iSequenceID )
{
    int count = 0;
    for( auto section : iTrack.GetAllSections() )
    {
        UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );
        TArray<FMovieScenePossessable> bindings = GetPlaneBindings( iPlayer, *subsection, iSequenceID );

        count = FMath::Max( count, bindings.Num() );
    }

    return count;
}

//static
TArray<FMovieScenePossessable>
CinematicBoardSectionBindingHelpers::GetPlaneBindings( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSection, FMovieSceneSequenceIDRef iSequenceID )
{
    TArray<FMovieScenePossessable> bindings;

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( iPlayer, iSection, iSequenceID );

    FMovieSceneSequenceID inner_sequence_id = result.mInnerSequenceId;
    UMovieSceneSequence* inner_sequence = result.mInnerSequence;
    UMovieScene* inner_movie_scene = result.mInnerMovieScene;

    if( !inner_movie_scene )
        return bindings;

    AStaticMeshActor* plane = nullptr;
    for( int i = 0; i < inner_movie_scene->GetPossessableCount(); i++ )
    {
        FMovieScenePossessable possessable = inner_movie_scene->GetPossessable( i );

        for( auto Object : iPlayer.FindBoundObjects( possessable.GetGuid(), inner_sequence_id ) )
        {
            plane = Cast<AStaticMeshActor>( Object.Get() );
            if( plane )
                bindings.Add( possessable );
        }
    }

    return bindings;
}

//---
//---
//---

//static
FMovieScenePossessable
CinematicBoardSectionBindingHelpers::GetCameraBinding( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSection, FMovieSceneSequenceIDRef iSequenceID )
{
    FMovieScenePossessable binding;

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( iPlayer, iSection, iSequenceID );

    FMovieSceneSequenceID inner_sequence_id = result.mInnerSequenceId;
    UMovieScene* inner_movie_scene = result.mInnerMovieScene;

    if( !inner_movie_scene )
        return binding;

    ACineCameraActor* camera = nullptr;
    for( int i = 0; i < inner_movie_scene->GetPossessableCount(); i++ )
    {
        FMovieScenePossessable possessable = inner_movie_scene->GetPossessable( i );

        for( auto Object : iPlayer.FindBoundObjects( possessable.GetGuid(), inner_sequence_id ) )
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
CinematicBoardSectionKeysHelpers::GetCameraTransformSections( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSection, FMovieSceneSequenceIDRef iSequenceID, const FMovieScenePossessable& iPossessable )
{
    TArray<UMovieScene3DTransformSection*> sections;

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( iPlayer, iSection, iSequenceID );

    UMovieScene* inner_moviescene = result.mInnerMovieScene;
    if( !inner_moviescene )
        return sections;

    if( !iPossessable.GetGuid().IsValid() )
        return sections;

    UMovieSceneTrack* track = inner_moviescene->FindTrack<UMovieScene3DTransformTrack>( iPossessable.GetGuid() );
    if( !track )
        return sections;

    for( auto section : track->GetAllSections() )
        sections.Add( Cast<UMovieScene3DTransformSection>( section ) );

    return sections;
}

//static
TArray<UMovieScene3DTransformSection*>
CinematicBoardSectionKeysHelpers::GetPlaneTransformSections( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSection, FMovieSceneSequenceIDRef iSequenceID, const FMovieScenePossessable& iPossessable )
{
    TArray<UMovieScene3DTransformSection*> sections;

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( iPlayer, iSection, iSequenceID );

    UMovieScene* inner_moviescene = result.mInnerMovieScene;
    if( !inner_moviescene )
        return sections;

    if( !iPossessable.GetGuid().IsValid() )
        return sections;

    UMovieSceneTrack* track = inner_moviescene->FindTrack<UMovieScene3DTransformTrack>( iPossessable.GetGuid() );
    if( !track )
        return sections;

    for( auto section : track->GetAllSections() )
        sections.Add( Cast<UMovieScene3DTransformSection>( section ) );

    return sections;
}

//static
TArray<UMovieScenePrimitiveMaterialSection*>
CinematicBoardSectionKeysHelpers::GetPlaneMaterialSections( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSection, FMovieSceneSequenceIDRef iSequenceID, const FMovieScenePossessable& iPossessable )
{
    TArray<UMovieScenePrimitiveMaterialSection*> sections;

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( iPlayer, iSection, iSequenceID );

    UMovieScene* inner_moviescene = result.mInnerMovieScene;
    if( !inner_moviescene )
        return sections;

    if( !iPossessable.GetGuid().IsValid() )
        return sections;

    TArrayView<TWeakObjectPtr<>> objects = iPlayer.FindBoundObjects( iPossessable.GetGuid(), result.mInnerSequenceId );
    if( objects.Num() != 1 )
        return sections;
    AStaticMeshActor* plane = Cast<AStaticMeshActor>( objects[0] );
    if( !plane )
        return sections;

    FGuid plane_component = iPlayer.FindObjectId( *plane->GetRootComponent(), result.mInnerSequenceId );

    UMovieSceneTrack* track = inner_moviescene->FindTrack<UMovieScenePrimitiveMaterialTrack>( plane_component );
    if( !track )
        return sections;

    for( auto section : track->GetAllSections() )
        sections.Add( Cast<UMovieScenePrimitiveMaterialSection>( section ) );

    return sections;
}

//---

//static
TSharedPtr<FMovieSceneChannelProxy>
CinematicBoardSectionKeysHelpers::BuildCameraTransformChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID )
{
    FMovieSceneChannelProxyData ChannelIndirection;

    FMovieScenePossessable possessable = CinematicBoardSectionBindingHelpers::GetCameraBinding( iPlayer, iSubSection, iSequenceID );

    //---

    int sort = 0;

    TArray<UMovieScene3DTransformSection*> camera_transform_sections = CinematicBoardSectionKeysHelpers::GetCameraTransformSections( iPlayer, iSubSection, iSequenceID, possessable );
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

    TArray<FMovieScenePossessable> possessables = CinematicBoardSectionBindingHelpers::GetPlaneBindings( iPlayer, iSubSection, iSequenceID );
    for( auto possessable : possessables )
    {
        FMovieSceneChannelProxyData ChannelIndirection;

        //---

        int sort = 0;

        TArray<UMovieScene3DTransformSection*> plane_transform_sections = CinematicBoardSectionKeysHelpers::GetPlaneTransformSections( iPlayer, iSubSection, iSequenceID, possessable );
        int section_index = -1;
        for( auto plane_transform_section : plane_transform_sections )
        {
            section_index++;

            const FMovieSceneChannelEntry* FloatChannelEntry = plane_transform_section->GetChannelProxy().FindEntry( FMovieSceneFloatChannel::StaticStruct()->GetFName() );
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

        proxies.Add( possessable.GetGuid(), ChannelProxy );
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

    TArray<FMovieScenePossessable> possessables = CinematicBoardSectionBindingHelpers::GetPlaneBindings( iPlayer, iSubSection, iSequenceID );
    for( auto possessable : possessables )
    {
        FMovieSceneChannelProxyData ChannelIndirection;

        //---

        int sort = 0;

        TArray<UMovieScenePrimitiveMaterialSection*> plane_material_sections = CinematicBoardSectionKeysHelpers::GetPlaneMaterialSections( iPlayer, iSubSection, iSequenceID, possessable );
        int section_index = -1;
        for( auto plane_material_section : plane_material_sections )
        {
            section_index++;

            const FMovieSceneChannelEntry* ObjectPathChannelEntry = plane_material_section->GetChannelProxy().FindEntry( FMovieSceneObjectPathChannel::StaticStruct()->GetFName() );
            if( ObjectPathChannelEntry )
            {
                TArrayView<FMovieSceneChannel* const>                   ObjectPathChannels = ObjectPathChannelEntry->GetChannels();
                TArrayView<const FMovieSceneChannelMetaData>            MetaData = ObjectPathChannelEntry->GetMetaData();
                TArrayView<const TMovieSceneExternalValue<UObject*>>    MetaDataExt = ObjectPathChannelEntry->GetAllExtendedEditorData<FMovieSceneObjectPathChannel>();

                for( int32 Index = 0; Index < ObjectPathChannels.Num(); ++Index )
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

                    ChannelIndirection.Add( *static_cast<FMovieSceneObjectPathChannel*>( ObjectPathChannels[Index] ), MetaDataEntry, MetaDataExt[Index] );
                }
            }

            // UDN: Hook into TransformSection::OnSignatureChangedEvent to invalidate this section's channel proxy if the transform is changed.
            // Set the delegate to the whole subsequence, then every changes (even removing section) will call the it
            //if( !camera_transform_section->OnSignatureChanged().IsBoundToObject( this ) )
            //    camera_transform_section->OnSignatureChanged().AddUObject( this, &UMovieSceneCinematicBoardSection::HandleInvalidateChannelProxy );
        }

        TSharedPtr<FMovieSceneChannelProxy> ChannelProxy = MakeShared<FMovieSceneChannelProxy>( MoveTemp( ChannelIndirection ) );

        proxies.Add( possessable.GetGuid(), ChannelProxy );
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
