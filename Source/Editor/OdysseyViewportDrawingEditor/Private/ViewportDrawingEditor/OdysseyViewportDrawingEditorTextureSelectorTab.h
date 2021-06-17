// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyEditorTab.h"
#include "Widgets/SBoxPanel.h"
#include <ULIS3>

class FOdysseyViewportDrawingEditor;

class ODYSSEYVIEWPORTDRAWINGEDITOR_API FOdysseyViewportDrawingEditorTextureSelectorTab :
	public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyViewportDrawingEditorTextureSelectorTab();
    FOdysseyViewportDrawingEditorTextureSelectorTab(FOdysseyViewportDrawingEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    TSharedRef<SWidget> OnGetMenuContent();
    void OnMenuClosed( bool iOpen );
    TSharedRef<SWidget> GenerateMeshSelectorComboButtonItem( TSharedPtr<FString> iItem );
    FText CreateTextMeshSelector() const;
    TSharedRef<SWidget> CreateMeshComponentMenuWidget();

protected:
    // Widget Getters
    FString PaintTexturePath() const;
    FString PaintMaterialPath() const;
    bool ShouldFilterTextureAsset(const FAssetData& iAssetData);
    bool ShouldFilterMaterialAsset(const FAssetData& iAssetData) const;

protected:
    // Event Listeners
    void OnTextureChanged(const FAssetData& iAssetData);
    void OnMaterialChanged(const FAssetData& iAssetData);
    FReply OnMeshComponentChanged(const FString iName);
    ECheckBoxState GetMeshComponentCheckState(const FString iName);

private:
    FOdysseyViewportDrawingEditor* mEditor;
    TSharedPtr<SVerticalBox> mMeshSelectorVerticalBox;
    TSharedPtr<SComboButton> mMeshSelectComboButton;
};

