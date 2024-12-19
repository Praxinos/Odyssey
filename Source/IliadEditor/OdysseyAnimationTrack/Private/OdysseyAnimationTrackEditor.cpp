// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationTrackEditor.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationActor.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationComponentTrack.h"
#include "OdysseyAnimationComponentSection.h"
#include "SequencerUtilities.h"
#include "MVVM/Extensions/ITrackExtension.h"
#include "MVVM/ViewModels/SequencerEditorViewModel.h"
#include "MVVM/ViewModels/OutlinerColumns/OutlinerColumnTypes.h"
#include "MVVM/Views/ViewUtilities.h"
#include "Widgets/SOdysseyAnimationComponentTrack.h"
#include "OdysseyAnimationTrackEditorSection.h"

#define LOCTEXT_NAMESPACE "AnimationTrack"

FOdysseyAnimationTrackEditor::~FOdysseyAnimationTrackEditor()
{
}

FOdysseyAnimationTrackEditor::FOdysseyAnimationTrackEditor( TSharedRef<ISequencer> InSequencer )
    : FMovieSceneTrackEditor( InSequencer )
{
}

TSharedRef<ISequencerTrackEditor>
FOdysseyAnimationTrackEditor::CreateTrackEditor( TSharedRef<ISequencer> OwningSequencer )
{
    return MakeShareable( new FOdysseyAnimationTrackEditor( OwningSequencer ) );
}

void
FOdysseyAnimationTrackEditor::OnNewActorTrackAdded(const AActor& iActor, const FGuid& iBinding, TSharedPtr< ISequencer > iSequencer)
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
    const bool bShouldActuallyTransact = !GIsTransacting;        // Don't transact if we're recording in a PIE world.  That type of keyframe capture cannot be undone.
    FScopedTransaction AutoKeyTransaction( LOCTEXT("PropertyChanged", "Animatable Property Changed"), bShouldActuallyTransact );

    FGuid componentBinding = FSequencerUtilities::CreateBinding(iSequencer.ToSharedRef(), *animationComponent);

    UMovieSceneTrack* NewTrack = MovieScene->AddTrack(UOdysseyAnimationComponentTrack::StaticClass(), componentBinding);
    if (!NewTrack)
        return;

    UOdysseyAnimationComponentTrack* animationTrack = Cast<UOdysseyAnimationComponentTrack>(NewTrack);
    if (!animationTrack)
        return;

    animationTrack->Modify();

    UMovieSceneSection* section = animationTrack->AddNewSection(iSequencer->GetLocalTime().Time.FrameNumber, GetDefaultSectionDuration(animationComponent));
    section->Modify();

    iSequencer->EmptySelection();
    iSequencer->SelectSection(section);
    iSequencer->ThrobSectionSelection();
}

float
FOdysseyAnimationTrackEditor::GetDefaultSectionDuration(UOdysseyAnimationComponent* iComponent)
{
    float duration = 10.f;
    if (!iComponent)
        return duration;

    UOdysseyAnimation* animation = iComponent->GetActiveAnimation();
    if (!animation)
        return duration;

    FInt32Range range = animation->GetFrameRange();
    int32 lastFrame = range.GetUpperBoundValue();
    if (lastFrame >= 0)
    {
        duration = (lastFrame + 1) / animation->GetFramesPerSecond();
    }

    return duration;
}

bool
FOdysseyAnimationTrackEditor::SupportsType( TSubclassOf<class UMovieSceneTrack> TrackClass ) const
{
    return TrackClass == UOdysseyAnimationComponentTrack::StaticClass();
}

void
FOdysseyAnimationTrackEditor::BuildObjectBindingTrackMenu(FMenuBuilder& iMenuBuilder, const TArray<FGuid>& iObjectBindings, const UClass* iObjectClass)
{
    if (!iObjectClass->IsChildOf(UOdysseyAnimationComponent::StaticClass()))
        return;

    iMenuBuilder.AddMenuEntry(
        LOCTEXT("component-track.object-binding-track-menu.animation-track.name", "Timeline"),
        LOCTEXT("component-track.object-binding-track-menu.animation-track.tooltip", "Adds a track that can play an animation component."),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateRaw(this, &FOdysseyAnimationTrackEditor::AddAnimationTrack, iObjectBindings)
        ),
        NAME_None,
        EUserInterfaceActionType::Button
    );
}

void
FOdysseyAnimationTrackEditor::AddAnimationTrack(TArray<FGuid> ObjectBindings)
{
    UMovieScene* FocusedMovieScene = GetFocusedMovieScene();
    if (FocusedMovieScene == nullptr || FocusedMovieScene->IsReadOnly())
        return;

    AnimatablePropertyChanged(FOnKeyProperty::CreateRaw(this, &FOdysseyAnimationTrackEditor::AddAnimationTrackKeyInternal, ObjectBindings));
}

FKeyPropertyResult
FOdysseyAnimationTrackEditor::AddAnimationTrackKeyInternal(FFrameNumber KeyTime, TArray<FGuid> ObjectBindings)
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
                        UMovieSceneSection* NewSection = animationTrack->AddNewSection(KeyTime, GetDefaultSectionDuration(component));
                        NewSection->Modify();
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
FOdysseyAnimationTrackEditor::BuildOutlinerColumnWidget(const FBuildColumnWidgetParams& iParams, const FName& iColumnName)
{
    UOdysseyAnimationComponentTrack* track = Cast<UOdysseyAnimationComponentTrack>(iParams.TrackModel->GetTrack());
    ::UE::Sequencer::TViewModelPtr< ::UE::Sequencer::FSequencerEditorViewModel > editorViewModel = iParams.Editor->CastThisShared< ::UE::Sequencer::FSequencerEditorViewModel >();
    ::UE::Sequencer::TViewModelPtr<::UE::Sequencer::IOutlinerExtension> outlinerExtension = iParams.ViewModel.ImplicitCast();
    if (!track || !editorViewModel || !outlinerExtension)
        return nullptr;


    TSharedPtr<ISequencer> SequencerPtr = GetSequencer();
    if (!SequencerPtr)
        return nullptr;

    UOdysseyAnimationComponent* component = nullptr;
    TArrayView<TWeakObjectPtr<>> boundObjects = SequencerPtr->FindObjectsInCurrentSequence(track->FindObjectBindingGuid());
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
    }

    if (!component)
        return nullptr;

    if (iColumnName == ::UE::Sequencer::FCommonOutlinerNames::Edit)
        return nullptr;

    if (iColumnName == ::UE::Sequencer::FCommonOutlinerNames::Add)
    {
        FGuid objectBinding = track->FindObjectBindingGuid();
        return /*SNew(SBox)
            .HeightOverride(FOdysseyAnimationTrackEditorSection::GetUncollapsedSectionHeight(component))
            .VAlign(VAlign_Top)
            [
                SNew(SBox)
                .HeightOverride(FOdysseyAnimationTrackEditorSection::GetCollapsedSectionHeight())
                .VAlign(VAlign_Center)
                .HAlign(HAlign_Center)
                [*/
                    UE::Sequencer::MakeAddButton(LOCTEXT("sequencer.animation-timeline-track.add-button.tooltip", "Add Section"), FOnClicked::CreateRaw(this, &FOdysseyAnimationTrackEditor::OnAddButtonClicked, objectBinding), iParams.ViewModel);
                /*]
            ];*/
    }

    if (iColumnName == ::UE::Sequencer::FCommonOutlinerNames::Label)
    {
        return SNew(SOdysseyAnimationComponentTrack, component, track, iParams)
                .Clipping(EWidgetClipping::ClipToBoundsAlways);
    }

    return FMovieSceneTrackEditor::BuildOutlinerColumnWidget(iParams, iColumnName);
}

TSharedRef<ISequencerSection>
FOdysseyAnimationTrackEditor::MakeSectionInterface( UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding )
{
    UOdysseyAnimationComponentSection* animationComponentSection = Cast<UOdysseyAnimationComponentSection>(&SectionObject);
    checkf( animationComponentSection != nullptr, TEXT("Unsupported section type.") );

    return MakeShareable(new FOdysseyAnimationTrackEditorSection(GetSequencer(), animationComponentSection));
}

FReply
FOdysseyAnimationTrackEditor::OnAddButtonClicked(FGuid iObjectBinding)
{
    AddAnimationTrack({ iObjectBinding });
    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
