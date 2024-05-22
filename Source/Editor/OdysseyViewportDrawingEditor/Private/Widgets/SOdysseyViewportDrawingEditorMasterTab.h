// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "ViewportDrawingEditor/OdysseyViewportDrawingEditorExtension.h"

class FOdysseyViewportDrawingEditorExtension;

class ODYSSEYVIEWPORTDRAWINGEDITOR_API SOdysseyViewportDrawingEditorMasterTab
	: public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SOdysseyViewportDrawingEditorMasterTab)
		{}
	SLATE_END_ARGS()

	// Construct the widget
	void Construct(const FArguments& InArgs, FOdysseyViewportDrawingEditorExtension* iExtension);

protected:
    // FOdysseyEditorTab interface
    TSharedRef<SWidget> OnGetMenuContent();
    void OnMenuClosed( bool iOpen );
    FText CreateTextMeshSelector() const;
    TSharedRef<SWidget> CreateMeshComponentMenuWidget();
    
    TSharedRef<SWidget> GeneratePaintingMethodComboBoxItem(TSharedPtr<EOdysseyViewportDrawingPaintingAdapterMethod> iItem);
    void ChangeSelectionPaintingMethodComboBoxItem(TSharedPtr<EOdysseyViewportDrawingPaintingAdapterMethod> iNewSelection, ESelectInfo::Type iSelectInfo);
    static FText GetMethodAsText(EOdysseyViewportDrawingPaintingAdapterMethod iMethod);
    static FText GetTooltipAsText(EOdysseyViewportDrawingPaintingAdapterMethod iMethod);


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
    FOdysseyViewportDrawingEditorExtension* mExtension;
    TSharedPtr<SVerticalBox> mMeshSelectorVerticalBox;
    TSharedPtr<SComboButton> mMeshSelectComboButton;
    TArray<TSharedPtr<EOdysseyViewportDrawingPaintingAdapterMethod>> mOptions;

    TSharedPtr<FAssetThumbnailPool> mThumbnailPool;
};

