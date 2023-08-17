// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

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
    void Init();
	void CreateLayout(TSharedPtr<FTabManager::FLayout> iLayout);

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
	TSharedRef<FTabManager::FSplitter>	CreateMainSection();
	TSharedRef<FTabManager::FSplitter>	CreateLeftSection();
	TSharedRef<FTabManager::FSplitter>	CreateRightSection();
	TSharedRef<FTabManager::FSplitter>	CreateMiddleSection();
    TSharedRef<FTabManager::FSplitter>	CreateBottomSection();

public:
    // Getters
    /* TSharedPtr<FOdysseyPainterEditorHUDTab>& GetHUDTab();
    TSharedPtr<FOdysseyPainterEditorViewportTab>& GetViewportTab();
    TSharedPtr<FOdysseyPainterEditorBrushSelectorTab>& GetBrushSelectorTab();
    TSharedPtr<FOdysseyPainterEditorMeshSelectorTab>& GetMeshSelectorTab();
    TSharedPtr<FOdysseyPainterEditorPaletteTab>& GetColorPaletteTab();
    TSharedPtr<FOdysseyPainterEditorColorWheelTab>& GetColorWheelTab();
    TSharedPtr<FOdysseyPainterEditorColorSlidersTab>& GetColorSlidersTab();
    TSharedPtr<FOdysseyPainterEditorTopTab>& GetTopTab();
    TSharedPtr<FOdysseyPainterEditorToolsTab>& GetToolsTab();
    TSharedPtr<FOdysseyPainterEditorToolOptionsTab>& GetToolOptionsTab();
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& GetSelectedVectorObjectTab(); */

protected:
    // Shortcuts
    void AboutIliad();
    void VisitPraxinosWebsite();
    void ManualAndReleaseNotes();
    void GetBrushPack();
    void Discord();
    void SwitchTabletAPI();
    void Group( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void ResetView( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void DeleteSelection( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void GroupPaint( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void Ungroup( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void SendBackward( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void BringForward( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void FlipHorizontal( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void FlipVertical( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void StitchVertices( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );

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
    TSharedPtr<FOdysseyPainterEditorPaletteTab>            mColorPaletteTab;
    TSharedPtr<FOdysseyPainterEditorColorWheelTab>              mColorWheelTab;
    TSharedPtr<FOdysseyPainterEditorColorSlidersTab>            mColorSlidersTab;
    TSharedPtr<FOdysseyPainterEditorTopTab>                     mTopTab;
    TSharedPtr<FOdysseyPainterEditorToolsTab>                   mToolsTab;
    TSharedPtr<FOdysseyPainterEditorToolOptionsTab>             mToolOptionsTab;
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>    mSelectedVectorObjectTab; */
};

