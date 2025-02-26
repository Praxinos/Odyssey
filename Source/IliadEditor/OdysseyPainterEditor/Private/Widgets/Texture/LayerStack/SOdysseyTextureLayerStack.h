// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

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

    void Construct(const FArguments& InArgs, FOdysseyPainterEditor* iEditor);

private:
    TSharedRef<ITableRow> OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable);
    void RebuildWidgets();

private:
    FOdysseyPainterEditor* mEditor;
    TSlateAttribute<UOdysseyLayerStack*> mLayerStack;
    TSharedPtr<SOdysseyTextureLayerStackTreeView> mTreeView;
};
