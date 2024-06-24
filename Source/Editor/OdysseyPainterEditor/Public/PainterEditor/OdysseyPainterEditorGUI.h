// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyEditorLayoutBuilder.h"

class FOdysseyPainterEditor;
class FOdysseyVectorEngine;
class FOdysseyVectorScene;

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

public:
    // Menu And Toolbar
    void ExtendMenu( FToolMenuOwner iOwner, FName iMenuName );
    void ExtendMenuAbout(FToolMenuOwner iOwner, FName iMenuName );

protected:
	// Layout
	void	CreateLeftSection(FOdysseyEditorLayoutBuilder& iBuilder);
	void	CreateRightSection(FOdysseyEditorLayoutBuilder& iBuilder);
	void	CreateCenterSection(FOdysseyEditorLayoutBuilder& iBuilder);

protected:
    // Shortcuts
    void AboutIliad();
    void VisitPraxinosWebsite();
    void ManualAndReleaseNotes();
    void GetBrushPack();
    void Discord();
    void SwitchTabletAPI();

    void ClearCurrentLayer();
    void ClearCurrentSelection();
    void CopyCurrentSelection();
    void PasteCurrentSelection();

    void ToggleEraserButton();

private:
    FOdysseyPainterEditor*                          mEditor;
	TSharedPtr<FTabManager::FLayout>                mLayout;
    TSharedPtr<SWidget>                             mWidget;

protected:
    /*
    TSharedPtr<FOdysseyPainterEditorHUDTab>                     mHUDTab;
    TSharedPtr<FOdysseyPainterEditorViewportTab>                mViewportTab;
    TSharedPtr<FOdysseyPainterEditorBrushSelectorTab>           mBrushSelectorTab;
    TSharedPtr<FOdysseyPainterEditorMeshSelectorTab>            mMeshSelectorTab;
    TSharedPtr<FOdysseyPainterEditorPaletteTab>                 mColorPaletteTab;
    TSharedPtr<FOdysseyPainterEditorColorWheelTab>              mColorWheelTab;
    TSharedPtr<FOdysseyPainterEditorColorSlidersTab>            mColorSlidersTab;
    TSharedPtr<FOdysseyPainterEditorTopTab>                     mTopTab;
    TSharedPtr<FOdysseyPainterEditorToolsTab>                   mToolsTab;
    TSharedPtr<FOdysseyPainterEditorToolOptionsTab>             mToolOptionsTab; */
};

