// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "TextureEditor/OdysseyTextureEditorGUI.h"

class FOdysseyViewportDrawingEditor;
class FOdysseyViewportDrawingEditorMasterTab;

enum class EOdysseyViewportSelectedView: uint8
{
    kBrushSettings,
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
    TSharedRef<FTabManager::FSplitter> CreateMainSection() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

public:
    // GettersFName
	virtual FName GetLayoutName() override;
    TSharedPtr<FOdysseyViewportDrawingEditorMasterTab>& GetMasterTab();

public:
    // Layout
	virtual TSharedPtr<SWidget> CreateWidget() override;

private:
	FOdysseyViewportDrawingEditor* mEditor;
    TSharedPtr<FOdysseyViewportDrawingEditorMasterTab> mMasterTab;

private:
    TSharedPtr<FUICommandList> mCommandList;
};
