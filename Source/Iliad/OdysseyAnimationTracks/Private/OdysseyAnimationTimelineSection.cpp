// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyAnimationTimelineSection.h"

#include "MovieScene.h"
#include "MovieSceneSequence.h"

#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationComponent.h"

#include "Channels/MovieSceneChannelData.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "EntitySystem/BuiltInComponentTypes.h"
#include "EntitySystem/MovieSceneEntityBuilder.h"
#include "Evaluation/MovieSceneRootOverridePath.h"
#include "EntitySystem/MovieSceneEntitySystemLinker.h"
#include "EntitySystem/MovieSceneInstanceRegistry.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationTimelineSection)

#define LOCTEXT_NAMESPACE "UOdysseyAnimationTimelineSection"

UOdysseyAnimationTimelineSection::UOdysseyAnimationTimelineSection(const FObjectInitializer& ObjInitializer)
    : Super(ObjInitializer)
{
}

//---

void
UOdysseyAnimationTimelineSection::RebuildAnimationCuts()
{
    CacheChannelProxy();
}

EMovieSceneChannelProxyType
UOdysseyAnimationTimelineSection::CacheChannelProxy()
{
    BuildAnimationCuts();

    // This function will still not create a real ChannelProxy
    // Because in this case, keys will appears in the timeline section
    // This function is only used to update when needed our internal animation cut channel

    FMovieSceneChannelProxyData Channels;
    ChannelProxy = MakeShared<FMovieSceneChannelProxy>(MoveTemp(Channels));
    return EMovieSceneChannelProxyType::Dynamic;
}

FOdysseyAnimationCutChannel&
UOdysseyAnimationTimelineSection::GetAnimationCutChannel()
{
    return AnimationCutChannel;
}

const FOdysseyAnimationCutChannel&
UOdysseyAnimationTimelineSection::GetAnimationCutChannel() const
{
    return AnimationCutChannel;
}

UOdysseyAnimation*
UOdysseyAnimationTimelineSection::GetAnimation() const
{
    return Animation;
}
EOdysseyAnimationPlayerPostBehaviour
UOdysseyAnimationTimelineSection::GetPreBehaviour() const
{
    return PreBehaviour;
}

EOdysseyAnimationPlayerPostBehaviour
UOdysseyAnimationTimelineSection::GetPostBehaviour() const
{
    return PostBehaviour;
}

FFrameNumber
UOdysseyAnimationTimelineSection::GetStartFrameOffset() const
{
    return StartFrameOffset;
}

void
UOdysseyAnimationTimelineSection::SetAnimation(UOdysseyAnimation* iAnimation)
{
    Animation = iAnimation;
    MarkAsChanged();
}

void
UOdysseyAnimationTimelineSection::SetPreBehaviour(EOdysseyAnimationPlayerPostBehaviour iValue)
{
    PreBehaviour = iValue;
    MarkAsChanged();
}

void
UOdysseyAnimationTimelineSection::SetPostBehaviour(EOdysseyAnimationPlayerPostBehaviour iValue)
{
    PostBehaviour = iValue;
    MarkAsChanged();
}

void
UOdysseyAnimationTimelineSection::SetStartFrameOffset(FFrameNumber iOffset)
{
    StartFrameOffset = iOffset;
    MarkAsChanged();
}

void
UOdysseyAnimationTimelineSection::BuildAnimationCuts()
{
    AnimationCutChannel.Reset();

    UOdysseyAnimationLayerStack* layer_stack = GetAnimation() ? GetAnimation()->GetLayerStack() : nullptr;
    TArray<UOdysseyLayer*> layers = layer_stack ? layer_stack->GetLayers() : TArray<UOdysseyLayer*>();
    TSet<UOdysseyAnimationLayer*> animation_layers;
    for( UOdysseyLayer* layer : layers )
        animation_layers.Add( Cast<UOdysseyAnimationLayer>( layer ) );
    animation_layers.Remove( nullptr );

    for( UOdysseyAnimationLayer* layer : animation_layers )
    {
        UOdysseyAnimationCell* previous_cell = nullptr;
        TArray<UOdysseyAnimationCell*> cells = layer->GetCells();
        for( UOdysseyAnimationCell* cell : cells )
        {
            {
                FAnimationCutEntry animationcutentry( previous_cell, cell );
                FFrameNumber frame_in_timeline = animationcutentry.GetFrameReference();
                FFrameNumber frame_in_sequence = ConvertFrameFromTimelineToSequence( frame_in_timeline );

                if( GetRange().Contains( frame_in_sequence ) )
                {
                    int32 index = AnimationCutChannel.GetData().FindKey( frame_in_sequence );
                    if( index != INDEX_NONE )
                    {
                        AnimationCutChannel.GetData().GetValues()[index].Value.AddNewEntry( animationcutentry );
                    }
                    else
                    {
                        FAnimationCut new_animationcut;
                        new_animationcut.AddNewEntry( animationcutentry );

                        AnimationCutChannel.GetData().AddKey( frame_in_sequence, FOdysseyAnimationCutValue{ new_animationcut } );
                    }
                }
            }

            previous_cell = cell;

            if( cell == cells.Last() )
            {
                FAnimationCutEntry animationcutentry( previous_cell, nullptr );
                FFrameNumber frame_in_timeline = animationcutentry.GetFrameReference();
                FFrameNumber frame_in_sequence = ConvertFrameFromTimelineToSequence( frame_in_timeline );

                if( GetRange().Contains( frame_in_sequence ) )
                {
                    int32 index = AnimationCutChannel.GetData().FindKey( frame_in_sequence );
                    if( index != INDEX_NONE )
                    {
                        AnimationCutChannel.GetData().GetValues()[index].Value.AddNewEntry( animationcutentry );
                    }
                    else
                    {
                        FAnimationCut new_animationcut;
                        new_animationcut.AddNewEntry( animationcutentry );

                        AnimationCutChannel.GetData().AddKey( frame_in_sequence, FOdysseyAnimationCutValue{ new_animationcut } );
                    }
                }
            }
        }
    }
}

void
UOdysseyAnimationTimelineSection::MigrateFrameTimes(FFrameRate SourceRate, FFrameRate DestinationRate)
{
    FFrameNumber NewStartFrameOffset = ConvertFrameTime(FFrameTime(StartFrameOffset), SourceRate, DestinationRate).FloorToFrame();
    StartFrameOffset = NewStartFrameOffset;
}

TRange<FFrameNumber>
UOdysseyAnimationTimelineSection::GetDefaultSectionRange(UOdysseyAnimationTimelineSection* iSection)
{
    UOdysseyAnimation* animation = iSection->Animation;
    if (!animation)
        return TRange<FFrameNumber>();

    FFrameRate animationFrameRate(animation->GetFramesPerSecond() * 100, 100);

    FInt32Range frameRange = animation->GetFrameRange();
    FFrameNumber animationLeftBoundFrame(frameRange.GetLowerBoundValue());
    FFrameNumber animationRightBoundFrame(frameRange.GetUpperBoundValue() + 1);

    UMovieScene* outer_movie_scene = iSection->GetTypedOuter<UMovieScene>();
    FFrameTime animationLeftBoundTime = FFrameRate::TransformTime(animationLeftBoundFrame, animationFrameRate, outer_movie_scene->GetDisplayRate());
    animationLeftBoundFrame = FFrameRate::TransformTime(animationLeftBoundTime, outer_movie_scene->GetDisplayRate(), outer_movie_scene->GetTickResolution()).GetFrame();
    FFrameTime animationRightBoundTime = FFrameRate::TransformTime(animationRightBoundFrame, animationFrameRate, outer_movie_scene->GetDisplayRate());
    animationRightBoundFrame = FFrameRate::TransformTime(animationRightBoundTime, outer_movie_scene->GetDisplayRate(), outer_movie_scene->GetTickResolution()).GetFrame();

    return TRange<FFrameNumber>(animationLeftBoundFrame, animationRightBoundFrame);
}

FFrameNumber
UOdysseyAnimationTimelineSection::ConvertFrameFromTimelineToSequence( FFrameNumber iFrameInTimeline )
{
    UMovieSceneSequence* sequence = GetTypedOuter<UMovieSceneSequence>();

    // Convert a real timeline frame to a sequence frame (change framerate from animation framerate to sequence framerate (aka tickresolution))
    FFrameTime frametime_in_timeline = FFrameRate::TransformTime( iFrameInTimeline, FFrameRate( GetAnimation()->GetFramesPerSecond() * 1000, 1000 ), sequence->GetMovieScene()->GetTickResolution() );
    FFrameNumber frame_in_timeline = frametime_in_timeline.GetFrame(); // It should be ok (?), otherwise return a FFrameTime

    // Apply all offsets in sequence framerate (aka tickresolution)
    FFrameNumber frame_in_section = frame_in_timeline - GetStartFrameOffset();
    FFrameNumber frame_in_sequence = frame_in_section + GetTrueRange().GetLowerBoundValue();

    return frame_in_sequence;
}

FFrameNumber
UOdysseyAnimationTimelineSection::ConvertFrameFromSequenceToTimeline( FFrameNumber iFrameInSequence )
{
    UMovieSceneSequence* sequence = GetTypedOuter<UMovieSceneSequence>();

    // Apply all offsets in sequence framerate (aka tickresolution)
    FFrameNumber frame_in_section = iFrameInSequence - GetTrueRange().GetLowerBoundValue();
    FFrameNumber frame_in_timeline = frame_in_section + GetStartFrameOffset();

    // Convert a sequence frame to a real timeline frame (change framerate from sequence framerate (aka tickresolution) to animation framerate)
    FFrameTime frametime_in_timeline = FFrameRate::TransformTime( frame_in_timeline, sequence->GetMovieScene()->GetTickResolution(), FFrameRate( GetAnimation()->GetFramesPerSecond() * 1000, 1000 ) );
    frame_in_timeline = frametime_in_timeline.GetFrame();

    return frame_in_timeline;
}

#undef LOCTEXT_NAMESPACE
