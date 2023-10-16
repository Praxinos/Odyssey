// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationEditorExtension;
class FOdysseyAnimationCell;
class UOdysseyAnimationLayer;

/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageTimeline
    : public SCompoundWidget
{
public:
    // Construction / Destruction
    virtual ~SOdysseyAnimationLayerImageTimeline();
    SOdysseyAnimationLayerImageTimeline();

protected:
    void Construct(
        FOdysseyAnimationEditorExtension* iExtension,
        UOdysseyAnimationLayer* iLayer
    );

public:
    virtual FReply OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent) override;

protected:
    virtual TSharedRef<FOdysseyAnimationCell> OnCreateCell() = 0;
    virtual TSharedRef<SWidget> OnGenerateCellWidget(TSharedPtr<FOdysseyAnimationCell> iCell) = 0;

private:
    FInt32Range GetSelectableFrames() const;
    FInt32Range GetSelectedFrames() const;
    void OnFramesSelectionChanged(FInt32Range iSelectedFrames);
    void OnFramesSelectionEnded(int iFrame);

private:
    //Context Menu
    void SelectAllFrames();
    void DeleteSelectedFrames();
    void CopyFrames();
    void CutFrames();
    void PasteFrames();
    void BuildContextMenu(FMenuBuilder& iMenuBuilder, int iFrame);
    void MapActions(TSharedPtr<FUICommandList> iCommandList);

protected:
    FOdysseyAnimationEditorExtension* mExtension;
    UOdysseyAnimationLayer* mLayer;
	
	TSharedRef<FUICommandList> mCommandList;
};
