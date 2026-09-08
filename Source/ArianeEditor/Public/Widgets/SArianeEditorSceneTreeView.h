// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STreeView.h"

class FArianeEditor;
struct FArianeGroup;
struct FArianeObject;
class FSceneTreeViewItem;
class UArianeLayer;
class UArianeLayerDrawing;
class UArianeLayerStack;
class FUICommandList;

#define VSTV_OBJECT_VISIBLE     "Visible"
#define VSTV_OBJECT_HUDCOLOR    "HUD Color"
#define VSTV_OBJECT_TRANSFORMED "Transform"
#define VSTV_OBJECT_NAME        "Name"
#define VSTV_OBJECT_TAGS        "Tags"

/**
 * Implements the Scene Tree View Widget
 */
class ARIANEEDITOR_API SArianeEditorSceneTreeView
    : public STreeView<TSharedPtr<FSceneTreeViewItem>>
{
    //SLATE_DECLARE_WIDGET(SArianeEditorSceneTreeView, STreeView<TSharedPtr<FSceneTreeViewItem>>)

    public:
/*
        SLATE_BEGIN_ARGS(SArianeEditorSceneTreeView)
        : _Editor(nullptr)
            {}
            SLATE_ARGUMENT( FArianeEditor*, Editor )
        SLATE_END_ARGS()
*/

    public:
        // Construction / Destruction
        ~SArianeEditorSceneTreeView();
        SArianeEditorSceneTreeView();

        void Construct( const FArguments& InArgs, FArianeEditor* InEditor );

        void Update();

        void RenameSelectedItem();

        TSharedPtr<FSceneTreeViewItem> GetRootItem();

        FArianeEditor* GetEditor() const;
        void BindComponentDelegates();
        void UnbindComponentDelegates();

    protected:
        /**
         * @brief Called when the treeview asks for the children of a specific item
         *
         * @param iParent
         * @param oChildren
         */
        void OnGetChildren(TSharedPtr<FSceneTreeViewItem> iParent, TArray<TSharedPtr<FSceneTreeViewItem>>& oChildren) const;
        TSharedPtr<SWidget> OnContextMenuOpening();
        TSharedRef<ITableRow> OnGenerateRow( TSharedPtr<FSceneTreeViewItem> iItem, const TSharedRef<STableViewBase>& iOwnerTable );
        void OnSelectionChanged( TSharedPtr<FSceneTreeViewItem> iItem, ESelectInfo::Type SelectInfo );
        void OnExpansionChanged( TSharedPtr<FSceneTreeViewItem> iItem, bool mExpanded );
        void ExpandTree( const TSharedPtr<FSceneTreeViewItem> iItem );
        void BuildTree( const TSharedPtr<FSceneTreeViewItem> iItem );
        //void SelectTree( const TSharedPtr<FVectorSceneTreeViewItem> iItem );

        void MapActionsToCommandList();

        // actions
        void CopyObjects();
        void PasteObjects();
        void DeleteObjects();
        void SelectAll();
        void CutObjects();

        FReply OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent );

        void OnSceneChanged();
        void OnCurrentLayerChanged();
        void OnSourceChanged();

        void OnPrePainting3DComponentUpdate( bool bInteractive );
        void OnPostPainting3DComponentUpdate( bool bInteractive );
        FArianeGroup* GetRootGroup();

        void OnPostLayerStackHierarchyChanged();
        void OnPreLayerStackHierarchyChanged();
        void OnPostLayerStackSelectionChanged();
        void OnPreLayerStackSelectionChanged();
        void OnPre3DPaintingComponentSelectionChanged();
        void OnPost3DPaintingComponentSelectionChanged();
        void OnPostImageChanged();

    protected:
        FArianeEditor* Editor;
        TSharedPtr<FSceneTreeViewItem> RootItem;
        TArray<TSharedPtr<FSceneTreeViewItem>> ItemsSource;
        TSharedRef<FUICommandList> CommandList;
        bool bDoUpdate;
};
