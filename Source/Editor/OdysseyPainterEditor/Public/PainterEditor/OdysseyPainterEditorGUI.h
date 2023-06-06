// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorGUI.h"

#include "OdysseyPainterEditorBrushSelectorTab.h"
#include "OdysseyPainterEditorColorSlidersTab.h"
#include "OdysseyPainterEditorColorWheelTab.h"
#include "OdysseyPainterEditorHUDTab.h"
#include "OdysseyPainterEditorMeshSelectorTab.h"
#include "OdysseyPainterEditorTopTab.h"
#include "OdysseyPainterEditorToolsTab.h"
#include "OdysseyPainterEditorToolOptionsTab.h"
#include "OdysseyPainterEditorSelectedVectorObjectTab.h"
#include "OdysseyPainterEditorViewportTab.h"

#include "Undo/OdysseyVectorUndo.h"

class FOdysseyPainterEditor;
class FOdysseyPainterEditorController;

/**
 * Implements an Editor toolkit for textures.
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorGUI :
	public FOdysseyEditorGUI
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorGUI();
    FOdysseyPainterEditorGUI(FOdysseyPainterEditor* iEditor);

public:
    void Init();

public:
    // Tabs
    virtual void CreateTabs();
    virtual void BindShortcuts(FBaseToolkit* iToolkit);

public:
    // Menu And Toolbar
    virtual void ExtendMenu( FToolMenuOwner iOwner, FName iMenuName );
    virtual void ExtendMenuAbout(FToolMenuOwner iOwner, FName iMenuName );

protected:
	// Layout
	virtual TSharedPtr<FTabManager::FLayout> CreateLayout() override;
	virtual TSharedRef<FTabManager::FSplitter>	CreateMainSection();
	virtual TSharedRef<FTabManager::FSplitter>	CreateLeftSection();
	virtual TSharedRef<FTabManager::FSplitter>	CreateRightSection();
	virtual TSharedRef<FTabManager::FSplitter>	CreateMiddleSection();
    virtual TSharedRef<FTabManager::FSplitter>	CreateBottomSection();

public:
    // Getters
    TSharedPtr<FOdysseyPainterEditorHUDTab>& GetHUDTab();
    TSharedPtr<FOdysseyPainterEditorViewportTab>& GetViewportTab();
    TSharedPtr<FOdysseyPainterEditorBrushSelectorTab>& GetBrushSelectorTab();
    TSharedPtr<FOdysseyPainterEditorMeshSelectorTab>& GetMeshSelectorTab();
    TSharedPtr<FOdysseyPainterEditorColorWheelTab>& GetColorWheelTab();
    TSharedPtr<FOdysseyPainterEditorColorSlidersTab>& GetColorSlidersTab();
    TSharedPtr<FOdysseyPainterEditorTopTab>& GetTopTab();
    TSharedPtr<FOdysseyPainterEditorToolsTab>& GetToolsTab();
    TSharedPtr<FOdysseyPainterEditorToolOptionsTab>& GetToolOptionsTab();
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& GetSelectedVectorObjectTab();

protected:
    // Shortcuts
    virtual void AboutIliad();
    virtual void VisitPraxinosWebsite();
    virtual void ManualAndReleaseNotes();
    virtual void GetBrushPack();
    virtual void Discord();
    virtual void SwitchTabletAPI();
    void Group( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, UObject* iStoreUndoObject );
    void ResetView( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, UObject* iStoreUndoObject );
    void RemoveSelectedObjects( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, UObject* iStoreUndoObject );
    void GroupPaint( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, UObject* iStoreUndoObject );
    void Ungroup( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, UObject* iStoreUndoObject );
    void SendBackward( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, UObject* iStoreUndoObject );
    void BringForward( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, UObject* iStoreUndoObject );

private:
    FOdysseyPainterEditor*                          mEditor;

protected:
    TSharedPtr<FOdysseyPainterEditorHUDTab>                     mHUDTab;
    TSharedPtr<FOdysseyPainterEditorViewportTab>                mViewportTab;
    TSharedPtr<FOdysseyPainterEditorBrushSelectorTab>           mBrushSelectorTab;
    TSharedPtr<FOdysseyPainterEditorMeshSelectorTab>            mMeshSelectorTab;
    TSharedPtr<FOdysseyPainterEditorColorWheelTab>              mColorWheelTab;
    TSharedPtr<FOdysseyPainterEditorColorSlidersTab>            mColorSlidersTab;
    TSharedPtr<FOdysseyPainterEditorTopTab>                     mTopTab;
    TSharedPtr<FOdysseyPainterEditorToolsTab>                   mToolsTab;
    TSharedPtr<FOdysseyPainterEditorToolOptionsTab>             mToolOptionsTab;
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>    mSelectedVectorObjectTab;
};

