// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal Headers
#include "CoreMinimal.h"
#include "IDetailsView.h"
// Ariane Headers
#include "ArianeLayer.h"
#include "ArianeLayerDrawing.h"
#include "ArianeEditorLayerView.h"
#include "ArianeEditorLayerDrawingView.h"

class FArianeEditor;
class UArianeEditorTool;

/**
 * Implements the Scene Tree View Widget
 */
class ARIANEEDITOR_API SArianeEditorLayerStackPanel
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArianeEditorLayerStackPanel)
        {}
        //SLATE_ARGUMENT(UArianeEditorTool*, CurrentTool)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SArianeEditorLayerStackPanel();
    SArianeEditorLayerStackPanel();

    void Construct(const FArguments& InArgs, FArianeEditor* InEditor);

protected:
    FReply NewLayer();
    TSharedPtr<SWidget> CreateTransformWidget();
    void OnTransformChanged( const FPropertyChangedEvent& PropertyChangedEvent );
    void OnLayerSelectionChanged();
    void OnPre3DPaintingComponentSelectionChanged();
    void OnPost3DPaintingComponentSelectionChanged();

protected:
    FArianeEditor* Editor;
    UArianeEditorLayerView* LayerView;
    UArianeEditorLayerDrawingView* LayerDrawingView;
    TSharedPtr<IDetailsView> LayerDetailsView;
};
