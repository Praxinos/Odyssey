// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationTimelineTrackEditor.h"

#include "ISequencerObjectChangeListener.h"
#include "SequencerUtilities.h"
#include "MVVM/Extensions/ITrackExtension.h"
#include "MVVM/ViewModels/SequencerEditorViewModel.h"
#include "MVVM/ViewModels/OutlinerColumns/OutlinerColumnTypes.h"
#include "MVVM/ViewModels/ViewModel.h"
#include "MVVM/ViewModels/ViewModelIterators.h"
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
    if( GetSequencer() )
    {
        FAnimatedPropertyKey propertyKey = FAnimatedPropertyKey::FromObjectType( UOdysseyAnimation::StaticClass() );
        GetSequencer()->GetObjectChangeListener().GetOnAnimatablePropertyChanged( propertyKey ).RemoveAll( this );
    }
}

FOdysseyAnimationTimelineTrackEditor::FOdysseyAnimationTimelineTrackEditor( TSharedRef<ISequencer> InSequencer )
    : FMovieSceneTrackEditor( InSequencer )
{
    FAnimatedPropertyKey propertyKey = FAnimatedPropertyKey::FromObjectType( UOdysseyAnimation::StaticClass() );
    // This is called when the Animation attribute of the animation component is modified in the details panel
    // So the Animation in the current section can be update as well
    GetSequencer()->GetObjectChangeListener().GetOnAnimatablePropertyChanged( propertyKey ).AddRaw( this, &FOdysseyAnimationTimelineTrackEditor::OnAnimatedPropertyChanged );
}

void
FOdysseyAnimationTimelineTrackEditor::OnAnimatedPropertyChanged( const FPropertyChangedParams& PropertyChangedParams )
{
    FMovieSceneTrackEditor::AnimatablePropertyChanged( FOnKeyProperty::CreateRaw( this, &FOdysseyAnimationTimelineTrackEditor::UpdateAnimationInDetailsInternal, PropertyChangedParams ) );
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
FOdysseyAnimationTimelineTrackEditor::UpdateAnimationInDetailsInternal( FFrameNumber iKeyTime, FPropertyChangedParams iPropertyChangedParams )
{
    FKeyPropertyResult keyPropertyResult;

    TSharedPtr<ISequencer> SequencerPtr = GetSequencer();
    if( !SequencerPtr.IsValid() )
        return keyPropertyResult;

    // If Binding is invalid, all other attributes MUST also BE considered as invalid
    struct FResult
    {
        FGuid Binding;
        UObject* Object;
        UMovieSceneSequence* Sequence;
        FFrameNumber FrameNumberInSequence;
    };
    auto FindBindingFromObject = [SequencerPtr]( UObject* iObject, FFrameNumber iFrameNumber ) -> FResult
        {
            FResult result;
            result.Object = iObject;
            result.Sequence = SequencerPtr->GetFocusedMovieSceneSequence();
            check( result.Sequence );
            result.FrameNumberInSequence = iFrameNumber;

            // Try to find the corresponding binding of the object in the focused sequence
            FGuid binding = result.Sequence->FindBindingFromObject( result.Object, SequencerPtr->GetSharedPlaybackState() );
            if( binding.IsValid() )
            {
                result.Binding = binding;
                return result;
            }

            // If the object is not in the focused sequence, try to find it in a subsequence
            // (should be UMovieSceneCinematicBoardTrack, but UMovieSceneSubTrack is used to avoid dependencies of epos)
            for( UMovieSceneTrack* track : result.Sequence->GetMovieScene()->GetTracks() )
            {
                UMovieSceneSubTrack* subtrack = Cast<UMovieSceneSubTrack>( track );
                if( !subtrack )
                    continue;

                // Find the subsection at the current time
                UMovieSceneSection* section = MovieSceneHelpers::FindSectionAtTime( subtrack->GetAllSections(), iFrameNumber ); // Always use the frame number of the focused sequence (as subtracks are always from the focused sequence)
                UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );
                // get its subsequence
                result.Sequence = subsection ? subsection->GetSequence() : nullptr;
                if( !result.Sequence )
                    continue;

                // Convert the current time in the subsequence reference
                result.FrameNumberInSequence = ( result.FrameNumberInSequence * subsection->OuterToInnerTransform() ).GetFrame();

                // Try to find the corresponding binding of the object in the subsequence
                binding = result.Sequence->FindBindingFromObject( result.Object, SequencerPtr->GetSharedPlaybackState() );
                if( binding.IsValid() )
                {
                    result.Binding = binding;
                    return result;
                }
            }

            return result;
        };

    for( UObject* object : iPropertyChangedParams.ObjectsThatChanged )
    {
        FResult result = FindBindingFromObject( object, iKeyTime );

        // If no binding at all, process the next object
        if( !result.Binding.IsValid() )
            continue;

        // Once a binding is found, get the animation in actor
        UOdysseyAnimationComponent* animationComponent = Cast<UOdysseyAnimationComponent>( object );
        UOdysseyAnimation* animation_in_actor = animationComponent ? animationComponent->GetAnimation() : nullptr;
        if( !animation_in_actor )
            continue;

        // Find the timeline track of the binding
        UOdysseyAnimationTimelineTrack* animationTrack = result.Sequence->GetMovieScene()->FindTrack<UOdysseyAnimationTimelineTrack>( result.Binding );
        if( !animationTrack || !animationTrack->CanModify() )
            continue;

        // Find the section and its animation
        UMovieSceneSection* section = MovieSceneHelpers::FindSectionAtTime( animationTrack->GetAllSections(), result.FrameNumberInSequence );
        UOdysseyAnimationTimelineSection* animationSection = Cast<UOdysseyAnimationTimelineSection>( section );
        UOdysseyAnimation* animation_in_section = animationSection ? animationSection->GetAnimation() : nullptr;
        if( !animation_in_section )
            continue;

        // If animation (of the actor) and animation (of the section) is the same, nothing to do
        if( animation_in_actor == animation_in_section )
            continue;

        // Otherwise, update the section with the new actor animation
        animationTrack->Modify();
        animationSection->Modify();
        keyPropertyResult.bTrackModified = true;
        animationSection->SetAnimation( animation_in_actor );
    }

    return keyPropertyResult;
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
    using namespace UE::Sequencer;

    ::UE::Sequencer::TViewModelPtr<::UE::Sequencer::FViewModel> ViewModel = iParams.ViewModel;
    if( !ViewModel )
        return SNullWidget::NullWidget;

    ::UE::Sequencer::TViewModelPtr<::UE::Sequencer::ITrackExtension> TrackModel = ViewModel->FindAncestorOfType<::UE::Sequencer::ITrackExtension>( true );
    if( !TrackModel )
        return SNullWidget::NullWidget;

    UMovieSceneTrack* Track = TrackModel->GetTrack();

    UOdysseyAnimationTimelineTrack* track = Cast<UOdysseyAnimationTimelineTrack>( Track );
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
