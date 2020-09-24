// Copyright Epic Games, Inc. All Rights Reserved.

#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
#include "Tracks/MovieSceneSpawnTrack.h"
#include "MovieScene.h"
#include "MovieSceneCommonHelpers.h"
#include "MovieSceneTimeHelpers.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"
#include "Evaluation/MovieSceneEvaluationTrack.h"
#include "Compilation/MovieSceneCompilerRules.h"
#include "IMovieSceneTracksModule.h"

#define LOCTEXT_NAMESPACE "MovieSceneSingleCameraCutTrack"

/* UMovieSceneCameraCutTrack interface
 *****************************************************************************/
UMovieSceneSingleCameraCutTrack::UMovieSceneSingleCameraCutTrack( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
	, bCanBlend(false)
{
#if WITH_EDITORONLY_DATA
	TrackTint = FColor(120, 120, 120, 65);
#endif

	// By default, don't evaluate camera cuts in pre and postroll
	EvalOptions.bEvaluateInPreroll = EvalOptions.bEvaluateInPostroll = false;

	SupportedBlendTypes.Add(EMovieSceneBlendType::Absolute);
}

UMovieSceneSingleCameraCutSection* UMovieSceneSingleCameraCutTrack::AddNewSingleCameraCut(const FMovieSceneObjectBindingID& CameraBindingID, FFrameNumber StartTime)
{
	Modify();

	FFrameNumber NewSectionEndTime = FindEndTimeForCameraCut(StartTime);

	// If there's an existing section, just swap the camera guid
	UMovieSceneSingleCameraCutSection* ExistingSection = nullptr;
	for (auto Section : Sections)
	{
		if (Section->HasStartFrame() && Section->HasEndFrame() && Section->GetInclusiveStartFrame() == StartTime && Section->GetExclusiveEndFrame() == NewSectionEndTime)
		{
			ExistingSection = Cast<UMovieSceneSingleCameraCutSection>(Section);
			break;
		}
	}

	UMovieSceneSingleCameraCutSection* NewSection = ExistingSection;
	if (ExistingSection != nullptr)
	{
		ExistingSection->SetCameraBindingID(CameraBindingID);
	}
	else
	{
		NewSection = NewObject<UMovieSceneSingleCameraCutSection>(this, NAME_None, RF_Transactional);
		NewSection->SetRange(TRange<FFrameNumber>(StartTime, NewSectionEndTime));
		NewSection->SetCameraBindingID(CameraBindingID);

		AddSection(*NewSection);
	}

	// When a new CameraCut is added, sort all CameraCuts to ensure they are in the correct order
	MovieSceneHelpers::SortConsecutiveSections(Sections);

	// Once CameraCuts are sorted fixup the surrounding CameraCuts to fix any gaps
	if (bCanBlend)
	{
		MovieSceneHelpers::FixupConsecutiveBlendingSections(Sections, *NewSection, false);
	}
	else
	{
		MovieSceneHelpers::FixupConsecutiveSections(Sections, *NewSection, false);
	}

	return NewSection;
}


/* UMovieSceneTrack interface
 *****************************************************************************/

void UMovieSceneSingleCameraCutTrack::AddSection(UMovieSceneSection& Section)
{
	if (UMovieSceneSingleCameraCutSection* CutSection = Cast<UMovieSceneSingleCameraCutSection>(&Section))
	{
		Sections.Add(CutSection);
	}
}

bool UMovieSceneSingleCameraCutTrack::SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const
{
	return SectionClass == UMovieSceneSingleCameraCutSection::StaticClass();
}

UMovieSceneSection* UMovieSceneSingleCameraCutTrack::CreateNewSection()
{
	return NewObject<UMovieSceneSingleCameraCutSection>(this, NAME_None, RF_Transactional);
}

bool UMovieSceneSingleCameraCutTrack::SupportsMultipleRows() const
{
	return false;
}

EMovieSceneTrackEasingSupportFlags UMovieSceneSingleCameraCutTrack::SupportsEasing(FMovieSceneSupportsEasingParams& Params) const
{
	if (!bCanBlend)
	{
		return EMovieSceneTrackEasingSupportFlags::None;
	}
	if (Params.ForSection != nullptr)
	{
		const int32 NumSections = Sections.Num();
		if (NumSections == 1)
		{
			return EMovieSceneTrackEasingSupportFlags::AutomaticEasing | EMovieSceneTrackEasingSupportFlags::ManualEasing;
		}
		else if (NumSections > 1)
		{
			if (Params.ForSection == Sections[0])
			{
				return EMovieSceneTrackEasingSupportFlags::AutomaticEasing | EMovieSceneTrackEasingSupportFlags::ManualEaseIn;
			}
			if (Params.ForSection == Sections.Last())
			{
				return EMovieSceneTrackEasingSupportFlags::AutomaticEasing | EMovieSceneTrackEasingSupportFlags::ManualEaseOut;
			}
		}
	}
	return EMovieSceneTrackEasingSupportFlags::AutomaticEasing;
}

const TArray<UMovieSceneSection*>& UMovieSceneSingleCameraCutTrack::GetAllSections() const
{
	return Sections;
}

void UMovieSceneSingleCameraCutTrack::RemoveSection(UMovieSceneSection& Section)
{
	Sections.Remove(&Section);

	if (bCanBlend)
	{
		MovieSceneHelpers::FixupConsecutiveSections(Sections, Section, true);
	}
	else
	{
		MovieSceneHelpers::FixupConsecutiveBlendingSections(Sections, Section, true);
	}

	// @todo Sequencer: The movie scene owned by the section is now abandoned.  Should we offer to delete it?  
}

void UMovieSceneSingleCameraCutTrack::RemoveSectionAt(int32 SectionIndex)
{
	UMovieSceneSection* SectionToDelete = Sections[SectionIndex];
	if (bCanBlend)
	{
		MovieSceneHelpers::FixupConsecutiveSections(Sections, *SectionToDelete, true);
	}
	else
	{
		MovieSceneHelpers::FixupConsecutiveBlendingSections(Sections, *SectionToDelete, true);
	}

	Sections.RemoveAt(SectionIndex);
	MovieSceneHelpers::SortConsecutiveSections(Sections);
}

void UMovieSceneSingleCameraCutTrack::RemoveAllAnimationData()
{
	Sections.Empty();
}

#if WITH_EDITORONLY_DATA
FText UMovieSceneSingleCameraCutTrack::GetDefaultDisplayName() const
{
	return LOCTEXT("TrackName", "Camera Cuts");
}
#endif


#if WITH_EDITOR
void UMovieSceneSingleCameraCutTrack::OnSectionMoved(UMovieSceneSection& Section, const FMovieSceneSectionMovedParams& Params)
{
	if (bCanBlend)
	{
		MovieSceneHelpers::FixupConsecutiveBlendingSections(Sections, Section, false);
	}
	else
	{
		MovieSceneHelpers::FixupConsecutiveSections(Sections, Section, false);
	}
}
#endif

FFrameNumber UMovieSceneSingleCameraCutTrack::FindEndTimeForCameraCut( FFrameNumber StartTime )
{
	UMovieScene* OwnerScene = GetTypedOuter<UMovieScene>();

	// End time should default to end where the movie scene ends. Ensure it is at least the same as start time (this should only happen when the movie scene has an initial time range smaller than the start time)
	FFrameNumber ExclusivePlayEnd = UE::MovieScene::DiscreteExclusiveUpper(OwnerScene->GetPlaybackRange());
	FFrameNumber ExclusiveEndTime = FMath::Max( ExclusivePlayEnd, StartTime );

	for( UMovieSceneSection* Section : Sections )
	{
		if( Section->HasStartFrame() && Section->GetInclusiveStartFrame() > StartTime )
		{
			ExclusiveEndTime = Section->GetInclusiveStartFrame();
			break;
		}
	}

	if( StartTime == ExclusiveEndTime )
	{
		// Give the CameraCut a reasonable length of time to start out with.  A 0 time CameraCut is not usable
		ExclusiveEndTime = (StartTime + .5f * OwnerScene->GetTickResolution()).FrameNumber;
	}

	return ExclusiveEndTime;
}

void UMovieSceneSingleCameraCutTrack::PreCompileImpl()
{
	for (UMovieSceneSection* Section : Sections)
	{
		if (UMovieSceneSingleCameraCutSection* CameraCutSection = CastChecked<UMovieSceneSingleCameraCutSection>(Section))
		{
			CameraCutSection->ComputeInitialCameraCutTransform();
		}
	}
}

#undef LOCTEXT_NAMESPACE
