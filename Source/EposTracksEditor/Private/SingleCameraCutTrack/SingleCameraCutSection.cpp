// Copyright Epic Games, Inc. All Rights Reserved.

#include "SingleCameraCutTrack/SingleCameraCutSection.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"
#include "Textures/SlateIcon.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GameFramework/Actor.h"
#include "Editor.h"
#include "MovieScene.h"
#include "SequencerSectionPainter.h"
#include "ScopedTransaction.h"
#include "MovieSceneSequence.h"
#include "MovieSceneCommonHelpers.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "EditorStyleSet.h"
#include "EngineUtils.h"
#include "Camera/CameraComponent.h"


#define LOCTEXT_NAMESPACE "FSingleCameraCutSection"


/* FCameraCutSection structors
 *****************************************************************************/

FSingleCameraCutSection::FSingleCameraCutSection(TSharedPtr<ISequencer> InSequencer, TSharedPtr<FTrackEditorThumbnailPool> InThumbnailPool, UMovieSceneSection& InSection) : FViewportThumbnailSection(InSequencer, InThumbnailPool, InSection)
{
	AdditionalDrawEffect = ESlateDrawEffect::NoGamma;
}

FSingleCameraCutSection::~FSingleCameraCutSection()
{
}


/* ISequencerSection interface
 *****************************************************************************/

void FSingleCameraCutSection::SetSingleTime(double GlobalTime)
{
	UMovieSceneSingleCameraCutSection* CameraCutSection = Cast<UMovieSceneSingleCameraCutSection>(Section);
	if (CameraCutSection && CameraCutSection->HasStartFrame())
	{
		double ReferenceOffsetSeconds = CameraCutSection->GetInclusiveStartFrame() / CameraCutSection->GetTypedOuter<UMovieScene>()->GetTickResolution();
		CameraCutSection->SetThumbnailReferenceOffset(GlobalTime - ReferenceOffsetSeconds);
	}
}

void FSingleCameraCutSection::Tick(const FGeometry& AllottedGeometry, const FGeometry& ClippedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	UMovieSceneSingleCameraCutSection* CameraCutSection = Cast<UMovieSceneSingleCameraCutSection>(Section);
	if (CameraCutSection)
	{
		if (GetDefault<UMovieSceneUserThumbnailSettings>()->bDrawSingleThumbnails && CameraCutSection->HasStartFrame())
		{
			double ReferenceOffsetSeconds = CameraCutSection->GetInclusiveStartFrame() / CameraCutSection->GetTypedOuter<UMovieScene>()->GetTickResolution() + CameraCutSection->GetThumbnailReferenceOffset();
			ThumbnailCache.SetSingleReferenceFrame(ReferenceOffsetSeconds);
		}
		else
		{
			ThumbnailCache.SetSingleReferenceFrame(TOptional<double>());
		}
	}

	FViewportThumbnailSection::Tick(AllottedGeometry, ClippedGeometry, InCurrentTime, InDeltaTime);
}

void FSingleCameraCutSection::BuildSectionContextMenu(FMenuBuilder& MenuBuilder, const FGuid& ObjectBinding)
{
	FViewportThumbnailSection::BuildSectionContextMenu(MenuBuilder, ObjectBinding);

	UWorld* World = GEditor->GetEditorWorldContext().World();

	if (World == nullptr || !Section->HasStartFrame())
	{
		return;
	}

	AActor* CameraActor = GetCameraForFrame(Section->GetInclusiveStartFrame());

	if (CameraActor)
	{
		MenuBuilder.AddMenuSeparator();

		MenuBuilder.AddMenuEntry(
			FText::Format(LOCTEXT("SelectCameraTextFormat", "Select {0}"), FText::FromString(CameraActor->GetActorLabel())),
			FText::Format(LOCTEXT("SelectCameraTooltipFormat", "Select {0}"), FText::FromString(CameraActor->GetActorLabel())),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateRaw(this, &FSingleCameraCutSection::HandleSelectCameraMenuEntryExecute, CameraActor))
		);
	}

	// get list of available cameras
	TArray<AActor*> AllCameras;

	for (FActorIterator ActorIt(World); ActorIt; ++ActorIt)
	{
		AActor* Actor = *ActorIt;

		if ((Actor != CameraActor) && Actor->IsListedInSceneOutliner())
		{
			UCameraComponent* CameraComponent = MovieSceneHelpers::CameraComponentFromActor(Actor);
			if (CameraComponent)
			{
				AllCameras.Add(Actor);
			}
		}
	}

	if (AllCameras.Num() == 0)
	{
		return;
	}

	MenuBuilder.BeginSection(NAME_None, LOCTEXT("ChangeCameraMenuText", "Change Camera"));
	{
		for (auto EachCamera : AllCameras)
		{
			FText ActorLabel = FText::FromString(EachCamera->GetActorLabel());

			MenuBuilder.AddMenuEntry(
				FText::Format(LOCTEXT("SetCameraMenuEntryTextFormat", "{0}"), ActorLabel),
				FText::Format(LOCTEXT("SetCameraMenuEntryTooltipFormat", "Assign {0} to this camera cut"), FText::FromString(EachCamera->GetPathName())),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateRaw(this, &FSingleCameraCutSection::HandleSetCameraMenuEntryExecute, EachCamera))
			);
		}
	}
	MenuBuilder.EndSection();
}


/* FThumbnailSection interface
 *****************************************************************************/

AActor* FSingleCameraCutSection::GetCameraForFrame(FFrameNumber Time) const
{
	UMovieSceneSingleCameraCutSection* CameraCutSection = Cast<UMovieSceneSingleCameraCutSection>(Section);
	TSharedPtr<ISequencer> Sequencer = SequencerPtr.Pin();

	if (CameraCutSection && Sequencer.IsValid())
	{
		UCameraComponent* CameraComponent = CameraCutSection->GetFirstCamera(*Sequencer, Sequencer->GetFocusedTemplateID());
		if (CameraComponent)
		{
			return CameraComponent->GetOwner();
		}

		FMovieSceneSpawnable* Spawnable = Sequencer->GetFocusedMovieSceneSequence()->GetMovieScene()->FindSpawnable(CameraCutSection->GetCameraBindingID().GetGuid());
		if (Spawnable)
		{
			return Cast<AActor>(Spawnable->GetObjectTemplate());
		}
	}

	return nullptr;
}

FText FSingleCameraCutSection::GetSectionTitle() const
{
	return HandleThumbnailTextBlockText();
}

float FSingleCameraCutSection::GetSectionHeight() const
{
	return FViewportThumbnailSection::GetSectionHeight() + 10.f;
}

FMargin FSingleCameraCutSection::GetContentPadding() const
{
	return FMargin(6.f, 10.f);
}

int32 FSingleCameraCutSection::OnPaintSection(FSequencerSectionPainter& InPainter) const
{
	static const FSlateBrush* FilmBorder = FEditorStyle::GetBrush("Sequencer.Section.FilmBorder");

	InPainter.LayerId = InPainter.PaintSectionBackground();
	return FViewportThumbnailSection::OnPaintSection(InPainter);
}

FText FSingleCameraCutSection::HandleThumbnailTextBlockText() const
{
	const AActor* CameraActor = Section->HasStartFrame() ? GetCameraForFrame(Section->GetInclusiveStartFrame()) : nullptr;
	if (CameraActor)
	{
		return FText::FromString(CameraActor->GetActorLabel());
	}

	return FText::GetEmpty();
}


/* FCameraCutSection callbacks
 *****************************************************************************/

void FSingleCameraCutSection::HandleSelectCameraMenuEntryExecute(AActor* InCamera)
{
	GEditor->SelectActor(InCamera, true, true);
}

void FSingleCameraCutSection::HandleSetCameraMenuEntryExecute(AActor* InCamera)
{
	auto Sequencer = SequencerPtr.Pin();

	if (Sequencer.IsValid())
	{
		FGuid ObjectGuid = Sequencer->GetHandleToObject(InCamera, true);

		UMovieSceneSingleCameraCutSection* CameraCutSection = Cast<UMovieSceneSingleCameraCutSection>(Section);

		CameraCutSection->SetFlags(RF_Transactional);

		const FScopedTransaction Transaction(LOCTEXT("SetCameraCut", "Set Camera Cut"));

		CameraCutSection->Modify();
	
		CameraCutSection->SetCameraGuid(ObjectGuid);
	
		Sequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
	}
}

UCameraComponent* FSingleCameraCutSection::GetViewCamera()
{
	UMovieSceneSingleCameraCutSection* CameraCutSection = Cast<UMovieSceneSingleCameraCutSection>(Section);
	TSharedPtr<ISequencer>             Sequencer        = SequencerPtr.Pin();

	if (CameraCutSection && Sequencer.IsValid())
	{
		return CameraCutSection->GetFirstCamera(*Sequencer, Sequencer->GetFocusedTemplateID());
	}

	return nullptr;
}


#undef LOCTEXT_NAMESPACE
