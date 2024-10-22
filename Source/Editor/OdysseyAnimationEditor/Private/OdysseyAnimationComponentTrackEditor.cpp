// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationComponentTrackEditor.h"

#include "OdysseyAnimationComponentTrack.h"
#include "OdysseyAnimationComponentSection.h"
#include "SequencerUtilities.h"
#include "MVVM/Extensions/IOutlinerExtension.h"
#include "MVVM/Extensions/ITrackExtension.h"
#include "MVVM/ViewModels/EditorViewModel.h"
#include "MVVM/ViewModels/OutlinerColumns/OutlinerColumnTypes.h"
#include "MVVM/ViewModels/SequencerEditorViewModel.h"
#include "MVVM/ViewModelPtr.h"
#include "MVVM/Views/SOutlinerItemViewBase.h"
#include "Widgets/SOdysseyAnimationComponentTrack.h"
#include "OdysseyAnimationComponentTrackEditorSection.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationComponentTrackEditor::~FOdysseyAnimationComponentTrackEditor()
{
}

FOdysseyAnimationComponentTrackEditor::FOdysseyAnimationComponentTrackEditor( TSharedRef<ISequencer> InSequencer )
	: FMovieSceneTrackEditor( InSequencer )
{
}

TSharedRef<ISequencerTrackEditor>
FOdysseyAnimationComponentTrackEditor::CreateTrackEditor( TSharedRef<ISequencer> OwningSequencer )
{
	return MakeShareable( new FOdysseyAnimationComponentTrackEditor( OwningSequencer ) );
}

void
FOdysseyAnimationComponentTrackEditor::OnNewActorTrackAdded(const AActor& iActor, const FGuid& iBinding, TSharedPtr< ISequencer > iSequencer)
{
	const AActor* actor = &iActor;
	if (!actor->IsA<AOdysseyAnimationActor>())
		return;

	const AOdysseyAnimationActor* animationActor = Cast<const AOdysseyAnimationActor>(actor);
	if (!animationActor)
		return;

	UOdysseyAnimationComponent* animationComponent = animationActor->AnimationComponent;
	if (!animationComponent)
		return;
	
	UMovieScene* MovieScene = iSequencer->GetFocusedMovieSceneSequence()->GetMovieScene();
	if (MovieScene == nullptr || MovieScene->IsReadOnly())
		return;

	// @todo Sequencer - The sequencer probably should have taken care of this
	iSequencer->GetFocusedMovieSceneSequence()->SetFlags(RF_Transactional);
	
	// Create a transaction record because we are about to add keys
	const bool bShouldActuallyTransact = !GIsTransacting;		// Don't transact if we're recording in a PIE world.  That type of keyframe capture cannot be undone.
	FScopedTransaction AutoKeyTransaction( LOCTEXT("PropertyChanged", "Animatable Property Changed"), bShouldActuallyTransact );

	FGuid componentBinding = FSequencerUtilities::CreateBinding(iSequencer.ToSharedRef(), *animationComponent);

	UMovieSceneTrack* NewTrack = MovieScene->AddTrack(UOdysseyAnimationComponentTrack::StaticClass(), componentBinding);
	if (!NewTrack)
		return;

	UOdysseyAnimationComponentTrack* animationTrack = Cast<UOdysseyAnimationComponentTrack>(NewTrack);
	if (!animationTrack)
		return;

	animationTrack->Modify();
	animationTrack->Component = animationComponent;
	UMovieSceneSection* section = animationTrack->AddNewSection(iSequencer->GetLocalTime().Time.FrameNumber);

	iSequencer->EmptySelection();
	iSequencer->SelectSection(section);
	iSequencer->ThrobSectionSelection();
}

bool
FOdysseyAnimationComponentTrackEditor::SupportsType( TSubclassOf<class UMovieSceneTrack> TrackClass ) const
{
	return TrackClass == UOdysseyAnimationComponentTrack::StaticClass();
}

void
FOdysseyAnimationComponentTrackEditor::BuildObjectBindingTrackMenu(FMenuBuilder& iMenuBuilder, const TArray<FGuid>& iObjectBindings, const UClass* iObjectClass)
{
	if (!iObjectClass->IsChildOf(UOdysseyAnimationComponent::StaticClass()))
		return;

	iMenuBuilder.AddMenuEntry(
		LOCTEXT("component-track.object-binding-track-menu.animation-track.name", "Animation Track"),
		LOCTEXT("component-track.object-binding-track-menu.animation-track.tooltip", "Adds a track that can play an animation component."),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateRaw(this, &FOdysseyAnimationComponentTrackEditor::AddAnimationTrack, iObjectBindings)
		),
		NAME_None,
		EUserInterfaceActionType::Button
	);
}

void
FOdysseyAnimationComponentTrackEditor::AddAnimationTrack(TArray<FGuid> ObjectBindings)
{
	UMovieScene* FocusedMovieScene = GetFocusedMovieScene();
	if (FocusedMovieScene == nullptr || FocusedMovieScene->IsReadOnly())
		return;

	AnimatablePropertyChanged(FOnKeyProperty::CreateRaw(this, &FOdysseyAnimationComponentTrackEditor::AddAnimationTrackKeyInternal, ObjectBindings));
}

FKeyPropertyResult
FOdysseyAnimationComponentTrackEditor::AddAnimationTrackKeyInternal(FFrameNumber KeyTime, TArray<FGuid> ObjectBindings)
{
	FKeyPropertyResult KeyPropertyResult;

	TArray<UMovieSceneSection*> NewSections;
	TSharedPtr<ISequencer> SequencerPtr = GetSequencer();
	if (SequencerPtr.IsValid())
	{
		for (const FGuid& ObjectBindingGuid : ObjectBindings)
		{
			if (ObjectBindingGuid.IsValid())
			{
				FFindOrCreateTrackResult TrackResult = FindOrCreateTrackForObject(ObjectBindingGuid, UOdysseyAnimationComponentTrack::StaticClass());
				UMovieSceneTrack* Track = TrackResult.Track;
				UOdysseyAnimationComponentTrack* animationTrack = Cast<UOdysseyAnimationComponentTrack>(Track);
				KeyPropertyResult.bTrackCreated |= TrackResult.bWasCreated;

				if (ensure(animationTrack) && animationTrack->CanModify())
				{
					UOdysseyAnimationComponent* component = nullptr;
					TArrayView<TWeakObjectPtr<>> boundObjects = SequencerPtr->FindObjectsInCurrentSequence(ObjectBindingGuid);
					for (TWeakObjectPtr<>& boundObjectPtr : boundObjects)
					{
						UObject* boundObject = boundObjectPtr.Get();
						if (!boundObject)
							continue;

						if (!boundObject->IsA<UOdysseyAnimationComponent>())
							continue;

						component = Cast<UOdysseyAnimationComponent>(boundObject);
						if (!component)
							continue;

						break;
					}

					if (component)
					{
						animationTrack->Modify();
						
						if (TrackResult.bWasCreated)
						{
							animationTrack->Component = component;
						}

						UMovieSceneSection* NewSection = animationTrack->AddNewSection(KeyTime);
						KeyPropertyResult.bTrackModified = true;
						KeyPropertyResult.SectionsCreated.Add(NewSection);
						NewSections.Add(NewSection);
					}
				}
			}
		}
	}

	if (NewSections.Num() > 0)
	{
		GetSequencer()->EmptySelection();
		for (UMovieSceneSection* NewSection : NewSections)
		{
			GetSequencer()->SelectSection(NewSection);
		}
		GetSequencer()->ThrobSectionSelection();
	}

	return KeyPropertyResult;
}

TSharedPtr<SWidget>
FOdysseyAnimationComponentTrackEditor::BuildOutlinerColumnWidget(const FBuildColumnWidgetParams& iParams, const FName& iColumnName)
{
	UOdysseyAnimationComponentTrack* track = Cast<UOdysseyAnimationComponentTrack>(iParams.TrackModel->GetTrack());
	::UE::Sequencer::TViewModelPtr< ::UE::Sequencer::FSequencerEditorViewModel > editorViewModel = iParams.Editor->CastThisShared< ::UE::Sequencer::FSequencerEditorViewModel >();
	::UE::Sequencer::TViewModelPtr<::UE::Sequencer::IOutlinerExtension>        outlinerExtension = iParams.ViewModel.ImplicitCast();
	if (!track || !editorViewModel || !outlinerExtension)
		return SNullWidget::NullWidget;

	if (iColumnName == ::UE::Sequencer::FCommonOutlinerNames::Edit)
		return nullptr;

	if (iColumnName == ::UE::Sequencer::FCommonOutlinerNames::Add)
		return nullptr;

	if (iColumnName == ::UE::Sequencer::FCommonOutlinerNames::Label)
	{
		TSharedPtr<ISequencer> SequencerPtr = GetSequencer();
		if (!SequencerPtr)
			return nullptr;

		TSharedPtr<SVerticalBox> verticalBox = SNew(SVerticalBox);
			/* + SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(::UE::Sequencer::SOutlinerItemViewBase, outlinerExtension, iParams.Editor, iParams.TreeViewRow)
			]; */

		TArrayView<TWeakObjectPtr<>> boundObjects = SequencerPtr->FindObjectsInCurrentSequence(track->FindObjectBindingGuid());
		for (TWeakObjectPtr<>& boundObjectPtr : boundObjects)
		{
			UObject* boundObject = boundObjectPtr.Get();
			if (!boundObject)
				continue;

			if (!boundObject->IsA<UOdysseyAnimationComponent>())
				continue;

			UOdysseyAnimationComponent* animationComponent = Cast<UOdysseyAnimationComponent>(boundObject);
			if (!animationComponent)
				continue;

			verticalBox->AddSlot()
			.AutoHeight()
			[
				SNew(SOdysseyAnimationComponentTrack, animationComponent)
				.Clipping(EWidgetClipping::ClipToBoundsAlways)
				/* SNew(SOdysseyAnimationLayerStack)
				.Animation_UObject(animationComponent, &UOdysseyAnimationComponent::GetActiveAnimation)
				.Player_UObject(animationComponent, &UOdysseyAnimationComponent::GetActivePlayer)
				.PlayerControlsVisibility(EVisibility::Collapsed)
				.PlayerControlsVisibility(EVisibility::Collapsed)
				.TimelinePosition(timelinePosition)
				.TimelineCellSelection(timelineCellSelection)
				.OnActivateOutOfPegs_Lambda([](UOdysseyAnimationCell* iCell){})
				.OnInactivateOutOfPegs_Lambda([](){})
				.OnIsOutOfPegsChecked_Lambda([](UOdysseyAnimationCell* iCell){ return ECheckBoxState::Unchecked; }) */
			];
		}
		
		return verticalBox;
	}

	return FMovieSceneTrackEditor::BuildOutlinerColumnWidget(iParams, iColumnName);
}

TSharedRef<ISequencerSection>
FOdysseyAnimationComponentTrackEditor::MakeSectionInterface( UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding )
{
	UOdysseyAnimationComponentSection* animationComponentSection = Cast<UOdysseyAnimationComponentSection>(&SectionObject);
	checkf( animationComponentSection != nullptr, TEXT("Unsupported section type.") );

	return MakeShareable(new FOdysseyAnimationComponentTrackEditorSection(GetSequencer(), animationComponentSection));
}

#undef LOCTEXT_NAMESPACE