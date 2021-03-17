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
#include "CinematicBoardTrack/MetaChannelProxy.h"
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

// From ...\Source\Editor\MovieSceneTools\Private\Channels\BuiltInChannelEditors.cpp
static
void
DrawKeys( FMovieSceneFloatChannel* Channel, const FKeyHandle& InKeyHandle, FKeyDrawParams& OutKeyDrawParams )
//DrawKeys( FMovieSceneFloatChannel* Channel, TArrayView<const FKeyHandle> InKeyHandles, const UMovieSceneSection* InOwner, TArrayView<FKeyDrawParams> OutKeyDrawParams )
{
    static const FName CircleKeyBrushName( "Sequencer.KeyCircle" );
    static const FName DiamondKeyBrushName( "Sequencer.KeyDiamond" );
    static const FName SquareKeyBrushName( "Sequencer.KeySquare" );
    static const FName TriangleKeyBrushName( "Sequencer.KeyTriangle" );

    const FSlateBrush* CircleKeyBrush = FEditorStyle::GetBrush( CircleKeyBrushName );
    const FSlateBrush* DiamondKeyBrush = FEditorStyle::GetBrush( DiamondKeyBrushName );
    const FSlateBrush* SquareKeyBrush = FEditorStyle::GetBrush( SquareKeyBrushName );
    const FSlateBrush* TriangleKeyBrush = FEditorStyle::GetBrush( TriangleKeyBrushName );

    TMovieSceneChannelData<FMovieSceneFloatValue> ChannelData = Channel->GetData();
    TArrayView<const FMovieSceneFloatValue> Values = ChannelData.GetValues();

    FKeyDrawParams TempParams;
    TempParams.BorderBrush = TempParams.FillBrush = DiamondKeyBrush;

    //for( int32 Index = 0; Index < InKeyHandles.Num(); ++Index )
    {
        //FKeyHandle Handle = InKeyHandles[Index];

        const int32 KeyIndex = ChannelData.GetIndex( InKeyHandle );
        //const int32 KeyIndex = ChannelData.GetIndex( Handle );

        ERichCurveInterpMode InterpMode = KeyIndex == INDEX_NONE ? RCIM_None : Values[KeyIndex].InterpMode.GetValue();
        ERichCurveTangentMode TangentMode = KeyIndex == INDEX_NONE ? RCTM_None : Values[KeyIndex].TangentMode.GetValue();

        TempParams.FillOffset = FVector2D( 0.f, 0.f );

        switch( InterpMode )
        {
            case RCIM_Linear:
                TempParams.BorderBrush = TempParams.FillBrush = TriangleKeyBrush;
                TempParams.FillTint = FLinearColor( 0.0f, 0.617f, 0.449f, 1.0f ); // blueish green
                TempParams.FillOffset = FVector2D( 0.0f, 1.0f );
                break;

            case RCIM_Constant:
                TempParams.BorderBrush = TempParams.FillBrush = SquareKeyBrush;
                TempParams.FillTint = FLinearColor( 0.0f, 0.445f, 0.695f, 1.0f ); // blue
                break;

            case RCIM_Cubic:
                TempParams.BorderBrush = TempParams.FillBrush = CircleKeyBrush;

                switch( TangentMode )
                {
                    case RCTM_Auto:  TempParams.FillTint = FLinearColor( 0.972f, 0.2f, 0.2f, 1.0f );     break; // vermillion
                    case RCTM_Break: TempParams.FillTint = FLinearColor( 0.336f, 0.703f, 0.5f, 0.91f );  break; // sky blue
                    case RCTM_User:  TempParams.FillTint = FLinearColor( 0.797f, 0.473f, 0.5f, 0.652f ); break; // reddish purple
                    default:         TempParams.FillTint = FLinearColor( 0.75f, 0.75f, 0.75f, 1.0f );    break; // light gray
                }
                break;

            default:
                TempParams.BorderBrush = TempParams.FillBrush = DiamondKeyBrush;
                TempParams.FillTint = FLinearColor( 1.0f, 1.0f, 1.0f, 1.0f ); // white
                break;
        }

        OutKeyDrawParams = TempParams;
        //OutKeyDrawParams[Index] = TempParams;
    }
}

//static
TSharedPtr<FMetaChannelProxy>
CinematicBoardSectionKeysHelpers::BuildCameraTransformMetaChannelProxy( const TSharedPtr<FMovieSceneChannelProxy> iChannelProxy, const TRange<FFrameNumber>& iRange )
{
    TSharedPtr<FMetaChannelProxy> meta_channel_proxy = MakeShared<FMetaChannelProxy>();

    TArrayView<FMovieSceneFloatChannel*>                float_channels = iChannelProxy->GetChannels<FMovieSceneFloatChannel>();
    //TArrayView<const FMovieSceneChannelMetaData>        metaData = iChannelProxy->GetMetaData<FMovieSceneFloatChannel>();
    //TArrayView<const TMovieSceneExternalValue<float>>   metaDataExt = iChannelProxy->GetAllExtendedEditorData<FMovieSceneFloatChannel>();

    int32 nb_float_channels = float_channels.Num();

    for( int32 channel_index = 0; channel_index < nb_float_channels; ++channel_index )
    {
        TMovieSceneChannelHandle<FMovieSceneFloatChannel> channel_handle = iChannelProxy->MakeHandle<FMovieSceneFloatChannel>( channel_index );

        FMovieSceneFloatChannel* channel = channel_handle.Get();
        if( !channel || !channel->GetNumKeys() )
            continue;

        TMovieSceneChannelData<FMovieSceneFloatValue> channel_data = channel->GetData();

        for( int32 key_index = 0; key_index < channel->GetNumKeys(); ++key_index )
        {
            FKeyHandle key_handle = channel_data.GetHandle( key_index );

            FMetaSubKey sub_key;
            sub_key.mChannelHandle = channel_handle;
            sub_key.mKeyHandle = key_handle;
            DrawKeys( channel, sub_key.mKeyHandle, sub_key.mKeyDrawParam );

            FFrameNumber time = channel_data.GetTimes()[key_index];
            if( !iRange.Contains( time ) )
                continue;

            FMetaKey& meta_key = meta_channel_proxy->mMetaKeys.FindOrAdd( iRange.HasLowerBound() && iRange.HasUpperBound() ? iRange.GetLowerBoundValue() + iRange.Size<FFrameNumber>() / 2 : time );

            meta_key.mSubKeys.Add( sub_key );
            meta_key.mFlags = FMetaKey::EFlags::kNone;
        }
    }

    for( auto& meta_key : meta_channel_proxy->mMetaKeys )
    {
        const TArray<FMetaSubKey>& sub_keys = meta_key.Value.mSubKeys;

        check( sub_keys.Num() );
        meta_key.Value.mMetaKeyDrawParam = sub_keys[0].mKeyDrawParam; // Init

        for( int i = 1; i < sub_keys.Num(); i++ )
        {
            if( sub_keys[i-1].mKeyDrawParam != sub_keys[i].mKeyDrawParam )
            {
                meta_key.Value.mFlags = FMetaKey::EFlags::kPartial;

                static const FSlateBrush* PartialKeyBrush = FEditorStyle::GetBrush( "Sequencer.PartialKey" );
                meta_key.Value.mMetaKeyDrawParam.BorderBrush = PartialKeyBrush;
                meta_key.Value.mMetaKeyDrawParam.FillBrush = PartialKeyBrush;
                meta_key.Value.mMetaKeyDrawParam.FillOffset = FVector2D( 0.f, 0.f );
                meta_key.Value.mMetaKeyDrawParam.FillTint = FLinearColor::White;
                meta_key.Value.mMetaKeyDrawParam.BorderTint = FLinearColor::White;

                break;
            }
        }
    }

    return meta_channel_proxy;
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
