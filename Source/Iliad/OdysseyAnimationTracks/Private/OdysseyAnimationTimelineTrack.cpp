// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationTimelineTrack.h"

#include "OdysseyAnimation.h"
#include "MovieScene.h"

#include "OdysseyAnimationTimelineTemplate.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationTimelineTrack)

#define LOCTEXT_NAMESPACE "Animation"

UOdysseyAnimationTimelineTrack::UOdysseyAnimationTimelineTrack(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedBlendTypes = FMovieSceneBlendTypeField::None();
}

UMovieSceneSection*
UOdysseyAnimationTimelineTrack::AddNewSection(FFrameNumber KeyTime, UOdysseyAnimation* iAnimation)
{
    UMovieScene* movieScene = GetTypedOuter<UMovieScene>();
    check( movieScene );

    UOdysseyAnimationTimelineSection* NewSection = Cast<UOdysseyAnimationTimelineSection>(CreateNewSection());
    NewSection->SetAnimation(iAnimation);

    if( NewSection->GetAnimation() )
    {
        TRange<FFrameNumber> defaultRange = UOdysseyAnimationTimelineSection::GetDefaultSectionRange( NewSection );
        int32 animationDuration = UE::MovieScene::DiscreteSize( defaultRange );

        FFrameRate animationFrameRate( iAnimation->GetFramesPerSecond() * 100, 100 );
        FInt32Range frameRange = iAnimation->GetFrameRange();
        FFrameNumber animationLeftBoundFrame( frameRange.GetLowerBoundValue() );

        // Compute the range (as it is done in InitialPlacement())
        TRange<FFrameNumber> animationRange = TRange<FFrameNumber>( KeyTime, KeyTime + animationDuration );
        check( animationDuration == UE::MovieScene::DiscreteSize( animationRange ) );

        // Get the playback range of the moviescene
        // And increase the upper bound of the animation range (if needed) to the playback upper bound
        // So when dropping an animation actor, the section will go to the playback upper bound (if needed),
        // and won't be only a 1 frame section for a 1 frame animation
        TRange<FFrameNumber> playbackRange = movieScene->GetPlaybackRange();
        if( TRangeBound<FFrameNumber>::MaxUpper( animationRange.GetUpperBound(), playbackRange.GetUpperBound() ) == playbackRange.GetUpperBound() )
            animationRange.SetUpperBound( playbackRange.GetUpperBound() );
        animationDuration = UE::MovieScene::DiscreteSize( animationRange );

        NewSection->SetStartFrameOffset( FFrameRate::TransformTime( animationLeftBoundFrame, animationFrameRate, movieScene->GetDisplayRate() ).GetFrame() );
        NewSection->InitialPlacement( Sections, KeyTime, animationDuration, false );
    }
    else
    {
        // If no animation, just create a 1 frame (in display rate) section
        TRange<FFrameNumber> defaultRange( FFrameRate::TransformTime( 0, movieScene->GetDisplayRate(), movieScene->GetTickResolution() ).GetFrame()
                                           , FFrameRate::TransformTime( 1, movieScene->GetDisplayRate(), movieScene->GetTickResolution() ).GetFrame() );
        int32 animationDuration = UE::MovieScene::DiscreteSize( defaultRange );
        NewSection->InitialPlacement( Sections, KeyTime, animationDuration, false );
    }

    AddSection(*NewSection);
    UpdateEasing();

    return NewSection;
}

//---

bool
UOdysseyAnimationTimelineTrack::SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const //override
{
    return SectionClass == UOdysseyAnimationTimelineSection::StaticClass();
}

EMovieSceneTrackEasingSupportFlags
UOdysseyAnimationTimelineTrack::SupportsEasing(FMovieSceneSupportsEasingParams& Params) const //override
{
    return EMovieSceneTrackEasingSupportFlags::None;
}

void
UOdysseyAnimationTimelineTrack::RemoveAllAnimationData() //override
{
    Sections.Empty();
}

bool
UOdysseyAnimationTimelineTrack::HasSection( const UMovieSceneSection& Section ) const //override
{
    return Sections.Contains( &Section );
}

void
UOdysseyAnimationTimelineTrack::AddSection( UMovieSceneSection& Section ) //override
{
    Sections.Add( &Section );
}

void
UOdysseyAnimationTimelineTrack::RemoveSection( UMovieSceneSection& Section ) //override
{
    Sections.Remove( &Section );
}

void
UOdysseyAnimationTimelineTrack::RemoveSectionAt( int32 SectionIndex ) //override
{
    Sections.RemoveAt( SectionIndex );
}

bool
UOdysseyAnimationTimelineTrack::IsEmpty() const //override
{
    return Sections.Num() == 0;
}

const TArray<UMovieSceneSection*>&
UOdysseyAnimationTimelineTrack::GetAllSections() const //override
{
    return Sections;
}

bool
UOdysseyAnimationTimelineTrack::SupportsMultipleRows() const //override
{
    return false;
}

UMovieSceneSection*
UOdysseyAnimationTimelineTrack::CreateNewSection() //override
{
    return NewObject<UOdysseyAnimationTimelineSection>(this, NAME_None, RF_Transactional);
}

FMovieSceneEvalTemplatePtr
UOdysseyAnimationTimelineTrack::CreateTemplateForSection(const UMovieSceneSection& InSection) const //override
{
    return FOdysseyAnimationTimelineTemplate(*CastChecked<const UOdysseyAnimationTimelineSection>(&InSection), *this);
}

#if WITH_EDITORONLY_DATA

FText
UOdysseyAnimationTimelineTrack::GetDefaultDisplayName() const //override
{
    return LOCTEXT("TrackName", "Animation");
}

#endif

//---

#if WITH_EDITORONLY_DATA

int32
UOdysseyAnimationTimelineTrack::GetRowHeight() const
{
    return RowHeight;
}

void
UOdysseyAnimationTimelineTrack::SetRowHeight( int32 NewRowHeight )
{
    RowHeight = FMath::Max( 16, NewRowHeight );
}

#endif

#undef LOCTEXT_NAMESPACE
