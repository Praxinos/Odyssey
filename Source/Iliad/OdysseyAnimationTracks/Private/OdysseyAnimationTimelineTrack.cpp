// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyAnimationTimelineTrack.h"

#include "MovieScene.h"

#include "OdysseyAnimationTimelineTemplate.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationTimelineTrack)

#define LOCTEXT_NAMESPACE "Animation"

UOdysseyAnimationTimelineTrack::UOdysseyAnimationTimelineTrack(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedBlendTypes = FMovieSceneBlendTypeField::None();
}

bool
UOdysseyAnimationTimelineTrack::SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const
{
    return SectionClass == UOdysseyAnimationTimelineSection::StaticClass();
}

UMovieSceneSection*
UOdysseyAnimationTimelineTrack::CreateNewSection()
{
    return NewObject<UOdysseyAnimationTimelineSection>(this, NAME_None, RF_Transactional);
}

#if WITH_EDITORONLY_DATA

FText
UOdysseyAnimationTimelineTrack::GetDisplayName() const
{
    return LOCTEXT("TrackName", "Animation");
}

#endif

UMovieSceneSection*
UOdysseyAnimationTimelineTrack::AddNewSection(FFrameNumber KeyTime, float iDurationInSeconds)
{

    UMovieScene* movieScene = GetTypedOuter<UMovieScene>();
    if (!movieScene)
        return nullptr;

    UOdysseyAnimationTimelineSection* NewSection = Cast<UOdysseyAnimationTimelineSection>(CreateNewSection());
    NewSection->InitialPlacement(Sections, KeyTime, movieScene->GetTickResolution().AsFrameNumber(iDurationInSeconds).Value, 0);

    AddSection(*NewSection);
    UpdateEasing();

    return NewSection;
}

EMovieSceneTrackEasingSupportFlags
UOdysseyAnimationTimelineTrack::SupportsEasing(FMovieSceneSupportsEasingParams& Params) const
{
    return EMovieSceneTrackEasingSupportFlags::None;
}

bool
UOdysseyAnimationTimelineTrack::SupportsMultipleRows() const
{
    return false;
}

FMovieSceneEvalTemplatePtr
UOdysseyAnimationTimelineTrack::CreateTemplateForSection(const UMovieSceneSection& InSection) const
{
    return FOdysseyAnimationTimelineTemplate(*CastChecked<const UOdysseyAnimationTimelineSection>(&InSection), *this);
}

#undef LOCTEXT_NAMESPACE
