// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class FOdysseyPainterEditor;
class FOdysseyVectorGroupPaint;
class FVectorSceneTreeViewItem;
class FOdysseyVectorLayer;
class UOdysseyLayerStack;
struct FOdysseyVectorObjectInvalidationFlags;

#define VSTV_OBJECT_VISIBLE     "Visible"
#define VSTV_OBJECT_HUD_COLOR   "HUD Color"
#define VSTV_OBJECT_NAME        "Name"
#define VSTV_OBJECT_TRANSFORMED "Transformed"
#define VSTV_OBJECT_TAGS        "Tags"

/**
 * Implements the Scene Tree View Widget
 */
class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorVectorSceneTreeView
    : public STreeView<TSharedPtr<FVectorSceneTreeViewItem>>
{
    SLATE_DECLARE_WIDGET(SOdysseyPainterEditorVectorSceneTreeView, STreeView<TSharedPtr<FVectorSceneTreeViewItem>>)

    public:
        SLATE_BEGIN_ARGS(SOdysseyPainterEditorVectorSceneTreeView)
        : _Editor(nullptr)
            {}
            SLATE_ARGUMENT( FOdysseyPainterEditor*, Editor )
            SLATE_ATTRIBUTE(FOdysseyVectorGroupPaint*, Scene)
        SLATE_END_ARGS()

    public:
        // Construction / Destruction
        ~SOdysseyPainterEditorVectorSceneTreeView();
        SOdysseyPainterEditorVectorSceneTreeView();

        void Construct(const FArguments& InArgs );

        void Update();

        void RenameSelectedItem();

        TSharedPtr<FVectorSceneTreeViewItem> GetRootItem();

        FOdysseyPainterEditor* GetEditor() const;
        void UnbindLayerDelegates();
        void BindLayerDelegates();

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
/*
        virtual void Private_SetItemSelection ( TSharedPtr<FVectorSceneTreeViewItem> iItem
                                              , bool bShouldBeSelected
                                              , bool bWasUserDirected ) override;
        virtual void Private_ClearSelection() override;
        virtual void Private_SelectRangeFromCurrentTo ( TSharedPtr<FVectorSceneTreeViewItem> iItem ) override;
*/
        void MapActionsToCommandList();

        // actions
        void CopyObjects();
        void PasteObjects();
        void DeleteObjects();
        void SelectAll();
        void CutObjects();

        FReply OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent );

        void OnVectorLayerNotify( const FOdysseyVectorObjectInvalidationFlags& iSignalFlags
                                , uint32 iUpdateFlags );
        void ParseVectorNotifications( const FOdysseyVectorObjectInvalidationFlags& iSignalFlags );
        void OnSceneChanged();
        void OnCurrentLayerChanged( UOdysseyLayerStack* iLayerStack );
        void OnSourceChanged();
        void UpdateCurrentLayer( UOdysseyLayerStack* iLayerStack );

    protected:
        FOdysseyPainterEditor* mEditor;
        TSlateAttribute<FOdysseyVectorGroupPaint*> mScene;
        TSharedPtr<FOdysseyVectorLayer> mVectorLayer;

        TSharedPtr<FVectorSceneTreeViewItem> mRootItem;
        TArray<TSharedPtr<FVectorSceneTreeViewItem>> mItemsSource;
        TSharedRef<FUICommandList> mCommandList;
};
