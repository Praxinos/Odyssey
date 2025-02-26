// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationTimelineSectionEditor.h"

#include "ISequencer.h"
#include "EditorModeManager.h"

#include "Widgets/Animation/LayerStack/SOdysseyAnimationTimelineTreeView.h"
#include "Animation/OdysseyAnimationEditorTimelinePosition.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "OdysseyViewportDrawingEditorEdMode.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "Animation/OdysseyAnimationEditorExtension.h"
#include "OdysseyViewportDrawingEditorToolkit.h"
#include "Tools/OutOfPegsTool/OdysseyAnimationEditorOutOfPegsTool.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationTimelineTrack.h"
#include "OdysseyAnimationTimelineSection.h"
#include "OdysseyAnimation.h"

#define LOCTEXT_NAMESPACE "AnimationTrack"

FOdysseyAnimationTimelineSectionEditor::~FOdysseyAnimationTimelineSectionEditor()
{
    if (mComponent)
    {
        mComponent->OnAnimationChanged().RemoveAll(this);
        mComponent->OnPlayerChanged().RemoveAll(this);
        mComponent->OnModeChanged().RemoveAll(this);
    }
}

FOdysseyAnimationTimelineSectionEditor::FOdysseyAnimationTimelineSectionEditor(TSharedPtr<ISequencer> InSequencer, UOdysseyAnimationTimelineSection* InSection)
    : mSequencer( InSequencer )
    , mSection(InSection)
    , mTimelinePosition(MakeShared<FOdysseyAnimationEditorTimelinePosition>())
{
    mTimelinePosition->SetPadding(0.f);
    mTimelinePosition->HasMinZoom(false);
    mTimelinePosition->HasMaxZoom(false);

    mComponent = GetComponent();
    if (mComponent)
    {
        mComponent->OnAnimationChanged().AddRaw(this, &FOdysseyAnimationTimelineSectionEditor::OnAnimationChanged);
        mComponent->OnPlayerChanged().AddRaw(this, &FOdysseyAnimationTimelineSectionEditor::OnPlayerChanged);
        mComponent->OnModeChanged().AddRaw(this, &FOdysseyAnimationTimelineSectionEditor::OnModeChanged);
    }
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

/* float
FOdysseyAnimationTimelineSectionEditor::GetLayerHeight(UOdysseyLayer* iLayer)
{
    float height = 0.f; //line padding

    TArray<FName> rows = iLayer->GetRows();
    for (const FName& row : rows)
    {
        if (!iLayer->IsRowVisible(row))
            continue;

        height += iLayer->GetRowHeight(row);
        FMargin padding = iLayer->GetRowPadding(row);
        height += padding.Top + padding.Bottom; //Padding between each subrow
    }

    if (iLayer->DisplayChildren)
    {
        for (UOdysseyLayer* child : iLayer->Children)
        {
            height += GetLayerHeight(child);
        }
    }

    return height;
} */

float
FOdysseyAnimationTimelineSectionEditor::GetSectionHeight( const UE::Sequencer::FViewDensityInfo& ViewDensity ) const
{
    /* int height = GetCollapsedSectionHeight();

    UOdysseyAnimationTimelineTrack* track = mSection->GetTypedOuter<UOdysseyAnimationTimelineTrack>();
    if (track && track->DisplayLayers)
        height = GetUncollapsedSectionHeight(GetComponent());

    */
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
    mSectionWidget = SNew(SBox);
    RebuildSectionWidget();
    return mSectionWidget.ToSharedRef();
}

void
FOdysseyAnimationTimelineSectionEditor::RebuildSectionWidget()
{
    UOdysseyAnimationTimelineTrack* track = mSection->GetTypedOuter<UOdysseyAnimationTimelineTrack>();
    if (!track)
        return;

    UOdysseyAnimationComponent* component = GetComponent();
    if (!component)
        return;

    UOdysseyAnimation* animation = component->GetActiveAnimation();

    if (!animation)
    {
        mSectionWidget->SetContent(SNullWidget::NullWidget);
        return;
    }

    UOdysseyAnimationLayerStack* layerStack = animation->GetLayerStack();

    mSectionWidget->SetContent(
        SNew(SVerticalBox)
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
            SNew( SOdysseyAnimationTimelineTreeView )
            .PainterEditor_Lambda(
                [animation]() ->FOdysseyPainterEditor*
                {
                    if (!animation)
                        return nullptr;

                    if (!GLevelEditorModeTools().IsModeActive( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId ))
                        return nullptr;

                    FEdMode* edMode = GLevelEditorModeTools().GetActiveMode( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
                    if (!edMode)
                        return nullptr;

                    FOdysseyViewportDrawingEditorEdMode* odysseyEdMode = static_cast<FOdysseyViewportDrawingEditorEdMode*>(edMode);

                    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = odysseyEdMode->GetViewportDrawingEditorToolkit();
                    if(!toolkit)
                        return nullptr;

                    TSharedPtr<FOdysseyAnimationEditorExtension> animationExtension = toolkit->GetAnimationExtension();
                    if (!animationExtension)
                        return nullptr;

                    if (animationExtension->Animation() != animation)
                        return nullptr;

                    FOdysseyPainterEditor* editor = odysseyEdMode->GetEditor();
                    if (!editor)
                        return nullptr;

                    return editor;
                }
            )
            .Visibility(this, &FOdysseyAnimationTimelineSectionEditor::GetLayersVisibility)
            .LayerStack(layerStack)
            .TimelinePosition(mTimelinePosition)
            .OnActivateOutOfPegs_Lambda(
                [](UOdysseyAnimationCell* iCell)
                {
                    if (!GLevelEditorModeTools().IsModeActive( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId ))
                        return;

                    FEdMode* edMode = GLevelEditorModeTools().GetActiveMode( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
                    if (!edMode)
                        return;

                    FOdysseyViewportDrawingEditorEdMode* odysseyEdMode = static_cast<FOdysseyViewportDrawingEditorEdMode*>(edMode);
                    FOdysseyPainterEditor* editor = odysseyEdMode->GetEditor();
                    if (!editor)
                        return;

                    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = odysseyEdMode->GetViewportDrawingEditorToolkit();
                    if(!toolkit)
                        return;

                    UOdysseyAnimation* animation = iCell->GetAnimation();
                    if (!animation)
                        return;

                    TSharedPtr<FOdysseyAnimationEditorExtension> animationExtension = toolkit->GetAnimationExtension();
                    if (!animationExtension)
                        return;

                    if (animationExtension->Animation() != animation)
                        return;

                    animationExtension->GetOutOfPegsTool()->SetCell(iCell);
                    editor->ActivateTemporaryTool(animationExtension->GetOutOfPegsTool());
                }
            )
            .OnInactivateOutOfPegs_Lambda(
                []()
                {
                    if (!GLevelEditorModeTools().IsModeActive( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId ))
                        return;

                    FEdMode* edMode = GLevelEditorModeTools().GetActiveMode( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
                    if (!edMode)
                        return;

                    FOdysseyViewportDrawingEditorEdMode* odysseyEdMode = static_cast<FOdysseyViewportDrawingEditorEdMode*>(edMode);
                    FOdysseyPainterEditor* editor = odysseyEdMode->GetEditor();
                    if (!editor)
                        return;

                    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = odysseyEdMode->GetViewportDrawingEditorToolkit();
                    if(!toolkit)
                        return;

                    TSharedPtr<FOdysseyAnimationEditorExtension> animationExtension = toolkit->GetAnimationExtension();
                    if (!animationExtension)
                        return;

                    editor->InactivateTemporaryTool();
                }
            )
            .OnIsOutOfPegsChecked_Lambda(
                [](UOdysseyAnimationCell* iCell)
                {
                    if (!GLevelEditorModeTools().IsModeActive( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId))
                        return ECheckBoxState::Unchecked;

                    FEdMode* edMode = GLevelEditorModeTools().GetActiveMode( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
                    if (!edMode)
                        return ECheckBoxState::Unchecked;

                    FOdysseyViewportDrawingEditorEdMode* odysseyEdMode = static_cast<FOdysseyViewportDrawingEditorEdMode*>(edMode);
                    FOdysseyPainterEditor* editor = odysseyEdMode->GetEditor();
                    if (!editor)
                        return ECheckBoxState::Unchecked;

                    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = odysseyEdMode->GetViewportDrawingEditorToolkit();
                    if(!toolkit)
                        return ECheckBoxState::Unchecked;

                    UOdysseyAnimation* animation = iCell->GetAnimation();
                    if (!animation)
                        return ECheckBoxState::Unchecked;

                    TSharedPtr<FOdysseyAnimationEditorExtension> animationExtension = toolkit->GetAnimationExtension();
                    if (!animationExtension)
                        return ECheckBoxState::Unchecked;

                    if (animationExtension->Animation() != animation)
                        return ECheckBoxState::Unchecked;

                    UOdysseyPainterEditorTool* tool = editor->GetCurrentTool();
                    if (!tool)
                        return ECheckBoxState::Unchecked;

                    bool isToolActive = tool->IsA(UOdysseyAnimationEditorOutOfPegsTool::StaticClass());
                    if (!isToolActive)
                        return ECheckBoxState::Unchecked;

                    UOdysseyAnimationEditorOutOfPegsTool* outOfPegsTool = Cast<UOdysseyAnimationEditorOutOfPegsTool>(tool);
                    if (outOfPegsTool->GetCell() != iCell)
                        return ECheckBoxState::Unchecked;

                    return ECheckBoxState::Checked;
                }
            )
            .ExternalScrollbar( SNew(SScrollBar) )
        ]
    );
}

void
FOdysseyAnimationTimelineSectionEditor::Tick( const FGeometry& AllottedGeometry, const FGeometry& ClippedGeometry, const double InCurrentTime, const float InDeltaTime )
{
    ISequencerSection::Tick( AllottedGeometry, ClippedGeometry, InCurrentTime, InDeltaTime );

    UOdysseyAnimationTimelineTrack* track = mSection->GetTypedOuter<UOdysseyAnimationTimelineTrack>();
    UMovieScene* movieScene = track->GetTypedOuter<UMovieScene>();

    UOdysseyAnimationComponent* component = GetComponent();
    if (!component)
        return;

    UOdysseyAnimation* animation = component->GetActiveAnimation();
    if (!animation)
        return;

    float animationFramesPerSecond = animation->GetFramesPerSecond();

    FMovieSceneFrameRange sectionRange = mSection->SectionRange;
    FFrameNumber sectionFrameLength = sectionRange.Value.GetUpperBoundValue() - sectionRange.Value.GetLowerBoundValue();
    double sectionSecondLength = movieScene->GetTickResolution().AsSeconds(sectionFrameLength);

    double animationSecondInPixels = (animationFramesPerSecond * mTimelinePosition->GetBaseFrameSize());
    double sequencerSecondInPixels = AllottedGeometry.Size.X / sectionSecondLength;

    double zoom = sequencerSecondInPixels / animationSecondInPixels;
    mTimelinePosition->SetZoom(zoom);

    double offset = movieScene->GetTickResolution().AsSeconds(mSection->StartFrameOffset) * animationFramesPerSecond;
    mTimelinePosition->SetOffset(offset);
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

/* float
FOdysseyAnimationTimelineSectionEditor::GetTreeViewHeight(UOdysseyAnimationComponent* iComponent)
{
    if (!iComponent)
        return 0;

    UOdysseyAnimation* animation = iComponent->GetActiveAnimation();
    if (!animation)
        return 0;

    int height = 0;
    height += 27.f;
    UOdysseyAnimationLayerStack* layerStack = animation->GetLayerStack();
    TArray<UOdysseyLayer*> layers = layerStack->GetRootLayers();
    int layersHeight = 0.f; //Initial treeview padding
    for (UOdysseyLayer* layer : layers)
    {
        layersHeight += GetLayerHeight(layer);
    }
    height += layersHeight;
    return height;
} */

/* float
FOdysseyAnimationTimelineSectionEditor::GetUncollapsedSectionHeight(UOdysseyAnimationComponent* iComponent)
{
    int height = 0;
    height += GetCollapsedSectionHeight(); //Expander Arrow + Name + Section Add Button
    height += GetTreeViewHeight(iComponent);
    return height;
} */

EVisibility
FOdysseyAnimationTimelineSectionEditor::GetLayersVisibility() const
{
    UOdysseyAnimationTimelineTrack* track = mSection->GetTypedOuter<UOdysseyAnimationTimelineTrack>();
    if (!track)
        return EVisibility::Collapsed;

    return track->DisplayLayers ? EVisibility::Visible : EVisibility::Collapsed;
}

void
FOdysseyAnimationTimelineSectionEditor::OnAnimationChanged()
{
    RebuildSectionWidget();
}

void
FOdysseyAnimationTimelineSectionEditor::OnPlayerChanged()
{
    RebuildSectionWidget();
}

void
FOdysseyAnimationTimelineSectionEditor::OnModeChanged()
{
    RebuildSectionWidget();
}

#undef LOCTEXT_NAMESPACE
