// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Engine/Blueprint.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STreeView.h"

class FOdysseyBrushEditor;
class FUICommandList;
class SSearchBox;

class SOdysseyBrushBookmarks : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyBrushBookmarks)
    {}
        SLATE_ARGUMENT(TSharedPtr<FOdysseyBrushEditor>, EditorContext)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    void RefreshBookmarksTree();

protected:
    enum class ETreeViewNodeType
    {
        Root,
        Folder,
        Comment,
        LocalBookmark,
        SharedBookmark
    };

    struct FTreeViewItem : public TSharedFromThis<FTreeViewItem>
    {
        DECLARE_DELEGATE(FOnRequestRename);

        ETreeViewNodeType NodeType;
        FBPEditorBookmarkNode& BookmarkNode;
        const FEditedDocumentInfo* DocumentInfo;
        TArray<TSharedPtr<FTreeViewItem>> Children;
        FOnRequestRename OnRequestRenameDelegate;

        FTreeViewItem(ETreeViewNodeType InNodeType, FBPEditorBookmarkNode& InBookmarkNode, const FEditedDocumentInfo* InDocumentInfo = nullptr)
            :NodeType(InNodeType)
            ,BookmarkNode(InBookmarkNode)
            ,DocumentInfo(InDocumentInfo)
        {}

        FORCEINLINE bool IsRootNode() const
        {
            return NodeType == ETreeViewNodeType::Root;
        }

        FORCEINLINE bool IsBookmarkNode() const
        {
            return NodeType == ETreeViewNodeType::LocalBookmark || NodeType == ETreeViewNodeType::SharedBookmark;
        }

        FORCEINLINE bool IsCommentNode() const
        {
            return NodeType == ETreeViewNodeType::Comment;
        }

        FORCEINLINE bool IsFolderNode() const
        {
            return NodeType == ETreeViewNodeType::Folder;
        }
    };

    typedef TSharedPtr<FTreeViewItem> FTreeViewItemPtr;

    class STreeItemRow : public SMultiColumnTableRow<FTreeViewItemPtr>
    {
    public:
        SLATE_BEGIN_ARGS(STreeItemRow) {}
            SLATE_ARGUMENT(FTreeViewItemPtr, ItemPtr)
        SLATE_END_ARGS()

        void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, TWeakPtr<FOdysseyBrushEditor> InEditorContext);
        virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& InColumnName) override;

    protected:
        FText GetItemNameText() const;
        void OnNameTextCommitted(const FText& InNewName, ETextCommit::Type InTextCommit);

    private:
        FTreeViewItemPtr ItemPtr;
        TWeakPtr<FOdysseyBrushEditor> EditorContext;
    };

    void OnFilterTextCommitted(const FText& InText, ETextCommit::Type CommitType);

    void OnDeleteSelectedTreeViewItems();
    bool CanDeleteSelectedTreeViewItems() const;
    void OnRenameSelectedTreeViewItems();
    bool CanRenameSelectedTreeViewItem() const;

    bool CanNavigateToSelection() const;
    TSharedRef<ITableRow> OnGenerateTreeViewRow(FTreeViewItemPtr TreeItem, const TSharedRef<STableViewBase>& OwnerTable);
    void OnGetTreeViewChildren(FTreeViewItemPtr TreeItem, TArray<FTreeViewItemPtr>& OutChildren);
    void OnTreeViewItemDoubleClick(FTreeViewItemPtr TreeItem);
    TSharedPtr<SWidget> OnOpenTreeViewContextMenu();

    bool IsShowCommentNodesChecked() const;
    void OnToggleShowCommentNodes();

    bool IsShowBookmarksForCurrentDocumentOnlyChecked() const;
    void OnToggleShowBookmarksForCurrentDocumentOnly();

private:
    TWeakPtr<FOdysseyBrushEditor> EditorContext;

    FBPEditorBookmarkNode CommentsRootNode;
    FBPEditorBookmarkNode BookmarksRootNode;
    TArray<FBPEditorBookmarkNode> CommentNodes;
    TMap<FGuid, FEditedDocumentInfo> CommentNodeInfo;

    TArray<FTreeViewItemPtr> TreeViewRootItems;

    TSharedPtr<FUICommandList> CommandList;
    TSharedPtr<SSearchBox> SearchBoxWidget;
    TSharedPtr<STreeView<FTreeViewItemPtr> > TreeViewWidget;

    FText FilterText;
};
