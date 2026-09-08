// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Framework/Docking/TabManager.h"

class UOdysseyPalette;
class SOdysseyPaletteTreeView;

/**
 * Implements an Editor toolkit for the Painter Editor.
 * The toolkit is the main entry point for the Painter Editor
 */
class FOdysseyPaletteEditorToolkit
    : public FAssetEditorToolkit
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPaletteEditorToolkit();
    FOdysseyPaletteEditorToolkit(UOdysseyPalette* iPalette);

public:
    void Open();

protected:
    // FAssetEditorToolkit interface
    virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager) override;
    virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager) override;
    virtual FText GetToolkitName() const override;
    virtual FText GetToolkitToolTipText() const override;
    virtual FLinearColor GetWorldCentricTabColorScale() const override;
    virtual FText GetBaseToolkitName() const override;
    virtual FName GetToolkitFName() const override;
    virtual FString GetWorldCentricTabPrefix() const override;
    virtual void InitToolMenuContext(FToolMenuContext& MenuContext) override;

private:
    TSharedRef<FTabManager::FLayout> CreateLayout();
    void ExtendToolbar();
    TSharedRef<SDockTab> SpawnColorsTab(const FSpawnTabArgs& Args);

    void AddEntry(UClass* iEntryClass);
    FGuid GetCurrentSet() const;
    void OnCurrentSetSelected(FGuid iSet);
    FReply OnRefreshClicked();

    void BuildToolbarPaletteSection(FToolBarBuilder& iBuilder);

private:
    FName mColorsTabId;
    UOdysseyPalette* mPalette;
    FGuid mCurrentSet;

    TSharedPtr<SButton> mRefreshButton;
    TSharedPtr<SOdysseyPaletteTreeView> mTreeView;

private:
    // Telemetry
    FDateTime SessionStartTime;
};
