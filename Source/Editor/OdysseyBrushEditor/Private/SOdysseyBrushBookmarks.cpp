// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SOdysseyBrushBookmarks.h"
#include "OdysseyBrushEditor.h"
#include "BlueprintEditorSettings.h"
#include "EditorStyleSet.h"
#include "EdGraphNode_Comment.h"
#include "Framework/Commands/GenericCommands.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Widgets/Views/SExpanderArrow.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushBookmarks"

void SOdysseyBrushBookmarks::Construct(const FArguments& InArgs)
{
    EditorContext = InArgs._EditorContext;

    BookmarksRootNode.DisplayName = LOCTEXT("BookmarksRootNodeLabel", "Bookmarks");
    CommentsRootNode.DisplayName = LOCTEXT("CommentsRootNodeLabel", "Comment Nodes");

    CommandList = MakeShareable(new FUICommandList);
    CommandList->MapAction(FGenericCommands::Get().Delete,
        FUIAction(
            FExecuteAction::CreateSP(this, &SOdysseyBrushBookmarks::OnDeleteSelectedTreeViewItems),
            FCanExecuteAction::CreateSP(this, &SOdysseyBrushBookmarks::CanDeleteSelectedTreeViewItems)
        )
    );
    CommandList->MapAction(FGenericCommands::Get().Rename,
        FUIAction(
            FExecuteAction::CreateSP(this, &SOdysseyBrushBookmarks::OnRenameSelectedTreeViewItems),
            FCanExecuteAction::CreateSP(this, &SOdysseyBrushBookmarks::CanRenameSelectedTreeViewItem)
        )
    );

    FMenuBuilder ViewOptions(true, nullptr);

    ViewOptions.AddMenuEntry(
        LOCTEXT("ShowCommentNodes", "Show Comment Blocks"),
        LOCTEXT("ShowCommentNodesTooltip", "Should comment blocks be shown in the tree view?"),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateSP(this, &SOdysseyBrushBookmarks::OnToggleShowCommentNodes),
            FCanExecuteAction(),
            FIsActionChecked::CreateSP(this, &SOdysseyBrushBookmarks::IsShowCommentNodesChecked)
        ),
        NAME_None,
        EUserInterfaceActionType::ToggleButton);

    ViewOptions.AddMenuEntry(
        LOCTEXT("ShowBookmarksForCurrentDocumentOnly", "Show Bookmarks for Current Graph Only"),
        LOCTEXT("ShowBookmarksForCurrentDocumentOnlyTooltip", "Only show bookmarks for the current graph."),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateSP(this, &SOdysseyBrushBookmarks::OnToggleShowBookmarksForCurrentDocumentOnly),
            FCanExecuteAction(),
            FIsActionChecked::CreateSP(this, &SOdysseyBrushBookmarks::IsShowBookmarksForCurrentDocumentOnlyChecked)
        ),
        NAME_None,
        EUserInterfaceActionType::ToggleButton);

    SAssignNew(SearchBoxWidget, SSearchBox)
    .SelectAllTextWhenFocused(true)
    .OnTextCommitted(this, &SOdysseyBrushBookmarks::OnFilterTextCommitted)
    .OnTextChanged(this, &SOdysseyBrushBookmarks::OnFilterTextCommitted, ETextCommit::Default);

    SAssignNew(TreeViewWidget, STreeView<FTreeViewItemPtr>)
    .TreeItemsSource(&TreeViewRootItems)
    .OnGenerateRow(this, &SOdysseyBrushBookmarks::OnGenerateTreeViewRow)
    .OnGetChildren(this, &SOdysseyBrushBookmarks::OnGetTreeViewChildren)
    .OnContextMenuOpening(this, &SOdysseyBrushBookmarks::OnOpenTreeViewContextMenu)
    .OnMouseButtonDoubleClick(this, &SOdysseyBrushBookmarks::OnTreeViewItemDoubleClick)
    .HeaderRow
    (
        SNew(SHeaderRow)
        +SHeaderRow::Column("Name")
        .DefaultLabel(LOCTEXT("NameColumnHeader", "Name"))
        .FillWidth(0.6f)
        +SHeaderRow::Column("Graph")
        .DefaultLabel(LOCTEXT("GraphColumnHeader", "Graph"))
        .FillWidth(0.4f)
    );

    ChildSlot
    [
        SNew(SVerticalBox)
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SBorder)
            .Padding(2.f)
            .BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
            [
                SNew(SHorizontalBox)
                +SHorizontalBox::Slot()
                .FillWidth(1.f)
                .Padding(2.f, 0.f)
                .VAlign(VAlign_Center)
                [
                    SearchBoxWidget.ToSharedRef()
                ]
                +SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(2.f, 0.f)
                [
                    SNew(SButton)
                    .Visibility(EVisibility::Collapsed) // @TODO: remove once folders are supported
                    .ButtonStyle(FEditorStyle::Get(), "FlatButton")
                    [
                        SNew(SImage)
                        .Image(FEditorStyle::Get().GetBrush("Bookmarks.AddFolderButtonIcon"))
                    ]
                ]
                +SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(2.f, 0.f)
                [
                    SNew(SComboButton)
                    .ComboButtonStyle(FEditorStyle::Get(), "ToolbarComboButton")
                    .ForegroundColor(FSlateColor::UseForeground())
                    .HasDownArrow(true)
                    .ContentPadding(FMargin(1, 0))
                    .MenuContent()
                    [
                        ViewOptions.MakeWidget()
                    ]
                    .ButtonContent()
                    [
                        SNew(SImage)
                        .Image(FEditorStyle::GetBrush("GenericViewButton"))
                    ]
                ]
            ]
        ]
        +SVerticalBox::Slot()
        .FillHeight(1.f)
        .Padding(0.f, 2.0f, 0.f, 0.f)
        [
            SNew(SBorder)
            .Padding(2.f)
            .BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
            [
                TreeViewWidget.ToSharedRef()
            ]
        ]
    ];

    RefreshBookmarksTree();
}

void SOdysseyBrushBookmarks::STreeItemRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, TWeakPtr<FOdysseyBrushEditor> InEditorContext)
{
    ItemPtr = InArgs._ItemPtr;
    check(ItemPtr.IsValid());

    EditorContext = InEditorContext;
    SMultiColumnTableRow<FTreeViewItemPtr>::Construct(FSuperRowType::FArguments(), InOwnerTableView);
}

TSharedRef<SWidget> SOdysseyBrushBookmarks::STreeItemRow::GenerateWidgetForColumn(const FName& InColumnName)
{
    if (InColumnName == "Name")
    {
        TSharedPtr<SWidget> TextWidget;

        if (ItemPtr->IsBookmarkNode())
        {
            TSharedPtr<SInlineEditableTextBlock> InlineEditableTextWidget = SNew(SInlineEditableTextBlock)
                .Text(this, &SOdysseyBrushBookmarks::STreeItemRow::GetItemNameText)
                .IsSelected(this, &STreeItemRow::IsSelectedExclusively)
                .OnTextCommitted(this, &STreeItemRow::OnNameTextCommitted);

            ItemPtr->OnRequestRenameDelegate = FTreeViewItem::FOnRequestRename::CreateSP(InlineEditableTextWidget.Get(), &SInlineEditableTextBlock::EnterEditingMode);

            TextWidget = InlineEditableTextWidget;
        }
        else
        {
            TextWidget = SNew(STextBlock)
                .Text(this, &SOdysseyBrushBookmarks::STreeItemRow::GetItemNameText)
                .Font(FEditorStyle::GetFontStyle(ItemPtr->IsRootNode() ? "Bookmarks.TreeViewRootItemFont" : "Bookmarks.TreeViewItemFont"));
        }

        return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SExpanderArrow, SharedThis(this))
        ]
        + SHorizontalBox::Slot()
        .FillWidth(1.f)
        [
            TextWidget.ToSharedRef()
        ];
    }
    else if (InColumnName == "Graph")
    {
        const UEdGraph* GraphContext = nullptr;

        if (ItemPtr->IsBookmarkNode())
        {
            if (ensure(ItemPtr->DocumentInfo))
            {
                GraphContext = Cast<UEdGraph>(ItemPtr->DocumentInfo->EditedObjectPath.ResolveObject());
            }
        }
        else if (ItemPtr->IsCommentNode())
        {
            if (ensure(ItemPtr->DocumentInfo))
            {
                if (const UEdGraphNode* NodeContext = Cast<UEdGraphNode>(ItemPtr->DocumentInfo->EditedObjectPath.ResolveObject()))
                {
                    GraphContext = NodeContext->GetGraph();
                }
            }
        }

        if (GraphContext)
        {
            return SNew(STextBlock)
                .Text_Lambda([GraphContext]() { return FText::FromName(GraphContext->GetFName()); })
                .Font(FEditorStyle::GetFontStyle("Bookmarks.TreeViewItemFont"));
        }
    }

    return SNullWidget::NullWidget;
}

FText SOdysseyBrushBookmarks::STreeItemRow::GetItemNameText() const
{
    return ItemPtr->BookmarkNode.DisplayName;
}

void SOdysseyBrushBookmarks::STreeItemRow::OnNameTextCommitted(const FText& InNewName, ETextCommit::Type InTextCommit)
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditor = EditorContext.Pin();
    if (OdysseyBrushEditor.IsValid())
    {
        UBlueprint* OdysseyBrush = OdysseyBrushEditor->GetBlueprintObj();
        check(OdysseyBrush);

        OdysseyBrush->Modify();
        ItemPtr->BookmarkNode.DisplayName = InNewName;
    }
}

void SOdysseyBrushBookmarks::OnFilterTextCommitted(const FText& InText, ETextCommit::Type CommitType)
{
    FilterText = InText;

    RefreshBookmarksTree();
}

void SOdysseyBrushBookmarks::OnDeleteSelectedTreeViewItems()
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditor = EditorContext.Pin();
    if (OdysseyBrushEditor.IsValid())
    {
        for(FTreeViewItemPtr SelectedItem : TreeViewWidget->GetSelectedItems())
        {
            // The item contains a reference to the node in the source array, so use a local copy of the GUID since this will use RemoveSwap().
            const FGuid NodeGuid = SelectedItem->BookmarkNode.NodeGuid;
            OdysseyBrushEditor->RemoveBookmark(NodeGuid, false);
        }

        RefreshBookmarksTree();
    }
}

bool SOdysseyBrushBookmarks::CanDeleteSelectedTreeViewItems() const
{
    TArray<FTreeViewItemPtr> SelectedItems = TreeViewWidget->GetSelectedItems();
    bool bCanDelete = SelectedItems.Num() > 0;
    if(bCanDelete)
    {
        for (FTreeViewItemPtr SelectedItem : SelectedItems)
        {
            if (!SelectedItem->IsBookmarkNode())
            {
                bCanDelete = false;
                break;
            }
        }
    }

    return bCanDelete;
}

void SOdysseyBrushBookmarks::OnRenameSelectedTreeViewItems()
{
    TArray<FTreeViewItemPtr> SelectedItems = TreeViewWidget->GetSelectedItems();
    if (SelectedItems.Num() == 1)
    {
        SelectedItems[0]->OnRequestRenameDelegate.ExecuteIfBound();
    }
}

bool SOdysseyBrushBookmarks::CanRenameSelectedTreeViewItem() const
{
    TArray<FTreeViewItemPtr> SelectedItems = TreeViewWidget->GetSelectedItems();
    return SelectedItems.Num() == 1 && SelectedItems[0]->IsBookmarkNode();
}

TSharedRef<ITableRow> SOdysseyBrushBookmarks::OnGenerateTreeViewRow(FTreeViewItemPtr TreeItem, const TSharedRef<STableViewBase>& OwnerTable)
{
    check(TreeItem.IsValid());

    return SNew(STreeItemRow, OwnerTable, EditorContext)
        .ItemPtr(TreeItem);
}

void SOdysseyBrushBookmarks::OnGetTreeViewChildren(FTreeViewItemPtr TreeItem, TArray<FTreeViewItemPtr>& OutChildren)
{
    OutChildren += TreeItem->Children;
}

bool SOdysseyBrushBookmarks::CanNavigateToSelection() const
{
    TArray<FTreeViewItemPtr> SelectedItems = TreeViewWidget->GetSelectedItems();
    bool bCanNavigate = SelectedItems.Num() > 0;
    if (bCanNavigate)
    {
        for (FTreeViewItemPtr SelectedItem : SelectedItems)
        {
            if (SelectedItem->DocumentInfo == nullptr)
            {
                bCanNavigate = false;
                break;
            }
        }
    }

    return bCanNavigate;
}

void SOdysseyBrushBookmarks::OnTreeViewItemDoubleClick(FTreeViewItemPtr TreeItem)
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditor = EditorContext.Pin();
    if (OdysseyBrushEditor.IsValid() && TreeItem->DocumentInfo)
    {
        OdysseyBrushEditor->JumpToHyperlink(TreeItem->DocumentInfo->EditedObjectPath.ResolveObject());

        if (TreeItem->IsBookmarkNode())
        {
            FGuid CurrentBookmarkId;
            OdysseyBrushEditor->GetViewBookmark(CurrentBookmarkId);
            if (CurrentBookmarkId != TreeItem->BookmarkNode.NodeGuid)
            {
                OdysseyBrushEditor->SetViewLocation(TreeItem->DocumentInfo->SavedViewOffset, TreeItem->DocumentInfo->SavedZoomAmount, TreeItem->BookmarkNode.NodeGuid);
            }
        }
    }
}

TSharedPtr<SWidget> SOdysseyBrushBookmarks::OnOpenTreeViewContextMenu()
{
    TArray<FTreeViewItemPtr> SelectedItems = TreeViewWidget->GetSelectedItems();
    if (SelectedItems.Num() > 0)
    {
        FMenuBuilder ContextMenuBuilder(true, CommandList);

        ContextMenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);
        ContextMenuBuilder.AddMenuEntry(FGenericCommands::Get().Rename);
        ContextMenuBuilder.AddMenuEntry(LOCTEXT("ContextMenuItem_JumpToBookmark", "Jump to Bookmark"),
            LOCTEXT("ContextMenuItemTooltip_JumpToBookmark", "Navigate to this bookmark's location."),
            FSlateIcon(),
            FUIAction(
                FExecuteAction::CreateSP(this, &SOdysseyBrushBookmarks::OnTreeViewItemDoubleClick, SelectedItems[0]),
                FCanExecuteAction::CreateSP(this, &SOdysseyBrushBookmarks::CanNavigateToSelection)
            )
        );

        return ContextMenuBuilder.MakeWidget();
    }

    return SNullWidget::NullWidget;
}

void SOdysseyBrushBookmarks::RefreshBookmarksTree()
{
    CommentNodes.Empty();
    CommentNodeInfo.Empty();
    TreeViewRootItems.Empty();

    UBlueprintEditorSettings* BlueprintEditorSettings = GetMutableDefault<UBlueprintEditorSettings>();

    FTreeViewItemPtr BookmarksTreeViewRoot = MakeShareable(new FTreeViewItem(ETreeViewNodeType::Root, BookmarksRootNode));
    TreeViewRootItems.Add(BookmarksTreeViewRoot);

    TreeViewWidget->SetItemExpansion(BookmarksTreeViewRoot, true);

    FTreeViewItemPtr CommentsTreeViewRoot;
    if (BlueprintEditorSettings->bIncludeCommentNodesInBookmarksTab)
    {
        CommentsTreeViewRoot = MakeShareable(new FTreeViewItem(ETreeViewNodeType::Root, CommentsRootNode));
        TreeViewRootItems.Add(CommentsTreeViewRoot);
    }

    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditor = EditorContext.Pin();
    if (OdysseyBrushEditor.IsValid())
    {
        UBlueprint* OdysseyBrush = OdysseyBrushEditor->GetBlueprintObj();
        check(OdysseyBrush);

        const UEdGraph* FocusedGraph = OdysseyBrushEditor->GetFocusedGraph();
        const bool bCurrentGraphOnly = BlueprintEditorSettings->bShowBookmarksForCurrentDocumentOnlyInTab;

        const FText& LocalFilterText = FilterText;
        auto IsNodeFilteredOutLambda = [LocalFilterText](const FBPEditorBookmarkNode& InNode)
        {
            return !LocalFilterText.IsEmpty() && !InNode.DisplayName.ToString().Contains(LocalFilterText.ToString());
        };

        for (FBPEditorBookmarkNode& BookmarkNode : BlueprintEditorSettings->BookmarkNodes)
        {
            if (!IsNodeFilteredOutLambda(BookmarkNode))
            {
                if (const FEditedDocumentInfo* BookmarkInfo = BlueprintEditorSettings->Bookmarks.Find(BookmarkNode.NodeGuid))
                {
                    if (const UEdGraph* GraphContext = Cast<UEdGraph>(BookmarkInfo->EditedObjectPath.ResolveObject()))
                    {
                        if (OdysseyBrush == FBlueprintEditorUtils::FindBlueprintForGraph(GraphContext))
                        {
                            if (!bCurrentGraphOnly || (GraphContext == FocusedGraph))
                            {
                                BookmarksTreeViewRoot->Children.Add(MakeShareable(new FTreeViewItem(ETreeViewNodeType::LocalBookmark, BookmarkNode, BookmarkInfo)));
                            }
                        }
                    }
                }
            }
        }

        for (FBPEditorBookmarkNode& BookmarkNode : OdysseyBrush->BookmarkNodes)
        {
            if (!IsNodeFilteredOutLambda(BookmarkNode))
            {
                const FEditedDocumentInfo* BookmarkInfo = OdysseyBrush->Bookmarks.Find(BookmarkNode.NodeGuid);
                if (!bCurrentGraphOnly || (BookmarkInfo && BookmarkInfo->EditedObjectPath.ResolveObject() == FocusedGraph))
                {
                    BookmarksTreeViewRoot->Children.Add(MakeShareable(new FTreeViewItem(ETreeViewNodeType::SharedBookmark, BookmarkNode, BookmarkInfo)));
                }
            }
        }

        if (CommentsTreeViewRoot.IsValid())
        {
            TArray<UEdGraph*> AllGraphs;
            OdysseyBrush->GetAllGraphs(AllGraphs);

            TArray<UEdGraphNode_Comment*> GraphCommentNodes;
            for (UEdGraph* Graph : AllGraphs)
            {
                if (!bCurrentGraphOnly || Graph == FocusedGraph)
                {
                    Graph->GetNodesOfClass(GraphCommentNodes);
                }
            }

            if (GraphCommentNodes.Num() > 0)
            {
                CommentNodes.Reserve(GraphCommentNodes.Num());
                CommentNodeInfo.Reserve(GraphCommentNodes.Num());

                for (UEdGraphNode_Comment* GraphCommentNode : GraphCommentNodes)
                {
                    FBPEditorBookmarkNode& CommentNode = *new(CommentNodes) FBPEditorBookmarkNode;
                    CommentNode.NodeGuid = FGuid::NewGuid();
                    CommentNode.DisplayName = GraphCommentNode->GetNodeTitle(ENodeTitleType::EditableTitle);

                    FEditedDocumentInfo& CommentInfo = CommentNodeInfo.Add(CommentNode.NodeGuid);
                    CommentInfo.EditedObjectPath = GraphCommentNode;

                    if (!IsNodeFilteredOutLambda(CommentNode))
                    {
                        CommentsTreeViewRoot->Children.Add(MakeShareable(new FTreeViewItem(ETreeViewNodeType::Comment, CommentNode, &CommentInfo)));
                    }
                }
            }

            TreeViewWidget->SetItemExpansion(CommentsTreeViewRoot, true);
        }
    }

    TreeViewWidget->RequestTreeRefresh();
}

bool SOdysseyBrushBookmarks::IsShowCommentNodesChecked() const
{
    return GetDefault<UBlueprintEditorSettings>()->bIncludeCommentNodesInBookmarksTab;
}

void SOdysseyBrushBookmarks::OnToggleShowCommentNodes()
{
    UBlueprintEditorSettings* Settings = GetMutableDefault<UBlueprintEditorSettings>();
    Settings->bIncludeCommentNodesInBookmarksTab = !Settings->bIncludeCommentNodesInBookmarksTab;
    Settings->PostEditChange();
    Settings->SaveConfig();

    RefreshBookmarksTree();
}

bool SOdysseyBrushBookmarks::IsShowBookmarksForCurrentDocumentOnlyChecked() const
{
    return GetDefault<UBlueprintEditorSettings>()->bShowBookmarksForCurrentDocumentOnlyInTab;
}

void SOdysseyBrushBookmarks::OnToggleShowBookmarksForCurrentDocumentOnly()
{
    UBlueprintEditorSettings* Settings = GetMutableDefault<UBlueprintEditorSettings>();
    Settings->bShowBookmarksForCurrentDocumentOnlyInTab = !Settings->bShowBookmarksForCurrentDocumentOnlyInTab;
    Settings->PostEditChange();
    Settings->SaveConfig();

    RefreshBookmarksTree();
}

#undef LOCTEXT_NAMESPACE
