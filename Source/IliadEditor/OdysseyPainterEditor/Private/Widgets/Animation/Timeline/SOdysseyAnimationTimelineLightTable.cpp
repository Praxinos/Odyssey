// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineLightTable.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineLightTableKey.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "OdysseyAnimation.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineSection.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "OdysseyPainterEditor.h"
#include "UObject/OdysseyObjectEditorUtils.h"

void
SOdysseyAnimationTimelineLightTable::Construct(const FArguments& InArgs, UOdysseyAnimationLayer* iLayer)
{
    UOdysseyAnimation::OnCurrentFrameChanged().AddSP(SharedThis(this), &SOdysseyAnimationTimelineLightTable::OnCurrentFrameChanged);
    FOdysseyRenderingAbility::OnRenderingChangedDelegate().AddSP(this, &SOdysseyAnimationTimelineLightTable::OnRenderingChanged);

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
                    if (!cell)
                        return 0;

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
                    if (cellIndex < 0 || cellIndex >= cells.Num() || !cells[cellIndex])
                        return 0;

                    return cells[cellIndex]->Exposure;
                }
            )
            [

                SNew(SOdysseyAnimationTimelineLightTableKey)
                .Visibility_Lambda(
                    [this, i]()
                    {
                        if (!mCurrentCell)
                            return EVisibility::Collapsed;

                        int cellIndex = mCurrentCell->IndexInLayer - i - 1;
                        if (cellIndex < 0 || cellIndex >= mLayer->GetCells().Num() || !mLayer->GetCells()[cellIndex])
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
                        if (cellIndex < 0 || cellIndex >= cells.Num() || !mLayer->GetCells()[cellIndex])
                            return nullptr;

                        return cells[cellIndex];
                    }
                )
                .OnChanged_Lambda(
                    [this, i](FOdysseyAnimationLightTableKey iKey)
                    {
                        FOdysseyAnimationLightTable lighttable = mLayer->Lighttable;
                        lighttable.PreviousKeys[i] = iKey;
                        FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable, EPropertyChangeType::Interactive);
                    }
                )
                .OnCommited_Lambda(
                    [this, i](FOdysseyAnimationLightTableKey iKey)
                    {
                        FOdysseyAnimationLightTable lighttable = mLayer->Lighttable;
                        lighttable.PreviousKeys[i] = iKey;
                        FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable, EPropertyChangeType::ValueSet);
                    }
                )
                .TimelinePosition(InArgs._TimelinePosition)
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
                    if (cellIndex < 0 || cellIndex >= cells.Num() || !cells[cellIndex])
                        return 0;

                    return cells[cellIndex]->Exposure;
                }
            )
            [

                SNew(SOdysseyAnimationTimelineLightTableKey)
                .Visibility_Lambda(
                    [this, i]()
                    {
                        if (!mCurrentCell)
                            return EVisibility::Collapsed;

                        int cellIndex = mCurrentCell->IndexInLayer + i + 1;
                        if (cellIndex < 0 || cellIndex >= mLayer->GetCells().Num() || !mLayer->GetCells()[cellIndex])
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
                        if (cellIndex < 0 || cellIndex >= cells.Num() || !mLayer->GetCells()[cellIndex])
                            return nullptr;

                        return cells[cellIndex];
                    }
                )
                .OnChanged_Lambda(
                    [this, i](FOdysseyAnimationLightTableKey iKey)
                    {
                        FOdysseyAnimationLightTable lighttable = mLayer->Lighttable;
                        lighttable.NextKeys[i] = iKey;
                        FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable, EPropertyChangeType::Interactive);
                    }
                )
                .OnCommited_Lambda(
                    [this, i](FOdysseyAnimationLightTableKey iKey)
                    {
                        FOdysseyAnimationLightTable lighttable = mLayer->Lighttable;
                        lighttable.NextKeys[i] = iKey;
                        FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable, EPropertyChangeType::ValueSet);
                    }
                )
                .TimelinePosition(InArgs._TimelinePosition)
            ]
        ];
    }

    ChildSlot
    [
        horizontalBox
    ];

    Update();
}

void
SOdysseyAnimationTimelineLightTable::OnCurrentFrameChanged(UOdysseyAnimation* iAnimation)
{
    if (iAnimation != mLayer->GetAnimation())
        return;

    Update();
}

void
SOdysseyAnimationTimelineLightTable::OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(SOdysseyAnimationTimelineLightTable::OnRenderingChanged);
    if (iEvent.IsInteractive())
        return;

    if (iEvent.GetType() != FOdysseyRenderingChangedEvent::eEventType::kCompositionChange)
        return;

    UOdysseyAnimation* animation = mLayer->GetAnimation();
    if (!animation)
        return;

    TArray<FGuid> composition = mLayer->GetRenderingComposition(EOdysseyRenderingType::Editor, animation->CurrentFrame);
    if (!composition.Contains(iEvent.GetId()))
        return;

    Update();
}

void
SOdysseyAnimationTimelineLightTable::Update()
{
    TRACE_CPUPROFILER_EVENT_SCOPE(SOdysseyAnimationTimelineLightTable::Update);
    UOdysseyAnimation* animation = mLayer->GetAnimation();
    if (!animation)
        return;

    int currentFrame = animation->CurrentFrame;
    mCurrentCell = mLayer->GetCellAtFrame(currentFrame);
}
