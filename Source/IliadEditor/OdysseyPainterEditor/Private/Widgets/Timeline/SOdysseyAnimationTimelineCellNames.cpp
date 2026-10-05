// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationTimelineCellNames.h"
#include "SOdysseyAnimationTimelineCellNamesKey.h"
#include "SOdysseyAnimationTimelineSection.h"
#include "OdysseyLayerCell.h"

SLATE_IMPLEMENT_WIDGET(SOdysseyAnimationTimelineCellNames)
void
SOdysseyAnimationTimelineCellNames::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mCells, EInvalidateWidgetReason::None)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyAnimationTimelineCellNames&>(Widget).OnCellsChanged();
        }
    ));
}

SOdysseyAnimationTimelineCellNames::SOdysseyAnimationTimelineCellNames()
    : mCells(*this, {})
{

}

void
SOdysseyAnimationTimelineCellNames::Construct(const FArguments& InArgs)
{
    mCells.Assign(*this, InArgs._Cells);
    mTimelinePosition = InArgs._TimelinePosition;
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
                        TArray<UOdysseyLayerCell*> cells = mCells.Get();
                        UOdysseyLayerCell* cell = cells[0];
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

    TArray<UOdysseyLayerCell*> cells = mCells.Get();
    for( UOdysseyLayerCell* cell : cells )
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

void
SOdysseyAnimationTimelineCellNames::OnCellsChanged()
{
    RebuildRow();
}
