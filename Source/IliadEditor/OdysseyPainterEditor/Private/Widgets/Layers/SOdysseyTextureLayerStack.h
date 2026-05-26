// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SOdysseyTextureLayerStackTreeView;
class UOdysseyLayerStack;
class UOdysseyLayer;
class FOdysseyPainterEditor;

/**
 * Implements the Texture Layer stack widget
 */
class SOdysseyTextureLayerStack
    : public SCompoundWidget
{

SLATE_DECLARE_WIDGET(SOdysseyTextureLayerStack, SCompoundWidget)

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
    TSharedRef<class ITableRow> OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<class STableViewBase>& iOwnerTable);
    void RebuildWidgets();

private:
    TSlateAttribute<UOdysseyLayerStack*> mLayerStack;
    TSharedPtr<SOdysseyTextureLayerStackTreeView> mTreeView;
};
