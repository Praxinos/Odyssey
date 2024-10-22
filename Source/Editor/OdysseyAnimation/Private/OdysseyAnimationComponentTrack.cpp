// Copyright Epic Games, Inc. All Rights Reserved.

#include "OdysseyAnimationComponentTrack.h"
#include "MovieScene.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationComponentTrack)

#define LOCTEXT_NAMESPACE "Animation"

UOdysseyAnimationComponentTrack::UOdysseyAnimationComponentTrack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedBlendTypes.Add(EMovieSceneBlendType::Absolute);
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
UOdysseyAnimationComponentTrack::AddNewSection(FFrameNumber KeyTime)
{
	UOdysseyAnimationComponentSection* NewSection = Cast<UOdysseyAnimationComponentSection>(CreateNewSection());
	{
		UMovieScene* OuterMovieScene = GetTypedOuter<UMovieScene>();
		UOdysseyAnimation* animation = Component->GetActiveAnimation();
		if (animation)
		{
			FInt32Range range = animation->GetFrameRange();
			float duration = 0.f;
			int32 lastFrame = range.GetUpperBoundValue();
			if (lastFrame >= 0)
			{
				duration = (lastFrame + 1) / animation->GetFramesPerSecond();
			}
		
			NewSection->InitialPlacement(Sections, KeyTime, OuterMovieScene->GetTickResolution().AsFrameNumber(duration).Value, false);
		}
		else
		{
			
			NewSection->InitialPlacement(Sections, KeyTime, OuterMovieScene->GetTickResolution().AsFrameNumber(10).Value, false);
		}
	}

	AddSection(*NewSection);
	UpdateEasing();

	return NewSection;
}

#undef LOCTEXT_NAMESPACE

