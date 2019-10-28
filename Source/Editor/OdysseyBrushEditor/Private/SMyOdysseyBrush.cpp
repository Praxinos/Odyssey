// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.


#include "SMyOdysseyBrush.h"
#include "UObject/UObjectHash.h"
#include "UObject/UObjectIterator.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboButton.h"
#include "Settings/EditorExperimentalSettings.h"
#include "Engine/MemberReference.h"
#include "Components/TimelineComponent.h"
#include "Engine/TimelineTemplate.h"
#include "Dialogs/Dialogs.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EdGraphSchema_OdysseyBrush.h"
#include "K2Node_AddComponent.h"
#include "K2Node_Event.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Tunnel.h"
#include "K2Node_Composite.h"
#include "K2Node_CreateDelegate.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_EventNodeInterface.h"
#include "ScopedTransaction.h"

#include "DetailLayoutBuilder.h"

#include "SOdysseyInspector.h"
#include "SOdysseySCSEditor.h"
#include "GraphEditorDragDropAction.h"
#include "ODFunctionDragDropAction.h"
#include "ODVariableDragDropAction.h"
#include "ODDelegateDragDropAction.h"
#include "SOdysseyBrushPalette.h"
#include "OdysseyBrushEditorCommands.h"
#include "GraphEditorActions.h"

#include "AnimationGraph.h"


#include "SOdysseyBrushEditorToolbar.h"

#include "Kismet2/BlueprintEditorUtils.h"
#include "ObjectEditorUtils.h"
#include "GraphEditor/Private/GraphActionNode.h"
#include "SourceCodeNavigation.h"
#include "EditorCategoryUtils.h"
#include "Widgets/Input/SSearchBox.h"
#include "Framework/Commands/GenericCommands.h"

#include "BlueprintEditorSettings.h"
#include "SOdysseyReplaceNodeReferences.h"

#define LOCTEXT_NAMESPACE "MyOdysseyBrush"

//////////////////////////////////////////////////////////////////////////

void FMyOdysseyBrushCommands::RegisterCommands()
{
    UI_COMMAND( OpenGraph, "Open Graph", "Opens up this function, macro, or event graph's graph panel up.", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( OpenGraphInNewTab, "Open in New Tab", "Opens up this function, macro, or event graph's graph panel up in a new tab. Hold down Ctrl and double click for shortcut.", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( FocusNode, "Focus", "Focuses on the associated node", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( FocusNodeInNewTab, "Focus in New Tab", "Focuses on the associated node in a new tab", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( ImplementFunction, "Implement Function", "Implements this overridable function as a new function.", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND(DeleteEntry, "Delete", "Deletes this function or variable from this blueprint.", EUserInterfaceActionType::Button, FInputChord(EKeys::Platform_Delete));
    UI_COMMAND( GotoNativeVarDefinition, "Goto Code Definition", "Goto the native code definition of this variable", EUserInterfaceActionType::Button, FInputChord() );
}

//////////////////////////////////////////////////////////////////////////

class FMyOdysseyBrushCategoryDragDropAction : public FGraphEditorDragDropAction
{
public:
    DRAG_DROP_OPERATOR_TYPE(FMyOdysseyBrushCategoryDragDropAction, FGraphEditorDragDropAction)

    virtual void HoverTargetChanged() override
    {
        const FSlateBrush* StatusSymbol = FEditorStyle::GetBrush(TEXT("NoBrush"));
        FText Message = DraggedCategory;

        FFormatNamedArguments Args;
        Args.Add(TEXT("DraggedCategory"), DraggedCategory);

        if (!HoveredCategoryName.IsEmpty())
        {
            if(HoveredCategoryName.EqualTo(DraggedCategory))
            {
                StatusSymbol = FEditorStyle::GetBrush(TEXT("Graph.ConnectorFeedback.Error"));


                Message = FText::Format( LOCTEXT("MoveCatOverSelf", "Cannot insert category '{DraggedCategory}' before itself."), Args );
            }
            else
            {
                StatusSymbol = FEditorStyle::GetBrush(TEXT("Graph.ConnectorFeedback.OK"));
                Args.Add(TEXT("HoveredCategory"), HoveredCategoryName);
                Message = FText::Format( LOCTEXT("MoveCatOK", "Move category '{DraggedCategory}' before '{HoveredCategory}'"), Args );
            }
        }
        else if (HoveredAction.IsValid())
        {
            StatusSymbol = FEditorStyle::GetBrush(TEXT("Graph.ConnectorFeedback.Error"));
            Message = LOCTEXT("MoveCatOverAction", "Can only insert before another category.");
        }
        else
        {
            StatusSymbol = FEditorStyle::GetBrush(TEXT("Graph.ConnectorFeedback.Error"));
            Message = FText::Format(LOCTEXT("MoveCatAction", "Moving category '{DraggedCategory}'"), Args);
        }

        SetSimpleFeedbackMessage(StatusSymbol, FLinearColor::White, Message);
    }

    virtual FReply DroppedOnCategory(FText OnCategory) override
    {
        // Get MyOdysseyBrush via MyOdysseyBrushPtr
        TSharedPtr<SMyOdysseyBrush> MyOdysseyBrush = MyOdysseyBrushPtr.Pin();
        if(MyOdysseyBrush.IsValid())
        {
            // Move the category in the blueprint category sort list
            MyOdysseyBrush->MoveCategoryBeforeCategory( DraggedCategory, OnCategory );
        }

        return FReply::Handled();
    }

    static TSharedRef<FMyOdysseyBrushCategoryDragDropAction> New(const FText& InCategory, TSharedPtr<SMyOdysseyBrush> InMyOdysseyBrush)
    {
        TSharedRef<FMyOdysseyBrushCategoryDragDropAction> Operation = MakeShareable(new FMyOdysseyBrushCategoryDragDropAction);
        Operation->DraggedCategory = InCategory;
        Operation->MyOdysseyBrushPtr = InMyOdysseyBrush;
        Operation->Construct();
        return Operation;
    }

    /** Category we were dragging */
    FText DraggedCategory;
    /** MyOdysseyBrush widget we dragged from */
    TWeakPtr<SMyOdysseyBrush>    MyOdysseyBrushPtr;
};

//////////////////////////////////////////////////////////////////////////
// FGraphActionSort

// Helper structure to aid category sorting
struct FGraphActionSort
{
public:
    FGraphActionSort(TArray<FName>& OdysseyBrushCategorySorting)
        : bCategoriesModified(false)
        , CategorySortIndices(OdysseyBrushCategorySorting)
    {
        CategoryUsage.Init(0, CategorySortIndices.Num());
    }

    void AddAction(const FString& Category, TSharedPtr<FEdGraphSchemaAction> Action)
    {
        // Find root category
        int32 RootCategoryDelim = Category.Find(TEXT("|"));
        FName RootCategory = RootCategoryDelim == INDEX_NONE ? *Category : *Category.Left(RootCategoryDelim);
        // Get root sort index
        const int32 SortIndex = GetSortIndex(RootCategory) + Action->GetSectionID();

        SortedActions.Add(SortIndex, Action);
    }

    void AddAction(TSharedPtr<FEdGraphSchemaAction> Action)
    {
        const FString UserCategoryName = FEditorCategoryUtils::GetCategoryDisplayString(Action->GetCategory().ToString());
        AddAction(UserCategoryName, Action);
    }

    void GetAllActions(FGraphActionListBuilderBase& OutActions)
    {
        SortedActions.KeySort(TLess<int32>());

        for (const auto& Iter : SortedActions)
        {
            OutActions.AddAction(Iter.Value);
        }
    }

    void CleanupCategories()
    {
        // Scrub unused categories from the blueprint
        if (bCategoriesModified)
        {
            for (int32 CategoryIdx = CategoryUsage.Num() - 1; CategoryIdx >= 0; CategoryIdx--)
            {
                if (CategoryUsage[CategoryIdx] == 0)
                {
                    CategorySortIndices.RemoveAt(CategoryIdx);
                }
            }
            bCategoriesModified = false;
        }
    }

private:
    const int32 GetSortIndex(FName Category)
    {
        int32 SortIndex = CategorySortIndices.Find(Category);

        if (SortIndex == INDEX_NONE)
        {
            bCategoriesModified = true;
            SortIndex = CategorySortIndices.Add(Category);
            CategoryUsage.Add(0);
        }
        CategoryUsage[SortIndex]++;
        // Spread the sort values so we can fine tune sorting
        SortIndex *= 1000;

        return SortIndex + SortedActions.Num();
    }

private:
    /** Signals if the blueprint categories have been modified and require cleanup */
    bool bCategoriesModified;
    /** Tracks category usage to aid removal of unused categories */
    TArray<int32> CategoryUsage;
    /** Reference to the category sorting in the blueprint */
    TArray<FName>& CategorySortIndices;
    /** Map used to sort Graph actions */
    TMultiMap<int32, TSharedPtr<FEdGraphSchemaAction>> SortedActions;
};

//////////////////////////////////////////////////////////////////////////

void SMyOdysseyBrush::Construct(const FArguments& InArgs, TWeakPtr<FOdysseyBrushEditor> InOdysseyBrushEditor, const UBlueprint* InOdysseyBrush )
{
    bNeedsRefresh = false;
    bShowReplicatedVariablesOnly = false;

    OdysseyBrushEditorPtr = InOdysseyBrushEditor;
    EdGraph = nullptr;

    TSharedPtr<SWidget> ToolbarBuilderWidget = TSharedPtr<SWidget>();

    if( InOdysseyBrushEditor.IsValid() )
    {
        OdysseyBrush = OdysseyBrushEditorPtr.Pin()->GetBlueprintObj();

        TSharedPtr<FUICommandList> ToolKitCommandList = InOdysseyBrushEditor.Pin()->GetToolkitCommands();

        ToolKitCommandList->MapAction( FMyOdysseyBrushCommands::Get().OpenGraph,
            FExecuteAction::CreateSP(this, &SMyOdysseyBrush::OnOpenGraph),
            FCanExecuteAction(), FIsActionChecked(),
            FIsActionButtonVisible::CreateSP(this, &SMyOdysseyBrush::CanOpenGraph) );

        ToolKitCommandList->MapAction( FMyOdysseyBrushCommands::Get().OpenGraphInNewTab,
            FExecuteAction::CreateSP(this, &SMyOdysseyBrush::OnOpenGraphInNewTab),
            FCanExecuteAction(), FIsActionChecked(),
            FIsActionButtonVisible::CreateSP(this, &SMyOdysseyBrush::CanOpenGraph) );

        ToolKitCommandList->MapAction( FMyOdysseyBrushCommands::Get().FocusNode,
            FExecuteAction::CreateSP(this, &SMyOdysseyBrush::OnFocusNode),
            FCanExecuteAction(), FIsActionChecked(),
            FIsActionButtonVisible::CreateSP(this, &SMyOdysseyBrush::CanFocusOnNode) );

        ToolKitCommandList->MapAction( FMyOdysseyBrushCommands::Get().FocusNodeInNewTab,
            FExecuteAction::CreateSP(this, &SMyOdysseyBrush::OnFocusNodeInNewTab),
            FCanExecuteAction(), FIsActionChecked(),
            FIsActionButtonVisible::CreateSP(this, &SMyOdysseyBrush::CanFocusOnNode) );

        ToolKitCommandList->MapAction( FMyOdysseyBrushCommands::Get().ImplementFunction,
            FExecuteAction::CreateSP(this, &SMyOdysseyBrush::OnImplementFunction),
            FCanExecuteAction(), FIsActionChecked(),
            FIsActionButtonVisible::CreateSP(this, &SMyOdysseyBrush::CanImplementFunction) );

        ToolKitCommandList->MapAction( FGraphEditorCommands::Get().FindReferences,
            FExecuteAction::CreateSP(this, &SMyOdysseyBrush::OnFindReference),
            FCanExecuteAction(),
            FIsActionChecked(),
            FIsActionButtonVisible::CreateSP(this, &SMyOdysseyBrush::CanFindReference) );

        ToolKitCommandList->MapAction( FGraphEditorCommands::Get().FindAndReplaceReferences,
            FExecuteAction::CreateSP(this, &SMyOdysseyBrush::OnFindAndReplaceReference),
            FCanExecuteAction(),
            FIsActionChecked(),
            FIsActionButtonVisible::CreateSP(this, &SMyOdysseyBrush::CanFindAndReplaceReference) );

        ToolKitCommandList->MapAction( FMyOdysseyBrushCommands::Get().DeleteEntry,
            FExecuteAction::CreateSP(this, &SMyOdysseyBrush::OnDeleteEntry),
            FCanExecuteAction::CreateSP(this, &SMyOdysseyBrush::CanDeleteEntry) );

        ToolKitCommandList->MapAction( FGenericCommands::Get().Duplicate,
            FExecuteAction::CreateSP(this, &SMyOdysseyBrush::OnDuplicateAction),
            FCanExecuteAction::CreateSP(this, &SMyOdysseyBrush::CanDuplicateAction),
            FIsActionChecked(),
            FIsActionButtonVisible::CreateSP(this, &SMyOdysseyBrush::IsDuplicateActionVisible) );

        ToolKitCommandList->MapAction( FMyOdysseyBrushCommands::Get().GotoNativeVarDefinition,
            FExecuteAction::CreateSP(this, &SMyOdysseyBrush::GotoNativeCodeVarDefinition),
            FCanExecuteAction(),
            FIsActionChecked(),
            FIsActionButtonVisible::CreateSP(this, &SMyOdysseyBrush::IsNativeVariable) );

        TSharedPtr<FOdysseyBrushEditorToolbar> Toolbar = MakeShareable(new FOdysseyBrushEditorToolbar(InOdysseyBrushEditor.Pin()));
        TSharedPtr<FExtender> Extender = MakeShareable(new FExtender);
        Toolbar->AddNewToolbar(Extender);
        ToolbarBuilderWidget = SNullWidget::NullWidget;

        ToolKitCommandList->MapAction(FGenericCommands::Get().Rename,
            FExecuteAction::CreateSP(this, &SMyOdysseyBrush::OnRequestRenameOnActionNode),
            FCanExecuteAction::CreateSP(this, &SMyOdysseyBrush::CanRequestRenameOnActionNode));
    }
    else
    {
        // we're in read only mode when there's no blueprint editor:
        OdysseyBrush = const_cast<UBlueprint*>(InOdysseyBrush);
        check(OdysseyBrush);
        ToolbarBuilderWidget = SNew(SBox);
    }

    TSharedPtr<SWidget> AddNewMenu = SNullWidget::NullWidget;

    AddNewMenu = SNew(SComboButton)
        .ComboButtonStyle(FEditorStyle::Get(), "ToolbarComboButton")
        .ButtonStyle(FEditorStyle::Get(), "FlatButton.Success")
        .ForegroundColor(FLinearColor::White)
        .ToolTipText(LOCTEXT("AddNewToolTip", "Add a new Variable, Graph, Function, Macro, or Event Dispatcher."))
        .OnGetMenuContent(this, &SMyOdysseyBrush::CreateAddNewMenuWidget)
        .HasDownArrow(true)
        .ContentPadding(FMargin(1, 0, 2, 0))
        .AddMetaData<FTagMetaData>(FTagMetaData(TEXT("MyOdysseyBrushAddNewCombo")))
        .IsEnabled(this, &SMyOdysseyBrush::IsEditingMode)
        .ButtonContent()
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(FMargin(0, 1))
        [
            SNew(SImage)
            .Image(FEditorStyle::GetBrush("Plus"))
        ]

    + SHorizontalBox::Slot()
        .VAlign(VAlign_Center)
        .AutoWidth()
        .Padding(FMargin(2, 0, 2, 0))
        [
            SNew(STextBlock)
            .Text(LOCTEXT("AddNew", "Add New"))
        ]
        ];

    FMenuBuilder ViewOptions(true, nullptr);

    ViewOptions.AddMenuEntry(
        LOCTEXT("ShowInheritedVariables", "Show Inherited Variables"),
        LOCTEXT("ShowInheritedVariablesTooltip", "Should inherited variables from parent classes and blueprints be shown in the tree?"),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateSP( this, &SMyOdysseyBrush::OnToggleShowInheritedVariables ),
            FCanExecuteAction(),
            FIsActionChecked::CreateSP( this, &SMyOdysseyBrush::IsShowingInheritedVariables )
        ),
        NAME_None,
        EUserInterfaceActionType::ToggleButton,
        TEXT("MyOdysseyBrush_ShowInheritedVariables")
    );

    ViewOptions.AddMenuEntry(
        LOCTEXT("ShowEmptySections", "Show Empty Sections"),
        LOCTEXT("ShowEmptySectionsTooltip", "Should we show empty sections? eg. Graphs, Functions...etc."),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateSP( this, &SMyOdysseyBrush::OnToggleShowEmptySections ),
            FCanExecuteAction(),
            FIsActionChecked::CreateSP(this, &SMyOdysseyBrush::IsShowingEmptySections)
        ),
        NAME_None,
        EUserInterfaceActionType::ToggleButton,
        TEXT("MyOdysseyBrush_ShowEmptySections")
    );

    ViewOptions.AddMenuEntry(
        LOCTEXT("ShowReplicatedVariablesOnly", "Show Replicated Variables Only"),
        LOCTEXT("ShowReplicatedVariablesOnlyTooltip", "Should we only show variables that are replicated?"),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateSP(this, &SMyOdysseyBrush::OnToggleShowReplicatedVariablesOnly),
            FCanExecuteAction(),
            FIsActionChecked::CreateSP(this, &SMyOdysseyBrush::IsShowingReplicatedVariablesOnly)
        ),
        NAME_None,
        EUserInterfaceActionType::ToggleButton,
        TEXT("MyOdysseyBrush_ShowReplicatedVariablesOnly")
    );

    SAssignNew(FilterBox, SSearchBox)
        .OnTextChanged( this, &SMyOdysseyBrush::OnFilterTextChanged );

    // create the main action list piece of this widget
    SAssignNew(GraphActionMenu, SGraphActionMenu, false)
        .OnGetFilterText(this, &SMyOdysseyBrush::GetFilterText)
        .OnCreateWidgetForAction(this, &SMyOdysseyBrush::OnCreateWidgetForAction)
        .OnCollectAllActions(this, &SMyOdysseyBrush::CollectAllActions)
        .OnCollectStaticSections(this, &SMyOdysseyBrush::CollectStaticSections)
        .OnActionDragged(this, &SMyOdysseyBrush::OnActionDragged)
        .OnCategoryDragged(this, &SMyOdysseyBrush::OnCategoryDragged)
        .OnActionSelected(this, &SMyOdysseyBrush::OnGlobalActionSelected)
        .OnActionDoubleClicked(this, &SMyOdysseyBrush::OnActionDoubleClicked)
        .OnContextMenuOpening(this, &SMyOdysseyBrush::OnContextMenuOpening)
        .OnCategoryTextCommitted(this, &SMyOdysseyBrush::OnCategoryNameCommitted)
        .OnCanRenameSelectedAction(this, &SMyOdysseyBrush::CanRequestRenameOnActionNode)
        .OnGetSectionTitle(this, &SMyOdysseyBrush::OnGetSectionTitle)
        .OnGetSectionWidget(this, &SMyOdysseyBrush::OnGetSectionWidget)
        .AlphaSortItems(false)
        .UseSectionStyling(true);


    // now piece together all the content for this widget
    ChildSlot
    [
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SBorder)
            .Padding(4.0f)
            .BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
            .AddMetaData<FTagMetaData>(FTagMetaData(TEXT("MyOdysseyBrushPanel")))
            [
                SNew(SVerticalBox)

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    ToolbarBuilderWidget.ToSharedRef()
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SHorizontalBox)

                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(0, 0, 2, 0)
                    [
                        AddNewMenu.ToSharedRef()
                    ]

                    + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    .VAlign(VAlign_Center)
                    [
                        FilterBox.ToSharedRef()
                    ]

                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(2, 0, 0, 0)
                    [
                        SNew(SComboButton)
                        .ComboButtonStyle(FEditorStyle::Get(), "ToolbarComboButton")
                        .ForegroundColor(FSlateColor::UseForeground())
                        .HasDownArrow(true)
                        .ContentPadding(FMargin(1, 0))
                        .AddMetaData<FTagMetaData>(FTagMetaData(TEXT("ViewOptions")))
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
        ]

        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        [
            GraphActionMenu.ToSharedRef()
        ]
    ];

    ResetLastPinType();

    if( !OdysseyBrushEditorPtr.IsValid() )
    {
        Refresh();
    }

    TMap<int32, bool> ExpandedSections;
    ExpandedSections.Add(NodeSectionID::VARIABLE, true);
    ExpandedSections.Add(NodeSectionID::FUNCTION, true);
    ExpandedSections.Add(NodeSectionID::MACRO, true);
    // Avoid showing delegates
    ExpandedSections.Add(NodeSectionID::DELEGATE, false);
    ExpandedSections.Add(NodeSectionID::GRAPH, true);
    ExpandedSections.Add(NodeSectionID::LOCAL_VARIABLE, true);

    GraphActionMenu->SetSectionExpansion(ExpandedSections);

    FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this, &SMyOdysseyBrush::OnObjectPropertyChanged);
}

SMyOdysseyBrush::~SMyOdysseyBrush()
{
    FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);
}

void SMyOdysseyBrush::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

    if(bNeedsRefresh)
    {
        Refresh();
    }
}

void SMyOdysseyBrush::OnCategoryNameCommitted(const FText& InNewText, ETextCommit::Type InTextCommit, TWeakPtr< FGraphActionNode > InAction )
{
    // Remove excess whitespace and prevent categories with just spaces
    FText CategoryName = FText::TrimPrecedingAndTrailing(InNewText);

    TArray<TSharedPtr<FEdGraphSchemaAction>> Actions;
    GraphActionMenu->GetCategorySubActions(InAction, Actions);

    if (Actions.Num())
    {
        const FScopedTransaction Transaction( LOCTEXT( "RenameCategory", "Rename Category" ) );

        GetBlueprintObj()->Modify();

        for (int32 i = 0; i < Actions.Num(); ++i)
        {
            if (Actions[i]->GetTypeId() == FEdGraphSchemaAction_K2Var::StaticGetTypeId())
            {
                FEdGraphSchemaAction_K2Var* VarAction = (FEdGraphSchemaAction_K2Var*)Actions[i].Get();

                if(UProperty* TargetProperty = VarAction->GetProperty())
                {
                    UClass* OuterClass = CastChecked<UClass>(VarAction->GetProperty()->GetOuter());
                    const bool bIsNativeVar = (OuterClass->ClassGeneratedBy == NULL);

                    // If the variable is not native and it's outer is the skeleton generated class, we can rename the category
                    if(!bIsNativeVar && OuterClass == GetBlueprintObj()->SkeletonGeneratedClass)
                    {
                        FBlueprintEditorUtils::SetBlueprintVariableCategory(GetBlueprintObj(), VarAction->GetVariableName(), NULL, CategoryName, true);
                    }
                }
            }
            else if (Actions[i]->GetTypeId() == FEdGraphSchemaAction_K2LocalVar::StaticGetTypeId())
            {
                FEdGraphSchemaAction_K2LocalVar* LocalVarAction = (FEdGraphSchemaAction_K2LocalVar*)Actions[i].Get();

                FBlueprintEditorUtils::SetBlueprintVariableCategory(GetBlueprintObj(), LocalVarAction->GetVariableName(), LocalVarAction->GetVariableScope(), CategoryName, true);
            }
            else if (Actions[i]->GetTypeId() == FEdGraphSchemaAction_K2Delegate::StaticGetTypeId())
            {
                FEdGraphSchemaAction_K2Delegate* DelegateAction = (FEdGraphSchemaAction_K2Delegate*)Actions[i].Get();
                FBlueprintEditorUtils::SetBlueprintVariableCategory(GetBlueprintObj(), DelegateAction->GetDelegateProperty()->GetFName(), NULL, CategoryName, true);
            }
            else if (Actions[i]->GetTypeId() == FEdGraphSchemaAction_K2Graph::StaticGetTypeId())
            {
                // Do not allow renaming of any graph actions outside of the following
                if(Actions[i]->GetSectionID() == NodeSectionID::FUNCTION || Actions[i]->GetSectionID() == NodeSectionID::MACRO)
                {
                    FEdGraphSchemaAction_K2Graph* GraphAction = (FEdGraphSchemaAction_K2Graph*)Actions[i].Get();

                    // Don't allow changing the category of a graph who's parent is not the current OdysseyBrush
                    if( /*GraphAction && !FBlueprintEditorUtils::IsPaletteActionReadOnly(Actions[i], OdysseyBrushEditorPtr.Pin()) && */ FBlueprintEditorUtils::FindBlueprintForGraph(GraphAction->EdGraph) == GetBlueprintObj())
                    {
                        UK2Node_EditablePinBase* EntryNode = FBlueprintEditorUtils::GetEntryNode(GraphAction->EdGraph);
                        EntryNode->Modify();
                        if (UK2Node_FunctionEntry* FunctionEntryNode = Cast<UK2Node_FunctionEntry>(EntryNode))
                        {
                            FunctionEntryNode->MetaData.Category = CategoryName;
                        }
                        else if (UK2Node_Tunnel* TypedEntryNode = ExactCast<UK2Node_Tunnel>(EntryNode))
                        {
                            TypedEntryNode->MetaData.Category = CategoryName;
                        }

                        if(UFunction* Function = GetBlueprintObj()->SkeletonGeneratedClass->FindFunctionByName(GraphAction->EdGraph->GetFName()))
                        {
                            Function->SetMetaData(FBlueprintMetadata::MD_FunctionCategory, *CategoryName.ToString());
                        }
                    }
                }
            }
        }
        Refresh();
        FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprintObj());
        SelectItemByName(FName(*CategoryName.ToString()), ESelectInfo::OnMouseClick, InAction.Pin()->SectionID, true);
    }
}

FText SMyOdysseyBrush::OnGetSectionTitle( int32 InSectionID )
{
    FText SeperatorTitle;
    /* Setup an appropriate name for the section for this node */
    switch( InSectionID )
    {
    case NodeSectionID::VARIABLE:
        SeperatorTitle = NSLOCTEXT("GraphActionNode", "Variables", "Variables");
        break;
    case NodeSectionID::COMPONENT:
        SeperatorTitle = NSLOCTEXT("GraphActionNode", "Components", "Components");
        break;
    case NodeSectionID::FUNCTION:
        if ( OverridableFunctionActions.Num() > 0 )
        {
            SeperatorTitle = FText::Format(NSLOCTEXT("GraphActionNode", "FunctionsOverridableFormat", "Functions <TinyText.Subdued>({0} Overridable)</>"), FText::AsNumber(OverridableFunctionActions.Num()));
        }
        else
        {
            SeperatorTitle = NSLOCTEXT("GraphActionNode", "Functions", "Functions");
        }
        SeperatorTitle = NSLOCTEXT("GraphActionNode", "Functions", "Functions");
        break;
    case NodeSectionID::FUNCTION_OVERRIDABLE:
        SeperatorTitle = NSLOCTEXT("GraphActionNode", "OverridableFunctions", "Overridable Functions");
        break;
    case NodeSectionID::MACRO:
        SeperatorTitle = NSLOCTEXT("GraphActionNode", "Macros", "Macros");
        break;
    case NodeSectionID::INTERFACE:
        SeperatorTitle = NSLOCTEXT("GraphActionNode", "Interfaces", "Interfaces");
        break;
    case NodeSectionID::DELEGATE:
        SeperatorTitle = NSLOCTEXT("GraphActionNode", "EventDispatchers", "Event Dispatchers");
        break;
    case NodeSectionID::GRAPH:
        SeperatorTitle = NSLOCTEXT("GraphActionNode", "Graphs", "Graphs");
        break;
    case NodeSectionID::USER_ENUM:
        SeperatorTitle = NSLOCTEXT("GraphActionNode", "Userenums", "User Enums");
        break;
    case NodeSectionID::LOCAL_VARIABLE:
        if ( GetFocusedGraph() )
        {
            SeperatorTitle = FText::Format(NSLOCTEXT("GraphActionNode", "LocalVariables_Focused", "Local Variables <TinyText.Subdued>({0})</>"), FText::FromName(GetFocusedGraph()->GetFName()));
        }
        else
        {
            SeperatorTitle = NSLOCTEXT("GraphActionNode", "LocalVariables", "Local Variables");
        }
        break;
    case NodeSectionID::USER_STRUCT:
        SeperatorTitle = NSLOCTEXT("GraphActionNode", "Userstructs", "User Structs");
        break;
    default:
    case NodeSectionID::NONE:
        SeperatorTitle = FText::GetEmpty();
        break;
    }
    return SeperatorTitle;
}

TSharedRef<SWidget> SMyOdysseyBrush::OnGetSectionWidget(TSharedRef<SWidget> RowWidget, int32 InSectionID)
{
    TWeakPtr<SWidget> WeakRowWidget = RowWidget;

    FText AddNewText;
    FName MetaDataTag;

    switch ( InSectionID )
    {
    case NodeSectionID::VARIABLE:
        AddNewText = LOCTEXT("AddNewVariable", "Variable");
        MetaDataTag = TEXT("AddNewVariable");
        break;
    case NodeSectionID::FUNCTION:
        AddNewText = LOCTEXT("AddNewFunction", "Function");
        MetaDataTag = TEXT("AddNewFunction");
        // Avoid creating override menu
        if ( OverridableFunctionActions.Num() > 0 )
        {
            return SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SAssignNew(FunctionSectionButton, SComboButton)
                    .IsEnabled(this, &SMyOdysseyBrush::IsEditingMode)
                    .Visibility(this, &SMyOdysseyBrush::OnGetSectionTextVisibility, WeakRowWidget, InSectionID)
                    .ButtonStyle(FEditorStyle::Get(), "RoundButton")
                    .ForegroundColor(FEditorStyle::GetSlateColor("DefaultForeground"))
                    .ContentPadding(FMargin(2, 0))
                    .OnGetMenuContent(this, &SMyOdysseyBrush::OnGetFunctionListMenu)
                    .HasDownArrow(true)
                    .ButtonContent()
                    [
                        SNew(STextBlock)
                        .Font(IDetailLayoutBuilder::GetDetailFontBold())
                        .Text(LOCTEXT("Override", "Override"))
                        .ShadowOffset(FVector2D(1, 1))
                    ]
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(2, 0,0,0)
                [
                    CreateAddToSectionButton(InSectionID, WeakRowWidget, AddNewText, MetaDataTag)
                ];
        }

        break;
    case NodeSectionID::MACRO:
        AddNewText = LOCTEXT("AddNewMacro", "Macro");
        MetaDataTag = TEXT("AddNewMacro");
        break;
    case NodeSectionID::DELEGATE:
        AddNewText = LOCTEXT("AddNewDelegate", "Event Dispatcher");
        MetaDataTag = TEXT("AddNewDelegate");
        break;
    case NodeSectionID::GRAPH:
        AddNewText = LOCTEXT("AddNewGraph", "New Graph");
        MetaDataTag = TEXT("AddNewGraph");
        // Avoid creating new graphs
        return SNullWidget::NullWidget;
        break;
    case NodeSectionID::LOCAL_VARIABLE:
        AddNewText = LOCTEXT("AddNewLocalVariable", "Local Variable");
        MetaDataTag = TEXT("AddNewLocalVariable");
        break;
    default:
        return SNullWidget::NullWidget;
    }

    return CreateAddToSectionButton(InSectionID, WeakRowWidget, AddNewText, MetaDataTag);
}

TSharedRef<SWidget> SMyOdysseyBrush::CreateAddToSectionButton(int32 InSectionID, TWeakPtr<SWidget> WeakRowWidget, FText AddNewText, FName MetaDataTag)
{
    return SNew(SButton)
        .ButtonStyle(FEditorStyle::Get(), "RoundButton")
        .ForegroundColor(FEditorStyle::GetSlateColor("DefaultForeground"))
        .ContentPadding(FMargin(2, 0))
        .OnClicked(this, &SMyOdysseyBrush::OnAddButtonClickedOnSection, InSectionID)
        .IsEnabled(this, &SMyOdysseyBrush::IsEditingMode)
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        .AddMetaData<FTagMetaData>(FTagMetaData(MetaDataTag))
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(FMargin(0, 1))
            [
                SNew(SImage)
                .Image(FEditorStyle::GetBrush("Plus"))
            ]

            + SHorizontalBox::Slot()
            .VAlign(VAlign_Center)
            .AutoWidth()
            .Padding(FMargin(2,0,0,0))
            [
                SNew(STextBlock)
                .Font(IDetailLayoutBuilder::GetDetailFontBold())
                .Text(AddNewText)
                .Visibility(this, &SMyOdysseyBrush::OnGetSectionTextVisibility, WeakRowWidget, InSectionID)
                .ShadowOffset(FVector2D(1,1))
            ]
        ];
}

FReply SMyOdysseyBrush::OnAddButtonClickedOnSection(int32 InSectionID)
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditor = OdysseyBrushEditorPtr.Pin();

    switch ( InSectionID )
    {
    case NodeSectionID::VARIABLE:
        OdysseyBrushEditor->GetToolkitCommands()->ExecuteAction(FOdysseyBrushEditorCommands::Get().AddNewVariable.ToSharedRef());
        break;
    case NodeSectionID::FUNCTION:
        OdysseyBrushEditor->GetToolkitCommands()->ExecuteAction(FOdysseyBrushEditorCommands::Get().AddNewFunction.ToSharedRef());
        break;
    case NodeSectionID::MACRO:
        OdysseyBrushEditor->GetToolkitCommands()->ExecuteAction(FOdysseyBrushEditorCommands::Get().AddNewMacroDeclaration.ToSharedRef());
        break;
    case NodeSectionID::DELEGATE:
        OdysseyBrushEditor->GetToolkitCommands()->ExecuteAction(FOdysseyBrushEditorCommands::Get().AddNewDelegate.ToSharedRef());
        break;
    case NodeSectionID::GRAPH:
        OdysseyBrushEditor->GetToolkitCommands()->ExecuteAction(FOdysseyBrushEditorCommands::Get().AddNewEventGraph.ToSharedRef());
        break;
    case NodeSectionID::LOCAL_VARIABLE:
        OnAddNewLocalVariable();
        break;
    }

    return FReply::Handled();
}

EVisibility SMyOdysseyBrush::OnGetSectionTextVisibility(TWeakPtr<SWidget> RowWidget, int32 InSectionID) const
{
    bool ShowText = RowWidget.Pin()->IsHovered();
    if ( InSectionID == NodeSectionID::FUNCTION && FunctionSectionButton.IsValid() && FunctionSectionButton->IsOpen() )
    {
        ShowText = true;
    }

    // If the row is currently hovered, or a menu is being displayed for a button, keep the button expanded.
    if ( ShowText )
    {
        return EVisibility::SelfHitTestInvisible;
    }
    else
    {
        return EVisibility::Collapsed;
    }
}

TSharedRef<SWidget> SMyOdysseyBrush::OnGetFunctionListMenu()
{
    const bool bShouldCloseWindowAfterMenuSelection = true;
    FMenuBuilder MenuBuilder(bShouldCloseWindowAfterMenuSelection, OdysseyBrushEditorPtr.Pin()->GetToolkitCommands());

    BuildOverridableFunctionsMenu(MenuBuilder);

    TSharedRef<SWidget> MenuWidget = MenuBuilder.MakeWidget();

    return MenuWidget;
}

void SMyOdysseyBrush::BuildOverridableFunctionsMenu(FMenuBuilder& MenuBuilder)
{
    MenuBuilder.BeginSection("OverrideFunction", LOCTEXT("OverrideFunction", "Override Function"));
    {
        for ( auto& OverrideAction : OverridableFunctionActions )
        {
            MenuBuilder.AddMenuEntry(
                OverrideAction->GetMenuDescription(),
                OverrideAction->GetTooltipDescription(),
                FSlateIcon(),
                FUIAction(
                    FExecuteAction::CreateSP(this, &SMyOdysseyBrush::ImplementFunction, OverrideAction),
                    FCanExecuteAction::CreateSP(this, &SMyOdysseyBrush::IsEditingMode)),
                NAME_None,
                EUserInterfaceActionType::Button
                );
        }
    }
    MenuBuilder.EndSection();
}

bool SMyOdysseyBrush::CanRequestRenameOnActionNode(TWeakPtr<FGraphActionNode> InSelectedNode) const
{
    bool bIsReadOnly = true;

    // If checking if renaming is available on a category node, the category must have a non-native entry
    if (InSelectedNode.Pin()->IsCategoryNode())
    {
        TArray<TSharedPtr<FEdGraphSchemaAction>> Actions;
        GraphActionMenu->GetCategorySubActions(InSelectedNode, Actions);

        for (TSharedPtr<FEdGraphSchemaAction> Action : Actions)
        {
            if (Action->GetPersistentItemDefiningObject().IsPotentiallyEditable())
            {
                bIsReadOnly = false;
                break;
            }
        }
    }
    else if (InSelectedNode.Pin()->IsActionNode())
    {
        check( InSelectedNode.Pin()->Actions.Num() > 0 && InSelectedNode.Pin()->Actions[0].IsValid() );
        bIsReadOnly = IsActionReadOnly( InSelectedNode.Pin()->Actions[0] );
    }

    return IsEditingMode() && !bIsReadOnly;
}

bool
SMyOdysseyBrush::IsActionReadOnly( TSharedPtr< FEdGraphSchemaAction > ActionIn ) const
{
    auto Ed = OdysseyBrushEditorPtr.Pin();
    check(Ed.IsValid());
    bool bIsReadOnly = false;
    if(!Ed->InEditingMode())
    {
        bIsReadOnly = true;
    }
    else
    {
        UBlueprint const* const BlueprintObj = Ed->GetBlueprintObj();
        if(ActionIn->GetTypeId() == FEdGraphSchemaAction_K2Graph::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2Graph* GraphAction = (FEdGraphSchemaAction_K2Graph*)ActionIn.Get();
            // No graph is evidence of an overridable function, don't let the user modify it
            if(GraphAction->EdGraph == nullptr)
            {
                bIsReadOnly = true;
            }
            else
            {
                // Graphs that cannot be deleted or re-named are read-only
                if ( !(GraphAction->EdGraph->bAllowDeletion || GraphAction->EdGraph->bAllowRenaming) )
                {
                    bIsReadOnly = true;
                }
                else
                {
                    if(GraphAction->GraphType == EEdGraphSchemaAction_K2Graph::Function)
                    {
                        // Check if the function is an override
                        UFunction* OverrideFunc = FindField<UFunction>(BlueprintObj->ParentClass, GraphAction->FuncName);
                        if ( OverrideFunc != nullptr )
                        {
                            bIsReadOnly = true;
                        }
                    }
                    else if(GraphAction->GraphType == EEdGraphSchemaAction_K2Graph::Interface)
                    {
                        // Interfaces cannot be renamed
                        bIsReadOnly = true;
                    }
                }
            }
        }
        else if(ActionIn->GetTypeId() == FEdGraphSchemaAction_K2Var::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2Var* VarAction = (FEdGraphSchemaAction_K2Var*)ActionIn.Get();

            bIsReadOnly = true;

            if( FBlueprintEditorUtils::FindNewVariableIndex(BlueprintObj, VarAction->GetVariableName()) != INDEX_NONE)
            {
                bIsReadOnly = false;
            }
            else if(BlueprintObj->FindTimelineTemplateByVariableName(VarAction->GetVariableName()))
            {
                bIsReadOnly = false;
            }
            else if(Ed->CanAccessComponentsMode())
            {
                // Wasn't in the introduced variable list; try to find the associated SCS node
                //@TODO: The SCS-generated variables should be in the variable list and have a link back;
                // As it stands, you cannot do any metadata operations on a SCS variable, and you have to do icky code like the following
                TArray<USCS_Node*> Nodes = BlueprintObj->SimpleConstructionScript->GetAllNodes();
                for (TArray<USCS_Node*>::TConstIterator NodeIt(Nodes); NodeIt; ++NodeIt)
                {
                    USCS_Node* CurrentNode = *NodeIt;
                    if (CurrentNode->GetVariableName() == VarAction->GetVariableName())
                    {
                        bIsReadOnly = false;
                        break;
                    }
                }
            }
        }
        else if(ActionIn->GetTypeId() == FEdGraphSchemaAction_K2Delegate::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2Delegate* DelegateAction = (FEdGraphSchemaAction_K2Delegate*)ActionIn.Get();

            if( FBlueprintEditorUtils::FindNewVariableIndex(BlueprintObj, DelegateAction->GetDelegateName()) == INDEX_NONE)
            {
                bIsReadOnly = true;
            }
        }
        else if (ActionIn->GetTypeId() == FEdGraphSchemaAction_K2Event::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2Event* EventAction = (FEdGraphSchemaAction_K2Event*)ActionIn.Get();
            UK2Node* AssociatedNode = EventAction->NodeTemplate;

            bIsReadOnly = (AssociatedNode == nullptr) || (!AssociatedNode->bCanRenameNode);
        }
        else if (ActionIn->GetTypeId() == FEdGraphSchemaAction_K2InputAction::StaticGetTypeId())
        {
            bIsReadOnly = true;
        }
    }

    return bIsReadOnly;
}

void SMyOdysseyBrush::Refresh()
{
    bNeedsRefresh = false;

    GraphActionMenu->RefreshAllActions(/*bPreserveExpansion=*/ true);
}

TSharedRef<SWidget> SMyOdysseyBrush::OnCreateWidgetForAction(FCreateWidgetForActionData* const InCreateData)
{
    return OdysseyBrushEditorPtr.IsValid() ? SNew(SOdysseyBrushPaletteItem, InCreateData, OdysseyBrushEditorPtr.Pin()) : SNew(SOdysseyBrushPaletteItem, InCreateData, GetBlueprintObj());
}

void SMyOdysseyBrush::GetChildGraphs(UEdGraph* InEdGraph, int32 const SectionId, FGraphActionSort& SortList, const FText& ParentCategory)
{
    check(InEdGraph);

    // Grab display info
    FGraphDisplayInfo EdGraphDisplayInfo;
    if (const UEdGraphSchema* Schema = InEdGraph->GetSchema())
    {
        Schema->GetGraphDisplayInformation(*InEdGraph, EdGraphDisplayInfo);
    }
    const FText EdGraphDisplayName = EdGraphDisplayInfo.DisplayName;

    // Grab children graphs
    for (UEdGraph* Graph : InEdGraph->SubGraphs)
    {
        check(Graph);

        FGraphDisplayInfo ChildGraphDisplayInfo;
        if (const UEdGraphSchema* ChildSchema = Graph->GetSchema())
        {
            ChildSchema->GetGraphDisplayInformation(*Graph, ChildGraphDisplayInfo);
        }

        FText DisplayText = ChildGraphDisplayInfo.DisplayName;

        FText Category;
        if (!ParentCategory.IsEmpty())
        {
            Category = FText::Format(FText::FromString(TEXT("{0}|{1}")), ParentCategory, EdGraphDisplayName);
        }
        else
        {
            Category = EdGraphDisplayName;
        }

        const FName DisplayName = FName(*DisplayText.ToString());
        FText ChildTooltip = DisplayText;
        FText ChildDesc = MoveTemp(DisplayText);

        TSharedPtr<FEdGraphSchemaAction_K2Graph> NewChildAction = MakeShareable(new FEdGraphSchemaAction_K2Graph(EEdGraphSchemaAction_K2Graph::Subgraph, Category, MoveTemp(ChildDesc), MoveTemp(ChildTooltip), 1, SectionId));
        NewChildAction->FuncName = DisplayName;
        NewChildAction->EdGraph = Graph;
        SortList.AddAction(NewChildAction);

        GetChildGraphs(Graph, SectionId, SortList, Category);
        GetChildEvents(Graph, SectionId, SortList, Category);
    }
}

void SMyOdysseyBrush::GetChildEvents(UEdGraph const* InEdGraph, int32 const SectionId, FGraphActionSort& SortList, const FText& ParentCategory) const
{
    if (!ensure(InEdGraph != NULL))
    {
        return;
    }

    // grab the parent graph's name
    FGraphDisplayInfo EdGraphDisplayInfo;
    if (UEdGraphSchema const* Schema = InEdGraph->GetSchema())
    {
        Schema->GetGraphDisplayInformation(*InEdGraph, EdGraphDisplayInfo);
    }
    FText EdGraphDisplayName = EdGraphDisplayInfo.DisplayName;
    FText ActionCategory;
    if (!ParentCategory.IsEmpty())
    {
        ActionCategory = FText::Format(FText::FromString(TEXT("{0}|{1}")), ParentCategory, EdGraphDisplayName);
    }
    else
    {
        ActionCategory = MoveTemp(EdGraphDisplayName);
    }

    for (UEdGraphNode* GraphNode : InEdGraph->Nodes)
    {
        if (GraphNode && GraphNode->GetClass()->ImplementsInterface(UK2Node_EventNodeInterface::StaticClass()))
        {
            TSharedPtr<FEdGraphSchemaAction> EventNodeAction = CastChecked<IK2Node_EventNodeInterface>(GraphNode)->GetEventNodeAction(ActionCategory);
            EventNodeAction->SectionID = SectionId;
            SortList.AddAction(EventNodeAction);
        }
    }
}

void SMyOdysseyBrush::GetLocalVariables(FGraphActionSort& SortList) const
{
    // We want to pull local variables from the top level function graphs
    UEdGraph* TopLevelGraph = FBlueprintEditorUtils::GetTopLevelGraph(GetFocusedGraph());
    if( TopLevelGraph )
    {
        // grab the parent graph's name
        FGraphDisplayInfo EdGraphDisplayInfo;
        if (UEdGraphSchema const* Schema = TopLevelGraph->GetSchema())
        {
            Schema->GetGraphDisplayInformation(*TopLevelGraph, EdGraphDisplayInfo);
        }

        TArray<UK2Node_FunctionEntry*> FunctionEntryNodes;
        TopLevelGraph->GetNodesOfClass<UK2Node_FunctionEntry>(FunctionEntryNodes);

        // Search in all FunctionEntry nodes for their local variables
        FText ActionCategory;
        for (UK2Node_FunctionEntry* const FunctionEntry : FunctionEntryNodes)
        {
            for (const FBPVariableDescription& Variable : FunctionEntry->LocalVariables)
            {
                FText Category = Variable.Category;
                if (Variable.Category.EqualTo(UEdGraphSchema_OdysseyBrush::VR_DefaultCategory))
                {
                    Category = FText::GetEmpty();
                }

                UFunction* Func = FindField<UFunction>(GetBlueprintObj()->SkeletonGeneratedClass, TopLevelGraph->GetFName());
                if (Func)
                {
                    TSharedPtr<FEdGraphSchemaAction_K2LocalVar> NewVarAction = MakeShareable(new FEdGraphSchemaAction_K2LocalVar(Category, FText::FromName(Variable.VarName), FText::GetEmpty(), 0, NodeSectionID::LOCAL_VARIABLE));
                    NewVarAction->SetVariableInfo(Variable.VarName, Func, Variable.VarType.PinCategory == UEdGraphSchema_OdysseyBrush::PC_Boolean);
                    SortList.AddAction(NewVarAction);
                }
            }
        }
    }
}

EVisibility SMyOdysseyBrush::GetLocalActionsListVisibility() const
{
    if( !OdysseyBrushEditorPtr.IsValid())
    {
        return EVisibility::Visible;
    }

    if( OdysseyBrushEditorPtr.IsValid() && OdysseyBrushEditorPtr.Pin()->NewDocument_IsVisibleForType(FOdysseyBrushEditor::CGT_NewLocalVariable))
    {
        return EVisibility::Visible;
    }
    return EVisibility::Collapsed;
}

void SMyOdysseyBrush::CollectAllActions(FGraphActionListBuilderBase& OutAllActions)
{
    const UEdGraphSchema_OdysseyBrush* K2Schema = GetDefault<UEdGraphSchema_OdysseyBrush>();

    UBlueprint* OdysseyBrushObj = GetBlueprintObj();
    check(OdysseyBrushObj);

    EFieldIteratorFlags::SuperClassFlags FieldIteratorSuperFlag = EFieldIteratorFlags::IncludeSuper;
    if ( ShowUserVarsOnly() )
    {
        FieldIteratorSuperFlag = EFieldIteratorFlags::ExcludeSuper;
    }

    bool bShowReplicatedOnly = IsShowingReplicatedVariablesOnly();

    // Initialise action sorting instance
    FGraphActionSort SortList( OdysseyBrushObj->CategorySorting );
    // List of names of functions we implement
    ImplementedFunctionCache.Empty();

    // Grab Variables
    for (TFieldIterator<UProperty> PropertyIt(OdysseyBrushObj->SkeletonGeneratedClass, FieldIteratorSuperFlag); PropertyIt; ++PropertyIt)
    {
        UProperty* Property = *PropertyIt;
        FName PropName = Property->GetFName();

        // If we're showing only replicated, ignore the rest
        if (bShowReplicatedOnly && (!Property->HasAnyPropertyFlags(CPF_Net | CPF_RepNotify) || Property->HasAnyPropertyFlags(CPF_RepSkip)))
        {
            continue;
        }

        // Don't show delegate properties, there is special handling for these
        const bool bMulticastDelegateProp = Property->IsA(UMulticastDelegateProperty::StaticClass());
        const bool bDelegateProp = (Property->IsA(UDelegateProperty::StaticClass()) || bMulticastDelegateProp);
        const bool bShouldShowAsVar = (!Property->HasAnyPropertyFlags(CPF_Parm) && Property->HasAllPropertyFlags(CPF_BlueprintVisible)) && !bDelegateProp;
        const bool bShouldShowAsDelegate = !Property->HasAnyPropertyFlags(CPF_Parm) && bMulticastDelegateProp
            && Property->HasAnyPropertyFlags(CPF_BlueprintAssignable | CPF_BlueprintCallable);
        UObjectPropertyBase* Obj = Cast<UObjectPropertyBase>(Property);
        if(!bShouldShowAsVar && !bShouldShowAsDelegate)
        {
            continue;
        }

        const FText PropertyTooltip = Property->GetToolTipText();
        const FName PropertyName = Property->GetFName();
        const FText PropertyDesc = FText::FromName(PropertyName);

        FText CategoryName = FObjectEditorUtils::GetCategoryText(Property);
        FText PropertyCategory = FObjectEditorUtils::GetCategoryText(Property);
        const FString UserCategoryName = FEditorCategoryUtils::GetCategoryDisplayString( PropertyCategory.ToString() );

        if (CategoryName.EqualTo(FText::FromString(OdysseyBrushObj->GetName())) || CategoryName.EqualTo(UEdGraphSchema_OdysseyBrush::VR_DefaultCategory))
        {
            CategoryName = FText::GetEmpty();        // default, so place in 'non' category
            PropertyCategory = FText::GetEmpty();
        }

        if (bShouldShowAsVar)
        {
            const bool bComponentProperty = Obj && Obj->PropertyClass ? Obj->PropertyClass->IsChildOf<UActorComponent>() : false;

            // By default components go into the variable section under the component category unless a custom category is specified.
            if ( bComponentProperty && CategoryName.IsEmpty() )
            {
                PropertyCategory = LOCTEXT("Components", "Components");
            }

            TSharedPtr<FEdGraphSchemaAction_K2Var> NewVarAction = MakeShareable(new FEdGraphSchemaAction_K2Var(PropertyCategory, PropertyDesc, PropertyTooltip, 0, NodeSectionID::VARIABLE));
            const UArrayProperty* ArrayProperty = Cast<const UArrayProperty>(Property);
            const UProperty* TestProperty = ArrayProperty ? ArrayProperty->Inner : Property;
            NewVarAction->SetVariableInfo(PropertyName, OdysseyBrushObj->SkeletonGeneratedClass, Cast<UBoolProperty>(TestProperty) != nullptr);
            SortList.AddAction( UserCategoryName, NewVarAction );
        }
        else if (bShouldShowAsDelegate)
        {
            TSharedPtr<FEdGraphSchemaAction_K2Delegate> NewDelegateAction;
            // Delegate is visible in MyOdysseyBrush when not-native or its category name is not empty.
            if (Property->HasAllPropertyFlags(CPF_Edit) || !PropertyCategory.IsEmpty())
            {
                NewDelegateAction = MakeShareable(new FEdGraphSchemaAction_K2Delegate(PropertyCategory, PropertyDesc, PropertyTooltip, 0, NodeSectionID::DELEGATE));
                NewDelegateAction->SetVariableInfo(PropertyName, OdysseyBrushObj->SkeletonGeneratedClass, false);
                SortList.AddAction( UserCategoryName, NewDelegateAction );
            }

            UClass* OwnerClass = CastChecked<UClass>(Property->GetOuter());
            UEdGraph* Graph = FBlueprintEditorUtils::GetDelegateSignatureGraphByName(OdysseyBrushObj, PropertyName);
            if (Graph && OwnerClass && (OdysseyBrushObj == OwnerClass->ClassGeneratedBy))
            {
                if (NewDelegateAction.IsValid())
                {
                    NewDelegateAction->EdGraph = Graph;
                }
                ImplementedFunctionCache.Add(PropertyName);
            }
        }
    }

    // Grab functions implemented by the blueprint
    for (UEdGraph* Graph : OdysseyBrushObj->FunctionGraphs)
    {
        check(Graph);

        FGraphDisplayInfo DisplayInfo;
        Graph->GetSchema()->GetGraphDisplayInformation(*Graph, DisplayInfo);

        FText FunctionCategory;
        if (OdysseyBrushObj->SkeletonGeneratedClass != nullptr)
        {
            UFunction* Function = OdysseyBrushObj->SkeletonGeneratedClass->FindFunctionByName(Graph->GetFName());
            if (Function != nullptr)
            {
                FunctionCategory = Function->GetMetaDataText(FBlueprintMetadata::MD_FunctionCategory, TEXT("UObjectCategory"), Function->GetFullGroupName(false));
            }
        }

        //@TODO: Should be a bit more generic (or the AnimGraph shouldn't be stored as a FunctionGraph...)
        const bool bIsConstructionScript = Graph->GetFName() == UEdGraphSchema_OdysseyBrush::FN_UserConstructionScript;
        int32 SectionID = Graph->IsA<UAnimationGraph>() ? NodeSectionID::GRAPH : NodeSectionID::FUNCTION;
        TSharedPtr<FEdGraphSchemaAction_K2Graph> NewFuncAction = MakeShareable(new FEdGraphSchemaAction_K2Graph(EEdGraphSchemaAction_K2Graph::Function, FunctionCategory, DisplayInfo.PlainName, DisplayInfo.Tooltip, bIsConstructionScript ? 2 : 1, SectionID));
        NewFuncAction->FuncName = Graph->GetFName();
        NewFuncAction->EdGraph = Graph;

        const FString UserCategoryName = FEditorCategoryUtils::GetCategoryDisplayString(FunctionCategory.ToString());
        SortList.AddAction(UserCategoryName, NewFuncAction);

        GetChildGraphs(Graph, NewFuncAction->GetSectionID(), SortList, FunctionCategory);
        GetChildEvents(Graph, NewFuncAction->GetSectionID(), SortList, FunctionCategory);

        ImplementedFunctionCache.Add(Graph->GetFName());
    }

    // Grab macros implemented by the blueprint
    for (int32 i = 0; i < OdysseyBrushObj->MacroGraphs.Num(); i++)
    {
        UEdGraph* Graph = OdysseyBrushObj->MacroGraphs[i];
        check(Graph);

        const FName MacroName = Graph->GetFName();

        FGraphDisplayInfo DisplayInfo;
        Graph->GetSchema()->GetGraphDisplayInformation(*Graph, DisplayInfo);

        FText MacroCategory = GetGraphCategory(Graph);

        TSharedPtr<FEdGraphSchemaAction_K2Graph> NewMacroAction = MakeShareable(new FEdGraphSchemaAction_K2Graph(EEdGraphSchemaAction_K2Graph::Macro, MacroCategory, DisplayInfo.PlainName, DisplayInfo.Tooltip, 1, NodeSectionID::MACRO));
        NewMacroAction->FuncName = MacroName;
        NewMacroAction->EdGraph = Graph;

        const FString UserCategoryName = FEditorCategoryUtils::GetCategoryDisplayString(MacroCategory.ToString());
        SortList.AddAction(UserCategoryName, NewMacroAction);

        GetChildGraphs(Graph, NewMacroAction->GetSectionID(), SortList, MacroCategory);
        GetChildEvents(Graph, NewMacroAction->GetSectionID(), SortList, MacroCategory);

        ImplementedFunctionCache.Add(MacroName);
    }

    OverridableFunctionActions.Reset();

    // Fill with functions names we've already collected for rename, to ensure we do not add the same function multiple times.
    TArray<FName> OverridableFunctionNames;

    // Cache potentially overridable functions
    UClass* ParentClass = OdysseyBrushObj->SkeletonGeneratedClass ? OdysseyBrushObj->SkeletonGeneratedClass->GetSuperClass() : *OdysseyBrushObj->ParentClass;
    for ( TFieldIterator<UFunction> FunctionIt(ParentClass, EFieldIteratorFlags::IncludeSuper); FunctionIt; ++FunctionIt )
    {
        const UFunction* Function = *FunctionIt;
        const FName FunctionName = Function->GetFName();

        if (    UEdGraphSchema_OdysseyBrush::CanKismetOverrideFunction(Function)
             && !OverridableFunctionNames.Contains(FunctionName)
             && !ImplementedFunctionCache.Contains(FunctionName)
             && !FObjectEditorUtils::IsFunctionHiddenFromClass(Function, ParentClass)
             && !FBlueprintEditorUtils::FindOverrideForFunction(OdysseyBrushObj, CastChecked<UClass>(Function->GetOuter()), Function->GetFName()) )
        {
            FText FunctionTooltip = FText::FromString(UK2Node_CallFunction::GetDefaultTooltipForFunction(Function));
            FText FunctionDesc = K2Schema->GetFriendlySignatureName(Function);
            if ( FunctionDesc.IsEmpty() )
            {
                FunctionDesc = FText::FromString(Function->GetName());
            }

            FText FunctionCategory = Function->GetMetaDataText(FBlueprintMetadata::MD_FunctionCategory, TEXT("UObjectCategory"), Function->GetFullGroupName(false));

            TSharedPtr<FEdGraphSchemaAction_K2Graph> NewFuncAction = MakeShareable(new FEdGraphSchemaAction_K2Graph(EEdGraphSchemaAction_K2Graph::Function, FunctionCategory, FunctionDesc, FunctionTooltip, 1, NodeSectionID::FUNCTION_OVERRIDABLE));
            NewFuncAction->FuncName = FunctionName;

            OverridableFunctionActions.Add(NewFuncAction);
            OverridableFunctionNames.Add(FunctionName);
        }
    }

    // Also function implemented for interfaces
    for (int32 i=0; i < OdysseyBrushObj->ImplementedInterfaces.Num(); i++)
    {
        FBPInterfaceDescription& InterfaceDesc = OdysseyBrushObj->ImplementedInterfaces[i];
        for (int32 FuncIdx = 0; FuncIdx < InterfaceDesc.Graphs.Num(); FuncIdx++)
        {
            UEdGraph* Graph = InterfaceDesc.Graphs[FuncIdx];
            check(Graph);

            const FName FunctionName = Graph->GetFName();
            FString FunctionTooltip = FunctionName.ToString();
            FString FunctionDesc = FunctionName.ToString();

            FText FunctionCategory;

            if (OdysseyBrushObj->SkeletonGeneratedClass != nullptr)
            {
                if (UFunction* Function = OdysseyBrushObj->SkeletonGeneratedClass->FindFunctionByName(Graph->GetFName()))
                {
                    FunctionCategory = Function->GetMetaDataText(FBlueprintMetadata::MD_FunctionCategory, TEXT("UObjectCategory"), Function->GetFullGroupName(false));
                }
            }

            TSharedPtr<FEdGraphSchemaAction_K2Graph> NewFuncAction = MakeShareable(new FEdGraphSchemaAction_K2Graph(EEdGraphSchemaAction_K2Graph::Interface, FunctionCategory, FText::FromString(FunctionDesc), FText::FromString(FunctionTooltip), 1, NodeSectionID::INTERFACE));
            NewFuncAction->FuncName = FunctionName;
            NewFuncAction->EdGraph = Graph;
            OutAllActions.AddAction(NewFuncAction);

            GetChildGraphs(Graph, NewFuncAction->GetSectionID(), SortList, FunctionCategory);
            GetChildEvents(Graph, NewFuncAction->GetSectionID(), SortList, FunctionCategory);
        }
    }

    // also walk up the class chain to look for overridable functions in natively implemented interfaces
    for ( UClass* TempClass=OdysseyBrushObj->ParentClass; TempClass; TempClass=TempClass->GetSuperClass() )
    {
        for (int32 Idx=0; Idx<TempClass->Interfaces.Num(); ++Idx)
        {
            FImplementedInterface const& I = TempClass->Interfaces[Idx];
            if (!I.bImplementedByK2)
            {
                // same as above, make a function?
                for (TFieldIterator<UFunction> FunctionIt(I.Class, EFieldIteratorFlags::IncludeSuper); FunctionIt; ++FunctionIt)
                {
                    const UFunction* Function = *FunctionIt;
                    const FName FunctionName = Function->GetFName();

                    if ( UEdGraphSchema_OdysseyBrush::CanKismetOverrideFunction(Function) && !ImplementedFunctionCache.Contains(FunctionName) && !UEdGraphSchema_OdysseyBrush::FunctionCanBePlacedAsEvent(Function) )
                    {
                        FText FunctionTooltip = Function->GetToolTipText();
                        FText FunctionDesc = K2Schema->GetFriendlySignatureName(Function);

                        FText FunctionCategory = Function->GetMetaDataText(FBlueprintMetadata::MD_FunctionCategory, TEXT("UObjectCategory"), Function->GetFullGroupName(false));

                        TSharedPtr<FEdGraphSchemaAction_K2Graph> NewFuncAction = MakeShareable(new FEdGraphSchemaAction_K2Graph(EEdGraphSchemaAction_K2Graph::Function, FunctionCategory, FunctionDesc, FunctionTooltip, 1, NodeSectionID::INTERFACE));
                        NewFuncAction->FuncName = FunctionName;
                        OutAllActions.AddAction(NewFuncAction);
                    }
                }
            }
        }
    }

    // Grab ubergraph pages
    for (int32 i = 0; i < OdysseyBrushObj->UbergraphPages.Num(); i++)
    {
        UEdGraph* Graph = OdysseyBrushObj->UbergraphPages[i];
        check(Graph);

        FGraphDisplayInfo DisplayInfo;
        Graph->GetSchema()->GetGraphDisplayInformation(*Graph, DisplayInfo);

        TSharedPtr<FEdGraphSchemaAction_K2Graph> NeUbergraphAction = MakeShareable(new FEdGraphSchemaAction_K2Graph(EEdGraphSchemaAction_K2Graph::Graph, FText::GetEmpty(), DisplayInfo.PlainName, DisplayInfo.Tooltip, 2, NodeSectionID::GRAPH));
        NeUbergraphAction->FuncName = Graph->GetFName();
        NeUbergraphAction->EdGraph = Graph;
        OutAllActions.AddAction(NeUbergraphAction);

        GetChildGraphs(Graph, NeUbergraphAction->GetSectionID(), SortList);
        GetChildEvents(Graph, NeUbergraphAction->GetSectionID(), SortList);
    }

    // Grab intermediate pages
    for (int32 i = 0; i < OdysseyBrushObj->IntermediateGeneratedGraphs.Num(); i++)
    {
        UEdGraph* Graph = OdysseyBrushObj->IntermediateGeneratedGraphs[i];
        check(Graph);

        const FName IntermediateName(*(FString(TEXT("$INTERMEDIATE$_")) + Graph->GetName()));
        FString IntermediateTooltip = IntermediateName.ToString();
        FString IntermediateDesc = IntermediateName.ToString();
        TSharedPtr<FEdGraphSchemaAction_K2Graph> NewIntermediateAction = MakeShareable(new FEdGraphSchemaAction_K2Graph(EEdGraphSchemaAction_K2Graph::Graph, FText::GetEmpty(), FText::FromString(IntermediateDesc), FText::FromString(IntermediateTooltip), 1));
        NewIntermediateAction->FuncName = IntermediateName;
        NewIntermediateAction->EdGraph = Graph;
        OutAllActions.AddAction(NewIntermediateAction);

        GetChildGraphs(Graph, NewIntermediateAction->GetSectionID(), SortList);
        GetChildEvents(Graph, NewIntermediateAction->GetSectionID(), SortList);
    }

    if (GetLocalActionsListVisibility().IsVisible())
    {
        GetLocalVariables(SortList);
    }

    // Add all the sorted variables, components, functions, etc...
    SortList.CleanupCategories();
    SortList.GetAllActions(OutAllActions);
}

void SMyOdysseyBrush::CollectStaticSections(TArray<int32>& StaticSectionIDs)
{
    if ( IsShowingEmptySections() )
    {
        TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditor = OdysseyBrushEditorPtr.Pin();
        const bool bIsEditor = OdysseyBrushEditor.IsValid();

        if (!bIsEditor || OdysseyBrushEditor->NewDocument_IsVisibleForType(FOdysseyBrushEditor::CGT_NewEventGraph))
        {
            StaticSectionIDs.Add(NodeSectionID::GRAPH);
        }
        if (!bIsEditor || OdysseyBrushEditor->NewDocument_IsVisibleForType(FOdysseyBrushEditor::CGT_NewMacroGraph))
        {
            StaticSectionIDs.Add(NodeSectionID::MACRO);
        }
        if (!bIsEditor || OdysseyBrushEditor->NewDocument_IsVisibleForType(FOdysseyBrushEditor::CGT_NewFunctionGraph))
        {
            StaticSectionIDs.Add(NodeSectionID::FUNCTION);
        }
        if (!bIsEditor || OdysseyBrushEditor->NewDocument_IsVisibleForType(FOdysseyBrushEditor::CGT_NewVariable))
        {
            StaticSectionIDs.Add(NodeSectionID::VARIABLE);
        }
        if (!bIsEditor || OdysseyBrushEditor->FOdysseyBrushEditor::AddNewDelegateIsVisible())
        {
            StaticSectionIDs.Add(NodeSectionID::DELEGATE);
        }
    }

    if ( GetLocalActionsListVisibility().IsVisible() )
    {
        StaticSectionIDs.Add(NodeSectionID::LOCAL_VARIABLE);
    }
}

bool SMyOdysseyBrush::IsShowingInheritedVariables() const
{
    return GetMutableDefault<UBlueprintEditorSettings>()->bShowInheritedVariables;
}

void SMyOdysseyBrush::OnToggleShowInheritedVariables()
{
    UBlueprintEditorSettings* Settings = GetMutableDefault<UBlueprintEditorSettings>();
    Settings->bShowInheritedVariables = !Settings->bShowInheritedVariables;
    Settings->PostEditChange();
    Settings->SaveConfig();

    Refresh();
}

void SMyOdysseyBrush::OnToggleShowEmptySections()
{
    UBlueprintEditorSettings* Settings = GetMutableDefault<UBlueprintEditorSettings>();
    Settings->bShowEmptySections = !Settings->bShowEmptySections;
    Settings->PostEditChange();
    Settings->SaveConfig();

    Refresh();
}

bool SMyOdysseyBrush::IsShowingEmptySections() const
{
    return GetMutableDefault<UBlueprintEditorSettings>()->bShowEmptySections;
}

void SMyOdysseyBrush::OnToggleShowReplicatedVariablesOnly()
{
    bShowReplicatedVariablesOnly = !bShowReplicatedVariablesOnly;
    Refresh();
}

bool SMyOdysseyBrush::IsShowingReplicatedVariablesOnly() const
{
    return bShowReplicatedVariablesOnly;
}

FReply SMyOdysseyBrush::OnActionDragged( const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions, const FPointerEvent& MouseEvent )
{
    if (!OdysseyBrushEditorPtr.IsValid())
    {
        return FReply::Unhandled();
    }

    TSharedPtr<FEdGraphSchemaAction> InAction( InActions.Num() > 0 ? InActions[0] : NULL );
    if(InAction.IsValid())
    {
        auto AnalyticsDelegate = FNodeCreationAnalytic::CreateSP( this, &SMyOdysseyBrush::UpdateNodeCreation );

        if(InAction->GetTypeId() == FEdGraphSchemaAction_K2Graph::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2Graph* FuncAction = (FEdGraphSchemaAction_K2Graph*)InAction.Get();

            if (FuncAction->GraphType == EEdGraphSchemaAction_K2Graph::Function ||FuncAction->GraphType == EEdGraphSchemaAction_K2Graph::Interface)
            {
                // Callback function to report that the user cannot drop this function in the graph
                auto CanDragDropAction = [](TSharedPtr<FEdGraphSchemaAction> /*DropAction*/, UEdGraph* /*HoveredGraphIn*/, FText& ImpededReasonOut, bool bIsBlueprintCallableFunction)->bool
                {
                    if (!bIsBlueprintCallableFunction)
                    {
                        ImpededReasonOut = LOCTEXT("NonBlueprintCallable", "This function was not marked as OdysseyBrush Callable and cannot be placed in a graph!");
                    }
                    return bIsBlueprintCallableFunction;
                };

                bool bIsBlueprintCallableFunction = false;
                if (FuncAction->EdGraph)
                {
                    for (UEdGraphNode* GraphNode : FuncAction->EdGraph->Nodes)
                    {
                        if (UK2Node_FunctionEntry* Node = Cast<UK2Node_FunctionEntry>(GraphNode))
                        {
                            // See whether this node is a blueprint callable function
                            if (Node->GetFunctionFlags() & (FUNC_BlueprintCallable|FUNC_BlueprintPure))
                            {
                                bIsBlueprintCallableFunction = true;
                            }
                        }
                    }
                }

                return FReply::Handled().BeginDragDrop(FKismetFunctionDragDropAction::New(InAction, FuncAction->FuncName, GetBlueprintObj()->SkeletonGeneratedClass, FMemberReference(), AnalyticsDelegate, FKismetDragDropAction::FCanBeDroppedDelegate::CreateLambda(CanDragDropAction, bIsBlueprintCallableFunction)));
            }
            else if (FuncAction->GraphType == EEdGraphSchemaAction_K2Graph::Macro)
            {
                if ((FuncAction->EdGraph != NULL) && GetBlueprintObj()->BlueprintType != BPTYPE_MacroLibrary)
                {
                    return FReply::Handled().BeginDragDrop(FKismetMacroDragDropAction::New(InAction, FuncAction->FuncName, GetBlueprintObj(), FuncAction->EdGraph, AnalyticsDelegate));
                }
            }
        }
        else if(InAction->GetTypeId() == FEdGraphSchemaAction_K2Delegate::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2Delegate* DelegateAction = (FEdGraphSchemaAction_K2Delegate*)InAction.Get();
            check(DelegateAction->GetDelegateName() != NAME_None);
            if (UClass* VarClass = DelegateAction->GetDelegateClass())
            {
                const bool bIsAltDown = MouseEvent.IsAltDown();
                const bool bIsCtrlDown = MouseEvent.IsLeftControlDown() || MouseEvent.IsRightControlDown();

                TSharedRef<FKismetVariableDragDropAction> DragOperation = FKismetDelegateDragDropAction::New(InAction, DelegateAction->GetDelegateName(), VarClass, AnalyticsDelegate);
                DragOperation->SetAltDrag(bIsAltDown);
                DragOperation->SetCtrlDrag(bIsCtrlDown);
                return FReply::Handled().BeginDragDrop(DragOperation);
            }
        }
        else if( InAction->GetTypeId() == FEdGraphSchemaAction_K2LocalVar::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2LocalVar* VarAction = (FEdGraphSchemaAction_K2LocalVar*)InAction.Get();
            if (UStruct* VariableScope = VarAction->GetVariableScope())
            {
                TSharedRef<FKismetVariableDragDropAction> DragOperation = FKismetVariableDragDropAction::New(InAction, VarAction->GetVariableName(), VariableScope, AnalyticsDelegate);
                DragOperation->SetAltDrag(MouseEvent.IsAltDown());
                DragOperation->SetCtrlDrag(MouseEvent.IsLeftControlDown() || MouseEvent.IsRightControlDown());
                return FReply::Handled().BeginDragDrop(DragOperation);
            }
        }
        else if(InAction->GetTypeId() == FEdGraphSchemaAction_K2Var::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2Var* VarAction = (FEdGraphSchemaAction_K2Var*)InAction.Get();
            if (UClass* VarClass = VarAction->GetVariableClass())
            {
                TSharedRef<FKismetVariableDragDropAction> DragOperation = FKismetVariableDragDropAction::New(InAction, VarAction->GetVariableName(), VarClass, AnalyticsDelegate);
                DragOperation->SetAltDrag(MouseEvent.IsAltDown());
                DragOperation->SetCtrlDrag(MouseEvent.IsLeftControlDown() || MouseEvent.IsRightControlDown());
                return FReply::Handled().BeginDragDrop(DragOperation);
            }
        }
        else if (InAction->GetTypeId() == FEdGraphSchemaAction_K2Event::StaticGetTypeId())
        {
            // Check if it's a custom event, it is preferable to drop a call function for custom events than to focus on the node
            FEdGraphSchemaAction_K2Event* FuncAction = (FEdGraphSchemaAction_K2Event*)InAction.Get();
            if (UK2Node_CustomEvent* CustomEvent = Cast<UK2Node_CustomEvent>(FuncAction->NodeTemplate))
            {
                return FReply::Handled().BeginDragDrop(FKismetFunctionDragDropAction::New(InAction, CustomEvent->GetFunctionName(), GetBlueprintObj()->SkeletonGeneratedClass, FMemberReference(), AnalyticsDelegate, FKismetDragDropAction::FCanBeDroppedDelegate()));
            }
            else
            {
                // don't need a valid FCanBeDroppedDelegate because this entry means we already have this
                // event placed (so this action will just focus it)
                TSharedRef<FKismetDragDropAction> DragOperation = FKismetDragDropAction::New(InAction, AnalyticsDelegate, FKismetDragDropAction::FCanBeDroppedDelegate());

                return FReply::Handled().BeginDragDrop(DragOperation);
            }
        }
    }

    return FReply::Unhandled();
}

FReply SMyOdysseyBrush::OnCategoryDragged(const FText& InCategory, const FPointerEvent& MouseEvent)
{
    TSharedRef<FMyOdysseyBrushCategoryDragDropAction> DragOperation = FMyOdysseyBrushCategoryDragDropAction::New(InCategory, SharedThis(this));
    return FReply::Handled().BeginDragDrop(DragOperation);
}

void SMyOdysseyBrush::OnGlobalActionSelected(const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions, ESelectInfo::Type InSelectionType)
{
    if (InSelectionType == ESelectInfo::OnMouseClick  || InSelectionType == ESelectInfo::OnKeyPress || InSelectionType == ESelectInfo::OnNavigation || InActions.Num() == 0)
    {
        OnActionSelected(InActions);
    }
}

void SMyOdysseyBrush::OnActionSelected( const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions )
{
    TSharedPtr<FEdGraphSchemaAction> InAction(InActions.Num() > 0 ? InActions[0] : NULL);
    UBlueprint* CurrentOdysseyBrush = OdysseyBrush;
    TSharedPtr<SKismetInspector> CurrentInspector = Inspector.Pin();

    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditor = OdysseyBrushEditorPtr.Pin();

    if (OdysseyBrushEditor.IsValid())
    {
        OdysseyBrushEditor->SetUISelectionState(FOdysseyBrushEditor::SelectionState_MyOdysseyBrush);

        CurrentOdysseyBrush = OdysseyBrushEditor->GetBlueprintObj();
        CurrentInspector = OdysseyBrushEditor->GetInspector();
    }
    OnActionSelectedHelper(InAction, OdysseyBrushEditorPtr, OdysseyBrush, CurrentInspector.ToSharedRef());
}

void SMyOdysseyBrush::OnActionSelectedHelper(TSharedPtr<FEdGraphSchemaAction> InAction, TWeakPtr< FOdysseyBrushEditor > InOdysseyBrushEditor, UBlueprint* OdysseyBrush, TSharedRef<SKismetInspector> Inspector)
{
    if (InAction.IsValid())
    {
        if (InAction->GetTypeId() == FEdGraphSchemaAction_K2Graph::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2Graph* GraphAction = (FEdGraphSchemaAction_K2Graph*)InAction.Get();

            if (GraphAction->EdGraph)
            {
                FGraphDisplayInfo DisplayInfo;
                GraphAction->EdGraph->GetSchema()->GetGraphDisplayInformation(*GraphAction->EdGraph, DisplayInfo);
                Inspector->ShowDetailsForSingleObject(GraphAction->EdGraph, SKismetInspector::FShowDetailsOptions(DisplayInfo.PlainName));
            }
        }
        else if (InAction->GetTypeId() == FEdGraphSchemaAction_K2Delegate::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2Delegate* DelegateAction = (FEdGraphSchemaAction_K2Delegate*)InAction.Get();
            if (UMulticastDelegateProperty* Property = DelegateAction->GetDelegateProperty())
            {
                Inspector->ShowDetailsForSingleObject(Property, SKismetInspector::FShowDetailsOptions(FText::FromString(Property->GetName())));
            }
        }
        else if (InAction->GetTypeId() == FEdGraphSchemaAction_K2Var::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2Var* VarAction = (FEdGraphSchemaAction_K2Var*)InAction.Get();

            SKismetInspector::FShowDetailsOptions Options(FText::FromName(VarAction->GetVariableName()));
            Options.bForceRefresh = true;

            Inspector->ShowDetailsForSingleObject(VarAction->GetProperty(), Options);
            if (InOdysseyBrushEditor.IsValid())
            {
                InOdysseyBrushEditor.Pin()->GetReplaceReferencesWidget()->SetSourceVariable(VarAction->GetProperty());
            }
        }
        else if (InAction->GetTypeId() == FEdGraphSchemaAction_K2LocalVar::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2LocalVar* VarAction = (FEdGraphSchemaAction_K2LocalVar*)InAction.Get();

            SKismetInspector::FShowDetailsOptions Options(FText::FromName(VarAction->GetVariableName()));

            Inspector->ShowDetailsForSingleObject(VarAction->GetProperty(), Options);
        }
        else if (InAction->GetTypeId() == FEdGraphSchemaAction_K2Enum::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2Enum* EnumAction = (FEdGraphSchemaAction_K2Enum*)InAction.Get();

            SKismetInspector::FShowDetailsOptions Options(FText::FromName(EnumAction->GetPathName()));
            Options.bForceRefresh = true;

            Inspector->ShowDetailsForSingleObject(EnumAction->Enum, Options);
        }
        else if (InAction->GetTypeId() == FEdGraphSchemaAction_K2Struct::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2Struct* StructAction = (FEdGraphSchemaAction_K2Struct*)InAction.Get();

            SKismetInspector::FShowDetailsOptions Options(FText::FromName(StructAction->GetPathName()));
            Options.bForceRefresh = true;

            Inspector->ShowDetailsForSingleObject(StructAction->Struct, Options);
        }
        else if (InAction->GetTypeId() == FEdGraphSchemaAction_K2TargetNode::StaticGetTypeId() ||
            InAction->GetTypeId() == FEdGraphSchemaAction_K2Event::StaticGetTypeId() ||
            InAction->GetTypeId() == FEdGraphSchemaAction_K2InputAction::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2TargetNode* TargetNodeAction = (FEdGraphSchemaAction_K2TargetNode*)InAction.Get();
            SKismetInspector::FShowDetailsOptions Options(TargetNodeAction->NodeTemplate->GetNodeTitle(ENodeTitleType::EditableTitle));
            Inspector->ShowDetailsForSingleObject(TargetNodeAction->NodeTemplate, Options);
        }
        else
        {
            Inspector->ShowDetailsForObjects(TArray<UObject*>());
        }
    }
    else
    {
        Inspector->ShowDetailsForObjects(TArray<UObject*>());
    }
}

void SMyOdysseyBrush::OnActionDoubleClicked(const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions)
{
    if ( !OdysseyBrushEditorPtr.IsValid() )
    {
        return;
    }

    TSharedPtr<FEdGraphSchemaAction> InAction(InActions.Num() > 0 ? InActions[0] : NULL);
    ExecuteAction(InAction);
}

void SMyOdysseyBrush::ExecuteAction(TSharedPtr<FEdGraphSchemaAction> InAction)
{
    // Force it to open in a new document if shift is pressed
    const bool bIsShiftPressed = FSlateApplication::Get().GetModifierKeys().IsShiftDown();
    FDocumentTracker::EOpenDocumentCause OpenMode = bIsShiftPressed ? FDocumentTracker::ForceOpenNewDocument : FDocumentTracker::OpenNewDocument;

    UBlueprint* OdysseyBrushObj = OdysseyBrushEditorPtr.Pin()->GetBlueprintObj();
    if(InAction.IsValid())
    {
        if(InAction->GetTypeId() == FEdGraphSchemaAction_K2Graph::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2Graph* GraphAction = (FEdGraphSchemaAction_K2Graph*)InAction.Get();

            if (GraphAction->EdGraph)
            {
                OdysseyBrushEditorPtr.Pin()->OpenDocument(GraphAction->EdGraph, OpenMode);
            }
        }
        if (InAction->GetTypeId() == FEdGraphSchemaAction_K2Delegate::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2Delegate* DelegateAction = (FEdGraphSchemaAction_K2Delegate*)InAction.Get();

            if (DelegateAction->EdGraph)
            {
                OdysseyBrushEditorPtr.Pin()->OpenDocument(DelegateAction->EdGraph, OpenMode);
            }
        }
        else if(InAction->GetTypeId() == FEdGraphSchemaAction_K2Var::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2Var* VarAction = (FEdGraphSchemaAction_K2Var*)InAction.Get();

            // timeline variables
            const UObjectPropertyBase* ObjectProperty = Cast<const UObjectPropertyBase>(VarAction->GetProperty());
            if (ObjectProperty &&
                ObjectProperty->PropertyClass &&
                ObjectProperty->PropertyClass->IsChildOf(UTimelineComponent::StaticClass()))
            {
                for (int32 i=0; i<OdysseyBrushObj->Timelines.Num(); i++)
                {
                    // Convert the Timeline's name to a variable name before comparing it to the variable
                    //ODYSSEY: PATCH
                    //if (FName(*UTimelineTemplate::TimelineTemplateNameToVariableName(OdysseyBrushObj->Timelines[i]->GetFName())) == VarAction->GetVariableName())
                    if ( OdysseyBrushObj->Timelines[i]->GetVariableName() == VarAction->GetVariableName())
                    {
                        OdysseyBrushEditorPtr.Pin()->OpenDocument(OdysseyBrushObj->Timelines[i], OpenMode);
                    }
                }
            }
        }
        else if (InAction->GetTypeId() == FEdGraphSchemaAction_K2Event::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2Event* EventNodeAction = (FEdGraphSchemaAction_K2Event*)InAction.Get();
            FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(EventNodeAction->NodeTemplate);
        }
        else if (InAction->GetTypeId() == FEdGraphSchemaAction_K2TargetNode::StaticGetTypeId() ||
            InAction->GetTypeId() == FEdGraphSchemaAction_K2InputAction::StaticGetTypeId())
        {
            FEdGraphSchemaAction_K2TargetNode* TargetNodeAction = (FEdGraphSchemaAction_K2TargetNode*)InAction.Get();
            FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(TargetNodeAction->NodeTemplate);
        }
    }
}

template<class SchemaActionType> SchemaActionType* SelectionAsType( const TSharedPtr< SGraphActionMenu >& GraphActionMenu )
{
    TArray<TSharedPtr<FEdGraphSchemaAction> > SelectedActions;
    GraphActionMenu->GetSelectedActions(SelectedActions);

    SchemaActionType* Selection = NULL;

    TSharedPtr<FEdGraphSchemaAction> SelectedAction( SelectedActions.Num() > 0 ? SelectedActions[0] : NULL );
    if ( SelectedAction.IsValid() &&
         SelectedAction->GetTypeId() == SchemaActionType::StaticGetTypeId() )
    {
        // TODO Why not? StaticCastSharedPtr<>()

        Selection = (SchemaActionType*)SelectedActions[0].Get();
    }

    return Selection;
}

FEdGraphSchemaAction_K2Enum* SMyOdysseyBrush::SelectionAsEnum() const
{
    return SelectionAsType<FEdGraphSchemaAction_K2Enum>( GraphActionMenu );
}


FEdGraphSchemaAction_K2Struct* SMyOdysseyBrush::SelectionAsStruct() const
{
    return SelectionAsType<FEdGraphSchemaAction_K2Struct>( GraphActionMenu );
}

FEdGraphSchemaAction_K2Graph* SMyOdysseyBrush::SelectionAsGraph() const
{
    return SelectionAsType<FEdGraphSchemaAction_K2Graph>( GraphActionMenu );
}

FEdGraphSchemaAction_K2Var* SMyOdysseyBrush::SelectionAsVar() const
{
    return SelectionAsType<FEdGraphSchemaAction_K2Var>( GraphActionMenu );
}

FEdGraphSchemaAction_K2LocalVar* SMyOdysseyBrush::SelectionAsLocalVar() const
{
    return SelectionAsType<FEdGraphSchemaAction_K2LocalVar>(GraphActionMenu);
}

FEdGraphSchemaAction_K2Delegate* SMyOdysseyBrush::SelectionAsDelegate() const
{
    return SelectionAsType<FEdGraphSchemaAction_K2Delegate>( GraphActionMenu );
}

FEdGraphSchemaAction_K2Event* SMyOdysseyBrush::SelectionAsEvent() const
{
    return SelectionAsType<FEdGraphSchemaAction_K2Event>( GraphActionMenu );
}

FEdGraphSchemaAction_K2InputAction* SMyOdysseyBrush::SelectionAsInputAction() const
{
    return SelectionAsType<FEdGraphSchemaAction_K2InputAction>(GraphActionMenu);
}

bool SMyOdysseyBrush::SelectionIsCategory() const
{
    return !SelectionHasContextMenu();
}

bool SMyOdysseyBrush::SelectionHasContextMenu() const
{
    TArray<TSharedPtr<FEdGraphSchemaAction> > SelectedActions;
    GraphActionMenu->GetSelectedActions(SelectedActions);
    return SelectedActions.Num() > 0;
}

FText SMyOdysseyBrush::GetGraphCategory(UEdGraph* InGraph) const
{
    FText ReturnCategory;

    // Pull the category from the required metadata based on the types of nodes we can discover in the graph
    UK2Node_EditablePinBase* EntryNode = FBlueprintEditorUtils::GetEntryNode(InGraph);
    if (UK2Node_FunctionEntry* FunctionEntryNode = Cast<UK2Node_FunctionEntry>(EntryNode))
    {
        ReturnCategory = FunctionEntryNode->MetaData.Category;
    }
    else if (UK2Node_Tunnel* TypedEntryNode = ExactCast<UK2Node_Tunnel>(EntryNode))
    {
        ReturnCategory = TypedEntryNode->MetaData.Category;
    }

    // Empty the category if it's default, we don't want to display the "default" category and items will just appear without a category
    if(ReturnCategory.EqualTo(UEdGraphSchema_OdysseyBrush::VR_DefaultCategory))
    {
        ReturnCategory = FText::GetEmpty();
    }

    return ReturnCategory;
}

void SMyOdysseyBrush::GetSelectedItemsForContextMenu(TArray<FOdysseyComponentEventConstructionData>& OutSelectedItems) const
{
    FEdGraphSchemaAction_K2Var* Var = SelectionAsVar();
    if ( Var != NULL )
    {
        UObjectProperty* ComponentProperty = Cast<UObjectProperty>(Var->GetProperty());

        if ( ComponentProperty != NULL &&
             ComponentProperty->PropertyClass != NULL &&
             ComponentProperty->PropertyClass->IsChildOf( UActorComponent::StaticClass() ) )
        {
            FOdysseyComponentEventConstructionData NewItem;
            NewItem.VariableName = Var->GetVariableName();
            NewItem.Component = Cast<UActorComponent>(ComponentProperty->PropertyClass->GetDefaultObject());

            OutSelectedItems.Add( NewItem );
        }
    }
}

TSharedPtr<SWidget> SMyOdysseyBrush::OnContextMenuOpening()
{
    if( !OdysseyBrushEditorPtr.IsValid() )
    {
        return TSharedPtr<SWidget>();
    }

    const bool bShouldCloseWindowAfterMenuSelection = true;
    FMenuBuilder MenuBuilder( bShouldCloseWindowAfterMenuSelection,OdysseyBrushEditorPtr.Pin()->GetToolkitCommands());

    // Check if the selected action is valid for a context menu
    if (SelectionHasContextMenu())
    {
        MenuBuilder.BeginSection("BasicOperations");
        {
            MenuBuilder.AddMenuEntry(FMyOdysseyBrushCommands::Get().OpenGraph);
            MenuBuilder.AddMenuEntry(FMyOdysseyBrushCommands::Get().OpenGraphInNewTab);
            MenuBuilder.AddMenuEntry(FMyOdysseyBrushCommands::Get().FocusNode);
            MenuBuilder.AddMenuEntry(FMyOdysseyBrushCommands::Get().FocusNodeInNewTab);
            MenuBuilder.AddMenuEntry(FGenericCommands::Get().Rename, NAME_None, LOCTEXT("Rename", "Rename"), LOCTEXT("Rename_Tooltip", "Renames this function or variable from blueprint.") );
            MenuBuilder.AddMenuEntry(FMyOdysseyBrushCommands::Get().ImplementFunction);
            MenuBuilder.AddMenuEntry(FGraphEditorCommands::Get().FindReferences);
            MenuBuilder.AddMenuEntry(FGraphEditorCommands::Get().FindAndReplaceReferences);
            MenuBuilder.AddMenuEntry(FMyOdysseyBrushCommands::Get().GotoNativeVarDefinition);
            MenuBuilder.AddMenuEntry(FGenericCommands::Get().Duplicate);
            MenuBuilder.AddMenuEntry(FMyOdysseyBrushCommands::Get().DeleteEntry);
        }
        MenuBuilder.EndSection();

        FEdGraphSchemaAction_K2Var* Var = SelectionAsVar();

        if ( Var && OdysseyBrushEditorPtr.IsValid() && FBlueprintEditorUtils::DoesSupportEventGraphs(GetBlueprintObj()) )
        {
            UObjectProperty* ComponentProperty = Cast<UObjectProperty>(Var->GetProperty());

            if ( ComponentProperty && ComponentProperty->PropertyClass &&
                 ComponentProperty->PropertyClass->IsChildOf( UActorComponent::StaticClass() ) )
            {
                if( FBlueprintEditorUtils::CanClassGenerateEvents( ComponentProperty->PropertyClass ))
                {
                    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditor(OdysseyBrushEditorPtr.Pin());

                    // If the selected item is valid, and is a component of some sort, build a context menu
                    // of events appropriate to the component.
                    MenuBuilder.AddSubMenu(    LOCTEXT("AddEventSubMenu", "Add Event"),
                                            LOCTEXT("AddEventSubMenu_ToolTip", "Add Event"),
                                            FNewMenuDelegate::CreateStatic(    &SSCSEditor::BuildMenuEventsSection,
                                                OdysseyBrushEditor->GetBlueprintObj(), ComponentProperty->PropertyClass,
                                                FCanExecuteAction::CreateRaw(this, &SMyOdysseyBrush::IsEditingMode),
                                                FOdysseyGetSelectedObjectsDelegate::CreateSP(this, &SMyOdysseyBrush::GetSelectedItemsForContextMenu)));
                }
            }
        }
    }
    else
    {
        BuildAddNewMenu(MenuBuilder);
    }

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget> SMyOdysseyBrush::CreateAddNewMenuWidget()
{
    const bool bShouldCloseWindowAfterMenuSelection = true;
    FMenuBuilder MenuBuilder(bShouldCloseWindowAfterMenuSelection, OdysseyBrushEditorPtr.Pin()->GetToolkitCommands());

    BuildAddNewMenu(MenuBuilder);

    return MenuBuilder.MakeWidget();
}

void SMyOdysseyBrush::BuildAddNewMenu(FMenuBuilder& MenuBuilder)
{
    MenuBuilder.BeginSection("AddNewItem", LOCTEXT("AddOperations", "Add New"));
    {
        MenuBuilder.AddMenuEntry(FOdysseyBrushEditorCommands::Get().AddNewVariable);
        MenuBuilder.AddMenuEntry(FOdysseyBrushEditorCommands::Get().AddNewLocalVariable);
        MenuBuilder.AddMenuEntry(FOdysseyBrushEditorCommands::Get().AddNewFunction);

        // If we cannot handle Function Graphs, we cannot handle function overrides
        if ( OverridableFunctionActions.Num() > 0 && OdysseyBrushEditorPtr.Pin()->NewDocument_IsVisibleForType(FOdysseyBrushEditor::CGT_NewFunctionGraph) )
        {
            MenuBuilder.AddSubMenu(
                LOCTEXT("OverrideFunction", "Override Function"),
                FText::GetEmpty(),
                FNewMenuDelegate::CreateSP(this, &SMyOdysseyBrush::BuildOverridableFunctionsMenu),
                false,
                FSlateIcon(FEditorStyle::GetStyleSetName(), "OdysseyBrushEditor.AddNewFunction.Small"));
        }

        MenuBuilder.AddMenuEntry(FOdysseyBrushEditorCommands::Get().AddNewMacroDeclaration);
        //MenuBuilder.AddMenuEntry(FOdysseyBrushEditorCommands::Get().AddNewEventGraph);
        //MenuBuilder.AddMenuEntry(FOdysseyBrushEditorCommands::Get().AddNewDelegate);
    }
    MenuBuilder.EndSection();
}

bool SMyOdysseyBrush::CanOpenGraph() const
{
    const FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph();
    const bool bGraph = GraphAction && GraphAction->EdGraph;
    const FEdGraphSchemaAction_K2Delegate* DelegateAction = SelectionAsDelegate();
    const bool bDelegate = DelegateAction && DelegateAction->EdGraph;
    return (bGraph || bDelegate) && OdysseyBrushEditorPtr.IsValid();
}

void SMyOdysseyBrush::OpenGraph(FDocumentTracker::EOpenDocumentCause InCause)
{
    UEdGraph* GraphToOpen = NULL;

    if (FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph())
    {
        GraphToOpen = GraphAction->EdGraph;
    }
    else if (FEdGraphSchemaAction_K2Delegate* DelegateAction = SelectionAsDelegate())
    {
        GraphToOpen = DelegateAction->EdGraph;
    }
    else if (FEdGraphSchemaAction_K2Event* EventAction = SelectionAsEvent())
    {
        GraphToOpen = EventAction->NodeTemplate->GetGraph();
    }
    else if (FEdGraphSchemaAction_K2InputAction* InputAction = SelectionAsInputAction())
    {
        GraphToOpen = InputAction->NodeTemplate->GetGraph();
    }

    if (GraphToOpen)
    {
        OdysseyBrushEditorPtr.Pin()->OpenDocument(GraphToOpen, InCause);
    }
}


void SMyOdysseyBrush::OnOpenGraph()
{
    OpenGraph(FDocumentTracker::OpenNewDocument);
}

void SMyOdysseyBrush::OnOpenGraphInNewTab()
{
    OpenGraph(FDocumentTracker::ForceOpenNewDocument);
}

bool SMyOdysseyBrush::CanFocusOnNode() const
{
    FEdGraphSchemaAction_K2Event const* const EventAction = SelectionAsEvent();
    FEdGraphSchemaAction_K2InputAction const* const InputAction = SelectionAsInputAction();
    return (EventAction && EventAction->NodeTemplate) || (InputAction && InputAction->NodeTemplate);
}

void SMyOdysseyBrush::OnFocusNode()
{
    FEdGraphSchemaAction_K2Event* EventAction = SelectionAsEvent();
    FEdGraphSchemaAction_K2InputAction* InputAction = SelectionAsInputAction();
    if (EventAction || InputAction)
    {
        UK2Node* Node = EventAction ? EventAction->NodeTemplate : InputAction->NodeTemplate;
        FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Node);
    }
}

void SMyOdysseyBrush::OnFocusNodeInNewTab()
{
    OpenGraph(FDocumentTracker::ForceOpenNewDocument);
    OnFocusNode();
}

bool SMyOdysseyBrush::CanImplementFunction() const
{
    FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph();
    return GraphAction && GraphAction->EdGraph == NULL;
}

void SMyOdysseyBrush::OnImplementFunction()
{
    if ( FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph() )
    {
        ImplementFunction(GraphAction);
    }
}

void SMyOdysseyBrush::ImplementFunction(TSharedPtr<FEdGraphSchemaAction_K2Graph> GraphAction)
{
    ImplementFunction(GraphAction.Get());
}

void SMyOdysseyBrush::ImplementFunction(FEdGraphSchemaAction_K2Graph* GraphAction)
{
    check(GetBlueprintObj()->SkeletonGeneratedClass);
    const UFunction* OverrideFunc = FindField<UFunction>(GetBlueprintObj()->SkeletonGeneratedClass, GraphAction->FuncName);

    // search up the class hiearchy, we want to find the original declaration of the function to match FBlueprintEventNodeSpawner.
    // Doing so ensures that we can find the existing node if there is one:
    const UClass* Iter = GetBlueprintObj()->SkeletonGeneratedClass->GetSuperClass();
    while (Iter != nullptr)
    {
        if (const UFunction* F = Iter->FindFunctionByName(GraphAction->FuncName))
        {
            OverrideFunc = F;
        }
        else
        {
            break;
        }
        Iter = Iter->GetSuperClass();
    }


    if (OverrideFunc == nullptr)
    {
        // maybe it's from a native interface, check those too
        for ( UClass* TempClass=GetBlueprintObj()->ParentClass; (nullptr != TempClass) && (nullptr == OverrideFunc); TempClass=TempClass->GetSuperClass() )
        {
            for (int32 Idx=0; Idx<TempClass->Interfaces.Num(); ++Idx)
            {
                FImplementedInterface const& I = TempClass->Interfaces[Idx];
                if (!I.bImplementedByK2)
                {
                    OverrideFunc = FindField<UFunction>(I.Class, GraphAction->FuncName);
                    if (OverrideFunc)
                    {
                        // found it, done
                        break;
                    }
                }
            }
        }
    }
    check(OverrideFunc);
    UClass* const OverrideFuncClass = CastChecked<UClass>(OverrideFunc->GetOuter())->GetAuthoritativeClass();

    // Some types of blueprints don't have an event graph (IE gameplay ability blueprints), in that case just make a new graph, even
    // for events:
    UEdGraph* EventGraph = FBlueprintEditorUtils::FindEventGraph(GetBlueprintObj());
    if (UEdGraphSchema_OdysseyBrush::FunctionCanBePlacedAsEvent(OverrideFunc) && EventGraph)
    {
        // Add to event graph
        FName EventName = OverrideFunc->GetFName();
        UK2Node_Event* ExistingNode = FBlueprintEditorUtils::FindOverrideForFunction(GetBlueprintObj(), OverrideFuncClass, EventName);

        if (ExistingNode)
        {
            FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(ExistingNode);
        }
        else
        {
            UK2Node_Event* NewEventNode = FEdGraphSchemaAction_K2NewNode::SpawnNode<UK2Node_Event>(
                EventGraph,
                EventGraph->GetGoodPlaceForNewNode(),
                EK2NewNodeFlags::SelectNewNode,
                [EventName, OverrideFuncClass](UK2Node_Event* NewInstance)
                {
                    NewInstance->EventReference.SetExternalMember(EventName, OverrideFuncClass);
                    NewInstance->bOverrideFunction = true;
                }
            );
            if (NewEventNode)
            {
                FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(NewEventNode);
            }
        }
    }
    else
    {
        // Implement the function graph
        UEdGraph* const NewGraph = FBlueprintEditorUtils::CreateNewGraph(GetBlueprintObj(), GraphAction->FuncName, UEdGraph::StaticClass(), UEdGraphSchema_OdysseyBrush::StaticClass());
        FBlueprintEditorUtils::AddFunctionGraph(GetBlueprintObj(), NewGraph, /*bIsUserCreated=*/ false, OverrideFuncClass);
        OdysseyBrushEditorPtr.Pin()->OpenDocument(NewGraph, FDocumentTracker::OpenNewDocument);
    }
}

void SMyOdysseyBrush::OnFindReference()
{
    bool bUseQuotes = true;
    FString SearchTerm;
    if (FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph())
    {
        SearchTerm = GraphAction->FuncName.ToString();
    }
    else if (FEdGraphSchemaAction_K2Var* VarAction = SelectionAsVar())
    {
        FString GuidTerm;
        const FGuid Guid = FBlueprintEditorUtils::FindMemberVariableGuidByName(OdysseyBrush, VarAction->GetVariableName());
        if (Guid.IsValid())
        {
            GuidTerm = FString::Printf(TEXT(" && MemberGuid(A=%i && B=%i && C=%i && D=%i)"), Guid.A, Guid.B, Guid.C, Guid.D);
        }

        const FString VariableName = VarAction->GetVariableName().ToString();

        // Search for both an explicit variable reference (finds get/sets of exactly that var, without including related-sounding variables)
        // and a softer search for (VariableName) to capture bound component/widget event nodes which wouldn't otherwise show up
        //@TODO: This logic is duplicated in SSCSEditor::OnFindReferences(), keep in sync
        SearchTerm = FString::Printf(TEXT("Nodes(VariableReference(MemberName=+\"%s\"%s) || Name=\"(%s)\")"), *VariableName, *GuidTerm, *VariableName);
        bUseQuotes = false;
    }
    else if (FEdGraphSchemaAction_K2LocalVar* LocalVarAction = SelectionAsLocalVar())
    {
        SearchTerm = FString::Printf(TEXT("Nodes(VariableReference(MemberName=+\"%s\" && MemberScope=+\"%s\"))"), *LocalVarAction->GetVariableName().ToString(), *LocalVarAction->GetVariableScope()->GetName());
        bUseQuotes = false;
    }
    else if (FEdGraphSchemaAction_K2Delegate* DelegateAction = SelectionAsDelegate())
    {
        SearchTerm = DelegateAction->GetDelegateName().ToString();
    }
    else if (FEdGraphSchemaAction_K2Enum* EnumAction = SelectionAsEnum())
    {
        SearchTerm = EnumAction->Enum->GetName();
    }
    else if (FEdGraphSchemaAction_K2Struct* StructAction = SelectionAsStruct())
    {
        SearchTerm = StructAction->Struct->GetName();
    }
    else if (FEdGraphSchemaAction_K2Event* EventAction = SelectionAsEvent())
    {
        SearchTerm = EventAction->NodeTemplate->GetFindReferenceSearchString();
    }
    else if (FEdGraphSchemaAction_K2InputAction* InputAction = SelectionAsInputAction())
    {
        SearchTerm = InputAction->NodeTemplate ?
            InputAction->NodeTemplate->GetNodeTitle(ENodeTitleType::FullTitle).ToString() :
            InputAction->GetMenuDescription().ToString();
    }

    if(!SearchTerm.IsEmpty())
    {
        if (bUseQuotes)
        {
            SearchTerm = FString::Printf(TEXT("\"%s\""), *SearchTerm);
        }
        OdysseyBrushEditorPtr.Pin()->SummonSearchUI(true, SearchTerm);
    }
}

bool SMyOdysseyBrush::CanFindReference() const
{
    // Nothing relevant to the category will ever be found, unless the name of the category overlaps with another item
    if (SelectionIsCategory())
    {
        return false;
    }

    return true;
}

void SMyOdysseyBrush::OnFindAndReplaceReference()
{
    OdysseyBrushEditorPtr.Pin()->SummonFindAndReplaceUI();
}

bool SMyOdysseyBrush::CanFindAndReplaceReference() const
{
    if (SelectionAsVar() && GetDefault<UEditorExperimentalSettings>()->bEnableFindAndReplaceReferences)
    {
        return true;
    }

    return false;
}

void SMyOdysseyBrush::OnDeleteGraph(UEdGraph* InGraph, EEdGraphSchemaAction_K2Graph::Type InGraphType)
{
    if (InGraph)
    {
        const FScopedTransaction Transaction( LOCTEXT("RemoveGraph", "Remove Graph") );
        GetBlueprintObj()->Modify();

        InGraph->Modify();

        if (InGraphType == EEdGraphSchemaAction_K2Graph::Subgraph)
        {
            // Remove any composite nodes bound to this graph
            TArray<UK2Node_Composite*> AllCompositeNodes;
            FBlueprintEditorUtils::GetAllNodesOfClass<UK2Node_Composite>(GetBlueprintObj(), AllCompositeNodes);

            const bool bDontRecompile = true;
            for (UK2Node_Composite* CompNode : AllCompositeNodes)
            {
                if (CompNode->BoundGraph == InGraph)
                {
                    FBlueprintEditorUtils::RemoveNode(GetBlueprintObj(), CompNode, bDontRecompile);
                }
            }
        }

        FBlueprintEditorUtils::RemoveGraph(GetBlueprintObj(), InGraph, EGraphRemoveFlags::Recompile);
        OdysseyBrushEditorPtr.Pin()->CloseDocumentTab(InGraph);

        for (TObjectIterator<UK2Node_CreateDelegate> It(RF_ClassDefaultObject, /** bIncludeDerivedClasses */ true, /** InternalExcludeFlags */ EInternalObjectFlags::PendingKill); It; ++It)
        {
            if (It->GetGraph() != InGraph)
            {
                if (!It->IsPendingKill() && It->GetGraph() && !It->GetGraph()->IsPendingKill())
                {
                    It->HandleAnyChange();
                }
            }
        }

        InGraph = NULL;
    }
}

UEdGraph* SMyOdysseyBrush::GetFocusedGraph() const
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtrPinned = OdysseyBrushEditorPtr.Pin();
    if( OdysseyBrushEditorPtrPinned.IsValid() )
    {
        return OdysseyBrushEditorPtrPinned->GetFocusedGraph();
    }

    return EdGraph;
}

void SMyOdysseyBrush::OnObjectPropertyChanged(UObject* InObject, FPropertyChangedEvent& InPropertyChangedEvent)
{
    if (InObject == OdysseyBrush && (InPropertyChangedEvent.ChangeType != EPropertyChangeType::ValueSet && InPropertyChangedEvent.ChangeType != EPropertyChangeType::ArrayClear))
    {
        bNeedsRefresh = true;
    }
}

bool SMyOdysseyBrush::IsEditingMode() const
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorSPtr = OdysseyBrushEditorPtr.Pin();
    return OdysseyBrushEditorSPtr.IsValid() && OdysseyBrushEditorSPtr->InEditingMode();
}

void SMyOdysseyBrush::OnDeleteDelegate(FEdGraphSchemaAction_K2Delegate* InDelegateAction)
{
    UEdGraph* GraphToActOn = InDelegateAction->EdGraph;
    UBlueprint* OdysseyBrushObj = GetBlueprintObj();
    if (GraphToActOn && OdysseyBrushObj)
    {
        const FScopedTransaction Transaction( LOCTEXT("RemoveDelegate", "Remove Event Dispatcher") );
        OdysseyBrushObj->Modify();

        OdysseyBrushEditorPtr.Pin()->CloseDocumentTab(GraphToActOn);
        GraphToActOn->Modify();

        FBlueprintEditorUtils::RemoveMemberVariable(OdysseyBrushObj, GraphToActOn->GetFName());
        FBlueprintEditorUtils::RemoveGraph(OdysseyBrushObj, GraphToActOn, EGraphRemoveFlags::Recompile);

        for (TObjectIterator<UK2Node_CreateDelegate> It(RF_ClassDefaultObject, /** bIncludeDerivedClasses */ true, /** InternalExcludeFlags */ EInternalObjectFlags::PendingKill); It; ++It)
        {
            if (!It->IsPendingKill() && It->GetGraph() && !It->GetGraph()->IsPendingKill())
            {
                It->HandleAnyChange();
            }
        }
    }
}

void SMyOdysseyBrush::OnDeleteEntry()
{
    if (FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph())
    {
        OnDeleteGraph(GraphAction->EdGraph, GraphAction->GraphType);
    }
    else if (FEdGraphSchemaAction_K2Delegate* DelegateAction = SelectionAsDelegate())
    {
        OnDeleteDelegate(DelegateAction);
    }
    else if ( FEdGraphSchemaAction_K2Var* VarAction = SelectionAsVar() )
    {
        if(FBlueprintEditorUtils::IsVariableUsed(GetBlueprintObj(), VarAction->GetVariableName()))
        {
            FText ConfirmDelete = FText::Format(LOCTEXT( "ConfirmDeleteVariableInUse",
                "Variable {0} is in use! Do you really want to delete it?"),
                FText::FromName( VarAction->GetVariableName() ) );

            // Warn the user that this may result in data loss
            FSuppressableWarningDialog::FSetupInfo Info( ConfirmDelete, LOCTEXT("DeleteVar", "Delete Variable"), "DeleteVariableInUse_Warning" );
            Info.ConfirmText = LOCTEXT( "DeleteVariable_Yes", "Yes");
            Info.CancelText = LOCTEXT( "DeleteVariable_No", "No");

            FSuppressableWarningDialog DeleteVariableInUse( Info );
            if ( DeleteVariableInUse.ShowModal() == FSuppressableWarningDialog::Cancel )
            {
                return;
            }
        }

        const FScopedTransaction Transaction( LOCTEXT( "RemoveVariable", "Remove Variable" ) );

        GetBlueprintObj()->Modify();
        FBlueprintEditorUtils::RemoveMemberVariable(GetBlueprintObj(), VarAction->GetVariableName());
    }
    else if ( FEdGraphSchemaAction_K2LocalVar* LocalVarAction = SelectionAsLocalVar() )
    {
        if(FBlueprintEditorUtils::IsVariableUsed(GetBlueprintObj(), LocalVarAction->GetVariableName(), FBlueprintEditorUtils::FindScopeGraph(GetBlueprintObj(), LocalVarAction->GetVariableScope())))
        {
            FText ConfirmDelete = FText::Format(LOCTEXT( "ConfirmDeleteLocalVariableInUse",
                "Local Variable {0} is in use! Do you really want to delete it?"),
                FText::FromName( LocalVarAction->GetVariableName() ) );

            // Warn the user that this may result in data loss
            FSuppressableWarningDialog::FSetupInfo Info( ConfirmDelete, LOCTEXT("DeleteVar", "Delete Variable"), "DeleteVariableInUse_Warning" );
            Info.ConfirmText = LOCTEXT( "DeleteVariable_Yes", "Yes");
            Info.CancelText = LOCTEXT( "DeleteVariable_No", "No");

            FSuppressableWarningDialog DeleteVariableInUse( Info );
            if ( DeleteVariableInUse.ShowModal() == FSuppressableWarningDialog::Cancel )
            {
                return;
            }
        }

        const FScopedTransaction Transaction( LOCTEXT( "RemoveLocalVariable", "Remove Local Variable" ) );

        GetBlueprintObj()->Modify();

        UEdGraph* FunctionGraph = FBlueprintEditorUtils::GetTopLevelGraph(GetFocusedGraph());
        TArray<UK2Node_FunctionEntry*> FunctionEntryNodes;
        FunctionGraph->GetNodesOfClass<UK2Node_FunctionEntry>(FunctionEntryNodes);
        check(FunctionEntryNodes.Num() == 1);
        FunctionEntryNodes[0]->Modify();

        FBlueprintEditorUtils::RemoveLocalVariable(GetBlueprintObj(), LocalVarAction->GetVariableScope(), LocalVarAction->GetVariableName());
    }
    else if (FEdGraphSchemaAction_K2Event* EventAction = SelectionAsEvent())
    {
        const FScopedTransaction Transaction(LOCTEXT( "RemoveEventNode", "Remove EventNode"));

        GetBlueprintObj()->Modify();
        FBlueprintEditorUtils::RemoveNode(GetBlueprintObj(), EventAction->NodeTemplate);
    }
    else if ( SelectionIsCategory() )
    {
        TArray<TSharedPtr<FEdGraphSchemaAction>> Actions;
        GraphActionMenu->GetSelectedCategorySubActions(Actions);
        if (Actions.Num())
        {
            FText TransactionTitle;

            switch((NodeSectionID::Type)Actions[0]->GetSectionID())
            {
            case NodeSectionID::VARIABLE:
            case NodeSectionID::LOCAL_VARIABLE:
                {
                    TransactionTitle = LOCTEXT( "BulkRemoveVariables", "Bulk Remove Variables" );
                    break;
                }
            case NodeSectionID::DELEGATE:
                {
                    TransactionTitle = LOCTEXT( "BulkRemoveDelegates", "Bulk Remove Delegates" );
                    break;
                }
            case NodeSectionID::FUNCTION:
                {
                    TransactionTitle = LOCTEXT( "BulkRemoveFunctions", "Bulk Remove Functions" );
                    break;
                }
            case NodeSectionID::MACRO:
                {
                    TransactionTitle = LOCTEXT( "BulkRemoveMacros", "Bulk Remove Macros" );
                    break;
                }
            default:
                {
                    TransactionTitle = LOCTEXT( "BulkRemove", "Bulk Remove Items" );
                }
            }

            const FScopedTransaction Transaction( TransactionTitle);

            GetBlueprintObj()->Modify();
            for (int32 i = 0; i < Actions.Num(); ++i)
            {
                if (Actions[i]->GetTypeId() == FEdGraphSchemaAction_K2Var::StaticGetTypeId())
                {
                    FEdGraphSchemaAction_K2Var* Var = (FEdGraphSchemaAction_K2Var*)Actions[i].Get();

                    FBlueprintEditorUtils::RemoveMemberVariable(GetBlueprintObj(), Var->GetVariableName());
                }
                else if (Actions[i]->GetTypeId() == FEdGraphSchemaAction_K2LocalVar::StaticGetTypeId())
                {
                    FEdGraphSchemaAction_K2LocalVar* K2LocalVarAction = (FEdGraphSchemaAction_K2LocalVar*)Actions[i].Get();

                    FBlueprintEditorUtils::RemoveLocalVariable(GetBlueprintObj(), K2LocalVarAction->GetVariableScope(), K2LocalVarAction->GetVariableName());
                }
                else if (Actions[i]->GetTypeId() == FEdGraphSchemaAction_K2Graph::StaticGetTypeId())
                {
                    FEdGraphSchemaAction_K2Graph* K2GraphAction = (FEdGraphSchemaAction_K2Graph*)Actions[i].Get();

                    OnDeleteGraph(K2GraphAction->EdGraph, K2GraphAction->GraphType);
                }
                else if (Actions[i]->GetTypeId() == FEdGraphSchemaAction_K2Delegate::StaticGetTypeId())
                {
                    OnDeleteDelegate((FEdGraphSchemaAction_K2Delegate*)Actions[i].Get());
                }
            }
        }
    }

    Refresh();
    OdysseyBrushEditorPtr.Pin()->GetInspector()->ShowDetailsForObjects(TArray<UObject*>());
}

struct FDeleteEntryHelper
{
    static bool CanDeleteVariable(const UBlueprint* OdysseyBrush, FName VarName)
    {
        check(NULL != OdysseyBrush);

        const UProperty* VariableProperty = FindField<UProperty>(OdysseyBrush->SkeletonGeneratedClass, VarName);
        const UClass* VarSourceClass = CastChecked<const UClass>(VariableProperty->GetOuter());
        const bool bIsOdysseyBrushVariable = (VarSourceClass == OdysseyBrush->SkeletonGeneratedClass);
        const int32 VarInfoIndex = FBlueprintEditorUtils::FindNewVariableIndex(OdysseyBrush, VariableProperty->GetFName());
        const bool bHasVarInfo = (VarInfoIndex != INDEX_NONE);

        return bIsOdysseyBrushVariable && bHasVarInfo;
    }
};

bool SMyOdysseyBrush::CanDeleteEntry() const
{
    // Cannot delete entries while not in editing mode
    if(!IsEditingMode())
    {
        return false;
    }

    if (FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph())
    {
        if (GraphAction->EdGraph != NULL)
        {
            // Allow the user to delete any graphs in the interface section if the function can be placed as an event,
            // this allows users to resolve warnings when a previously implemented graph has been changed to be an event.
            if (GraphAction->GetSectionID() == NodeSectionID::INTERFACE)
            {
                UFunction* Function = GetBlueprintObj()->SkeletonGeneratedClass->FindFunctionByName(GraphAction->EdGraph->GetFName());
                if (UEdGraphSchema_OdysseyBrush::FunctionCanBePlacedAsEvent(Function))
                {
                    return true;
                }
            }
            return GraphAction->EdGraph->bAllowDeletion;
        }
        return false;
    }
    else if (FEdGraphSchemaAction_K2Delegate* DelegateAction = SelectionAsDelegate())
    {
        return (DelegateAction->EdGraph != NULL) && (DelegateAction->EdGraph->bAllowDeletion) &&
            FDeleteEntryHelper::CanDeleteVariable(GetBlueprintObj(), DelegateAction->GetDelegateName());
    }
    else if (FEdGraphSchemaAction_K2Var* VarAction = SelectionAsVar())
    {
        return FDeleteEntryHelper::CanDeleteVariable(GetBlueprintObj(), VarAction->GetVariableName());
    }
    else if (FEdGraphSchemaAction_K2Event* EventAction = SelectionAsEvent())
    {
        return EventAction->NodeTemplate != NULL;
    }
    else if (FEdGraphSchemaAction_K2LocalVar* LocalVariable = SelectionAsLocalVar())
    {
        return true;
    }
    else if (SelectionIsCategory())
    {
        // Can't delete categories if they can't be renamed, that means they are native
        if(GraphActionMenu->CanRequestRenameOnActionNode())
        {
            return true;
        }
    }
    return false;
}

bool SMyOdysseyBrush::IsDuplicateActionVisible() const
{
    if (FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph())
    {
        // Functions in interface Blueprints cannot be duplicated
        if(GetBlueprintObj()->BlueprintType != BPTYPE_Interface)
        {
            // Only display it for valid function graphs
            return GraphAction->EdGraph && GraphAction->EdGraph->GetSchema()->CanDuplicateGraph(GraphAction->EdGraph);
        }
    }
    else if (SelectionAsVar() || SelectionAsLocalVar())
    {
        return true;
    }
    return false;
}

bool SMyOdysseyBrush::CanDuplicateAction() const
{
    // Cannot delete entries while not in editing mode
    if (!IsEditingMode())
    {
        return false;
    }

    if (FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph())
    {
        // Only support function graph duplication
        if(GraphAction->EdGraph)
        {
            return GraphAction->EdGraph->GetSchema()->CanDuplicateGraph(GraphAction->EdGraph);
        }
    }
    else if(FEdGraphSchemaAction_K2Var* VarAction = SelectionAsVar())
    {
        // if the property is not an allowable OdysseyBrush variable type, do not allow the variable to be duplicated.
        // Some actions (timelines) exist as variables but cannot be used in a user-defined variable.
        const UObjectPropertyBase* ObjectProperty = Cast<const UObjectPropertyBase>(VarAction->GetProperty());
        if (ObjectProperty &&
            ObjectProperty->PropertyClass &&
            !UEdGraphSchema_OdysseyBrush::IsAllowableBlueprintVariableType(ObjectProperty->PropertyClass))
        {
            return false;
        }
        return true;
    }
    else if(SelectionAsLocalVar())
    {
        return true;
    }
    return false;
}

void SMyOdysseyBrush::OnDuplicateAction()
{
    FName DuplicateActionName = NAME_None;

    if (FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph())
    {
        const FScopedTransaction Transaction( LOCTEXT( "DuplicateGraph", "Duplicate Graph" ) );
        GetBlueprintObj()->Modify();

        UEdGraph* DuplicatedGraph = GraphAction->EdGraph->GetSchema()->DuplicateGraph(GraphAction->EdGraph);
        check(DuplicatedGraph);

        DuplicatedGraph->Modify();

        // Generate new Guids and component templates for all relevant nodes in the graph
        // *NOTE* this cannot occur during PostDuplicate, node Guids and component templates need to remain static during duplication for OdysseyBrush compilation
        for (UEdGraphNode* EdGraphNode : DuplicatedGraph->Nodes)
        {
            if (EdGraphNode)
            {
                EdGraphNode->CreateNewGuid();

                if (UK2Node_AddComponent* AddComponentNode = Cast<UK2Node_AddComponent>(EdGraphNode))
                {
                    AddComponentNode->MakeNewComponentTemplate();
                }
            }
        }
        // Only function and macro duplication is supported
        EGraphType GraphType = DuplicatedGraph->GetSchema()->GetGraphType(GraphAction->EdGraph);
        check(GraphType == GT_Function || GraphType == GT_Macro);

        if (GraphType == GT_Function)
        {
            GetBlueprintObj()->FunctionGraphs.Add(DuplicatedGraph);
        }
        else if (GraphType == GT_Macro)
        {
            GetBlueprintObj()->MacroGraphs.Add(DuplicatedGraph);
        }
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprintObj());

        OdysseyBrushEditorPtr.Pin()->OpenDocument(DuplicatedGraph, FDocumentTracker::ForceOpenNewDocument);
        DuplicateActionName = DuplicatedGraph->GetFName();
    }
    else if (FEdGraphSchemaAction_K2Var* VarAction = SelectionAsVar())
    {
        const FScopedTransaction Transaction( LOCTEXT( "DuplicateVariable", "Duplicate Variable" ) );
        GetBlueprintObj()->Modify();

        if(FBlueprintEditorUtils::FindNewVariableIndex(GetBlueprintObj(), VarAction->GetVariableName()) != INDEX_NONE)
        {
            DuplicateActionName = FBlueprintEditorUtils::DuplicateVariable(GetBlueprintObj(), nullptr, VarAction->GetVariableName());
        }
        else
        {
            FEdGraphPinType VarPinType;
            GetDefault<UEdGraphSchema_OdysseyBrush>()->ConvertPropertyToPinType(VarAction->GetProperty(), VarPinType);
            FBlueprintEditorUtils::AddMemberVariable(GetBlueprintObj(), FBlueprintEditorUtils::FindUniqueKismetName(OdysseyBrush, VarAction->GetVariableName().ToString()), VarPinType);
        }
    }
    else if (FEdGraphSchemaAction_K2LocalVar* LocalVarAction = SelectionAsLocalVar())
    {
        const FScopedTransaction Transaction( LOCTEXT( "Duplicate Local Variable", "Duplicate Local Variable" ) );
        GetBlueprintObj()->Modify();

        DuplicateActionName = FBlueprintEditorUtils::DuplicateVariable(GetBlueprintObj(), LocalVarAction->GetVariableScope(), LocalVarAction->GetVariableName());
    }

    // Select and rename the duplicated action
    if(DuplicateActionName != NAME_None)
    {
        SelectItemByName(DuplicateActionName);
        OnRequestRenameOnActionNode();
    }
}

void SMyOdysseyBrush::GotoNativeCodeVarDefinition()
{
    if( FEdGraphSchemaAction_K2Var* VarAction = SelectionAsVar() )
    {
        if( UProperty* VarProperty = VarAction->GetProperty() )
        {
            FSourceCodeNavigation::NavigateToProperty( VarProperty );
        }
    }
}

bool SMyOdysseyBrush::IsNativeVariable() const
{
    if( FEdGraphSchemaAction_K2Var* VarAction = SelectionAsVar() )
    {
        UProperty* VarProperty = VarAction->GetProperty();

        if( VarProperty && VarProperty->IsNative())
        {
            return true;
        }
    }
    return false;
}

void SMyOdysseyBrush::OnResetItemFilter()
{
    FilterBox->SetText(FText::GetEmpty());
}

void SMyOdysseyBrush::EnsureLastPinTypeValid()
{
    LastPinType.bIsWeakPointer = false;
    LastFunctionPinType.bIsWeakPointer = false;

    const bool bLastPinTypeValid = (UEdGraphSchema_OdysseyBrush::PC_Struct != LastPinType.PinCategory) || LastPinType.PinSubCategoryObject.IsValid();
    const bool bLastFunctionPinTypeValid = (UEdGraphSchema_OdysseyBrush::PC_Struct != LastFunctionPinType.PinCategory) || LastFunctionPinType.PinSubCategoryObject.IsValid();
    const bool bConstType = LastPinType.bIsConst || LastFunctionPinType.bIsConst;
    if (!bLastPinTypeValid || !bLastFunctionPinTypeValid || bConstType)
    {
        ResetLastPinType();
    }
}

void SMyOdysseyBrush::ResetLastPinType()
{
    LastPinType.ResetToDefaults();
    LastPinType.PinCategory = UEdGraphSchema_OdysseyBrush::PC_Boolean;
    LastFunctionPinType = LastPinType;
}

void SMyOdysseyBrush::UpdateNodeCreation()
{
    if( OdysseyBrushEditorPtr.IsValid() )
    {
        OdysseyBrushEditorPtr.Pin()->UpdateNodeCreationStats( EOdysseyNodeCreateAction::MyOdysseyBrushDragPlacement );
    }
}

FReply SMyOdysseyBrush::OnAddNewLocalVariable()
{
    if( OdysseyBrushEditorPtr.IsValid() )
    {
        OdysseyBrushEditorPtr.Pin()->OnAddNewLocalVariable();
    }

    return FReply::Handled();
}

void SMyOdysseyBrush::OnFilterTextChanged( const FText& InFilterText )
{
    GraphActionMenu->GenerateFilteredItems(false);
}

FText SMyOdysseyBrush::GetFilterText() const
{
    return FilterBox->GetText();
}

void SMyOdysseyBrush::OnRequestRenameOnActionNode()
{
    // Attempt to rename in both menus, only one of them will have anything selected
    GraphActionMenu->OnRequestRenameOnActionNode();
}

bool SMyOdysseyBrush::CanRequestRenameOnActionNode() const
{
    TArray<TSharedPtr<FEdGraphSchemaAction> > SelectedActions;
    GraphActionMenu->GetSelectedActions(SelectedActions);

    // If there is anything selected in the GraphActionMenu, check the item for if it can be renamed.
    if (SelectedActions.Num() || SelectionIsCategory())
    {
        return GraphActionMenu->CanRequestRenameOnActionNode();
    }
    return false;
}

void SMyOdysseyBrush::SelectItemByName(const FName& ItemName, ESelectInfo::Type SelectInfo, int32 SectionId/* = INDEX_NONE*/, bool bIsCategory/* = false*/)
{
    // Check if the graph action menu is being told to clear
    if(ItemName == NAME_None)
    {
        ClearGraphActionMenuSelection();
    }
    else
    {
        // Attempt to select the item in the main graph action menu
        const bool bSucceededAtSelecting = GraphActionMenu->SelectItemByName(ItemName, SelectInfo, SectionId, bIsCategory);
        if (!bSucceededAtSelecting)
        {
            // We failed to select the item, maybe because it was filtered out?
            // Reset the item filter and try again (we don't do this first because someone went to the effort of typing
            // a filter and probably wants to keep it unless it is getting in the way, as it just has)
            OnResetItemFilter();
            GraphActionMenu->SelectItemByName(ItemName, SelectInfo, SectionId, bIsCategory);
        }
    }
}

void SMyOdysseyBrush::ClearGraphActionMenuSelection()
{
    GraphActionMenu->SelectItemByName(NAME_None);
}

void SMyOdysseyBrush::ExpandCategory(const FText& CategoryName)
{
    GraphActionMenu->ExpandCategory(CategoryName);
}

bool SMyOdysseyBrush::MoveCategoryBeforeCategory( const FText& InCategoryToMove, const FText& InTargetCategory )
{
    bool bResult = false;
    UBlueprint* OdysseyBrushObj = OdysseyBrushEditorPtr.Pin()->GetBlueprintObj();

    FString CategoryToMoveString = InCategoryToMove.ToString();
    FString TargetCategoryString = InTargetCategory.ToString();
    if( OdysseyBrushObj )
    {
        // Find root categories
        int32 RootCategoryDelim = CategoryToMoveString.Find( TEXT( "|" ), ESearchCase::CaseSensitive );
        FName CategoryToMove = RootCategoryDelim == INDEX_NONE ? *CategoryToMoveString : *CategoryToMoveString.Left( RootCategoryDelim );
        RootCategoryDelim = TargetCategoryString.Find( TEXT( "|" ), ESearchCase::CaseSensitive );
        FName TargetCategory = RootCategoryDelim == INDEX_NONE ? *TargetCategoryString : *TargetCategoryString.Left( RootCategoryDelim );

        TArray<FName>& CategorySort = OdysseyBrushObj->CategorySorting;
        const int32 RemovalIndex = CategorySort.Find( CategoryToMove );
        // Remove existing sort index
        if( RemovalIndex != INDEX_NONE )
        {
            CategorySort.RemoveAt( RemovalIndex );
        }
        // Update the Category sort order and refresh ( if the target category has an entry )
        const int32 InsertIndex = CategorySort.Find( TargetCategory );
        if( InsertIndex != INDEX_NONE )
        {
            CategorySort.Insert( CategoryToMove, InsertIndex );
            Refresh();
            bResult = true;
        }
    }

    return bResult;
}

#undef LOCTEXT_NAMESPACE
