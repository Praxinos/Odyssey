// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Toolkits/BaseToolkit.h"
#include "ArianeEditor.h"

class FArianeEditorViewportEdMode;
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

        virtual void Init( const TSharedPtr<IToolkitHost>& iInitToolkitHost ) override;
    /* Gary
        TSharedPtr<FArianeEditor> GetEditor() const;
    */
        virtual TSharedPtr<SWidget> GetInlineContent() const override;
        virtual FEdMode* GetEditorMode() const override;

        /** IToolkit interface */
        virtual FName GetToolkitFName() const override;
        virtual FText GetBaseToolkitName() const override;
    /* Gary
        void OnAddEditedObject(UObject* iObject);
        void OnRemoveEditedObject(UObject* iObject);
    */
        virtual void ExtendSecondaryModeToolbar(UToolMenu *InModeToolbarMenu) override;

        FArianeEditor& GetEditor();

        //If we fill the array with names, a mode toolbar will pop in our edMode
        //virtual void GetToolPaletteNames( TArray<FName>& ioPaletteNames ) const override;
        //virtual void BuildToolPalette( FName iPalette, class FToolBarBuilder& ioToolbarBuilder ) override;
    /* Gary
        TSharedPtr<FArianeViewportDrawingEditorExtension> GetViewportDrawingExtension() const;
    */

    public:
        //from IAssetEditorInstance
        virtual FName GetEditorName() const override;
        virtual void FocusWindow(UObject* ObjectToFocusOn = nullptr) override;
        virtual bool CloseWindow() override;
        virtual bool IsPrimaryEditor() const override;
        virtual void InvokeTab(const struct FTabId& TabId) override;
        virtual FName GetToolbarTabId() const override;
        virtual TSharedPtr<class FTabManager> GetAssociatedTabManager() override;
        virtual double GetLastActivationTime() override;
        virtual void RemoveEditingAsset(UObject* Asset) override;
        virtual void RequestModeUITabs() override;
        virtual void InvokeUI() override;
        virtual void ShutdownUI() override;
    /* Gary
        void RebuildLevelEditorMenu() const;

        FString GetOpenedTabIdsSavedPath() const;
        void LoadOpenedTabs();
        void SaveOpenedTabs();
        void OnEditorClose();
    */
    protected:
        void ExtendMenu ( FMenuBuilder& MenuBuilder );
        void AddActorMenuEntry( FToolMenuSection& InSection );

    private:
        FArianeEditorViewportEdMode* mEdMode;
        FArianeEditor mEditor;
/*
        TSharedPtr<FArianeViewportDrawingEditorExtension> mViewportDrawingExtension;
        FEdMode* mEdMode;
        bool mTabSaved;
*/
        TSharedPtr<FExtender> mLevelEditorMenuExtender;
};
