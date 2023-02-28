// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"


#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerStack"


SOdysseyAnimationLayerStack::~SOdysseyAnimationLayerStack()
{
}

SOdysseyAnimationLayerStack::SOdysseyAnimationLayerStack()
{
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void
SOdysseyAnimationLayerStack::Construct(const FArguments& InArgs)
{
    mLayerStack = InArgs._LayerStack;
    ChildSlot
    [
            SAssignNew(mTreeView, SOdysseyLayerStackTreeView)
            .LayerStack(mLayerStack)
            .OnGenerateRow(this, &SOdysseyAnimationLayerStack::OnGenerateRow)
            .AdditionalColumns(
            {
                SHeaderRow::Column("Timeline")
                .DefaultLabel(LOCTEXT("", ""))
                .VAlignCell(VAlign_Top)
                [
                    SNullWidget::NullWidget
                    //TODO: Add Timeline Header Widget
                ]
            }
        )
    ];
}

TSharedRef<ITableRow>
SOdysseyAnimationLayerStack::OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable)
{
    check(iLayer);

    UClass* layerClass = iLayer->GetClass();
    if (layerClass == UOdysseyAnimationLayerFolder::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerFolderRow, mTreeView.ToSharedRef(), Cast<UOdysseyAnimationLayerFolder>(iLayer));
    }
    else if (layerClass == UOdysseyAnimationLayerImageRaster::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerImageRasterRow, mTreeView.ToSharedRef(), Cast<UOdysseyAnimationLayerImageRaster>(iLayer));
    }

    return SNew(SOdysseyLayerRow, mTreeView.ToSharedRef(), Cast<UOdysseyLayer>(iLayer)); //Default widget
}

#undef LOCTEXT_NAMESPACE
