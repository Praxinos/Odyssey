// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyAnimationComponentTrack.h"
#include "OdysseyAnimationComponentTemplate.h"
#include "MovieScene.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationComponentTrack)

#define LOCTEXT_NAMESPACE "Animation"

UOdysseyAnimationComponentTrack::UOdysseyAnimationComponentTrack(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedBlendTypes = FMovieSceneBlendTypeField::None();
}

bool
UOdysseyAnimationComponentTrack::SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const
{
    return SectionClass == UOdysseyAnimationComponentSection::StaticClass();
}

UMovieSceneSection*
UOdysseyAnimationComponentTrack::CreateNewSection()
{
    return NewObject<UOdysseyAnimationComponentSection>(this, NAME_None, RF_Transactional);
}

#if WITH_EDITORONLY_DATA

FText
UOdysseyAnimationComponentTrack::GetDisplayName() const
{
    return LOCTEXT("TrackName", "Animation");
}

#endif

UMovieSceneSection*
UOdysseyAnimationComponentTrack::AddNewSection(FFrameNumber KeyTime, float iDurationInSeconds)
{

    UMovieScene* movieScene = GetTypedOuter<UMovieScene>();
    if (!movieScene)
        return nullptr;

    UOdysseyAnimationComponentSection* NewSection = Cast<UOdysseyAnimationComponentSection>(CreateNewSection());
    NewSection->InitialPlacement(Sections, KeyTime, movieScene->GetTickResolution().AsFrameNumber(iDurationInSeconds).Value, 0);

    AddSection(*NewSection);
    UpdateEasing();

    return NewSection;
}

EMovieSceneTrackEasingSupportFlags
UOdysseyAnimationComponentTrack::SupportsEasing(FMovieSceneSupportsEasingParams& Params) const
{
    return EMovieSceneTrackEasingSupportFlags::None;
}

bool
UOdysseyAnimationComponentTrack::SupportsMultipleRows() const
{
    return false;
}

FMovieSceneEvalTemplatePtr
UOdysseyAnimationComponentTrack::CreateTemplateForSection(const UMovieSceneSection& InSection) const
{
    return FOdysseyAnimationComponentTemplate(*CastChecked<const UOdysseyAnimationComponentSection>(&InSection), *this);
}

#undef LOCTEXT_NAMESPACE
