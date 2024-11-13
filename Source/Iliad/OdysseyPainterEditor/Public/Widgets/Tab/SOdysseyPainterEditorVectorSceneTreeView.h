// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/STreeView.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeViewRow.h"
#include "OdysseyVectorGroupPaint.h"

class FOdysseyPainterEditor;

/**
 * Implements the Scene Tree View Widget
 */
class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorVectorSceneTreeView
    : public STreeView<TSharedPtr<FVectorSceneTreeViewItem>>
{
    public:
        SLATE_BEGIN_ARGS(SOdysseyPainterEditorVectorSceneTreeView)
            {}
        SLATE_END_ARGS()

    public:
        // Construction / Destruction
        ~SOdysseyPainterEditorVectorSceneTreeView();
        SOdysseyPainterEditorVectorSceneTreeView();

        void Construct(const FArguments& InArgs, FOdysseyPainterEditor* iEditor);

        void Update( FOdysseyVectorGroupPaint* iScene );

        void RenameSelectedItem();

        TSharedPtr<FVectorSceneTreeViewItem> GetRootItem();

        FOdysseyPainterEditor* GetEditor() const;

    protected:
        /**
         * @brief Called when the treeview asks for the children of a specific item
         *
         * @param iParent
         * @param oChildren
         */
        void OnGetChildren(TSharedPtr<FVectorSceneTreeViewItem> iParent, TArray<TSharedPtr<FVectorSceneTreeViewItem>>& oChildren) const;
        TSharedPtr<SWidget> OnContextMenuOpening();
        TSharedRef<ITableRow> OnGenerateRow( TSharedPtr<FVectorSceneTreeViewItem> iItem, const TSharedRef<STableViewBase>& iOwnerTable );
        void OnSelectionChanged( TSharedPtr<FVectorSceneTreeViewItem> iItem, ESelectInfo::Type SelectInfo );
        void OnExpansionChanged( TSharedPtr<FVectorSceneTreeViewItem> iItem, bool mExpanded );
        void ExpandTree( const TSharedPtr<FVectorSceneTreeViewItem> iItem );
        void BuildTree( const TSharedPtr<FVectorSceneTreeViewItem> iItem );
        //void SelectTree( const TSharedPtr<FVectorSceneTreeViewItem> iItem );
        //FReply OnDragOver( const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent );
        virtual bool Private_IsItemSelected( const TSharedPtr<FVectorSceneTreeViewItem>& iItem ) const override;

        virtual void Private_SetItemSelection ( TSharedPtr<FVectorSceneTreeViewItem> iItem
                                              , bool bShouldBeSelected
                                              , bool bWasUserDirected ) override;
        virtual void Private_ClearSelection() override;
        virtual void Private_SelectRangeFromCurrentTo ( TSharedPtr<FVectorSceneTreeViewItem> iItem ) override;
        void MapActionsToCommandList();

        // actions
        void CopyObjects();
        void PasteObjects();
        void DeleteObjects();
        void SelectAll();
        void CutObjects();

        FReply OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent );

    protected:
        FOdysseyPainterEditor* mEditor;
        TSharedPtr<FVectorSceneTreeViewItem> mRootItem;
        TArray<TSharedPtr<FVectorSceneTreeViewItem>> mItemsSource;
        TSharedRef<FUICommandList> mCommandList;
};
