// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationTimelineSection.h"

#include "Channels/MovieSceneChannelProxy.h"
#include "MovieScene.h"
#include "MovieSceneSequence.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationCell.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationCut.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerStack.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationTimelineSection)

#define LOCTEXT_NAMESPACE "UOdysseyAnimationTimelineSection"

UOdysseyAnimationTimelineSection::UOdysseyAnimationTimelineSection(const FObjectInitializer& ObjInitializer)
    : Super(ObjInitializer)
    , AnimationCutChannel( this )
{
}

void
UOdysseyAnimationTimelineSection::SetRange( const TRange<FFrameNumber>& iNewRange )
{
    // It should be done in the Super class ? All section types should have this same behavior (?)
    if( !GetSupportsInfiniteRange() && ( !iNewRange.HasLowerBound() || !iNewRange.HasUpperBound() ) )
    {
        // Just to see if this condition happens AND if this condition is relevant
        // If the caller must be fixed, fix it
        checkNoEntry();
        return;
    }

    Super::SetRange( iNewRange );
}

//---

// During a copy/paste of a binding (actor) track:
//
// [during ue splash screen]
//
// Default object (section)
// - Ctor
// - PostInitProperties
//
//
// [loading a board/shot sequence]
//
// Original object with its hierarchy (binding track + binding section)
// - -------------------- sequence = SS_xxxx (GetOuter<MovieSceneSequence>())
// - Ctor
// - PostInitProperties
// - -------------------- Animation = A_xxxx
// - PostLoad()
//
//
// [CTRL+C]
//
// Copy Object - /Engine/Transient
// - Ctor
// - PostInitProperties
// - -------------------- Animation = A_xxxx
// - PostDuplicate()
// - PostLoad()
//
//
// [CTRL+V]
//
// Paste Object - /Engine/Sequencer/Editor/Transient
// - Ctor
// - PostInitProperties
// - PostEditImport
// - -------------------- Animation = A_xxxx
// - PostEditImport
//
//
// [Ask to duplicate the actor if needed]
//
//
// New Object - /Engine/Sequencer/Editor/Transient
// - Ctor
// - PostInitProperties
// - PostEditImport
// - -------------------- Animation = A_xxxx
// - PostEditImport
// - -------------------- sequence = SS_xxxx (GetOuter<MovieSceneSequence>()) (no more transient)
// - OnBindingIDsUpdated
//

void UOdysseyAnimationTimelineSection::PostInitProperties() //override
{
    Super::PostInitProperties();

    AnimationCutChannel.SetParentSection( this );
}

void UOdysseyAnimationTimelineSection::PostLoad() //override
{
    Super::PostLoad();

    if( Animation )
    {
        Animation->OnRenderingChangedDelegate().AddUObject( this, &UOdysseyAnimationTimelineSection::OnAnimationChanged );

        RebuildAnimationCutChannel();
    }
}

void UOdysseyAnimationTimelineSection::PostEditImport() //override
{
    Super::PostEditImport();
}

void UOdysseyAnimationTimelineSection::PostDuplicate( bool bDuplicateForPIE ) //override
{
    Super::Super::PostDuplicate( bDuplicateForPIE );
}

void UOdysseyAnimationTimelineSection::OnBindingIDsUpdated( const TMap<UE::MovieScene::FFixedObjectBindingID, UE::MovieScene::FFixedObjectBindingID>& OldFixedToNewFixedMap, FMovieSceneSequenceID LocalSequenceID, TSharedRef<UE::MovieScene::FSharedPlaybackState> SharedPlaybackState ) //override
{
    Super::OnBindingIDsUpdated( OldFixedToNewFixedMap, LocalSequenceID, SharedPlaybackState );

    if( ensure(Animation) )
        Animation->OnRenderingChangedDelegate().AddUObject( this, &UOdysseyAnimationTimelineSection::OnAnimationChanged );

    RebuildAnimationCutChannel();
}

#if WITH_EDITOR

void UOdysseyAnimationTimelineSection::PreEditChange( FProperty* PropertyAboutToChange )
{
    if( PropertyAboutToChange && PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED( UOdysseyAnimationTimelineSection, Animation ) )
    {
        SetAnimation( nullptr );
    }

    Super::PreEditChange( PropertyAboutToChange );
}

void UOdysseyAnimationTimelineSection::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    Super::PostEditChangeProperty( PropertyChangedEvent );

    const FName PropertyName = PropertyChangedEvent.GetPropertyName();
    if( PropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyAnimationTimelineSection, Animation ) )
    {
        SetAnimation( Animation );
    }
}

#endif

UOdysseyAnimationTimelineSection::FOnAnimationCutChannelChanged&
UOdysseyAnimationTimelineSection::OnAnimationCutChannelChanged()
{
    return mOnAnimationCutChannelChanged;
}

//---

EMovieSceneChannelProxyType
UOdysseyAnimationTimelineSection::CacheChannelProxy()
{
    // This function will still not create a real ChannelProxy
    // Because in this case, keys will appears in the timeline section

    FMovieSceneChannelProxyData Channels;
    ChannelProxy = MakeShared<FMovieSceneChannelProxy>(MoveTemp(Channels));
    return EMovieSceneChannelProxyType::Dynamic;
}

void
UOdysseyAnimationTimelineSection::OnAnimationChanged( const FOdysseyRenderingChangedEvent& iEvent )
{
    if( iEvent.IsInteractive() )
        return;

    if( iEvent.GetType() != FOdysseyRenderingChangedEvent::kCompositionChange )
        return;

    if( !LockChannelRebuild )
        RebuildAnimationCutChannel();
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
    if( Animation )
        Animation->OnRenderingChangedDelegate().RemoveAll( this );

    Animation = iAnimation;

    if( Animation )
        Animation->OnRenderingChangedDelegate().AddUObject( this, &UOdysseyAnimationTimelineSection::OnAnimationChanged );

    RebuildAnimationCutChannel();
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
UOdysseyAnimationTimelineSection::UpdateAnimationCutChannel( const TArray<FKeyHandle>& iKeyHandles, EPropertyChangeType::Type iChangeType )
{
    // When 'offseting' each animation cut (in Update()), the animation will be modified, which leads to call the delegate
    // As the section is bind to this delegate, it will rebuild its channel which will modify all key handles
    // Once done, all of the key handles here (during the second loop inside Update()) will be invalid
    // So when this function Update() is called, don't rebuild the channel (as it is what this function does)
    LockChannelRebuild = true;
    AnimationCutChannel.Update( iKeyHandles, iChangeType );
    LockChannelRebuild = false;
}

void
UOdysseyAnimationTimelineSection::RebuildAnimationCutChannel()
{
    AnimationCutChannel.Reset();

    UMovieSceneSequence* sequence = GetTypedOuter<UMovieSceneSequence>();
    if( !sequence )
        return;

    UOdysseyLayerStack* layer_stack = GetAnimation() ? GetAnimation()->GetLayerStack() : nullptr;
    TArray<UOdysseyLayer*> layers = layer_stack ? layer_stack->GetLayers() : TArray<UOdysseyLayer*>();
    TSet<UOdysseyAnimationLayer*> animation_layers;
    for( UOdysseyLayer* layer : layers )
        animation_layers.Add( Cast<UOdysseyAnimationLayer>( layer ) );
    animation_layers.Remove( nullptr );

    for( UOdysseyAnimationLayer* layer : animation_layers )
    {
        UOdysseyLayerCell* previous_cell = nullptr;
        TArray<UOdysseyLayerCell*> cells_unsafe = layer->GetCells();
        TArray<UOdysseyLayerCell*> cells = cells_unsafe.FilterByPredicate( []( const UOdysseyLayerCell* iCell )
                                                                           {
                                                                               return !!iCell;
                                                                           } );
        for( UOdysseyLayerCell* cell : cells )
        {
            {
                FAnimationCutEntry animationcutentry( previous_cell, cell );
                FFrameNumber frame_in_timeline = animationcutentry.GetFrameReference();
                FFrameNumber frame_in_sequence = ConvertFrameFromTimelineToSequence( frame_in_timeline );

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

            previous_cell = cell;

            if( cell == cells.Last() )
            {
                FAnimationCutEntry animationcutentry( previous_cell, nullptr );
                FFrameNumber frame_in_timeline = animationcutentry.GetFrameReference();
                FFrameNumber frame_in_sequence = ConvertFrameFromTimelineToSequence( frame_in_timeline );

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

    MarkAsChanged();
    //BroadcastChanged();
    mOnAnimationCutChannelChanged.Broadcast();
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
    // Default value if animation doesn't exist
    // Otherwise animation validity must be checked everywhere outside this function
    FFrameRate animation_framerate( 24000, 1000 );
    if( ensure( GetAnimation() ) )
        animation_framerate = FFrameRate( GetAnimation()->GetFramesPerSecond() * 1000, 1000 );

    // Convert a real timeline frame to a sequence frame (change framerate from animation framerate to sequence framerate (aka tickresolution))
    FFrameTime frametime_in_timeline = FFrameRate::TransformTime( iFrameInTimeline, animation_framerate, sequence->GetMovieScene()->GetTickResolution() );
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
    // Default value if animation doesn't exist
    // Otherwise animation validity must be checked everywhere outside this function
    FFrameRate animation_framerate( 24000, 1000 );
    if( ensure( GetAnimation() ) )
        animation_framerate = FFrameRate( GetAnimation()->GetFramesPerSecond() * 1000, 1000 );

    // Apply all offsets in sequence framerate (aka tickresolution)
    FFrameNumber frame_in_section = iFrameInSequence - GetTrueRange().GetLowerBoundValue();
    FFrameNumber frame_in_timeline = frame_in_section + GetStartFrameOffset();

    // Convert a sequence frame to a real timeline frame (change framerate from sequence framerate (aka tickresolution) to animation framerate)
    FFrameTime frametime_in_timeline = FFrameRate::TransformTime( frame_in_timeline, sequence->GetMovieScene()->GetTickResolution(), animation_framerate );
    frame_in_timeline = frametime_in_timeline.GetFrame();

    return frame_in_timeline;
}

#undef LOCTEXT_NAMESPACE
