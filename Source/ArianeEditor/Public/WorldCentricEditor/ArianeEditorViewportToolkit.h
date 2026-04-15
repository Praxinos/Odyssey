// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Toolkits/BaseToolkit.h"
#include "ArianeEditor.h"
#include "ArianeEditorViewportEdMode.h"

class FArianeViewportDrawingEditor;
class FArianeViewportDrawingEditorExtension;
class FEdMode;

class ARIANEEDITOR_API FArianeEditorViewportToolkit
    : public FModeToolkit
    , public IAssetEditorInstance
{
public:
    ~FArianeEditorViewportToolkit();
    FArianeEditorViewportToolkit( FArianeEditorViewportEdMode* iViewportDrawingEditorEdMode );

    /** Overriden from FModeToolkit */
    virtual void Init( const TSharedPtr<IToolkitHost>& iInitToolkitHost ) override;

    /** Overriden from FModeToolkit */
    virtual TSharedPtr<SWidget> GetInlineContent() const override;

    /** Overriden from FModeToolkit */
    virtual FArianeEditorViewportEdMode* GetEditorMode() const override;

    /** IToolkit interface */
    virtual FName GetToolkitFName() const override;
    virtual FText GetBaseToolkitName() const override;

/* Gary
    void OnAddEditedObject(UObject* iObject);
    void OnRemoveEditedObject(UObject* iObject);
*/
    /** Overriden from FModeToolkit */
    virtual void ExtendSecondaryModeToolbar(UToolMenu *InModeToolbarMenu) override;

    FArianeEditor& GetEditor();

    //If we fill the array with names, a mode toolbar will pop in our edMode
    //virtual void GetToolPaletteNames( TArray<FName>& ioPaletteNames ) const override;
    //virtual void BuildToolPalette( FName iPalette, class FToolBarBuilder& ioToolbarBuilder ) override;
/* Gary
    TSharedPtr<FArianeViewportDrawingEditorExtension> GetViewportDrawingExtension() const;
*/

public:
    /** Overriden from IAssetEditorInstance */
    virtual FName GetEditorName() const override;
    /** Overriden from IAssetEditorInstance */
    virtual void FocusWindow(UObject* ObjectToFocusOn = nullptr) override;
    /** Overriden from IAssetEditorInstance */
    virtual bool CloseWindow() override;
    /** Overriden from IAssetEditorInstance */
    virtual bool IsPrimaryEditor() const override;
    /** Overriden from IAssetEditorInstance */
    virtual void InvokeTab(const struct FTabId& TabId) override;
    /** Overriden from IAssetEditorInstance */
    virtual FName GetToolbarTabId() const override;
    /** Overriden from IAssetEditorInstance */
    virtual TSharedPtr<class FTabManager> GetAssociatedTabManager() override;
    /** Overriden from IAssetEditorInstance */
    virtual double GetLastActivationTime() override;
    /** Overriden from IAssetEditorInstance */
    virtual void RemoveEditingAsset(UObject* Asset) override;

    /** Overriden from FModeToolkit */
    virtual void RequestModeUITabs() override;
    /** Overriden from FModeToolkit */
    virtual void InvokeUI() override;
    /** Overriden from FModeToolkit */
    virtual void ShutdownUI() override;


    void RebuildLevelEditorMenu() const;

    FString GetOpenedTabIdsSavedPath() const;
    void LoadOpenedTabs();
    void SaveOpenedTabs();
    void OnEditorClose();
    void Exit();

protected:
    void ExtendMenu ( FMenuBuilder& MenuBuilder );
    void AddActorMenuEntry( FToolMenuSection& InSection );

private:
    FArianeEditorViewportEdMode* EdMode;
    TSharedPtr<FArianeEditor> Editor;
/*
    TSharedPtr<FArianeViewportDrawingEditorExtension> mViewportDrawingExtension;
    FEdMode* mEdMode;
    bool mTabSaved;
*/
    TSharedPtr<FExtender> LevelEditorMenuExtender;

TSharedPtr<FTabManager> LocalTabManager;
TSharedPtr<SWidget> TabStackWidget;
};
