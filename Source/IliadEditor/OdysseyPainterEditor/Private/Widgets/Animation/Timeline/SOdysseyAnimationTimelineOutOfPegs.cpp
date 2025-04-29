// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineOutOfPegs.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineOutOfPegsKey.h"
#include "OdysseyAnimationLightTable.h"
#include "Tools/OutOfPegsTool/OdysseyPainterEditorAnimationOutOfPegsTool.h"
#include "OdysseyAnimation.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineSection.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationCell.h"
#include "OdysseyPainterEditor.h"

void
SOdysseyAnimationTimelineOutOfPegs::Construct(const FArguments& InArgs, UOdysseyAnimationLayer* iLayer)
{
    mCurrentFrame = InArgs._CurrentFrame;
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
                    UOdysseyLayerCell* currentCell = GetCurrentCell();
                    if (!currentCell)
                        return 0;

                    int firstCellIndex = FMath::Max(currentCell->GetIndexInLayer() - 10, 0);
                    UOdysseyLayerCell* cell = mLayer->GetCells()[firstCellIndex];
                    if( !cell )
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
                    UOdysseyLayerCell* currentCell = GetCurrentCell();
                    if (!currentCell)
                        return 0;

                    const TArray<UOdysseyLayerCell*>& cells = mLayer->GetCells();
                    int cellIndex = currentCell->GetIndexInLayer() - i - 1;
                    if (cellIndex < 0 || cellIndex >= cells.Num() || !mLayer->GetCells()[cellIndex])
                        return 0;

                    return cells[cellIndex]->GetExposure();
                }
            )
            [

                SNew(SOdysseyAnimationTimelineOutOfPegsKey)
                .Visibility_Lambda(
                    [this, i]()
                    {
                        UOdysseyLayerCell* currentCell = GetCurrentCell();
                        if (!currentCell)
                            return EVisibility::Collapsed;

                        int cellIndex = currentCell->GetIndexInLayer() - i - 1;
                        if (cellIndex < 0 || cellIndex >= mLayer->GetCells().Num() || !mLayer->GetCells()[cellIndex])
                            return EVisibility::Collapsed;

                        return EVisibility::Visible;
                    }
                )
                .Key_Lambda([this, i]() { return mLayer->GetLighttable().PreviousKeys[i];})
                .Cell_Lambda(
                    [this, i]() -> UOdysseyAnimationCell*
                    {
                        UOdysseyLayerCell* currentCell = GetCurrentCell();
                        if (!currentCell)
                            return nullptr;

                        const TArray<UOdysseyLayerCell*>& cells = mLayer->GetCells();
                        int cellIndex = currentCell->GetIndexInLayer() - i - 1;
                        if (cellIndex < 0 || cellIndex >= cells.Num() || !mLayer->GetCells()[cellIndex])
                            return nullptr;

                        return Cast<UOdysseyAnimationCell>(cells[cellIndex]);
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
                UOdysseyLayerCell* currentCell = GetCurrentCell();
                if (!currentCell)
                    return EVisibility::Collapsed;

                return EVisibility::Visible;
            }
        )
        .WidthInFrames_Lambda(
            [this]()
            {
                UOdysseyLayerCell* currentCell = GetCurrentCell();
                if (!currentCell)
                    return 0;

                return currentCell->GetExposure();
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
                    UOdysseyLayerCell* currentCell = GetCurrentCell();
                    if (!currentCell)
                        return 0;

                    const TArray<UOdysseyLayerCell*>& cells = mLayer->GetCells();
                    int cellIndex = currentCell->GetIndexInLayer() + i + 1;
                    if (cellIndex < 0 || cellIndex >= cells.Num() || !mLayer->GetCells()[cellIndex])
                        return 0;

                    return cells[cellIndex]->GetExposure();
                }
            )
            [

                SNew(SOdysseyAnimationTimelineOutOfPegsKey)
                .Visibility_Lambda(
                    [this, i]()
                    {
                        UOdysseyLayerCell* currentCell = GetCurrentCell();
                        if (!currentCell)
                            return EVisibility::Collapsed;

                        int cellIndex = currentCell->GetIndexInLayer() + i + 1;
                        if (cellIndex < 0 || cellIndex >= mLayer->GetCells().Num() || !mLayer->GetCells()[cellIndex])
                            return EVisibility::Collapsed;

                        return EVisibility::Visible;
                    }
                )
                .Key_Lambda([this, i]() { return mLayer->GetLighttable().NextKeys[i];})
                .Cell_Lambda(
                    [this, i]() -> UOdysseyAnimationCell*
                    {
                        UOdysseyLayerCell* currentCell = GetCurrentCell();
                        if (!currentCell)
                            return nullptr;

                        const TArray<UOdysseyLayerCell*>& cells = mLayer->GetCells();
                        int cellIndex = currentCell->GetIndexInLayer() + i + 1;
                        if (cellIndex < 0 || cellIndex >= cells.Num() || !mLayer->GetCells()[cellIndex])
                            return nullptr;

                        return Cast<UOdysseyAnimationCell>(cells[cellIndex]);
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
}

UOdysseyAnimationCell*
SOdysseyAnimationTimelineOutOfPegs::GetCurrentCell() const
{
    return Cast<UOdysseyAnimationCell>(mLayer->GetCellAtFrame(mCurrentFrame.Get()));
}
