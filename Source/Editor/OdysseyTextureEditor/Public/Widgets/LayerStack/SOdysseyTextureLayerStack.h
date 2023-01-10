// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SOdysseyLayerStackTreeView.h"

/**
 * Implements the Texture Layer stack widget
 */
class ODYSSEYTEXTUREEDITOR_API SOdysseyTextureLayerStack
    : public SCompoundWidget
{

SLATE_DECLARE_WIDGET(SOdysseyTextureLayerStack, STreeView<UOdysseyLayer*>)

public:
    SLATE_BEGIN_ARGS(SOdysseyTextureLayerStack)
        {}
        SLATE_ATTRIBUTE( UOdysseyLayerStack*, LayerStack )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyTextureLayerStack();
    SOdysseyTextureLayerStack();
    
    void Construct(const FArguments& InArgs);

private:
    TSharedRef<ITableRow> OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable);
    void RebuildWidgets();

private:
    TSlateAttribute<UOdysseyLayerStack*> mLayerStack;
    TSharedPtr<SOdysseyLayerStackTreeView> mTreeView;
};
