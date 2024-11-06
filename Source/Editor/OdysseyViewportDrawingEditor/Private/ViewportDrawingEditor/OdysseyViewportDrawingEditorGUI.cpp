// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "ViewportDrawingEditor/OdysseyViewportDrawingEditorGUI.h"
#include "AnimationEditor/OdysseyAnimationEditorTimelineTab.h"
#include "OdysseyPainterEditor.h"
#include "ViewportDrawingEditor/OdysseyViewportDrawingEditorExtension.h"
#include "OdysseyAnimationComponentTrack.h"
#include "OdysseyAnimationComponentSection.h"
#include "MovieScene.h"

#define LOCTEXT_NAMESPACE "ViewportDrawingEditor"

/////////////////////////////////////////////////////
// FOdysseyViewportDrawingEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyViewportDrawingEditorGUI::~FOdysseyViewportDrawingEditorGUI()
{
}

FOdysseyViewportDrawingEditorGUI::FOdysseyViewportDrawingEditorGUI(FOdysseyViewportDrawingEditorExtension* iExtension)
    : mExtension(iExtension)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyViewportDrawingEditorGUI::Initialize()
{
    CustomizeAnimationEditorTimeline();
}

void
FOdysseyViewportDrawingEditorGUI::Finalize()
{

}

void
FOdysseyViewportDrawingEditorGUI::CustomizeAnimationEditorTimeline()
{
    FOdysseyPainterEditor* editor = mExtension->GetEditor();
    TSharedPtr<FOdysseyAnimationEditorTimelineTab> timelineTab = editor->FindTab<FOdysseyAnimationEditorTimelineTab>();
    if (!timelineTab)
        return;

    timelineTab->SetEmptyTimelineWidget(
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("animation.timeline-tab.empty-timeline-message-1", "Your timeline panel is empty for one of the following reasons:"))
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("animation.timeline-tab.empty-timeline-message-2", "- The selected Actor does not use a 2D Animation"))
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("animation.timeline-tab.empty-timeline-message-3", "- The Sequencer is not open."))
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("animation.timeline-tab.empty-timeline-message-4", "- The 2D Animation is not used in the Sequencer as a Media Track."))
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("animation.timeline-tab.empty-timeline-message-5", "- The cursor in the Sequencer is off of the corresponding Media Track."))
        ]
    );

    TAttribute<FInt32Range> validRange = MakeAttributeLambda(
        [this]() -> FInt32Range
        {
            UMeshComponent* component = mExtension->Component();
            if (!component->IsA<UOdysseyAnimationComponent>())
                return FInt32Range::Empty();

            UOdysseyAnimationComponent* animationComponent = Cast<UOdysseyAnimationComponent>(component);
            if (!animationComponent)
                return FInt32Range::Empty();

            UOdysseyAnimation* animation = animationComponent->GetActiveAnimation();
            if (!animation)
                return FInt32Range::Empty();

            TArray<TWeakPtr<ISequencer>> sequencers = mExtension->Sequencers();
            for (TWeakPtr<ISequencer> weakSequencer : sequencers)
            {
                TSharedPtr<ISequencer> sequencer = weakSequencer.Pin();
                if (!sequencer)
                    continue;

                UMovieSceneSequence* movieSceneSequence = sequencer->GetFocusedMovieSceneSequence();
                if (!movieSceneSequence)
                    continue;

                FGuid binding = sequencer->GetHandleToObject(animationComponent, false);
                if (!binding.IsValid())
                    continue;

                UMovieScene* movieScene = movieSceneSequence->GetMovieScene();
                if (!movieScene)
                    continue;

                UOdysseyAnimationComponentTrack* track =  movieScene->FindTrack<UOdysseyAnimationComponentTrack>(binding);
                if (!track)
                    continue;

                TArray<UMovieSceneSection*> sections = track->GetAllSections();

                UMovieSceneSection** sectionPtr = sections.FindByPredicate(
                    [sequencer](UMovieSceneSection* iSection)
                    {
                        if (!iSection->IsActive())
                            return false;

                        if (!iSection->IsTimeWithinSection(sequencer->GetGlobalTime().Time.FrameNumber))
                            return false;

                        return true;
                    }
                );

                if (!sectionPtr)
                    continue;

                UOdysseyAnimationComponentSection* section = Cast<UOdysseyAnimationComponentSection>(*sectionPtr);
                if (!section)
                    continue;

                FFrameRate displayRate = sequencer->GetFocusedDisplayRate();
                FFrameRate tickResolution = movieScene->GetTickResolution();
                TRange<FFrameNumber> sectionRange = section->GetTrueRange();
                FFrameNumber startFrame = sectionRange.GetLowerBoundValue();
                FFrameNumber endFrame = sectionRange.GetUpperBoundValue() - 1; //-1 because upperboundvalue is exclusive
                FFrameTime endTime = FFrameRate::TransformTime(FFrameRate::TransformTime(endFrame, tickResolution, displayRate).FloorToFrame(), displayRate, tickResolution);

                int validRangeStartFrame = FMath::FloorToInt(tickResolution.AsSeconds(section->StartFrameOffset) * animation->FramesPerSecond);
                int validRangeEndFrame = FMath::FloorToInt(tickResolution.AsSeconds(section->StartFrameOffset + endTime.FrameNumber - startFrame ) * animation->FramesPerSecond);
                FInt32Range validRange = FInt32Range::Inclusive(validRangeStartFrame, validRangeEndFrame);
                return validRange;
            }

            return FInt32Range::Empty();
        }
    );
    timelineTab->SetAnimationValidRange(validRange);
}

#undef LOCTEXT_NAMESPACE
