// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyTextureEditorGUI.h"

class FOdysseyViewportDrawingEditor;
class FOdysseyViewportDrawingEditorTextureSelectorTab;

enum class EOdysseyViewportSelectedView: uint8
{
    kBrushSettings,
    kStrokeOptions,
    kLayerStack,
    kTools
};

class ODYSSEYVIEWPORTDRAWINGEDITOR_API FOdysseyViewportDrawingEditorGUI :
	public FOdysseyTextureEditorGUI
{
public:
    // Construction / Destruction
    virtual ~FOdysseyViewportDrawingEditorGUI();
    FOdysseyViewportDrawingEditorGUI(FOdysseyViewportDrawingEditor* iEditor);

public:
    // Initialization
    virtual void CreateTabs() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

public:
    // GettersFName
	virtual FName GetLayoutName() override;
    TSharedPtr<FOdysseyViewportDrawingEditorTextureSelectorTab>& GetTextureSelectorTab();
    EOdysseyViewportSelectedView GetSelectedView();

public:
    // Layout
	virtual TSharedPtr<SWidget> CreateWidget() override;
    TSharedPtr<SWidget> CreateTabSelectorWidget();
    SVerticalBox::FSlot& CreateSection(TSharedPtr<SWidget> iWidget, FText iName);

public:
    void SetSelectedView(EOdysseyViewportSelectedView iView);

private:
    EVisibility GetViewVisibility(EOdysseyViewportSelectedView iView) const;

private:
	FOdysseyViewportDrawingEditor* mEditor;
    TSharedPtr<FOdysseyViewportDrawingEditorTextureSelectorTab>          mTextureSelectorTab;

private:
    EOdysseyViewportSelectedView mSelectedView;
    TSharedPtr<FUICommandList>  mCommandList;
};
