// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationTimelineCellNames.h"
#include "SOdysseyAnimationTimelineCellNamesKey.h"
#include "OdysseyAnimation.h"
#include "SOdysseyAnimationTimelineSection.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationCell.h"
#include "OdysseyPainterEditor.h"

void
SOdysseyAnimationTimelineCellNames::Construct(const FArguments& InArgs, UOdysseyAnimationLayer* iLayer)
{
    //mCurrentFrame = InArgs._CurrentFrame;
    mTimelinePosition = InArgs._TimelinePosition;
    mLayer = iLayer;

    mLayer->OnCellsChanged().AddSP( this, &SOdysseyAnimationTimelineCellNames::OnCellsChanged );

    mRowWidget = SNew( SHorizontalBox );
    RebuildRow();

    ChildSlot
    [
        mRowWidget.ToSharedRef()
    ];
}

void
SOdysseyAnimationTimelineCellNames::RebuildRow()
{
    mRowWidget->ClearChildren();

    mRowWidget->AddSlot()
        .AutoWidth()
        [
            SNew( SOdysseyAnimationTimelineSection )
                .TimelinePosition( mTimelinePosition )
                .WidthInFrames_Lambda(
                    [this]()
                    {
                        UOdysseyLayerCell* cell = mLayer->GetCells()[0];
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

    for( UOdysseyLayerCell* cell : mLayer->GetCells() )
    {
        //UOdysseyAnimationCell* animation_cell = Cast<UOdysseyAnimationCell>( cell );

        if( !IsValid( cell ) )
            continue;

        mRowWidget->AddSlot()
            .AutoWidth()
            [
                SNew( SOdysseyAnimationTimelineSection )
                    .TimelinePosition( mTimelinePosition )
                    .HAlign( HAlign_Fill )
                    .WidthInFrames_Lambda(
                        [cell]()
                        {
                            if( !IsValid( cell ) )
                                return 0;

                            return cell->GetExposure();
                        }
                    )
                    [
                        SNew( SOdysseyAnimationTimelineCellNamesKey )
                            .Cell_Lambda(
                                [cell]() -> UOdysseyLayerCell*
                                {
                                    return cell;
                                }
                            )
                            .TimelinePosition( mTimelinePosition )
                    ]
            ];
    }

}

//UOdysseyAnimationCell*
//SOdysseyAnimationTimelineCellNames::GetCurrentCell() const
//{
//    return Cast<UOdysseyAnimationCell>(mLayer->GetCellAtFrame(mCurrentFrame.Get()));
//}

void
SOdysseyAnimationTimelineCellNames::OnCellsChanged()
{
    RebuildRow();
}
