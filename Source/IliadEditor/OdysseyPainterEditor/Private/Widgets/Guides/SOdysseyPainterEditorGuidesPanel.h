// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "EditorUndoClient.h"
#include "Input/DragAndDrop.h"
#include "OdysseyPainterEditorGuideCollection.h"
#include "Templates/SharedPointer.h"

class FOdysseyPainterEditor;
class IDetailsView;
class SListViewBase;
template<typename ItemType> class SListView;

/** Drag operation used to reorder guides in the panel. */
class FGuideReorderDragDropOp : public FDragDropOperation
{
public:
    DRAG_DROP_OPERATOR_TYPE(FGuideReorderDragDropOp, FDragDropOperation)
    using FDragDropOperation::Construct;

    FGuideReorderDragDropOp(TWeakObjectPtr<UOdysseyPainterEditorGuide> InGuide);

    TWeakObjectPtr<UOdysseyPainterEditorGuide> Guide;
};

/** Slate panel for creating, ordering, selecting, and editing an asset's visual guides. */
class SOdysseyPainterEditorGuidesPanel : public SCompoundWidget, public FSelfRegisteringEditorUndoClient
{
public:
    SLATE_BEGIN_ARGS(SOdysseyPainterEditorGuidesPanel) {}
        /** Painter editor whose current asset owns the guide collection. */
        SLATE_ARGUMENT(FOdysseyPainterEditor*, Editor)
    SLATE_END_ARGS()

    /** Builds the guide list, Add Guide menu, viewport-edit button, and property details. */
    void Construct(const FArguments& Args);
    /** Reloads the list from the current asset's guide collection. */
    void Refresh();

    //FEditorUndoClient overrides
    virtual void PostUndo(bool bSuccess) override;
    virtual void PostRedo(bool bSuccess) override;

private:
    /** Called when the editor switches to another texture or animation. */
    void OnSourceChanged();
    UOdysseyPainterEditorGuideCollection* GetCollection(bool bCreateIfMissing);
    TSharedRef<SWidget> MakeAddMenu();
    TSharedRef<ITableRow> MakeGuideRow(TWeakObjectPtr<UOdysseyPainterEditorGuide> Guide, const TSharedRef<STableViewBase>& OwnerTable);
    TOptional<EItemDropZone> CanDropGuide(const FDragDropEvent& Event, EItemDropZone Zone, TWeakObjectPtr<UOdysseyPainterEditorGuide> TargetGuide);
    FReply DropGuide(const FDragDropEvent& Event, EItemDropZone Zone, TWeakObjectPtr<UOdysseyPainterEditorGuide> TargetGuide);
    FReply BeginGuideDrag(const FGeometry& Geometry, const FPointerEvent& Event, TWeakObjectPtr<UOdysseyPainterEditorGuide> Guide);
    void AddGuide(UClass* GuideClass);
    void SelectGuide(TWeakObjectPtr<UOdysseyPainterEditorGuide> Guide, ESelectInfo::Type SelectInfo);
    FReply ToggleVisibility(UOdysseyPainterEditorGuide* Guide);
    FReply ToggleLock(UOdysseyPainterEditorGuide* Guide);
    FReply DeleteGuide(UOdysseyPainterEditorGuide* Guide);
    FReply ToggleViewportEditing();
    FText GetGuideName(UOdysseyPainterEditorGuide* Guide) const;
    const FSlateBrush* GetVisibilityBrush(UOdysseyPainterEditorGuide* Guide) const;
    const FSlateBrush* GetLockBrush(UOdysseyPainterEditorGuide* Guide) const;
    const FSlateBrush* GetTypeBrush(UOdysseyPainterEditorGuide* Guide) const;
    void RenameGuide(const FText& NewName, ETextCommit::Type CommitType, UOdysseyPainterEditorGuide* Guide);
    FText GetEditButtonText() const;

private:
    FOdysseyPainterEditor* mEditor = nullptr;
    TWeakObjectPtr<UOdysseyPainterEditorGuideCollection> mCollection;
    TArray<TWeakObjectPtr<UOdysseyPainterEditorGuide>> mGuideItems;
    TSharedPtr<SListView<TWeakObjectPtr<UOdysseyPainterEditorGuide>>> mListView;
    TSharedPtr<IDetailsView> mDetailsView;
    TWeakObjectPtr<UOdysseyPainterEditorGuide> mSelectedGuide;
};
