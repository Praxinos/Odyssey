// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationEditorExtension;
class FOdysseyAnimationCell;
class UOdysseyAnimationLayerImageRaster;

/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageRasterTimeline
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageRasterTimeline)
        {}
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyAnimationLayerImageRasterTimeline();
    SOdysseyAnimationLayerImageRasterTimeline();
    void Construct(
        const FArguments& iArgs, 
        FOdysseyAnimationEditorExtension* iExtension,
        UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
    );

public:
    virtual FReply OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent) override;
    virtual FReply OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent ) override;
    
    virtual bool SupportsKeyboardFocus() const override;

private:
    TSharedRef<FOdysseyAnimationCell> OnCreateCell();
    TSharedRef<SWidget> OnCreateCellWidget(TSharedPtr<FOdysseyAnimationCell> iCell);
    EVisibility GetFrameSelectorVisibility() const;
    void OnBuildCellsContextMenu(FMenuBuilder& iMenuBuilder, int iFrame);
    void OnBuildFrameSelectorContextMenu(FMenuBuilder& iMenuBuilder, int iFrame);

    void SelectAllFrames();
    void DeleteSelectedFrames();
    void CopyFrames();
    void CutFrames();
    void PasteFrames();
    void BuildContextMenu(FMenuBuilder& iMenuBuilder, int iFrame);
    void MapActions(TSharedPtr<FUICommandList> iCommandList);

private:
    FOdysseyAnimationEditorExtension* mExtension;
    UOdysseyAnimationLayerImageRaster* mAnimationLayerImageRaster;
	
	TSharedRef<FUICommandList> mCommandList;
};
