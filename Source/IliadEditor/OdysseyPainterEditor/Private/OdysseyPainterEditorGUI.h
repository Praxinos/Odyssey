// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyEditorLayoutBuilder.h"

class FOdysseyPainterEditor;
class FOdysseyVectorEngine;
class FOdysseyVectorScene;
class FBaseToolkit;
class UOdysseyLayerStack;
class FOdysseyVectorGroupPaint;
class UOdysseyAnimation;

/**
 * Implements an Editor toolkit for textures.
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorGUI
    : public TSharedFromThis<FOdysseyPainterEditorGUI>
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorGUI();
    FOdysseyPainterEditorGUI(FOdysseyPainterEditor* iEditor);

public:
    static void ExtendLevelEditorLayout(FLayoutExtender& Extender);

public:
    void Initialize();
    void Finalize();

public:
    // Tabs
    void CreateTabs();
    void BindShortcuts(FBaseToolkit* iToolkit);

protected:
    // Shortcuts
    void SwitchTabletAPI();

    void ClearCurrentLayer();
    void SelectAll();
    void ClearCurrentSelection();
    void InvertSelection();

    void ToggleEraserButton();

private:
    //Vector
    void OnCurrentLayerChanged( UOdysseyLayerStack* iLayerStack );
    void OnSourceChanged();
    void OnVectorSceneNotify( FOdysseyVectorGroupPaint* iScene, uint64 iSignalFlags );
    void OnCurrentFrameChanged( UOdysseyAnimation* iAnimation );
    void ParseVectorNotifications( FOdysseyVectorGroupPaint* iScene, uint64 iSignalFlags );

private:
    FOdysseyPainterEditor*                          mEditor;
    TSharedPtr<FTabManager::FLayout>                mLayout;
    TSharedPtr<SWidget>                             mWidget;
};
