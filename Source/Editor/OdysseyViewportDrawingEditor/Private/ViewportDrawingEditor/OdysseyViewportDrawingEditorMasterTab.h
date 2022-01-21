// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyEditorTab.h"
#include "Widgets/SBoxPanel.h"
#include <ULIS>

class FOdysseyViewportDrawingEditor;

class ODYSSEYVIEWPORTDRAWINGEDITOR_API FOdysseyViewportDrawingEditorMasterTab :
	public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyViewportDrawingEditorMasterTab();
    FOdysseyViewportDrawingEditorMasterTab(FOdysseyViewportDrawingEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    TSharedRef<SWidget> OnGetMenuContent();
    void OnMenuClosed( bool iOpen );
    TSharedRef<SWidget> GenerateMeshSelectorComboButtonItem( TSharedPtr<FString> iItem );
    FText CreateTextMeshSelector() const;
    TSharedRef<SWidget> CreateMeshComponentMenuWidget();
    
    TSharedRef<SWidget> GeneratePaintingMethodComboBoxItem(TSharedPtr<EOdysseyViewportDrawingPaintingAdapterMethod> iItem);
    void ChangeSelectionPaintingMethodComboBoxItem(TSharedPtr<EOdysseyViewportDrawingPaintingAdapterMethod> iNewSelection, ESelectInfo::Type iSelectInfo);
    static FText GetMethodAsText(EOdysseyViewportDrawingPaintingAdapterMethod iMethod);


protected:
    // Widget Getters
    FString PaintActorPath() const;
    FString PaintMaterialPath() const;
    FString PaintTexturePath() const;
    bool ShouldFilterMaterialAsset(const FAssetData& iAssetData) const;
    bool ShouldFilterTextureAsset(const FAssetData& iAssetData) const;

protected:
    // Event Listeners
    void OnActorChanged(const FAssetData& iAssetData);
    FReply OnMeshComponentChanged(const FString iName);
    void OnMaterialChanged(const FAssetData& iAssetData);
    void OnTextureChanged(const FAssetData& iAssetData);

private:
    FOdysseyViewportDrawingEditor* mEditor;
    TSharedPtr<SVerticalBox> mMeshSelectorVerticalBox;
    TSharedPtr<SComboButton> mMeshSelectComboButton;
    TArray<TSharedPtr<EOdysseyViewportDrawingPaintingAdapterMethod>> mOptions;

    TSharedPtr<FAssetThumbnailPool> mThumbnailPool;
};

