// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationLayerRow.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerRow"

//PUBLIC API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyAnimationLayerRow::GenerateWidgetForColumn( const FName& InColumnName )
{
    if (InColumnName == "Timeline")
    {
        return GenerateTimelineWidget();
    }
    return SOdysseyLayerRow::GenerateWidgetForColumn(InColumnName);
}

TSharedRef<SWidget>
SOdysseyAnimationLayerRow::GenerateTimelineWidget()
{
    return SNullWidget::NullWidget;
}

#undef LOCTEXT_NAMESPACE