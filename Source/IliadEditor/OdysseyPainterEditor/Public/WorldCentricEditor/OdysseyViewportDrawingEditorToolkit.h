// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyPainterEditor.h"
#include "Toolkits/BaseToolkit.h"

class FOdysseyViewportDrawingEditor;
class FOdysseyViewportDrawingEditorExtension;
class FOdysseyAnimationEditorExtension;
class FEdMode;

class ODYSSEYPAINTEREDITOR_API FOdysseyViewportDrawingEditorToolkit
    : public FModeToolkit
    , public IAssetEditorInstance
{
public:
    ~FOdysseyViewportDrawingEditorToolkit();
    FOdysseyViewportDrawingEditorToolkit(FEdMode* iEdMode);

    void Initialize(
        FEdMode* iEditorMode,
        const TSharedPtr<IToolkitHost>& iInitToolkitHost
    );

    TSharedPtr<FOdysseyPainterEditor> GetEditor() const;

    virtual TSharedPtr<SWidget> GetInlineContent() const override;
    virtual FEdMode* GetEditorMode() const override;

    /** IToolkit interface */
    virtual FName GetToolkitFName() const override;
    virtual FText GetBaseToolkitName() const override;

    void OnAddEditedObject(UObject* iObject);
    void OnRemoveEditedObject(UObject* iObject);

    virtual void ExtendSecondaryModeToolbar(UToolMenu *InModeToolbarMenu) override;

    //If we fill the array with names, a mode toolbar will pop in our edMode
    //virtual void GetToolPaletteNames( TArray<FName>& ioPaletteNames ) const override;
    //virtual void BuildToolPalette( FName iPalette, class FToolBarBuilder& ioToolbarBuilder ) override;

    TSharedPtr<FOdysseyViewportDrawingEditorExtension> GetViewportDrawingExtension() const;
    TSharedPtr<FOdysseyAnimationEditorExtension> GetAnimationExtension() const;

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
    virtual void SetModeUILayer(const TSharedPtr<FAssetEditorModeUILayer> InLayer) override;

    void RebuildLevelEditorMenu() const;

    void OnToolkitHostReadyForUI();
    void OnToolkitHostShutdownUI();

    void SaveOpenedTabs();
    void LoadOpenedTabs();

private:
    TSharedPtr<FOdysseyPainterEditor> mEditor;
    TSharedPtr<FOdysseyViewportDrawingEditorExtension> mViewportDrawingExtension;
    TSharedPtr<FOdysseyAnimationEditorExtension> mAnimationExtension;
    FEdMode* mEdMode;
    bool mTabSaved;
    TSharedPtr<FExtender> mLevelEditorMenuExtender;
};
