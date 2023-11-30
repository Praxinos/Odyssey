// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/SOdysseyAnimationLayerRow.h"
#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineControl.h"

//PUBLIC API-----------------------------------------------------------

void
SOdysseyAnimationLayerRow::Construct(
    const FArguments& iArgs,
    const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
    FOdysseyAnimationEditorExtension* iExtension,
    UOdysseyAnimationLayer* iLayer
)
{
    mExtension = iExtension;

    SOdysseyLayerRow::Construct(
        SOdysseyLayerRow::FArguments(),
        iOwnerTableView,
		iLayer
    );
}

FOdysseyAnimationEditorExtension*
SOdysseyAnimationLayerRow::GetExtension()
{
    return mExtension;
}

TSharedRef<SWidget>
SOdysseyAnimationLayerRow::GenerateWidgetForColumn( const FName& InColumnName )
{
    if (InColumnName == "Timeline")
    {
        return 
            SNew(SBorder)
            .Padding(FMargin(4.f, 0.f, 0.f, 0.f)) //Patch
            [
                SNew(SOdysseyAnimationTimelineControl, mExtension)
                .Clipping(EWidgetClipping::ClipToBoundsAlways)
                [
                    GenerateTimelineWidget()
                ]
            ];
    }
    return SOdysseyLayerRow::GenerateWidgetForColumn(InColumnName);
}

TSharedRef<SWidget>
SOdysseyAnimationLayerRow::GenerateTimelineWidget()
{
    return SNullWidget::NullWidget;
}
