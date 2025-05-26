// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationTimelineSectionEditor.h"

#include "ISequencer.h"
#include "EditorModeManager.h"

#include "Animation/OdysseyPainterEditorAnimationTimelinePosition.h"
#include "OdysseyViewportDrawingEditorEdMode.h"
#include "OdysseyAnimationCell.h"
#include "OdysseyViewportDrawingEditorToolkit.h"
#include "Tools/OutOfPegsTool/OdysseyPainterEditorAnimationOutOfPegsTool.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationTimelineTrack.h"
#include "OdysseyAnimationTimelineSection.h"
#include "OdysseyAnimation.h"
#include "Widgets/SOdysseyAnimationTimelineSection.h"

#define LOCTEXT_NAMESPACE "AnimationTrack"

FOdysseyAnimationTimelineSectionEditor::~FOdysseyAnimationTimelineSectionEditor()
{
}

FOdysseyAnimationTimelineSectionEditor::FOdysseyAnimationTimelineSectionEditor(TSharedPtr<ISequencer> InSequencer, UOdysseyAnimationTimelineSection* InSection)
    : mSequencer( InSequencer )
    , mSection(InSection)
{
}

TSharedPtr<ISequencer>
FOdysseyAnimationTimelineSectionEditor::GetSequencer() const
{
    return mSequencer.Pin();
}

UMovieSceneSection*
FOdysseyAnimationTimelineSectionEditor::GetSectionObject() //override
{
    return mSection;
}

float
FOdysseyAnimationTimelineSectionEditor::GetSectionHeight( const UE::Sequencer::FViewDensityInfo& ViewDensity ) const
{
    UOdysseyAnimationTimelineTrack* track = mSection->GetTypedOuter<UOdysseyAnimationTimelineTrack>();
    TSharedPtr<SWidget> sectionWidget = mSectionWidget.Pin();
    track->SetRowHeight( sectionWidget ? sectionWidget->GetDesiredSize().Y : 0.f ); // Arbitrary value which should only be used for one (or some) tick(s) waiting the creation of the layout widget in the section
    return track->GetRowHeight();
}

float
FOdysseyAnimationTimelineSectionEditor::GetSectionGripHeight(float iSectionHeight) const
{
    return FMath::Min(iSectionHeight, GetCollapsedSectionHeight());
}

FText
FOdysseyAnimationTimelineSectionEditor::GetSectionTitle() const
{
    return FText::GetEmpty();
}

FText
FOdysseyAnimationTimelineSectionEditor::GetSectionToolTip() const
{
    return FText::GetEmpty();
}

TSharedRef<SWidget>
FOdysseyAnimationTimelineSectionEditor::GenerateSectionWidget()
{
    TSharedPtr<ISequencer> sequencer = GetSequencer();

    TSharedRef widget = SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SBox)
            .HeightOverride(GetCollapsedSectionHeight())
            [
                SNullWidget::NullWidget
            ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOdysseyAnimationTimelineSection, sequencer, mSection)
            .Animation(this, &FOdysseyAnimationTimelineSectionEditor::GetAnimation)
            .PreBehaviour(this, &FOdysseyAnimationTimelineSectionEditor::GetPreBehaviour)
            .PostBehaviour(this, &FOdysseyAnimationTimelineSectionEditor::GetPostBehaviour)
            .StartFrameOffset(this, &FOdysseyAnimationTimelineSectionEditor::GetStartFrameOffset)
            .OnPreBehaviourChanged(this, &FOdysseyAnimationTimelineSectionEditor::OnPreBehaviourChanged)
            .OnPostBehaviourChanged(this, &FOdysseyAnimationTimelineSectionEditor::OnPostBehaviourChanged)
        ];

    mSectionWidget = widget;
    return widget;
}


UOdysseyAnimation*
FOdysseyAnimationTimelineSectionEditor::GetAnimation() const
{
    return mSection->GetAnimation();
}

FFrameNumber
FOdysseyAnimationTimelineSectionEditor::GetStartFrameOffset() const
{
    return mSection->GetStartFrameOffset();
}

EOdysseyAnimationPlayerPostBehaviour
FOdysseyAnimationTimelineSectionEditor::GetPreBehaviour() const
{
    return mSection->GetPreBehaviour();
}

EOdysseyAnimationPlayerPostBehaviour
FOdysseyAnimationTimelineSectionEditor::GetPostBehaviour() const
{
    return mSection->GetPostBehaviour();
}

void
FOdysseyAnimationTimelineSectionEditor::OnPreBehaviourChanged(EOdysseyAnimationPlayerPostBehaviour iValue)
{
    mSection->SetPreBehaviour(iValue);
}

void
FOdysseyAnimationTimelineSectionEditor::OnPostBehaviourChanged(EOdysseyAnimationPlayerPostBehaviour iValue)
{
    mSection->SetPostBehaviour(iValue);
}

bool
FOdysseyAnimationTimelineSectionEditor::IsReadOnly() const //override
{
    check( mSection );
    return mSection->IsReadOnly();
}

int32
FOdysseyAnimationTimelineSectionEditor::OnPaintSection( FSequencerSectionPainter& InPainter ) const //override
{
    InPainter.LayerId = InPainter.PaintSectionBackground();

    //FSubSectionPainterUtil::PaintSection( this->GetSequencer(), SubSectionObject, InPainter, FSubSectionPainterParams( this->GetContentPadding() ) );

    return InPainter.LayerId;
}

void
FOdysseyAnimationTimelineSectionEditor::BeginResizeSection()
{
    mInitialStartOffsetDuringResize = mSection->GetStartFrameOffset();
    mInitialStartTimeDuringResize = mSection->HasStartFrame() ? mSection->GetInclusiveStartFrame() : 0;

    ISequencerSection::BeginResizeSection();
}

void
FOdysseyAnimationTimelineSectionEditor::ResizeSection(ESequencerSectionResizeMode iResizeMode, FFrameNumber iResizeTime)
{
    if (iResizeMode == SSRM_LeadingEdge)
    {
        mSection->SetStartFrameOffset(mInitialStartOffsetDuringResize - (mInitialStartTimeDuringResize - iResizeTime));
    }

    ISequencerSection::ResizeSection(iResizeMode, iResizeTime);
}

float
FOdysseyAnimationTimelineSectionEditor::GetCollapsedSectionHeight()
{
    return 28.f;
}

#undef LOCTEXT_NAMESPACE
