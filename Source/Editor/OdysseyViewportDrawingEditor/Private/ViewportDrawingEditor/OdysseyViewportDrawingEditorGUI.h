// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class FOdysseyViewportDrawingEditorExtension;
class FOdysseyViewportDrawingEditorMasterTab;

enum class EOdysseyViewportSelectedView: uint8
{
    kBrushSettings,
    kLayerStack,
    kTools
};

class ODYSSEYVIEWPORTDRAWINGEDITOR_API FOdysseyViewportDrawingEditorGUI
{
public:
    // Construction / Destruction
    virtual ~FOdysseyViewportDrawingEditorGUI();
    FOdysseyViewportDrawingEditorGUI(FOdysseyViewportDrawingEditorExtension* iExtension);

public:
    // Initialization
    void CreateTabs();
    void BindShortcuts(FBaseToolkit* iToolkit);

public:
    // Layout
	TSharedPtr<SWidget> CreateWidget();

private:
	FOdysseyViewportDrawingEditorExtension* mExtension;
    TSharedPtr<FOdysseyViewportDrawingEditorMasterTab> mMasterTab;

private:
    TSharedPtr<FUICommandList> mCommandList;
};
