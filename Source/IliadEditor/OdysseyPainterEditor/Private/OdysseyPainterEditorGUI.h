// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "OdysseyEditorLayoutBuilder.h"

class FOdysseyPainterEditor;
class FOdysseyVectorEngine;
class FOdysseyVectorScene;
class FBaseToolkit;
class UOdysseyLayerStack;
class FOdysseyVectorGroupPaint;

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
    void BuildLayout(FOdysseyEditorLayoutBuilder& iBuilder);

public:
    // Tabs
    void CreateTabs();
    void BindShortcuts(FBaseToolkit* iToolkit);

protected:
    // Layout
    void    CreateLeftSection(FOdysseyEditorLayoutBuilder& iBuilder);
    void    CreateRightSection(FOdysseyEditorLayoutBuilder& iBuilder);
    void    CreateCenterSection(FOdysseyEditorLayoutBuilder& iBuilder);
    void    CreateBottomSection(FOdysseyEditorLayoutBuilder& iBuilder);

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

private:
    FOdysseyPainterEditor*                          mEditor;
    TSharedPtr<FTabManager::FLayout>                mLayout;
    TSharedPtr<SWidget>                             mWidget;
};
