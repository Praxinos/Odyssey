// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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
class ODYSSEYPALETTE_API FOdysseyPaletteEditorToolkit
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
    int GetCurrentSet() const;
    void OnCurrentSetSelected(int iSet);

    void BuildToolbarPaletteSection(FToolBarBuilder& iBuilder);

private:
    FName mColorsTabId;
    UOdysseyPalette* mPalette;
    int mCurrentSet;

    TSharedPtr<SOdysseyPaletteTreeView> mTreeView;
};
