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
