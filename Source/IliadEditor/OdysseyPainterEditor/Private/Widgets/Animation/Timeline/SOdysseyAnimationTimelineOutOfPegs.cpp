// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineOutOfPegs.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineOutOfPegsKey.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "Tools/OutOfPegsTool/OdysseyPainterEditorAnimationOutOfPegsTool.h"
#include "OdysseyAnimation.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineSection.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "OdysseyPainterEditor.h"

void
SOdysseyAnimationTimelineOutOfPegs::Construct(const FArguments& InArgs, UOdysseyAnimationLayer* iLayer)
{
    UOdysseyAnimation::OnCurrentFrameChanged().AddSP(SharedThis(this), &SOdysseyAnimationTimelineOutOfPegs::OnCurrentFrameChanged);
    FOdysseyImageRenderingAbility::OnImageRenderingChangedDelegate().AddSP(this, &SOdysseyAnimationTimelineOutOfPegs::OnImageRenderingChanged);

    mLayer = iLayer;

    TSharedRef<SHorizontalBox> horizontalBox = SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SOdysseyAnimationTimelineSection)
            .TimelinePosition(InArgs._TimelinePosition)
            .WidthInFrames_Lambda(
                [this]()
                {
                    if (!mCurrentCell)
                        return 0;

                    int firstCellIndex = FMath::Max(mCurrentCell->IndexInLayer - 10, 0);
                    UOdysseyAnimationCell* cell = mLayer->GetCells()[firstCellIndex];
                    int width = cell->GetFrameRange().GetLowerBoundValue();
                    return width;
                }
            )
            .Content()
            [
                SNullWidget::NullWidget
            ]
        ];

    //Previous cells
    for (int i = 9; i >= 0; i--)
    {
        horizontalBox->AddSlot()
        .AutoWidth()
        [
            SNew(SOdysseyAnimationTimelineSection)
            .TimelinePosition(InArgs._TimelinePosition)
            .HAlign(HAlign_Left)
            .WidthInFrames_Lambda(
                [this, i]()
                {
                    if (!mCurrentCell)
                        return 0;

                    const TArray<UOdysseyAnimationCell*>& cells = mLayer->GetCells();
                    int cellIndex = mCurrentCell->IndexInLayer - i - 1;
                    if (cellIndex < 0 || cellIndex >= cells.Num())
                        return 0;

                    return cells[cellIndex]->Exposure;
                }
            )
            [

                SNew(SOdysseyAnimationTimelineOutOfPegsKey)
                .Visibility_Lambda(
                    [this, i]()
                    {
                        if (!mCurrentCell)
                            return EVisibility::Collapsed;

                        int cellIndex = mCurrentCell->IndexInLayer - i - 1;
                        if (cellIndex < 0 || cellIndex >= mLayer->GetCells().Num())
                            return EVisibility::Collapsed;

                        return EVisibility::Visible;
                    }
                )
                .Key_Lambda([this, i]() { return mLayer->Lighttable.PreviousKeys[i];})
                .Cell_Lambda(
                    [this, i]() -> UOdysseyAnimationCell*
                    {
                        if (!mCurrentCell)
                            return nullptr;

                        const TArray<UOdysseyAnimationCell*>& cells = mLayer->GetCells();
                        int cellIndex = mCurrentCell->IndexInLayer - i - 1;
                        if (cellIndex < 0 || cellIndex >= cells.Num())
                            return nullptr;

                        return cells[cellIndex];
                    }
                )
                .TimelinePosition(InArgs._TimelinePosition)
                .OnActivateOutOfPegs(InArgs._OnActivateOutOfPegs)
                .OnInactivateOutOfPegs(InArgs._OnInactivateOutOfPegs)
                .OnIsOutOfPegsChecked(InArgs._OnIsOutOfPegsChecked)
            ]
        ];
    }

    //Current Cell
    horizontalBox->AddSlot()
    .AutoWidth()
    [
        SNew(SOdysseyAnimationTimelineSection)
        .TimelinePosition(InArgs._TimelinePosition)
        .Visibility_Lambda(
            [this]()
            {
                if (!mCurrentCell)
                    return EVisibility::Collapsed;

                return EVisibility::Visible;
            }
        )
        .WidthInFrames_Lambda(
            [this]()
            {
                if (!mCurrentCell)
                    return 0;

                return mCurrentCell->Exposure;
            }
        )
        [
            SNullWidget::NullWidget
        ]
    ];

    //Next cells
    for (int i = 0; i <= 9; i++)
    {
        horizontalBox->AddSlot()
        .AutoWidth()
        [
            SNew(SOdysseyAnimationTimelineSection)
            .TimelinePosition(InArgs._TimelinePosition)
            .HAlign(HAlign_Left)
            .WidthInFrames_Lambda(
                [this, i]()
                {
                    if (!mCurrentCell)
                        return 0;

                    const TArray<UOdysseyAnimationCell*>& cells = mLayer->GetCells();
                    int cellIndex = mCurrentCell->IndexInLayer + i + 1;
                    if (cellIndex < 0 || cellIndex >= cells.Num())
                        return 0;

                    return cells[cellIndex]->Exposure;
                }
            )
            [

                SNew(SOdysseyAnimationTimelineOutOfPegsKey)
                .Visibility_Lambda(
                    [this, i]()
                    {
                        if (!mCurrentCell)
                            return EVisibility::Collapsed;

                        int cellIndex = mCurrentCell->IndexInLayer + i + 1;
                        if (cellIndex < 0 || cellIndex >= mLayer->GetCells().Num())
                            return EVisibility::Collapsed;

                        return EVisibility::Visible;
                    }
                )
                .Key_Lambda([this, i]() { return mLayer->Lighttable.NextKeys[i];})
                .Cell_Lambda(
                    [this, i]() -> UOdysseyAnimationCell*
                    {
                        if (!mCurrentCell)
                            return nullptr;

                        const TArray<UOdysseyAnimationCell*>& cells = mLayer->GetCells();
                        int cellIndex = mCurrentCell->IndexInLayer + i + 1;
                        if (cellIndex < 0 || cellIndex >= cells.Num())
                            return nullptr;

                        return cells[cellIndex];
                    }
                )
                .TimelinePosition(InArgs._TimelinePosition)
                .OnActivateOutOfPegs(InArgs._OnActivateOutOfPegs)
                .OnInactivateOutOfPegs(InArgs._OnInactivateOutOfPegs)
                .OnIsOutOfPegsChecked(InArgs._OnIsOutOfPegsChecked)
            ]
        ];
    }

    ChildSlot
    [
        SNew(SBox)
        .HeightOverride(FOptionalSize(mDesiredHeight))
        [
            horizontalBox
        ]
    ];

    Update();
}

void
SOdysseyAnimationTimelineOutOfPegs::OnCurrentFrameChanged(UOdysseyAnimation* iAnimation)
{
    if (iAnimation != mLayer->GetAnimation())
        return;

    Update();
}

void
SOdysseyAnimationTimelineOutOfPegs::OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(SOdysseyAnimationTimelineOutOfPegs::OnImageRenderingChanged);
    if (iEvent.IsInteractive())
        return;

    if (iEvent.GetType() != FOdysseyImageRenderingChangedEvent::eEventType::kCompositionChange)
        return;

    UOdysseyAnimation* animation = mLayer->GetAnimation();
    if (!animation)
        return;

    TArray<FGuid> composition = mLayer->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Editor, animation->CurrentFrame);
    if (!composition.Contains(iEvent.GetId()))
        return;

    Update();
}

void
SOdysseyAnimationTimelineOutOfPegs::Update()
{
    TRACE_CPUPROFILER_EVENT_SCOPE(SOdysseyAnimationTimelineOutOfPegs::Update);
    UOdysseyAnimation* animation = mLayer->GetAnimation();
    if (!animation)
        return;

    int currentFrame = animation->CurrentFrame;
    mCurrentCell = mLayer->GetCellAtFrame(currentFrame);
}
