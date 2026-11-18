// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "EposTracksEditorHelpers.h"

#include "Channels/MovieSceneChannelProxy.h"
#include "MovieScene.h"
#include "MovieSceneSequence.h"

#include "Board/BoardSequence.h"
#include "Shot/ShotSequence.h"
#include "OdysseyAnimationTimelineSection.h"
#include "OdysseyAnimationTimelineSectionEditorData.h"

//static
TArray<FFrameNumber>
FEposTracksEditorHelpers::GetAllAnimationCutTimes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FFrameNumber& iFrameNumber )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( iPlayer, iSequence, iSequenceID, iFrameNumber );
    if( !result.mInnerSequence || result.mInnerSequence->IsA<UBoardSequence>() )
        return TArray<FFrameNumber>();

    return FEposTracksEditorHelpers::GetAllAnimationCutTimes( iPlayer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
TMap<FGuid, FChannelProxyBySectionMap>
FEposTracksEditorHelpers::BuildAnimationsTimelineChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( iPlayer, iSubSection, iSequenceID );

    if( !result.mInnerSequence || result.mInnerSequence->IsA<UBoardSequence>() )
        return TMap<FGuid, FChannelProxyBySectionMap>();

    return FEposTracksEditorHelpers::BuildAnimationsTimelineChannelProxy( iPlayer, result.mInnerSequence, result.mInnerSequenceId );
}

//static
TArray<FFrameNumber>
FEposTracksEditorHelpers::GetAllAnimationCutTimes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    TArray<FFrameNumber> Times;
    for (const FGuid Binding : ShotSequenceHelpers::GetAnimationBindings(iPlayer, iSequence, iSequenceID))
    {
        const ShotSequenceHelpers::FFindOrCreateTimelineResult Result = ShotSequenceHelpers::FindTimelineTrackAndSections(iPlayer, iSequence, iSequenceID, Binding);
        for (const TWeakObjectPtr<UOdysseyAnimationTimelineSection>& Section : Result.mSections)
        {
            if (Section.IsValid())
            {
                Times.Append(UOdysseyAnimationTimelineSectionEditorData::GetOrCreate(*Section)->GetAnimationCutChannel().GetTimes());
            }
        }
    }
    Times.Sort();
    return Times;
}

//static
TMap<FGuid, FChannelProxyBySectionMap>
FEposTracksEditorHelpers::BuildAnimationsTimelineChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    TMap<FGuid, FChannelProxyBySectionMap> maps;

    TArray<FGuid> animation_bindings = ShotSequenceHelpers::GetAnimationBindings( iPlayer, iSequence, iSequenceID );

    for( FGuid animation_binding : animation_bindings )
    {
        FChannelProxyBySectionMap map;

        //---

        ShotSequenceHelpers::FFindOrCreateTimelineResult result = ShotSequenceHelpers::FindTimelineTrackAndSections( iPlayer, iSequence, iSequenceID, animation_binding );

        for( TWeakObjectPtr<UOdysseyAnimationTimelineSection> animation_timeline_section : result.mSections )
        {
            FMovieSceneChannelProxyData ChannelIndirection;

            //const FMovieSceneChannelEntry* ObjectPathChannelEntry = animation_timeline_section->GetChannelProxy().FindEntry( FMovieSceneObjectPathChannel::StaticStruct()->GetFName() );
            //if( ObjectPathChannelEntry )
            //{
                ChannelIndirection.Add( UOdysseyAnimationTimelineSectionEditorData::GetOrCreate(*animation_timeline_section)->GetAnimationCutChannel(), FMovieSceneChannelMetaData() );
            //}

            TSharedPtr<FMovieSceneChannelProxy> ChannelProxy = MakeShared<FMovieSceneChannelProxy>( MoveTemp( ChannelIndirection ) );

            map.Add( animation_timeline_section, ChannelProxy );

            // UDN: Hook into TransformSection::OnSignatureChangedEvent to invalidate this section's channel proxy if the transform is changed.
            // Set the delegate to the whole subsequence, then every changes (even removing section) will call the it
            //if( !camera_transform_section->OnSignatureChanged().IsBoundToObject( this ) )
            //    camera_transform_section->OnSignatureChanged().AddUObject( this, &UMovieSceneCinematicBoardSection::HandleInvalidateChannelProxy );
        }

        maps.Add( animation_binding, map );
    }

    return maps;
}
