// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationTimelineSectionEditor.h"

#include "ISequencer.h"
#include "EditorModeManager.h"

#include "Animation/OdysseyPainterEditorAnimationTimelinePosition.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "OdysseyViewportDrawingEditorEdMode.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
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
    mComponent = GetComponent();
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
    track->SetRowHeight( mSectionWidget ? mSectionWidget->GetDesiredSize().Y : 0.f ); // Arbitrary value which should only be used for one (or some) tick(s) waiting the creation of the layout widget in the section
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
    UOdysseyAnimationComponent* component = GetComponent();
    TSharedPtr<ISequencer> sequencer = GetSequencer();

    mSectionWidget = SNew(SVerticalBox)
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
            SNew(SOdysseyAnimationTimelineSection, sequencer, mSection, mComponent)
        ];

    return mSectionWidget.ToSharedRef();
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

UOdysseyAnimationComponent*
FOdysseyAnimationTimelineSectionEditor::GetComponent() const
{
    UOdysseyAnimationTimelineTrack* track = mSection->GetTypedOuter<UOdysseyAnimationTimelineTrack>();

    TSharedPtr<ISequencer> sequencer = GetSequencer();
    if (!sequencer)
        return nullptr;

    TArrayView<TWeakObjectPtr<>> boundObjects = sequencer->FindObjectsInCurrentSequence(track->FindObjectBindingGuid());
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

        return animationComponent;
    }

    return nullptr;
}

void
FOdysseyAnimationTimelineSectionEditor::BeginResizeSection()
{
    mInitialStartOffsetDuringResize = mSection->StartFrameOffset;
    mInitialStartTimeDuringResize = mSection->HasStartFrame() ? mSection->GetInclusiveStartFrame() : 0;

    ISequencerSection::BeginResizeSection();
}

void
FOdysseyAnimationTimelineSectionEditor::ResizeSection(ESequencerSectionResizeMode iResizeMode, FFrameNumber iResizeTime)
{
    if (iResizeMode == SSRM_LeadingEdge)
    {
        FFrameNumber mStartOffset = iResizeTime - mInitialStartTimeDuringResize;
        mStartOffset += mInitialStartOffsetDuringResize;

        // Ensure start offset is not less than 0
        if (mStartOffset < 0)
        {
            iResizeTime = iResizeTime - mStartOffset;
            mStartOffset = FFrameNumber(0);
        }

        mSection->StartFrameOffset = mStartOffset;
    }

    ISequencerSection::ResizeSection(iResizeMode, iResizeTime);
}

float
FOdysseyAnimationTimelineSectionEditor::GetCollapsedSectionHeight()
{
    return 28.f;
}

#undef LOCTEXT_NAMESPACE
