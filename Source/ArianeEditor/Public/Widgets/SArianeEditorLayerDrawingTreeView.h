// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#ifdef unused

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STreeView.h"

#include <list>

struct FArianeEditor;
struct FArianeGroup;
struct FArianeObject;
class FLayerDrawingTreeViewItem;
class UArianeLayer;
class UArianeLayerDrawing;
class UArianeLayerStack;
class FUICommandList;

#define VSTV_OBJECT_VISIBLE     "Visible"
#define VSTV_OBJECT_HUDCOLOR    "HUD Color"
#define VSTV_OBJECT_NAME        "Name"
#define VSTV_OBJECT_TRANSFORMED "Transformed"
#define VSTV_OBJECT_TAGS        "Tags"

/**
 * Implements the Scene Tree View Widget
 */
class ARIANEEDITOR_API SArianeEditorLayerDrawingTreeView
    : public STreeView<TSharedPtr<FLayerDrawingTreeViewItem>>
{
    SLATE_DECLARE_WIDGET(SArianeEditorLayerDrawingTreeView, STreeView<TSharedPtr<FLayerDrawingTreeViewItem>>)

    public:
        SLATE_BEGIN_ARGS(SArianeEditorLayerDrawingTreeView)
        : _Editor(nullptr)
            {}
            SLATE_ARGUMENT( FArianeEditor*, Editor )
            SLATE_ATTRIBUTE(FArianeGroup*, Scene)
        SLATE_END_ARGS()

    public:
        // Construction / Destruction
        ~SArianeEditorLayerDrawingTreeView();
        SArianeEditorLayerDrawingTreeView();

        void Construct(const FArguments& InArgs );

        void Update();

        void RenameSelectedItem();

        TSharedPtr<FLayerDrawingTreeViewItem> GetRootItem();

        FArianeEditor* GetEditor() const;
        void UnbindLayerDelegates();
        void BindLayerDelegates();

    protected:
        /**
         * @brief Called when the treeview asks for the children of a specific item
         *
         * @param iParent
         * @param oChildren
         */
        void OnGetChildren(TSharedPtr<FLayerDrawingTreeViewItem> iParent, TArray<TSharedPtr<FLayerDrawingTreeViewItem>>& oChildren) const;
        TSharedPtr<SWidget> OnContextMenuOpening();
        TSharedRef<ITableRow> OnGenerateRow( TSharedPtr<FLayerDrawingTreeViewItem> iItem, const TSharedRef<STableViewBase>& iOwnerTable );
        void OnSelectionChanged( TSharedPtr<FLayerDrawingTreeViewItem> iItem, ESelectInfo::Type SelectInfo );
        void OnExpansionChanged( TSharedPtr<FLayerDrawingTreeViewItem> iItem, bool mExpanded );
        void ExpandTree( const TSharedPtr<FLayerDrawingTreeViewItem> iItem );
        void BuildTree( const TSharedPtr<FLayerDrawingTreeViewItem> iItem );
        //void SelectTree( const TSharedPtr<FVectorSceneTreeViewItem> iItem );

        void MapActionsToCommandList();

        // actions
        void CopyObjects();
        void PasteObjects();
        void DeleteObjects();
        void SelectAll();
        void CutObjects();

        FReply OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent );

        void OnVectorLayerNotify( bool bInteractive );
        void ParseVectorNotifications( bool bInteractive );
        void OnSceneChanged();
        void OnCurrentLayerChanged();
        void OnSourceChanged();
        void UpdateCurrentLayer();

    protected:
        FArianeEditor* Editor;
        TSlateAttribute<FArianeObject*> Root;
        UArianeLayerDrawing* DrawingLayer;

        TSharedPtr<FLayerDrawingTreeViewItem> RootItem;
        TArray<TSharedPtr<FLayerDrawingTreeViewItem>> ItemsSource;
        //TSharedRef<FUICommandList> CommandList;
};

#endif
