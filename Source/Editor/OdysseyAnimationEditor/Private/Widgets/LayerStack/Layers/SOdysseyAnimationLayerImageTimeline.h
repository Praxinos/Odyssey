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
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
    virtual FReply OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent) override;
    virtual void OnDragEnter(const FGeometry& iGeometry, const FDragDropEvent& iEvent) override;
    virtual void OnDragLeave(const FDragDropEvent& iEvent) override;
    virtual FReply OnDragOver(const FGeometry& iGeometry, const FDragDropEvent& iEvent) override;
    virtual FReply OnDrop(const FGeometry& iGeometry, const FDragDropEvent& iEvent) override;

protected:
    virtual TSharedRef<FOdysseyAnimationCell> OnCreateCell() = 0;
    virtual TSharedRef<SWidget> OnGenerateCellWidget(TSharedPtr<FOdysseyAnimationCell> iCell) = 0;

private:
    FInt32Range GetSelectableFrames() const;
    FInt32Range GetSelectedFrames() const;
    void OnFramesSelectionChanged(FInt32Range iSelectedFrames);
    void OnFramesSelectionEnded(int iFrame);
    FReply OnFramesSelectionDragged();

private:
    //Context Menu
    void BuildContextMenu(FMenuBuilder& iMenuBuilder);
    void MapActions(TSharedPtr<FUICommandList> iCommandList, int iFrame);
    void BuildPostBehaviourSubMenu(FMenuBuilder& iMenuBuilder);
    void BuildPreBehaviourSubMenu(FMenuBuilder& iMenuBuilder);

    void SelectAllFrames();
    void DeleteSelectedFrames();
    void CopyFrames();
    void CutFrames();
    void PasteFrames();
    void StaggerCell(int iFrame);

    bool CanStaggerCell(int iFrame) const;

    void SetPostBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour);
    bool IsPostBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour) const;
    bool CanSetPostBehaviour() const;

    void SetPreBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour);
    bool IsPreBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour) const;
    bool CanSetPreBehaviour() const;

protected:
    FOdysseyAnimationEditorExtension* mExtension;
    UOdysseyAnimationLayer* mLayer;

    enum eDragState
    {
        kDrag_None, //The drop will do nothin
        kDrag_Move, //The drop will move the selected cells
        kDrag_Copy, //The drop will copy the selected cells
    };

    bool mIsDraggingOver;
    eDragState mDragState;
    int mDragPosition;
};
