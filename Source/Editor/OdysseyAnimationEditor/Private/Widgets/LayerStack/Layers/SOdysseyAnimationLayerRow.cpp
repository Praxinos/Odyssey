// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/SOdysseyAnimationLayerRow.h"
#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerRow"

//PUBLIC API-----------------------------------------------------------

void
SOdysseyAnimationLayerRow::Construct(
    const FArguments& iArgs,
    const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
    FOdysseyAnimationEditor* iEditor,
    UOdysseyAnimationLayer* iLayer
)
{
    mEditor = iEditor;

    SOdysseyLayerRow::Construct(
        SOdysseyLayerRow::FArguments(),
        iOwnerTableView,
		iLayer
    );
}

FOdysseyAnimationEditor*
SOdysseyAnimationLayerRow::GetEditor()
{
    return mEditor;
}

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
    return SNew(SOdysseyAnimationTimelineWidget, mEditor);
}


#undef LOCTEXT_NAMESPACE