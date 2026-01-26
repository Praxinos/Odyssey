// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationTimelineTrackEditor.h"

#include "SequencerUtilities.h"
#include "MVVM/Extensions/ITrackExtension.h"
#include "MVVM/ViewModels/SequencerEditorViewModel.h"
#include "MVVM/ViewModels/OutlinerColumns/OutlinerColumnTypes.h"
#include "MVVM/Views/ViewUtilities.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationActor.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationTimelineTrack.h"
#include "OdysseyAnimationTimelineSection.h"
#include "OdysseyAnimationTimelineSectionEditor.h"
#include "OdysseyStyle.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Widgets/SOdysseyAnimationTimelineTrack.h"

#define LOCTEXT_NAMESPACE "AnimationTrack"

FOdysseyAnimationTimelineTrackEditor::~FOdysseyAnimationTimelineTrackEditor()
{
}

FOdysseyAnimationTimelineTrackEditor::FOdysseyAnimationTimelineTrackEditor( TSharedRef<ISequencer> InSequencer )
    : FMovieSceneTrackEditor( InSequencer )
{
}

TSharedRef<ISequencerTrackEditor>
FOdysseyAnimationTimelineTrackEditor::CreateTrackEditor( TSharedRef<ISequencer> OwningSequencer )
{
    return MakeShareable( new FOdysseyAnimationTimelineTrackEditor( OwningSequencer ) );
}

void
FOdysseyAnimationTimelineTrackEditor::OnNewActorTrackAdded(const AActor& iActor, const FGuid& iBinding, TSharedPtr< ISequencer > iSequencer)
{
    const AActor* actor = &iActor;
    if (!actor->IsA<AOdysseyAnimationActor>())
        return;

    const AOdysseyAnimationActor* animationActor = Cast<const AOdysseyAnimationActor>(actor);
    if (!animationActor)
        return;

    // "const AActor& iActor" cannot be changed (as a delegate signature)
    // but FSequencerUtilities::CreateBinding() takes a non const object
    // so use const_cast
    //TODO: is there another way ?
    UOdysseyAnimationComponent* animationComponent = const_cast<UOdysseyAnimationComponent*>( animationActor->GetAnimationComponent() );
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

    const bool bCreateHandleIfMissing = true;
    FGuid componentBinding = iSequencer->GetHandleToObject( animationComponent, bCreateHandleIfMissing );
    if( !componentBinding.IsValid() )
        return;

    UOdysseyAnimationTimelineTrack* animationTrack = MovieScene->FindTrack<UOdysseyAnimationTimelineTrack>( componentBinding );
    if( !animationTrack )
    {
        animationTrack = MovieScene->AddTrack<UOdysseyAnimationTimelineTrack>( componentBinding );
        check( animationTrack )
        animationTrack->Modify();

        UMovieSceneSection* section = animationTrack->AddNewSection( iSequencer->GetLocalTime().Time.FrameNumber, animationComponent->GetAnimation() );
        check( section );
        section->Modify();

        iSequencer->EmptySelection();
        iSequencer->SelectSection( section );
        iSequencer->ThrobSectionSelection();
    }
}

bool
FOdysseyAnimationTimelineTrackEditor::SupportsType( TSubclassOf<class UMovieSceneTrack> TrackClass ) const
{
    return TrackClass == UOdysseyAnimationTimelineTrack::StaticClass();
}

void
FOdysseyAnimationTimelineTrackEditor::BuildObjectBindingTrackMenu(FMenuBuilder& iMenuBuilder, const TArray<FGuid>& iObjectBindings, const UClass* iObjectClass)
{
    if (!iObjectClass->IsChildOf(UOdysseyAnimationComponent::StaticClass()))
        return;

    iMenuBuilder.AddMenuEntry(
        LOCTEXT("timeline-track.object-binding-track-menu.animation-track.name", "Timeline"),
        LOCTEXT("timeline-track.object-binding-track-menu.animation-track.tooltip", "Adds a track that can play an animation component."),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineTrackEditor::AddAnimationTrack, iObjectBindings)
        ),
        NAME_None,
        EUserInterfaceActionType::Button
    );
}

void
FOdysseyAnimationTimelineTrackEditor::AddAnimationTrack(TArray<FGuid> ObjectBindings)
{
    UMovieScene* FocusedMovieScene = GetFocusedMovieScene();
    if (FocusedMovieScene == nullptr || FocusedMovieScene->IsReadOnly())
        return;

    AnimatablePropertyChanged(FOnKeyProperty::CreateRaw(this, &FOdysseyAnimationTimelineTrackEditor::AddAnimationTrackKeyInternal, ObjectBindings));
}

FKeyPropertyResult
FOdysseyAnimationTimelineTrackEditor::AddAnimationTrackKeyInternal(FFrameNumber KeyTime, TArray<FGuid> ObjectBindings)
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
                FFindOrCreateTrackResult TrackResult = FindOrCreateTrackForObject(ObjectBindingGuid, UOdysseyAnimationTimelineTrack::StaticClass());

                UMovieSceneTrack* Track = TrackResult.Track;
                UOdysseyAnimationTimelineTrack* animationTrack = Cast<UOdysseyAnimationTimelineTrack>(Track);
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
                        UMovieSceneSection* NewSection = animationTrack->AddNewSection(KeyTime, component->GetAnimation());
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
FOdysseyAnimationTimelineTrackEditor::BuildOutlinerColumnWidget(const FBuildColumnWidgetParams& iParams, const FName& iColumnName)
{
    UOdysseyAnimationTimelineTrack* track = Cast<UOdysseyAnimationTimelineTrack>(iParams.TrackModel->GetTrack());
    ::UE::Sequencer::TViewModelPtr< ::UE::Sequencer::FSequencerEditorViewModel > editorViewModel = iParams.Editor->CastThisShared< ::UE::Sequencer::FSequencerEditorViewModel >();
    ::UE::Sequencer::TViewModelPtr<::UE::Sequencer::IOutlinerExtension> outlinerExtension = iParams.ViewModel.ImplicitCast();
    if (!track || !editorViewModel || !outlinerExtension)
        return nullptr;

    TWeakPtr<ISequencer> WeakSequencer = GetSequencer();
    if (!WeakSequencer.IsValid())
        return nullptr;

    if (iColumnName == ::UE::Sequencer::FCommonOutlinerNames::Edit)
        return nullptr;

    if (iColumnName == ::UE::Sequencer::FCommonOutlinerNames::Add)
    {
        FGuid objectBinding = track->FindObjectBindingGuid();
        return UE::Sequencer::MakeAddButton(LOCTEXT("sequencer.animation-timeline-track.add-button.tooltip", "Add Section"), FOnClicked::CreateRaw(this, &FOdysseyAnimationTimelineTrackEditor::OnAddButtonClicked, objectBinding), iParams.ViewModel);
    }

    if (iColumnName == ::UE::Sequencer::FCommonOutlinerNames::Label)
    {
        const FCheckBoxStyle* displayLayersToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("Sequencer.AnimationTimelineTrack.DisplayLayersToggle");

        TWeakPtr<UE::Sequencer::ISequencerTreeViewRow> weakRow = iParams.TreeViewRow;

        return SNew(SBox)
            .HeightOverride_Lambda(
                [outlinerExtension]()
                {
                    return outlinerExtension->GetOutlinerSizing().GetTotalHeight();
                }
            )
            [

                SNew(SVerticalBox)
                .Clipping(EWidgetClipping::ClipToBounds)
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SBox)
                    .HeightOverride(FOdysseyAnimationTimelineSectionEditor::GetCollapsedSectionHeight())
                    .VAlign(VAlign_Center)
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .Padding(
                            MakeAttributeLambda(
                                [weakRow]() -> FMargin
                                {
                                    TSharedPtr<UE::Sequencer::ISequencerTreeViewRow> row = weakRow.Pin();
                                    if (!row)
                                        return FMargin(0);

                                    const int32 NestingDepth = FMath::Max(0, row->GetIndentLevel());
                                    const float Indent = 10.f;
                                    return FMargin( NestingDepth * Indent, 0.f, 2.f, 0.f );
                                }
                            )
                        )
                        .VAlign(VAlign_Center)
                        .AutoWidth()
                        [
                            SNew(SCheckBox)
                            .Style(displayLayersToggleStyle)
                            .OnCheckStateChanged_Lambda(
                                [track](ECheckBoxState iState)
                                {
                                    FOdysseyObjectEditorUtils::SetPropertyValue(track, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationTimelineTrack, DisplayLayers), iState == ECheckBoxState::Checked);
                                }
                            )
                            .IsChecked_Lambda(
                                [track]()
                                {
                                    return track->DisplayLayers ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
                                }
                            )
                        ]
                        + SHorizontalBox::Slot()
                        .VAlign(VAlign_Center)
                        .HAlign(HAlign_Left)
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("sequencer.animation-timeline-track.name", "Timeline"))
                        ]
                    ]
                ]
                + SVerticalBox::Slot()
                [
                    SNew(SOdysseyAnimationTimelineTrack, track, iParams, WeakSequencer.Pin())
                        .Visibility_Lambda(
                            [track]()
                            {
                                return track->DisplayLayers ? EVisibility::Visible : EVisibility::Collapsed;
                            }
                        )
                        .Clipping(EWidgetClipping::ClipToBoundsAlways)
                        .LayerStack_Lambda(
                            [WeakSequencer, track]() -> UOdysseyAnimationLayerStack*
                            {
                                if( !WeakSequencer.IsValid() )
                                    return nullptr;

                                UOdysseyAnimationComponent* component = nullptr;
                                TArrayView<TWeakObjectPtr<>> boundObjects = WeakSequencer.Pin()->FindObjectsInCurrentSequence( track->FindObjectBindingGuid() );
                                for( TWeakObjectPtr<>& boundObjectPtr : boundObjects )
                                {
                                    UObject* boundObject = boundObjectPtr.Get();
                                    if( !boundObject )
                                        continue;

                                    if( !boundObject->IsA<UOdysseyAnimationComponent>() )
                                        continue;

                                    component = Cast<UOdysseyAnimationComponent>( boundObject );
                                    if( !component )
                                        continue;
                                }

                                if (!component)
                                    return nullptr;

                                UOdysseyAnimation* animation = component->GetAnimation();
                                if (!animation)
                                    return nullptr;

                                return Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
                            }
                        )
                ]
            ];
    }

    return FMovieSceneTrackEditor::BuildOutlinerColumnWidget(iParams, iColumnName);
}

TSharedRef<ISequencerSection>
FOdysseyAnimationTimelineTrackEditor::MakeSectionInterface( UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding )
{
    UOdysseyAnimationTimelineSection* animationTimelineSection = Cast<UOdysseyAnimationTimelineSection>(&SectionObject);
    checkf( animationTimelineSection != nullptr, TEXT("Unsupported section type.") );

    return MakeShareable(new FOdysseyAnimationTimelineSectionEditor(GetSequencer(), animationTimelineSection));
}

FReply
FOdysseyAnimationTimelineTrackEditor::OnAddButtonClicked(FGuid iObjectBinding)
{
    AddAnimationTrack({ iObjectBinding });
    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
