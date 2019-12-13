// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
#include "FindInOdysseyBrush.h"
#include "Layout/WidgetPath.h"
#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "EditorStyleSet.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EdGraphSchema_OdysseyBrush.h"
#include "K2Node_Event.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Variable.h"
#include "K2Node_MacroInstance.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"
#include "Engine/SCS_Node.h"
#include "OdysseyBrushEditor.h"
#include "BlueprintEditor.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Widgets/SToolTip.h"
#include "IDocumentation.h"
#include "Widgets/Input/SSearchBox.h"
#include "Framework/Commands/GenericCommands.h"
#include "ImaginaryOdysseyBrushData.h"
#include "OdysseyFiBSearchInstance.h"
#include "OdysseyBrushEditorTabs.h"
#include "BlueprintEditorSettings.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Framework/Commands/UICommandList.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "FindInOdysseyBrush"


FText FindInBlueprintsHelpers::AsFText(TSharedPtr< FJsonValue > InJsonValue, const TMap<int32, FText>& InLookupTable)
{
    if (const FText* LookupText = InLookupTable.Find(FCString::Atoi(*InJsonValue->AsString())))
    {
        return *LookupText;
    }
    // Let's never get here.
    return LOCTEXT("FiBSerializationError", "There was an error in serialization!");
}

FText FindInBlueprintsHelpers::AsFText(int32 InValue, const TMap<int32, FText>& InLookupTable)
{
    if (const FText* LookupText = InLookupTable.Find(InValue))
    {
        return *LookupText;
    }
    // Let's never get here.
    return LOCTEXT("FiBSerializationError", "There was an error in serialization!");
}

bool FindInBlueprintsHelpers::IsTextEqualToString(const FText& InText, const FString& InString)
{
    return InString == InText.ToString() || InString == *FTextInspector::GetSourceString(InText);
}

FString FindInBlueprintsHelpers::GetPinTypeAsString(const FEdGraphPinType& InPinType)
{
    FString Result = InPinType.PinCategory.ToString();
    if(UObject* SubCategoryObject = InPinType.PinSubCategoryObject.Get())
    {
        Result += FString(" '") + SubCategoryObject->GetName() + "'";
    }
    else
    {
        Result += FString(" '") + InPinType.PinSubCategory.ToString() + "'";
    }

    return Result;
}

bool FindInBlueprintsHelpers::ParsePinType(FText InKey, FText InValue, FEdGraphPinType& InOutPinType)
{
    bool bParsed = true;

    if(InKey.CompareTo(FFindInOdysseyBrushSearchTags::FiB_PinCategory) == 0)
    {
        InOutPinType.PinCategory = *InValue.ToString();
    }
    else if(InKey.CompareTo(FFindInOdysseyBrushSearchTags::FiB_PinSubCategory) == 0)
    {
        InOutPinType.PinSubCategory = *InValue.ToString();
    }
    else if(InKey.CompareTo(FFindInOdysseyBrushSearchTags::FiB_ObjectClass) == 0)
    {
        InOutPinType.PinSubCategory = *InValue.ToString();
    }
    else if(InKey.CompareTo(FFindInOdysseyBrushSearchTags::FiB_IsArray) == 0)
    {
        InOutPinType.ContainerType = (InValue.ToString().ToBool() ? EPinContainerType::Array : EPinContainerType::None);
    }
    else if(InKey.CompareTo(FFindInOdysseyBrushSearchTags::FiB_IsReference) == 0)
    {
        InOutPinType.bIsReference = InValue.ToString().ToBool();
    }
    else
    {
        bParsed = false;
    }

    return bParsed;
}

void FindInBlueprintsHelpers::ExpandAllChildren(FSearchResult InTreeNode, TSharedPtr<STreeView<TSharedPtr<FFindInBlueprintsResult>>> InTreeView)
{
    if (InTreeNode->Children.Num())
    {
        InTreeView->SetItemExpansion(InTreeNode, true);
        for (int32 i = 0; i < InTreeNode->Children.Num(); i++)
        {
            ExpandAllChildren(InTreeNode->Children[i], InTreeView);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// FOdysseyBrushSearchResult

FFindInBlueprintsResult::FFindInBlueprintsResult(const FText& InDisplayText )
    : DisplayText(InDisplayText)
{
}

FFindInBlueprintsResult::FFindInBlueprintsResult( const FText& InDisplayText, TSharedPtr<FFindInBlueprintsResult> InParent)
    : Parent(InParent), DisplayText(InDisplayText)
{
}

FReply FFindInBlueprintsResult::OnClick()
{
    // If there is a parent, handle it using the parent's functionality
    if(Parent.IsValid())
    {
        return Parent.Pin()->OnClick();
    }
    else
    {
        // As a last resort, find the parent OdysseyBrush, and open that, it will get the user close to what they want
        UBlueprint* OdysseyBrush = GetParentOdysseyBrush();
        if(OdysseyBrush)
        {
            FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(OdysseyBrush, false);
        }
    }

    return FReply::Handled();
}

UObject* FFindInBlueprintsResult::GetObject(UBlueprint* InOdysseyBrush) const
{
    return GetParentOdysseyBrush();
}

FText FFindInBlueprintsResult::GetCategory() const
{
    return FText::GetEmpty();
}

TSharedRef<SWidget> FFindInBlueprintsResult::CreateIcon() const
{
    FLinearColor IconColor = FLinearColor::White;
    const FSlateBrush* Brush = NULL;

    return     SNew(SImage)
            .Image(Brush)
            .ColorAndOpacity(IconColor)
            .ToolTipText( GetCategory() );
}

FString FFindInBlueprintsResult::GetCommentText() const
{
    return CommentText;
}

UBlueprint* FFindInBlueprintsResult::GetParentOdysseyBrush() const
{
    UBlueprint* ResultOdysseyBrush = nullptr;
    if (Parent.IsValid())
    {
        ResultOdysseyBrush = Parent.Pin()->GetParentOdysseyBrush();
    }
    else
    {
        GIsEditorLoadingPackage = true;
        UObject* Object = LoadObject<UObject>(NULL, *DisplayText.ToString(), NULL, 0, NULL);
        GIsEditorLoadingPackage = false;

        if(UBlueprint* OdysseyBrushObj = Cast<UBlueprint>(Object))
        {
            ResultOdysseyBrush = OdysseyBrushObj;
        }
        else if(UWorld* WorldObj = Cast<UWorld>(Object))
        {
            if(WorldObj->PersistentLevel)
            {
                ResultOdysseyBrush = Cast<UBlueprint>((UObject*)WorldObj->PersistentLevel->GetLevelScriptBlueprint(true));
            }
        }

    }
    return ResultOdysseyBrush;
}

FText FFindInBlueprintsResult::GetDisplayString() const
{
    return DisplayText;
}

//////////////////////////////////////////////////////////
// FFindInBlueprintsGraphNode

FFindInBlueprintsGraphNode::FFindInBlueprintsGraphNode(const FText& InValue, TSharedPtr<FFindInBlueprintsResult> InParent)
    : FFindInBlueprintsResult(InValue, InParent)
    , Glyph("EditorStyle", "")
    , Class(nullptr)
{
}

FReply FFindInBlueprintsGraphNode::OnClick()
{
    UBlueprint* OdysseyBrush = GetParentOdysseyBrush();
    if(OdysseyBrush)
    {
        UEdGraphNode* OutNode = NULL;
        if(    UEdGraphNode* GraphNode = FBlueprintEditorUtils::GetNodeByGUID(OdysseyBrush, NodeGuid) )
        {
            FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(GraphNode, /*bRequestRename=*/false);
            return FReply::Handled();
        }
    }

    return FFindInBlueprintsResult::OnClick();
}

TSharedRef<SWidget> FFindInBlueprintsGraphNode::CreateIcon() const
{
    return     SNew(SImage)
        .Image(Glyph.GetOptionalIcon())
        .ColorAndOpacity(GlyphColor)
        .ToolTipText( GetCategory() );
}

void FFindInBlueprintsGraphNode::ParseSearchInfo(FText InKey, FText InValue)
{
    if(InKey.CompareTo(FFindInOdysseyBrushSearchTags::FiB_NodeGuid) == 0)
    {
        FString NodeGUIDAsString = InValue.ToString();
        FGuid::Parse(NodeGUIDAsString, NodeGuid);
    }

    if(InKey.CompareTo(FFindInOdysseyBrushSearchTags::FiB_ClassName) == 0)
    {
        ClassName = InValue.ToString();
    }
    else if(InKey.CompareTo(FFindInOdysseyBrushSearchTags::FiB_Name) == 0)
    {
        DisplayText = InValue;
    }
    else if(InKey.CompareTo(FFindInOdysseyBrushSearchTags::FiB_Comment) == 0)
    {
        CommentText = InValue.ToString();
    }
    else if(InKey.CompareTo(FFindInOdysseyBrushSearchTags::FiB_Glyph) == 0)
    {
        Glyph = FSlateIcon(Glyph.GetStyleSetName(), *InValue.ToString());
    }
    else if(InKey.CompareTo(FFindInOdysseyBrushSearchTags::FiB_GlyphStyleSet) == 0)
    {
        Glyph = FSlateIcon(*InValue.ToString(), Glyph.GetStyleName());
    }
    else if(InKey.CompareTo(FFindInOdysseyBrushSearchTags::FiB_GlyphColor) == 0)
    {
        GlyphColor.InitFromString(InValue.ToString());
    }
}

FText FFindInBlueprintsGraphNode::GetCategory() const
{
    if(Class == UK2Node_CallFunction::StaticClass())
    {
        return LOCTEXT("CallFuctionCat", "Function Call");
    }
    else if(Class == UK2Node_MacroInstance::StaticClass())
    {
        return LOCTEXT("MacroCategory", "Macro");
    }
    else if(Class == UK2Node_Event::StaticClass())
    {
        return LOCTEXT("EventCat", "Event");
    }
    else if(Class == UK2Node_VariableGet::StaticClass())
    {
        return LOCTEXT("VariableGetCategory", "Variable Get");
    }
    else if(Class == UK2Node_VariableSet::StaticClass())
    {
        return LOCTEXT("VariableSetCategory", "Variable Set");
    }

    return LOCTEXT("NodeCategory", "Node");
}

void FFindInBlueprintsGraphNode::FinalizeSearchData()
{
    if(!ClassName.IsEmpty())
    {
        Class = FindObject<UClass>(ANY_PACKAGE, *ClassName, true);
        ClassName.Empty();
    }
}

UObject* FFindInBlueprintsGraphNode::GetObject(UBlueprint* InOdysseyBrush) const
{
    return FBlueprintEditorUtils::GetNodeByGUID(InOdysseyBrush, NodeGuid);
}

//////////////////////////////////////////////////////////
// FFindInBlueprintsPin

FFindInBlueprintsPin::FFindInBlueprintsPin(const FText& InValue, TSharedPtr<FFindInBlueprintsResult> InParent, FString InSchemaName)
    : FFindInBlueprintsResult(InValue, InParent)
    , SchemaName(InSchemaName)
    , IconColor(FSlateColor::UseForeground())
{
}

TSharedRef<SWidget> FFindInBlueprintsPin::CreateIcon() const
{
    const FSlateBrush* Brush = nullptr;

    if( PinType.IsArray() )
    {
        Brush = FEditorStyle::GetBrush( TEXT("GraphEditor.ArrayPinIcon") );
    }
    else if( PinType.bIsReference )
    {
        Brush = FEditorStyle::GetBrush( TEXT("GraphEditor.RefPinIcon") );
    }
    else
    {
        Brush = FEditorStyle::GetBrush( TEXT("GraphEditor.PinIcon") );
    }

    return     SNew(SImage)
        .Image(Brush)
        .ColorAndOpacity(IconColor)
        .ToolTipText(FText::FromString(FindInBlueprintsHelpers::GetPinTypeAsString(PinType)));
}

void FFindInBlueprintsPin::ParseSearchInfo(FText InKey, FText InValue)
{
    if(InKey.CompareTo(FFindInOdysseyBrushSearchTags::FiB_Name) == 0)
    {
        DisplayText = InValue;
    }
    else
    {
        FindInBlueprintsHelpers::ParsePinType(InKey, InValue, PinType);
    }
}

FText FFindInBlueprintsPin::GetCategory() const
{
    return LOCTEXT("PinCategory", "Pin");
}

void FFindInBlueprintsPin::FinalizeSearchData()
{
    if(!PinType.PinSubCategory.IsNone())
    {
        PinType.PinSubCategoryObject = FindObject<UClass>(ANY_PACKAGE, *PinType.PinSubCategory.ToString(), true);
        if(!PinType.PinSubCategoryObject.IsValid())
        {
            PinType.PinSubCategoryObject = FindObject<UScriptStruct>(UObject::StaticClass(), *PinType.PinSubCategory.ToString());
        }

        if (PinType.PinSubCategoryObject.IsValid())
        {
            PinType.PinSubCategory = NAME_None;
        }
    }

    if(!SchemaName.IsEmpty())
    {
        UClass* SchemaClass = FindObject<UClass>(ANY_PACKAGE, *SchemaName, true);
        if(SchemaClass)
        {
            UEdGraphSchema* Schema = SchemaClass->GetDefaultObject<UEdGraphSchema>();
            IconColor = Schema->GetPinTypeColor(PinType);
        }

        SchemaName.Empty();
    }
}

//////////////////////////////////////////////////////////
// FFindInBlueprintsProperty

FFindInBlueprintsProperty::FFindInBlueprintsProperty(const FText& InValue, TSharedPtr<FFindInBlueprintsResult> InParent)
    : FFindInBlueprintsResult(InValue, InParent)
    , bIsSCSComponent(false)
{
}

FReply FFindInBlueprintsProperty::OnClick()
{
    if (bIsSCSComponent)
    {
        UBlueprint* OdysseyBrush = GetParentOdysseyBrush();
        if (OdysseyBrush)
        {
            TSharedPtr<IBlueprintEditor> OdysseyBrushEditor = FKismetEditorUtilities::GetIBlueprintEditorForObject(OdysseyBrush, false);

            if (OdysseyBrushEditor.IsValid())
            {
                // Open Viewport Tab
                OdysseyBrushEditor->FocusWindow();
                OdysseyBrushEditor->GetTabManager()->InvokeTab(FOdysseyBrushEditorTabs::SCSViewportID);

                // Find and Select the Component in the Viewport tab view
                const TArray<USCS_Node*>& Nodes = OdysseyBrush->SimpleConstructionScript->GetAllNodes();
                for (USCS_Node* Node : Nodes)
                {
                    if (Node->GetVariableName().ToString() == DisplayText.ToString())
                    {
                        UBlueprintGeneratedClass* GeneratedClass = Cast<UBlueprintGeneratedClass>(OdysseyBrush->GeneratedClass);
                        if (GeneratedClass)
                        {
                            UActorComponent* Component = Node->GetActualComponentTemplate(GeneratedClass);
                            if (Component)
                            {
                                OdysseyBrushEditor->FindAndSelectSCSEditorTreeNode(Component, false);
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    else
    {
        return FFindInBlueprintsResult::OnClick();
    }

    return FReply::Handled();
}

TSharedRef<SWidget> FFindInBlueprintsProperty::CreateIcon() const
{
    FLinearColor IconColor = FLinearColor::White;
    const FSlateBrush* Brush = UK2Node_Variable::GetVarIconFromPinType(PinType, IconColor).GetOptionalIcon();
    IconColor = UEdGraphSchema_OdysseyBrush::StaticClass()->GetDefaultObject<UEdGraphSchema_OdysseyBrush>()->GetPinTypeColor(PinType);

    return     SNew(SImage)
        .Image(Brush)
        .ColorAndOpacity(IconColor)
        .ToolTipText( FText::FromString(FindInBlueprintsHelpers::GetPinTypeAsString(PinType)) );
}

void FFindInBlueprintsProperty::ParseSearchInfo(FText InKey, FText InValue)
{
    if(InKey.CompareTo(FFindInOdysseyBrushSearchTags::FiB_Name) == 0)
    {
        DisplayText = InValue;
    }
    else if(InKey.CompareTo(FFindInOdysseyBrushSearchTags::FiB_IsSCSComponent) == 0)
    {
        bIsSCSComponent = true;
    }
    else
    {
        FindInBlueprintsHelpers::ParsePinType(InKey, InValue, PinType);
    }
}

FText FFindInBlueprintsProperty::GetCategory() const
{
    if(bIsSCSComponent)
    {
        return LOCTEXT("Component", "Component");
    }
    return LOCTEXT("Variable", "Variable");
}

void FFindInBlueprintsProperty::FinalizeSearchData()
{
    if(!PinType.PinSubCategory.IsNone())
    {
        PinType.PinSubCategoryObject = FindObject<UClass>(ANY_PACKAGE, *PinType.PinSubCategory.ToString(), true);
        if(!PinType.PinSubCategoryObject.IsValid())
        {
            PinType.PinSubCategoryObject = FindObject<UScriptStruct>(UObject::StaticClass(), *PinType.PinSubCategory.ToString());
        }

        if (PinType.PinSubCategoryObject.IsValid())
        {
            PinType.PinSubCategory = NAME_None;
        }
    }
}

//////////////////////////////////////////////////////////
// FFindInBlueprintsGraph

FFindInBlueprintsGraph::FFindInBlueprintsGraph(const FText& InValue, TSharedPtr<FFindInBlueprintsResult> InParent, EGraphType InGraphType)
    : FFindInBlueprintsResult(InValue, InParent)
    , GraphType(InGraphType)
{
}

FReply FFindInBlueprintsGraph::OnClick()
{
    UBlueprint* OdysseyBrush = GetParentOdysseyBrush();
    if(OdysseyBrush)
    {
        TArray<UEdGraph*> OdysseyBrushGraphs;
        OdysseyBrush->GetAllGraphs(OdysseyBrushGraphs);

        for( auto Graph : OdysseyBrushGraphs)
        {
            FGraphDisplayInfo DisplayInfo;
            Graph->GetSchema()->GetGraphDisplayInformation(*Graph, DisplayInfo);

            if(DisplayInfo.PlainName.EqualTo(DisplayText))
            {
                FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Graph);
                break;
            }
        }
    }
    else
    {
        return FFindInBlueprintsResult::OnClick();
    }
    return FReply::Handled();
}

TSharedRef<SWidget> FFindInBlueprintsGraph::CreateIcon() const
{
    const FSlateBrush* Brush = NULL;
    if(GraphType == GT_Function)
    {
        Brush = FEditorStyle::GetBrush(TEXT("GraphEditor.Function_16x"));
    }
    else if(GraphType == GT_Macro)
    {
        Brush = FEditorStyle::GetBrush(TEXT("GraphEditor.Macro_16x"));
    }

    return     SNew(SImage)
        .Image(Brush)
        .ToolTipText( GetCategory() );
}

void FFindInBlueprintsGraph::ParseSearchInfo(FText InKey, FText InValue)
{
    if(InKey.CompareTo(FFindInOdysseyBrushSearchTags::FiB_Name) == 0)
    {
        DisplayText = InValue;
    }
}

FText FFindInBlueprintsGraph::GetCategory() const
{
    if(GraphType == GT_Function)
    {
        return LOCTEXT("FunctionGraphCategory", "Function");
    }
    else if(GraphType == GT_Macro)
    {
        return LOCTEXT("MacroGraphCategory", "Macro");
    }
    return LOCTEXT("GraphCategory", "Graph");
}

//////////////////////////////////////////////////////////////////////////
// SOdysseyBrushSearch

void SFindInBlueprints::Construct( const FArguments& InArgs, TSharedPtr<FOdysseyBrushEditor> InOdysseyBrushEditor)
{
    OutOfDateWithLastSearchBPCount = 0;
    LastSearchedFiBVersion = EFiBVersion::FIB_VER_LATEST;
    OdysseyBrushEditorPtr = InOdysseyBrushEditor;

    HostTab = InArgs._ContainingTab;
    bIsLocked = false;

    if (HostTab.IsValid())
    {
        HostTab.Pin()->SetOnTabClosed(SDockTab::FOnTabClosedCallback::CreateSP(this, &SFindInBlueprints::OnHostTabClosed));
    }

    if (InArgs._bIsSearchWindow)
    {
        RegisterCommands();
    }

    bIsInFindWithinOdysseyBrushMode = OdysseyBrushEditorPtr.IsValid();
    bHasGlobalSearchResults = !bIsInFindWithinOdysseyBrushMode;

    const bool bHostFindInBlueprintsInGlobalTab = GetDefault<UBlueprintEditorSettings>()->bHostFindInBlueprintsInGlobalTab;

    this->ChildSlot
        [
            SAssignNew(MainVerticalBox, SVerticalBox)
            +SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                +SHorizontalBox::Slot()
                .FillWidth(1)
                [
                    SAssignNew(SearchTextField, SSearchBox)
                    .HintText(LOCTEXT("OdysseyBrushSearchHint", "Enter function or event name to find references..."))
                    .OnTextChanged(this, &SFindInBlueprints::OnSearchTextChanged)
                    .OnTextCommitted(this, &SFindInBlueprints::OnSearchTextCommitted)
                    .Visibility(InArgs._bHideSearchBar? EVisibility::Collapsed : EVisibility::Visible)
                ]
            ]
            +SVerticalBox::Slot()
            .FillHeight(1.0f)
            .Padding(0.f, 4.f, 0.f, 0.f)
            [
                SNew(SBorder)
                .BorderImage(FEditorStyle::GetBrush("Menu.Background"))
                [
                    SAssignNew(TreeView, STreeViewType)
                    .ItemHeight(24)
                    .TreeItemsSource( &ItemsFound )
                    .OnGenerateRow( this, &SFindInBlueprints::OnGenerateRow )
                    .OnGetChildren( this, &SFindInBlueprints::OnGetChildren )
                    .OnMouseButtonDoubleClick(this,&SFindInBlueprints::OnTreeSelectionDoubleClicked)
                    .SelectionMode( ESelectionMode::Multi )
                    .OnContextMenuOpening(this, &SFindInBlueprints::OnContextMenuOpening)
                ]
            ]

            +SVerticalBox::Slot()
                .AutoHeight()
            [
                SNew(SHorizontalBox)

                // Text
                +SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(0, 2)
                [
                    SNew(STextBlock)
                    .Font( FEditorStyle::GetFontStyle("AssetDiscoveryIndicator.MainStatusFont") )
                    .Text( LOCTEXT("SearchResults", "Searching...") )
                    .Visibility(this, &SFindInBlueprints::GetSearchbarVisiblity)
                ]

                // Progress bar
                +SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    .Padding(2.0f, 3.0f, 0.0f, 0.0f)
                [
                    SNew(SProgressBar)
                        .Visibility(this, &SFindInBlueprints::GetSearchbarVisiblity).Percent( this, &SFindInBlueprints::GetPercentCompleteSearch )
                ]
            ]
        ];
}

void SFindInBlueprints::ConditionallyAddCacheBar()
{
    FFindInOdysseyBrushSearchManager& FindInOdysseyBrushManager = FFindInOdysseyBrushSearchManager::Get();

    // Do not add a second cache bar and do not add it when there are no uncached Blueprints
    if(FindInOdysseyBrushManager.GetNumberUncachedAssets() > 0 || FindInOdysseyBrushManager.GetFailedToCacheCount() > 0)
    {
        if(MainVerticalBox.IsValid() && !CacheBarSlot.IsValid())
        {
            // Create a single string of all the OdysseyBrush paths that failed to cache, on separate lines
            FString PackageList;
            TSet<FName> FailedToCacheList = FFindInOdysseyBrushSearchManager::Get().GetFailedToCachePathList();
            for (FName Package : FailedToCacheList)
            {
                PackageList += Package.ToString() + TEXT("\n");
            }

            // Lambda to put together the popup menu detailing the failed to cache paths
            auto OnDisplayCacheFailLambda = [](TWeakPtr<SWidget> InParentWidget, FString InPackageList)->FReply
            {
                if (InParentWidget.IsValid())
                {
                    TSharedRef<SWidget> DisplayWidget =
                        SNew(SBox)
                        .MaxDesiredHeight(512)
                        .MaxDesiredWidth(512)
                        .Content()
                        [
                            SNew(SBorder)
                            .BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
                            [
                                SNew(SScrollBox)
                                +SScrollBox::Slot()
                                [
                                    SNew(SMultiLineEditableText)
                                    .AutoWrapText(true)
                                    .IsReadOnly(true)
                                    .Text(FText::FromString(InPackageList))
                                ]
                            ]
                        ];

                    FSlateApplication::Get().PushMenu(
                        InParentWidget.Pin().ToSharedRef(),
                        FWidgetPath(),
                        DisplayWidget,
                        FSlateApplication::Get().GetCursorPos(),
                        FPopupTransitionEffect(FPopupTransitionEffect::TypeInPopup)
                        );
                }
                return FReply::Handled();
            };

            MainVerticalBox.Pin()->AddSlot()
                .AutoHeight()
                [
                    SAssignNew(CacheBarSlot, SBorder)
                    .Visibility( this, &SFindInBlueprints::GetCachingBarVisibility )
                    .BorderBackgroundColor( this, &SFindInBlueprints::GetCachingBarColor )
                    .BorderImage( FCoreStyle::Get().GetBrush("ErrorReporting.Box") )
                    .Padding( FMargin(3,1) )
                    [
                        SNew(SVerticalBox)

                        +SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew(SHorizontalBox)
                            +SHorizontalBox::Slot()
                            .VAlign(EVerticalAlignment::VAlign_Center)
                            .AutoWidth()
                            [
                                SNew(STextBlock)
                                .Text(this, &SFindInBlueprints::GetUncachedAssetWarningText)
                                .ColorAndOpacity( FCoreStyle::Get().GetColor("ErrorReporting.ForegroundColor") )
                            ]

                            // Cache All button
                            +SHorizontalBox::Slot()
                                .AutoWidth()
                                .VAlign(EVerticalAlignment::VAlign_Center)
                                .Padding(6.0f, 2.0f, 4.0f, 2.0f)
                                [
                                    SNew(SButton)
                                    .Text(LOCTEXT("IndexAllBlueprints", "Index All"))
                                    .OnClicked( this, &SFindInBlueprints::OnCacheAllBlueprints )
                                    .Visibility( this, &SFindInBlueprints::GetCacheAllButtonVisibility )
                                    .ToolTip(IDocumentation::Get()->CreateToolTip(
                                    LOCTEXT("IndexAlLBlueprints_Tooltip", "Loads all non-indexed Blueprints and saves them with their search data. This can be a very slow process and the editor may become unresponsive."),
                                    NULL,
                                    TEXT("Shared/Editors/OdysseyBrushEditor"),
                                    TEXT("FindInOdysseyBrush_IndexAll")))
                                ]


                            // View of failed OdysseyBrush paths
                            +SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(4.0f, 2.0f, 0.0f, 2.0f)
                                [
                                    SNew(SButton)
                                    .Text(LOCTEXT("ShowFailedPackages", "Show Failed Packages"))
                                    .OnClicked(FOnClicked::CreateLambda(OnDisplayCacheFailLambda, TWeakPtr<SWidget>(SharedThis(this)), PackageList))
                                    .Visibility( this, &SFindInBlueprints::GetFailedToCacheListVisibility )
                                    .ToolTip(IDocumentation::Get()->CreateToolTip(
                                    LOCTEXT("FailedCache_Tooltip", "Displays a list of packages that failed to save."),
                                    NULL,
                                    TEXT("Shared/Editors/OdysseyBrushEditor"),
                                    TEXT("FindInOdysseyBrush_FailedCache")))
                                ]

                            // Cache progress bar
                            +SHorizontalBox::Slot()
                                .FillWidth(1.0f)
                                .Padding(4.0f, 2.0f, 4.0f, 2.0f)
                                [
                                    SNew(SProgressBar)
                                    .Percent( this, &SFindInBlueprints::GetPercentCompleteCache )
                                    .Visibility( this, &SFindInBlueprints::GetCachingProgressBarVisiblity )
                                ]

                            // Cancel button
                            +SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(4.0f, 2.0f, 0.0f, 2.0f)
                                [
                                    SNew(SButton)
                                    .Text(LOCTEXT("CancelCacheAll", "Cancel"))
                                    .OnClicked( this, &SFindInBlueprints::OnCancelCacheAll )
                                    .Visibility( this, &SFindInBlueprints::GetCachingProgressBarVisiblity )
                                    .ToolTipText( LOCTEXT("CancelCacheAll_Tooltip", "Stops the caching process from where ever it is, can be started back up where it left off when needed.") )
                                ]

                            // "X" to remove the bar
                            +SHorizontalBox::Slot()
                                .HAlign(HAlign_Right)
                                [
                                    SNew(SButton)
                                    .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                                    .ContentPadding(0)
                                    .HAlign(HAlign_Center)
                                    .VAlign(VAlign_Center)
                                    .OnClicked( this, &SFindInBlueprints::OnRemoveCacheBar )
                                    .ForegroundColor( FSlateColor::UseForeground() )
                                    [
                                        SNew(SImage)
                                        .Image( FCoreStyle::Get().GetBrush("EditableComboBox.Delete") )
                                        .ColorAndOpacity( FSlateColor::UseForeground() )
                                    ]
                                ]
                        ]

                        +SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(8.0f, 0.0f, 0.0f, 2.0f)
                            [
                                SNew(SVerticalBox)
                                +SVerticalBox::Slot()
                                .AutoHeight()
                                [
                                    SNew(STextBlock)
                                    .Text(this, &SFindInBlueprints::GetCurrentCacheOdysseyBrushName)
                                    .Visibility( this, &SFindInBlueprints::GetCachingOdysseyBrushNameVisiblity )
                                    .ColorAndOpacity( FCoreStyle::Get().GetColor("ErrorReporting.ForegroundColor") )
                                ]

                                +SVerticalBox::Slot()
                                .AutoHeight()
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("FiBUnresponsiveEditorWarning", "NOTE: the editor may become unresponsive for some time!"))
                                    .TextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>( "SmallText" ))
                                ]
                            ]
                    ]
                ];
        }
    }
    else
    {
        // Because there are no uncached Blueprints, remove the bar
        OnRemoveCacheBar();
    }
}

FReply SFindInBlueprints::OnRemoveCacheBar()
{
    if(MainVerticalBox.IsValid() && CacheBarSlot.IsValid())
    {
        MainVerticalBox.Pin()->RemoveSlot(CacheBarSlot.Pin().ToSharedRef());
    }

    return FReply::Handled();
}

SFindInBlueprints::~SFindInBlueprints()
{
    if(StreamSearch.IsValid())
    {
        StreamSearch->Stop();
        StreamSearch->EnsureCompletion();
    }

    FFindInOdysseyBrushSearchManager::Get().CancelCacheAll(this);
}

EActiveTimerReturnType SFindInBlueprints::UpdateSearchResults( double InCurrentTime, float InDeltaTime )
{
    if ( StreamSearch.IsValid() )
    {
        bool bShouldShutdownThread = false;
        bShouldShutdownThread = StreamSearch->IsComplete();

        TArray<FSearchResult> BackgroundItemsFound;

        StreamSearch->GetFilteredItems( BackgroundItemsFound );
        if ( BackgroundItemsFound.Num() )
        {
            for ( auto& Item : BackgroundItemsFound )
            {
                FindInBlueprintsHelpers::ExpandAllChildren(Item, TreeView);
                ItemsFound.Add( Item );
            }
            TreeView->RequestTreeRefresh();
        }

        // If the thread is complete, shut it down properly
        if ( bShouldShutdownThread )
        {
            if ( ItemsFound.Num() == 0 )
            {
                // Insert a fake result to inform user if none found
                ItemsFound.Add( FSearchResult( new FFindInBlueprintsResult( LOCTEXT( "OdysseyBrushSearchNoResults", "No Results found" ) ) ) );
                TreeView->RequestTreeRefresh();
            }

            // Add the cache bar if needed.
            ConditionallyAddCacheBar();

            StreamSearch->EnsureCompletion();

            TArray<FImaginaryFiBDataSharedPtr> ImaginaryResults;
            if (OnSearchComplete.IsBound())
            {
                // Pull out the filtered imaginary results if there is a callback to pass them to
                StreamSearch->GetFilteredImaginaryResults(ImaginaryResults);
            }
            OutOfDateWithLastSearchBPCount = StreamSearch->GetOutOfDateCount();

            StreamSearch.Reset();

            OnSearchComplete.ExecuteIfBound(ImaginaryResults);
        }
    }

    return StreamSearch.IsValid() ? EActiveTimerReturnType::Continue : EActiveTimerReturnType::Stop;
}

void SFindInBlueprints::RegisterCommands()
{
    CommandList = OdysseyBrushEditorPtr.IsValid() ? OdysseyBrushEditorPtr.Pin()->GetToolkitCommands() : MakeShareable(new FUICommandList());

    CommandList->MapAction( FGenericCommands::Get().Copy,
        FExecuteAction::CreateSP(this, &SFindInBlueprints::OnCopyAction) );

    CommandList->MapAction( FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateSP(this, &SFindInBlueprints::OnSelectAllAction) );
}

void SFindInBlueprints::FocusForUse(bool bSetFindWithinOdysseyBrush, FString NewSearchTerms, bool bSelectFirstResult)
{
    // NOTE: Careful, GeneratePathToWidget can be reentrant in that it can call visibility delegates and such
    FWidgetPath FilterTextBoxWidgetPath;
    FSlateApplication::Get().GeneratePathToWidgetUnchecked( SearchTextField.ToSharedRef(), FilterTextBoxWidgetPath );

    // Set keyboard focus directly
    FSlateApplication::Get().SetKeyboardFocus( FilterTextBoxWidgetPath, EFocusCause::SetDirectly );

    // Set the filter mode
    bIsInFindWithinOdysseyBrushMode = bSetFindWithinOdysseyBrush;

    if (!NewSearchTerms.IsEmpty())
    {
        SearchTextField->SetText(FText::FromString(NewSearchTerms));
        MakeSearchQuery(SearchValue, bIsInFindWithinOdysseyBrushMode);

        // Select the first result
        if (bSelectFirstResult && ItemsFound.Num())
        {
            auto ItemToFocusOn = ItemsFound[0];

            // We want the first childmost item to select, as that is the item that is most-likely to be what was searched for (parents being graphs).
            // Will fail back upward as neccessary to focus on a focusable item
            while(ItemToFocusOn->Children.Num())
            {
                ItemToFocusOn = ItemToFocusOn->Children[0];
            }
            TreeView->SetSelection(ItemToFocusOn);
            ItemToFocusOn->OnClick();
        }
    }
}

void SFindInBlueprints::MakeSearchQuery(FString InSearchString, bool bInIsFindWithinOdysseyBrush, enum ESearchQueryFilter InSearchFilterForImaginaryDataReturn/* = ESearchQueryFilter::AllFilter*/, EFiBVersion InMinimiumVersionRequirement/* = EFiBVersion::FIB_VER_LATEST*/, FOnSearchComplete InOnSearchComplete/* = FOnSearchComplete()*/)
{
    SearchTextField->SetText(FText::FromString(InSearchString));
    LastSearchedFiBVersion = InMinimiumVersionRequirement;

    if(ItemsFound.Num())
    {
        // Reset the scroll to the top
        TreeView->RequestScrollIntoView(ItemsFound[0]);
    }

    ItemsFound.Empty();

    if (InSearchString.Len() > 0)
    {
        OnRemoveCacheBar();

        TreeView->RequestTreeRefresh();
        HighlightText = FText::FromString( InSearchString );

        if (bInIsFindWithinOdysseyBrush)
        {
            if(StreamSearch.IsValid() && !StreamSearch->IsComplete())
            {
                StreamSearch->Stop();
                StreamSearch->EnsureCompletion();
                OutOfDateWithLastSearchBPCount = StreamSearch->GetOutOfDateCount();
                StreamSearch.Reset();
            }

            UBlueprint* OdysseyBrush = OdysseyBrushEditorPtr.Pin()->GetBlueprintObj();
            FString ParentClass;
            if (UProperty* ParentClassProp = OdysseyBrush->GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UBlueprint, ParentClass)))
            {
                ParentClassProp->ExportTextItem(ParentClass, ParentClassProp->ContainerPtrToValuePtr<uint8>(OdysseyBrush), nullptr, OdysseyBrush, 0);
            }

            TArray<FString> Interfaces;

            for (FBPInterfaceDescription& InterfaceDesc : OdysseyBrush->ImplementedInterfaces)
            {
                Interfaces.Add(InterfaceDesc.Interface->GetPathName());
            }
            FImaginaryFiBDataSharedPtr ImaginaryOdysseyBrush(new FImaginaryOdysseyBrush(OdysseyBrush->GetName(), OdysseyBrush->GetPathName(), ParentClass, Interfaces, FFindInOdysseyBrushSearchManager::Get().QuerySingleOdysseyBrush(OdysseyBrush)));
            TSharedPtr< FFiBSearchInstance > SearchInstance(new FFiBSearchInstance);
            FSearchResult SearchResult = RootSearchResult = SearchInstance->StartSearchQuery(SearchValue, ImaginaryOdysseyBrush);

            if (SearchResult.IsValid())
            {
                ItemsFound = SearchResult->Children;
            }

            if(ItemsFound.Num() == 0)
            {
                // Insert a fake result to inform user if none found
                ItemsFound.Add(FSearchResult(new FFindInBlueprintsResult(LOCTEXT("OdysseyBrushSearchNoResults", "No Results found"))));
                HighlightText = FText::GetEmpty();
            }
            else
            {
                for(auto Item : ItemsFound)
                {
                    FindInBlueprintsHelpers::ExpandAllChildren(Item, TreeView);
                }
            }

            TreeView->RequestTreeRefresh();
        }
        else
        {
            LaunchStreamThread(InSearchString, InSearchFilterForImaginaryDataReturn, InMinimiumVersionRequirement, InOnSearchComplete);
        }

        bHasGlobalSearchResults = !bInIsFindWithinOdysseyBrush;
    }
}

void SFindInBlueprints::OnSearchTextChanged( const FText& Text)
{
    SearchValue = Text.ToString();
}

void SFindInBlueprints::OnSearchTextCommitted( const FText& Text, ETextCommit::Type CommitType )
{
    if (CommitType == ETextCommit::OnEnter)
    {
        MakeSearchQuery(SearchValue, bIsInFindWithinOdysseyBrushMode);
    }
}

void SFindInBlueprints::OnFindModeChanged(ECheckBoxState CheckState)
{
    bIsInFindWithinOdysseyBrushMode = CheckState == ECheckBoxState::Checked;
}

ECheckBoxState SFindInBlueprints::OnGetFindModeChecked() const
{
    return bIsInFindWithinOdysseyBrushMode ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SFindInBlueprints::LaunchStreamThread(const FString& InSearchValue)
{
    if(StreamSearch.IsValid() && !StreamSearch->IsComplete())
    {
        StreamSearch->Stop();
        StreamSearch->EnsureCompletion();
    }
    else
    {
        // If the stream search wasn't already running, register the active timer
        RegisterActiveTimer( 0.f, FWidgetActiveTimerDelegate::CreateSP( this, &SFindInBlueprints::UpdateSearchResults ) );
    }

    StreamSearch = MakeShareable(new FStreamSearch(InSearchValue));
    OnSearchComplete = FOnSearchComplete();
}

void SFindInBlueprints::LaunchStreamThread(const FString& InSearchValue, enum ESearchQueryFilter InSearchFilterForRawDataReturn, EFiBVersion InMinimiumVersionRequirement, FOnSearchComplete InOnSearchComplete)
{
    if(StreamSearch.IsValid() && !StreamSearch->IsComplete())
    {
        StreamSearch->Stop();
        StreamSearch->EnsureCompletion();
    }
    else
    {
        // If the stream search wasn't already running, register the active timer
        RegisterActiveTimer( 0.f, FWidgetActiveTimerDelegate::CreateSP( this, &SFindInBlueprints::UpdateSearchResults ) );
    }

    StreamSearch = MakeShareable(new FStreamSearch(InSearchValue, InSearchFilterForRawDataReturn, InMinimiumVersionRequirement));
    OnSearchComplete = InOnSearchComplete;
}

TSharedRef<ITableRow> SFindInBlueprints::OnGenerateRow( FSearchResult InItem, const TSharedRef<STableViewBase>& OwnerTable )
{
    // Finalize the search data, this does some non-thread safe actions that could not be done on the separate thread.
    InItem->FinalizeSearchData();

    bool bIsACategoryWidget = !bIsInFindWithinOdysseyBrushMode && !InItem->Parent.IsValid();

    if (bIsACategoryWidget)
    {
        return SNew( STableRow< TSharedPtr<FFindInBlueprintsResult> >, OwnerTable )
            [
                SNew(SBorder)
                .VAlign(VAlign_Center)
                .BorderImage(FEditorStyle::GetBrush("PropertyWindow.CategoryBackground"))
                .Padding(FMargin(2.0f))
                .ForegroundColor(FEditorStyle::GetColor("PropertyWindow.CategoryForeground"))
                [
                    SNew(STextBlock)
                    .Text(InItem.Get(), &FFindInBlueprintsResult::GetDisplayString)
                    .ToolTipText(LOCTEXT("OdysseyBrushCatSearchToolTip", "OdysseyBrush"))
                ]
            ];
    }
    else // Functions/Event/Pin widget
    {
        FText CommentText = FText::GetEmpty();

        if(!InItem->GetCommentText().IsEmpty())
        {
            FFormatNamedArguments Args;
            Args.Add(TEXT("Comment"), FText::FromString(InItem->GetCommentText()));

            CommentText = FText::Format(LOCTEXT("NodeComment", "Node Comment:[{Comment}]"), Args);
        }

        FFormatNamedArguments Args;
        Args.Add(TEXT("Category"), InItem->GetCategory());
        Args.Add(TEXT("DisplayTitle"), InItem->DisplayText);

        FText Tooltip = FText::Format(LOCTEXT("OdysseyBrushResultSearchToolTip", "{Category} : {DisplayTitle}"), Args);

        return SNew( STableRow< TSharedPtr<FFindInBlueprintsResult> >, OwnerTable )
            [
                SNew(SHorizontalBox)
                +SHorizontalBox::Slot()
                .VAlign(VAlign_Center)
                .AutoWidth()
                [
                    InItem->CreateIcon()
                ]
                +SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                .Padding(2,0)
                [
                    SNew(STextBlock)
                        .Text(InItem.Get(), &FFindInBlueprintsResult::GetDisplayString)
                        .HighlightText(HighlightText)
                        .ToolTipText(Tooltip)
                ]
                +SHorizontalBox::Slot()
                .FillWidth(1)
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Center)
                .Padding(2,0)
                [
                    SNew(STextBlock)
                    .Text( CommentText )
                    .ColorAndOpacity(FLinearColor::Yellow)
                    .HighlightText(HighlightText)
                ]
            ];
    }
}

void SFindInBlueprints::OnGetChildren( FSearchResult InItem, TArray< FSearchResult >& OutChildren )
{
    OutChildren += InItem->Children;
}

void SFindInBlueprints::OnTreeSelectionDoubleClicked( FSearchResult Item )
{
    if(Item.IsValid())
    {
        Item->OnClick();
    }
}

TOptional<float> SFindInBlueprints::GetPercentCompleteSearch() const
{
    if(StreamSearch.IsValid())
    {
        return StreamSearch->GetPercentComplete();
    }
    return 0.0f;
}

EVisibility SFindInBlueprints::GetSearchbarVisiblity() const
{
    return StreamSearch.IsValid()? EVisibility::Visible : EVisibility::Collapsed;
}

void SFindInBlueprints::CacheAllBlueprints(FSimpleDelegate InOnFinished/* = FSimpleDelegate()*/, EFiBVersion InMinimiumVersionRequirement/* = EFiBVersion::FIB_VER_LATEST*/)
{
    OnCacheAllBlueprints(InOnFinished, InMinimiumVersionRequirement);
}

FReply SFindInBlueprints::OnCacheAllBlueprints()
{
    return OnCacheAllBlueprints(FSimpleDelegate(), LastSearchedFiBVersion);
}

FReply SFindInBlueprints::OnCacheAllBlueprints(FSimpleDelegate InOnFinished/* = FSimpleDelegate()*/, EFiBVersion InMinimiumVersionRequirement/* = EFiBVersion::FIB_VER_LATEST*/)
{
    if(!FFindInOdysseyBrushSearchManager::Get().IsCacheInProgress())
    {
        // Request from the SearchManager a delegate to use for ticking the cache system.
        FWidgetActiveTimerDelegate WidgetActiveTimer;
        FFindInOdysseyBrushSearchManager::Get().CacheAllUncachedAssets(SharedThis(this), WidgetActiveTimer, InOnFinished, InMinimiumVersionRequirement);
        RegisterActiveTimer(0.f, WidgetActiveTimer);
    }

    return FReply::Handled();
}

FReply SFindInBlueprints::OnCancelCacheAll()
{
    FFindInOdysseyBrushSearchManager::Get().CancelCacheAll(this);

    // Resubmit the last search
    OnSearchTextCommitted(SearchTextField->GetText(), ETextCommit::OnEnter);

    return FReply::Handled();
}

int32 SFindInBlueprints::GetCurrentCacheIndex() const
{
    return FFindInOdysseyBrushSearchManager::Get().GetCurrentCacheIndex();
}

TOptional<float> SFindInBlueprints::GetPercentCompleteCache() const
{
    return FFindInOdysseyBrushSearchManager::Get().GetCacheProgress();
}

EVisibility SFindInBlueprints::GetCachingProgressBarVisiblity() const
{
    return IsCacheInProgress()? EVisibility::Visible : EVisibility::Hidden;
}

EVisibility SFindInBlueprints::GetCacheAllButtonVisibility() const
{
    return IsCacheInProgress()? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility SFindInBlueprints::GetCachingBarVisibility() const
{
    FFindInOdysseyBrushSearchManager& FindInOdysseyBrushManager = FFindInOdysseyBrushSearchManager::Get();
    return (FindInOdysseyBrushManager.GetNumberUncachedAssets() > 0 || FindInOdysseyBrushManager.GetFailedToCacheCount())? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility SFindInBlueprints::GetCachingOdysseyBrushNameVisiblity() const
{
    return IsCacheInProgress()? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility SFindInBlueprints::GetFailedToCacheListVisibility() const
{
    return FFindInOdysseyBrushSearchManager::Get().GetFailedToCacheCount()? EVisibility::Visible : EVisibility::Collapsed;
}

bool SFindInBlueprints::IsCacheInProgress() const
{
    return FFindInOdysseyBrushSearchManager::Get().IsCacheInProgress();
}

FSlateColor SFindInBlueprints::GetCachingBarColor() const
{
    // The caching bar's default color is a darkish red
    FSlateColor ReturnColor = FSlateColor(FLinearColor(0.4f, 0.0f, 0.0f));
    if(IsCacheInProgress())
    {
        // It turns yellow when in progress
        ReturnColor = FSlateColor(FLinearColor(0.4f, 0.4f, 0.0f));
    }
    return ReturnColor;
}

FText SFindInBlueprints::GetUncachedAssetWarningText() const
{
    FFindInOdysseyBrushSearchManager& FindInOdysseyBrushManager = FFindInOdysseyBrushSearchManager::Get();

    int32 FailedToCacheCount = FindInOdysseyBrushManager.GetFailedToCacheCount();

    // The number of unindexed Blueprints is the total of those that failed to cache and those that haven't been attempted yet.
    FFormatNamedArguments Args;
    Args.Add(TEXT("Count"), FindInOdysseyBrushManager.GetNumberUncachedAssets() + OutOfDateWithLastSearchBPCount);

    FText ReturnDisplayText;
    if(IsCacheInProgress())
    {
        Args.Add(TEXT("CurrentIndex"), FindInOdysseyBrushManager.GetCurrentCacheIndex());

        ReturnDisplayText = FText::Format(LOCTEXT("CachingBlueprints", "Indexing Blueprints... {CurrentIndex}/{Count}"), Args);
    }
    else
    {
        Args.Add(TEXT("UnindexedCount"), FindInOdysseyBrushManager.GetNumberUncachedAssets());
        Args.Add(TEXT("OutOfDateCount"), OutOfDateWithLastSearchBPCount);

        ReturnDisplayText = FText::Format(LOCTEXT("UncachedAssets", "Search incomplete. {Count} ({UnindexedCount} Unindexed/{OutOfDateCount} Out-of-Date) Blueprints need to be indexed!"), Args);

        if (FailedToCacheCount > 0)
        {
            FFormatNamedArguments ArgsWithCacheFails;
            Args.Add(TEXT("BaseMessage"), ReturnDisplayText);
            Args.Add(TEXT("CacheFails"), FailedToCacheCount);
            ReturnDisplayText = FText::Format(LOCTEXT("UncachedAssetsWithCacheFails", "{BaseMessage} {CacheFails} Blueprints failed to cache."), Args);
        }
    }

    return ReturnDisplayText;
}

FText SFindInBlueprints::GetCurrentCacheOdysseyBrushName() const
{
    return FText::FromName(FFindInOdysseyBrushSearchManager::Get().GetCurrentCacheOdysseyBrushName());
}

void SFindInBlueprints::OnCacheComplete()
{
    // Resubmit the last search, which will also remove the bar if needed
    OnSearchTextCommitted(SearchTextField->GetText(), ETextCommit::OnEnter);
}

TSharedPtr<SWidget> SFindInBlueprints::OnContextMenuOpening()
{
    const bool bShouldCloseWindowAfterMenuSelection = true;
    FMenuBuilder MenuBuilder( bShouldCloseWindowAfterMenuSelection, CommandList);

    MenuBuilder.BeginSection("BasicOperations");
    {
        MenuBuilder.AddMenuEntry(FGenericCommands::Get().SelectAll);
        MenuBuilder.AddMenuEntry(FGenericCommands::Get().Copy);
    }

    return MenuBuilder.MakeWidget();
}

void SFindInBlueprints::SelectAllItemsHelper(FSearchResult InItemToSelect)
{
    // Iterates over all children and recursively selects all items in the results
    TreeView->SetItemSelection(InItemToSelect, true);

    for( const auto Child : InItemToSelect->Children )
    {
        SelectAllItemsHelper(Child);
    }
}

void SFindInBlueprints::OnSelectAllAction()
{
    for( const auto Item : ItemsFound )
    {
        SelectAllItemsHelper(Item);
    }
}

void SFindInBlueprints::OnCopyAction()
{
    TArray< FSearchResult > SelectedItems = TreeView->GetSelectedItems();

    FString SelectedText;

    for( const auto SelectedItem : SelectedItems)
    {
        // Add indents for each layer into the tree the item is
        for(auto ParentItem = SelectedItem->Parent; ParentItem.IsValid(); ParentItem = ParentItem.Pin()->Parent)
        {
            SelectedText += TEXT("\t");
        }

        // Add the display string
        SelectedText += SelectedItem->GetDisplayString().ToString();

        // If there is a comment, add two indents and then the comment
        FString CommentText = SelectedItem->GetCommentText();
        if(!CommentText.IsEmpty())
        {
            SelectedText += TEXT("\t\t") + CommentText;
        }

        // Line terminator so the next item will be on a new line
        SelectedText += LINE_TERMINATOR;
    }

    // Copy text to clipboard
    FPlatformApplicationMisc::ClipboardCopy( *SelectedText );
}

FReply SFindInBlueprints::OnOpenGlobalFindResults()
{
    TSharedPtr<SFindInBlueprints> GlobalFindResults = FFindInOdysseyBrushSearchManager::Get().GetGlobalFindResults();
    if (GlobalFindResults.IsValid())
    {
        GlobalFindResults->FocusForUse(false, SearchValue, true);
    }

    return FReply::Handled();
}

void SFindInBlueprints::OnHostTabClosed(TSharedRef<SDockTab> DockTab)
{
    FFindInOdysseyBrushSearchManager::Get().GlobalFindResultsClosed(SharedThis(this));
}

FReply SFindInBlueprints::OnLockButtonClicked()
{
    bIsLocked = !bIsLocked;
    return FReply::Handled();
}

const FSlateBrush* SFindInBlueprints::OnGetLockButtonImage() const
{
    if (bIsLocked)
    {
        return FEditorStyle::GetBrush("FindResults.LockButton_Locked");
    }
    else
    {
        return FEditorStyle::GetBrush("FindResults.LockButton_Unlocked");
    }
}

FName SFindInBlueprints::GetHostTabId() const
{
    TSharedPtr<SDockTab> HostTabPtr = HostTab.Pin();
    if (HostTabPtr.IsValid())
    {
        return HostTabPtr->GetLayoutIdentifier().TabType;
    }

    return NAME_None;
}

void SFindInBlueprints::CloseHostTab()
{
    TSharedPtr<SDockTab> HostTabPtr = HostTab.Pin();
    if (HostTabPtr.IsValid())
    {
        HostTabPtr->RequestCloseTab();
    }
}

#undef LOCTEXT_NAMESPACE
