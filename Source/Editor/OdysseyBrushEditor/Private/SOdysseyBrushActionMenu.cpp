// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.


#include "SOdysseyBrushActionMenu.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBox.h"
#include "SGraphActionMenu.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "EditorStyleSet.h"
#include "Editor/EditorPerProjectUserSettings.h"
#include "EdGraphSchema_OdysseyBrush.h"
#include "SOdysseyBrushPalette.h"
#include "OdysseyBrushEditor.h"
#include "SMyOdysseyBrush.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "OdysseyBrushActionMenuBuilder.h"
#include "BlueprintActionFilter.h"
#include "OdysseyBrushActionMenuUtils.h"
#include "BlueprintPaletteFavorites.h"
#include "IDocumentation.h"
#include "SOdysseySCSEditor.h"
#include "SOdysseyBrushContextTargetMenu.h"

#define LOCTEXT_NAMESPACE "SOdysseyBrushActionMenu"

/** Action to promote a pin to a variable */
USTRUCT()
struct FOdysseyBrushAction_PromoteVariable : public FEdGraphSchemaAction
{
    FOdysseyBrushAction_PromoteVariable(bool bInToMemberVariable)
        : FEdGraphSchemaAction(    FText(),
                                bInToMemberVariable? LOCTEXT("PromoteToVariable", "Promote to variable") : LOCTEXT("PromoteToLocalVariable", "Promote to local variable"),
                                bInToMemberVariable ? LOCTEXT("PromoteToVariable", "Promote to variable") : LOCTEXT("PromoteToLocalVariable", "Promote to local variable"),
                                1)
        , bToMemberVariable(bInToMemberVariable)
    {
    }

    // FEdGraphSchemaAction interface
    virtual UEdGraphNode* PerformAction( class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override
    {
        if( ( ParentGraph != NULL ) && ( FromPin != NULL ) )
        {
            UBlueprint* OdysseyBrush = FBlueprintEditorUtils::FindBlueprintForGraph(ParentGraph);
            if( ( MyOdysseyBrushEditor.IsValid() == true ) && ( OdysseyBrush != NULL ) )
            {
                MyOdysseyBrushEditor.Pin()->DoPromoteToVariable( OdysseyBrush, FromPin, bToMemberVariable );
            }
        }
        return NULL;
    }
    // End of FEdGraphSchemaAction interface

    /* Pointer to the blueprint editor containing the blueprint in which we will promote the variable. */
    TWeakPtr<class FOdysseyBrushEditor> MyOdysseyBrushEditor;

    /* TRUE if promoting to member variable, FALSE if promoting to local variable */
    bool bToMemberVariable;
};

/**
 * Static method for binding with delegates. Spawns an instance of the custom
 * expander.
 *
 * @param  ActionMenuData    A set of useful data for detailing the specific action menu row this is for.
 * @return A new widget, intended to lead entries in an SGraphActionMenu.
 */
static TSharedRef<SExpanderArrow> CreateCustomOdysseyBrushActionExpander(const FCustomExpanderData& ActionMenuData)
{
    return SNew(SOdysseyBrushActionMenuExpander, ActionMenuData);
}

/*******************************************************************************
* SOdysseyBrushActionFavoriteToggle
*******************************************************************************/

class SOdysseyBrushActionFavoriteToggle : public SCompoundWidget
{
    SLATE_BEGIN_ARGS( SOdysseyBrushActionFavoriteToggle ) {}
    SLATE_END_ARGS()

public:
    /**
     * Constructs a favorite-toggle widget (so that user can easily modify the
     * item's favorited state).
     *
     * @param  InArgs            A set of slate arguments, defined above.
     * @param  ActionPtrIn        The FEdGraphSchemaAction that the parent item represents.
     * @param  OdysseyBrushEdPtrIn    A pointer to the blueprint editor that the palette belongs to.
     */
    void Construct(const FArguments& InArgs, const FCustomExpanderData& CustomExpanderData)
    {
        Container = CustomExpanderData.WidgetContainer;
        ActionPtr = CustomExpanderData.RowAction;

        ChildSlot
        [
            SNew(SHorizontalBox)
            +SHorizontalBox::Slot()
                .VAlign(VAlign_Fill)
                .HAlign(HAlign_Center)
                .FillWidth(1.0)
            [
                SNew( SCheckBox )
                    .Visibility(this, &SOdysseyBrushActionFavoriteToggle::IsVisibile)
                    .ToolTipText(this, &SOdysseyBrushActionFavoriteToggle::GetToolTipText)
                    .IsChecked(this, &SOdysseyBrushActionFavoriteToggle::GetFavoritedState)
                    .OnCheckStateChanged(this, &SOdysseyBrushActionFavoriteToggle::OnFavoriteToggled)
                    .Style(FEditorStyle::Get(), "Kismet.Palette.FavoriteToggleStyle")
            ]
        ];
    }

private:
    /**
     * Used to determine the toggle's visibility (this is only visible when the
     * owning item is being hovered over, and the associated action can be favorited).
     *
     * @return True if this toggle switch should be showing, false if not.
     */
    EVisibility IsVisibile() const
    {
        bool bNoFavorites = false;
        GConfig->GetBool(TEXT("OdysseyBrushEditor.Palette"), TEXT("bUseLegacyLayout"), bNoFavorites, GEditorIni);

        UBlueprintPaletteFavorites const* const OdysseyBrushFavorites = GetDefault<UEditorPerProjectUserSettings>()->BlueprintFavorites;

        EVisibility CurrentVisibility = EVisibility::Hidden;
        if (!bNoFavorites && OdysseyBrushFavorites && OdysseyBrushFavorites->CanBeFavorited(ActionPtr.Pin()))
        {
            if (OdysseyBrushFavorites->IsFavorited(ActionPtr.Pin()) || Container->IsHovered())
            {
                CurrentVisibility = EVisibility::Visible;
            }
        }

        return CurrentVisibility;
    }

    /**
     * Retrieves tooltip that describes the current favorited state of the
     * associated action.
     *
     * @return Text describing what this toggle will do when you click on it.
     */
    FText GetToolTipText() const
    {
        if (GetFavoritedState() == ECheckBoxState::Checked)
        {
            return LOCTEXT("Unfavorite", "Click to remove this item from your favorites.");
        }
        return LOCTEXT("Favorite", "Click to add this item to your favorites.");
    }

    /**
     * Checks on the associated action's favorite state, and returns a
     * corresponding checkbox state to match.
     *
     * @return ECheckBoxState::Checked if the associated action is already favorited, ECheckBoxState::Unchecked if not.
     */
    ECheckBoxState GetFavoritedState() const
    {
        ECheckBoxState FavoriteState = ECheckBoxState::Unchecked;
        if (ActionPtr.IsValid())
        {
            const UEditorPerProjectUserSettings& EditorSettings = *GetDefault<UEditorPerProjectUserSettings>();
            if (UBlueprintPaletteFavorites* OdysseyBrushFavorites = EditorSettings.BlueprintFavorites)
            {
                FavoriteState = OdysseyBrushFavorites->IsFavorited(ActionPtr.Pin()) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
            }
        }
        return FavoriteState;
    }

    /**
     * Triggers when the user clicks this toggle, adds or removes the associated
     * action to the user's favorites.
     *
     * @param  InNewState    The new state that the user set the checkbox to.
     */
    void OnFavoriteToggled(ECheckBoxState InNewState)
    {
        if (InNewState == ECheckBoxState::Checked)
        {
            GetMutableDefault<UEditorPerProjectUserSettings>()->BlueprintFavorites->AddFavorite(ActionPtr.Pin());
        }
        else
        {
            GetMutableDefault<UEditorPerProjectUserSettings>()->BlueprintFavorites->RemoveFavorite(ActionPtr.Pin());
        }
    }

private:
    /** The action that the owning palette entry represents */
    TWeakPtr<FEdGraphSchemaAction> ActionPtr;

    /** The widget that this widget is nested inside */
    TSharedPtr<SPanel> Container;
};

/*******************************************************************************
* SOdysseyBrushActionMenu
*******************************************************************************/

SOdysseyBrushActionMenu::~SOdysseyBrushActionMenu()
{
    OnClosedCallback.ExecuteIfBound();
    OnCloseReasonCallback.ExecuteIfBound(bActionExecuted, ContextToggleIsChecked() == ECheckBoxState::Checked, DraggedFromPins.Num() > 0);
}

void SOdysseyBrushActionMenu::Construct( const FArguments& InArgs, TSharedPtr<FOdysseyBrushEditor> InEditor )
{
    bActionExecuted = false;

    this->GraphObj = InArgs._GraphObj;
    this->DraggedFromPins = InArgs._DraggedFromPins;
    this->NewNodePosition = InArgs._NewNodePosition;
    this->OnClosedCallback = InArgs._OnClosedCallback;
    this->bAutoExpandActionMenu = InArgs._AutoExpandActionMenu;
    this->EditorPtr = InEditor;
    this->OnCloseReasonCallback = InArgs._OnCloseReason;

    // Generate the context display; showing the user what they're picking something for
    //@TODO: Should probably be somewhere more schema-sensitive than the graph panel!
    FSlateColor TypeColor;
    FString TypeOfDisplay;
    const FSlateBrush* ContextIcon = nullptr;

    if (DraggedFromPins.Num() == 1)
    {
        UEdGraphPin* OnePin = DraggedFromPins[0];

        const UEdGraphSchema* Schema = OnePin->GetSchema();
        const UEdGraphSchema_OdysseyBrush* K2Schema = GetDefault<UEdGraphSchema_OdysseyBrush>();

        if (!Schema->IsA(UEdGraphSchema_OdysseyBrush::StaticClass()) || !K2Schema->IsExecPin(*OnePin))
        {
            // Get the type color and icon
            TypeColor = Schema->GetPinTypeColor(OnePin->PinType);
            ContextIcon = FEditorStyle::GetBrush( OnePin->PinType.IsArray() ? TEXT("Graph.ArrayPin.Connected") : TEXT("Graph.Pin.Connected") );
        }
    }

    FBlueprintActionContext MenuContext;
    ConstructActionContext(MenuContext);

    TSharedPtr<SComboButton> TargetContextSubMenuButton;

    // @TODO: would be nice if we could use a checkbox style for this, and have a different state for open/closed
    SAssignNew(TargetContextSubMenuButton, SComboButton)
        .MenuPlacement(MenuPlacement_MenuRight)
        .HasDownArrow(false)
        .ButtonStyle(FEditorStyle::Get(), "OdysseyBrushEditor.ContextMenu.TargetsButton")
        .MenuContent()
        [
            SAssignNew(ContextTargetSubMenu, SOdysseyBrushContextTargetMenu, MenuContext)
                .OnTargetMaskChanged(this, &SOdysseyBrushActionMenu::OnContextTargetsChanged)
        ];

    // Build the widget layout
    SBorder::Construct( SBorder::FArguments()
        .BorderImage( FEditorStyle::GetBrush("Menu.Background") )
        .Padding(5)
        [
            // Achieving fixed width by nesting items within a fixed width box.
            SNew(SBox)
            .WidthOverride(400)
            .HeightOverride(400)
            [
                SNew(SVerticalBox)

                // TYPE OF SEARCH INDICATOR
                +SVerticalBox::Slot()
                .AutoHeight()
                .Padding(2, 2, 2, 5)
                [
                    SNew(SHorizontalBox)

                    // Type pill
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    .Padding(0, 0, (ContextIcon != NULL) ? 5 : 0, 0)
                    [
                        SNew(SImage)
                        .ColorAndOpacity(TypeColor)
                        .Visibility(this, &SOdysseyBrushActionMenu::GetTypeImageVisibility)
                        .Image(ContextIcon)
                    ]

                    // Search context description
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    [
                        SNew(STextBlock)
                        .Text(this, &SOdysseyBrushActionMenu::GetSearchContextDesc)
                        .Font(FEditorStyle::GetFontStyle(FName("OdysseyBrushEditor.ActionMenu.ContextDescriptionFont")))
                        .ToolTip(IDocumentation::Get()->CreateToolTip(
                            LOCTEXT("OdysseyBrushActionMenuContextTextTooltip", "Describes the current context of the action list"),
                            NULL,
                            TEXT("Shared/Editors/OdysseyBrushEditor"),
                            TEXT("OdysseyBrushActionMenuContextText")))
                        .WrapTextAt(280)
                    ]

                    // Context Toggle
                    +SHorizontalBox::Slot()
                    .HAlign(HAlign_Right)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SCheckBox)
                        .OnCheckStateChanged(this, &SOdysseyBrushActionMenu::OnContextToggleChanged)
                        .IsChecked(this, &SOdysseyBrushActionMenu::ContextToggleIsChecked)
                        .ToolTip(IDocumentation::Get()->CreateToolTip(
                            LOCTEXT("OdysseyBrushActionMenuContextToggleTooltip", "Should the list be filtered to only actions that make sense in the current context?"),
                            NULL,
                            TEXT("Shared/Editors/OdysseyBrushEditor"),
                            TEXT("OdysseyBrushActionMenuContextToggle")))
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("OdysseyBrushActionMenuContextToggle", "Context Sensitive"))
                        ]
                    ]

                    +SHorizontalBox::Slot()
                    .HAlign(HAlign_Right)
                    .VAlign(VAlign_Center)
                    .AutoWidth()
                    .Padding(3.f, 0.f, 0.f, 0.f)
                    [
                        TargetContextSubMenuButton.ToSharedRef()
                    ]
                ]

                // ACTION LIST
                +SVerticalBox::Slot()
                [
                    SAssignNew(GraphActionMenu, SGraphActionMenu)
                        .OnActionSelected(this, &SOdysseyBrushActionMenu::OnActionSelected)
                        .OnCreateWidgetForAction(SGraphActionMenu::FOnCreateWidgetForAction::CreateSP(this, &SOdysseyBrushActionMenu::OnCreateWidgetForAction))
                        .OnCollectAllActions(this, &SOdysseyBrushActionMenu::CollectAllActions)
                        .OnCreateCustomRowExpander_Static(&CreateCustomOdysseyBrushActionExpander)
                ]
            ]
        ]
    );
}

EVisibility SOdysseyBrushActionMenu::GetTypeImageVisibility() const
{
    if (DraggedFromPins.Num() == 1 && EditorPtr.Pin()->GetIsContextSensitive())
    {
        UEdGraphPin* OnePin = DraggedFromPins[0];

        const UEdGraphSchema* Schema = OnePin->GetSchema();
        const UEdGraphSchema_OdysseyBrush* K2Schema = GetDefault<UEdGraphSchema_OdysseyBrush>();

        if (!Schema->IsA(UEdGraphSchema_OdysseyBrush::StaticClass()) || !K2Schema->IsExecPin(*OnePin))
        {
            return EVisibility::Visible;
        }
    }
    return EVisibility::Collapsed;
}

FText SOdysseyBrushActionMenu::GetSearchContextDesc() const
{
    bool bIsContextSensitive = EditorPtr.Pin()->GetIsContextSensitive();
    bool bHasPins = DraggedFromPins.Num() > 0;
    if (!bIsContextSensitive)
    {
        return LOCTEXT("MenuPrompt_AllPins", "All Possible Actions");
    }
    else if (!bHasPins)
    {
        return LOCTEXT("MenuPrompt_OdysseyBrushActions", "All Actions for this OdysseyBrush");
    }
    else if (DraggedFromPins.Num() == 1)
    {
        UEdGraphPin* OnePin = DraggedFromPins[0];

        const UEdGraphSchema* Schema = OnePin->GetSchema();
        const UEdGraphSchema_OdysseyBrush* K2Schema = GetDefault<UEdGraphSchema_OdysseyBrush>();

        if (Schema->IsA(UEdGraphSchema_OdysseyBrush::StaticClass()) && K2Schema->IsExecPin(*OnePin))
        {
            return LOCTEXT("MenuPrompt_ExecPin", "Executable actions");
        }
        else
        {
            // Get the type string
            const FString TypeStringRaw = UEdGraphSchema_OdysseyBrush::TypeToText(OnePin->PinType).ToString();

            //@TODO: Add a parameter to TypeToText indicating the kind of formating requested
            const FString TypeString = (TypeStringRaw.Replace(TEXT("'"), TEXT(" "))).TrimEnd();

            if (OnePin->Direction == EGPD_Input)
            {
                return FText::Format(LOCTEXT("MenuPrompt_InputPin", "Actions providing a(n) {0}"), FText::FromString(TypeString));
            }
            else
            {
                return FText::Format(LOCTEXT("MenuPrompt_OutputPin", "Actions taking a(n) {0}"), FText::FromString(TypeString));
            }
        }
    }
    else
    {
        return FText::Format(LOCTEXT("MenuPrompt_ManyPins", "Actions for {0} pins"), FText::AsNumber(DraggedFromPins.Num()));
    }
}

void SOdysseyBrushActionMenu::OnContextToggleChanged(ECheckBoxState CheckState)
{
    EditorPtr.Pin()->GetIsContextSensitive() = CheckState == ECheckBoxState::Checked;
    GraphActionMenu->RefreshAllActions(true, false);
}

void SOdysseyBrushActionMenu::OnContextTargetsChanged(uint32 /*ContextTargetMask*/)
{
    GraphActionMenu->RefreshAllActions(/*bPreserveExpansion =*/true, /*bHandleOnSelectionEvent =*/false);
}

ECheckBoxState SOdysseyBrushActionMenu::ContextToggleIsChecked() const
{
    return EditorPtr.Pin()->GetIsContextSensitive() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SOdysseyBrushActionMenu::CollectAllActions(FGraphActionListBuilderBase& OutAllActions)
{
    check(EditorPtr.IsValid());
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditor = EditorPtr.Pin();
    bool const bIsContextSensitive = OdysseyBrushEditor->GetIsContextSensitive();

    uint32 ContextTargetMask = 0;
    /*
    if (bIsContextSensitive && ContextTargetSubMenu.IsValid())
    {
        ContextTargetMask = ContextTargetSubMenu->GetContextTargetMask();
    }
    */

    FBlueprintActionContext FilterContext;
    ConstructActionContext(FilterContext);

    FOdysseyBrushActionMenuBuilder MenuBuilder(EditorPtr);
    // NOTE: cannot call GetGraphContextActions() during serialization and GC due to its use of FindObject()
    if(!GIsSavingPackage && !IsGarbageCollecting() && FilterContext.Blueprints.Num() > 0)
    {
        FOdysseyBrushActionMenuUtils::MakeContextMenu(FilterContext, bIsContextSensitive, ContextTargetMask, MenuBuilder);
    }
    // copy the added options back to the main list
    OutAllActions.Append(MenuBuilder); // @TODO: Avoid this copy
    // also try adding promote to variable if we can do so.
    TryInsertPromoteToVariable(FilterContext, OutAllActions);
}

void SOdysseyBrushActionMenu::ConstructActionContext(FBlueprintActionContext& ContextDescOut)
{
    check(EditorPtr.IsValid());
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditor = EditorPtr.Pin();
    bool const bIsContextSensitive = OdysseyBrushEditor->GetIsContextSensitive();

    // we still want context from the graph (even if the user has unchecked
    // "Context Sensitive"), otherwise the user would be presented with nodes
    // that can't be placed in the graph... if the user isn't being presented
    // with a valid node, then fix it up in filtering
    ContextDescOut.Graphs.Add(GraphObj);

    UBlueprint* OdysseyBrush = OdysseyBrushEditor->GetBlueprintObj();
    const bool bOdysseyBrushIsValid = IsValid(OdysseyBrush) && OdysseyBrush->GeneratedClass && (OdysseyBrush->GeneratedClass->ClassGeneratedBy == OdysseyBrush);
    if (!ensure(bOdysseyBrushIsValid))  // to track UE-11597 and UE-11595
    {
        return;
    }

    ContextDescOut.Blueprints.Add(OdysseyBrush);

    if (bIsContextSensitive)
    {
        ContextDescOut.Pins = DraggedFromPins;

        // Get selection from the "My OdysseyBrush" view.
        FEdGraphSchemaAction_K2Var* SelectedVar = OdysseyBrushEditor->GetMyOdysseyBrushWidget()->SelectionAsVar();
        if ((SelectedVar != nullptr) && (SelectedVar->GetProperty() != nullptr))
        {
            ContextDescOut.SelectedObjects.Add(SelectedVar->GetProperty());
        }
        // If the selection come from the SCS editor, add it to the filter context.
        else if (OdysseyBrush->SkeletonGeneratedClass && OdysseyBrushEditor->GetSCSEditor().IsValid())
        {
            TArray<FSCSEditorTreeNodePtrType> Nodes = OdysseyBrushEditor->GetSCSEditor()->GetSelectedNodes();
            if (Nodes.Num() == 1 && Nodes[0]->GetNodeType() == FSCSEditorTreeNode::ComponentNode)
            {
                FName PropertyName = Nodes[0]->GetVariableName();
                UObjectProperty* VariableProperty = FindField<UObjectProperty>(OdysseyBrush->SkeletonGeneratedClass, PropertyName);
                ContextDescOut.SelectedObjects.Add(VariableProperty);
            }
        }
    }
}

TSharedRef<SEditableTextBox> SOdysseyBrushActionMenu::GetFilterTextBox()
{
    return GraphActionMenu->GetFilterTextBox();
}


TSharedRef<SWidget> SOdysseyBrushActionMenu::OnCreateWidgetForAction(FCreateWidgetForActionData* const InCreateData)
{
    InCreateData->bHandleMouseButtonDown = true;
    return SNew(SOdysseyBrushPaletteItem, InCreateData, EditorPtr.Pin());
}

void SOdysseyBrushActionMenu::OnActionSelected( const TArray< TSharedPtr<FEdGraphSchemaAction> >& SelectedAction, ESelectInfo::Type InSelectionType )
{
    if (InSelectionType == ESelectInfo::OnMouseClick  || InSelectionType == ESelectInfo::OnKeyPress || SelectedAction.Num() == 0)
    {
        for ( int32 ActionIndex = 0; ActionIndex < SelectedAction.Num(); ActionIndex++ )
        {
            if ( SelectedAction[ActionIndex].IsValid() && GraphObj != NULL )
            {
                // Don't dismiss when clicking on dummy action
                if ( !bActionExecuted && (SelectedAction[ActionIndex]->GetTypeId() != FEdGraphSchemaAction_Dummy::StaticGetTypeId()))
                {
                    FSlateApplication::Get().DismissAllMenus();
                    bActionExecuted = true;
                }

                UEdGraphNode* ResultNode = SelectedAction[ActionIndex]->PerformAction(GraphObj, DraggedFromPins, NewNodePosition);

                if ( ResultNode != NULL )
                {
                    NewNodePosition.Y += UEdGraphSchema_OdysseyBrush::EstimateNodeHeight( ResultNode );
                }
            }
        }
    }
}

void SOdysseyBrushActionMenu::TryInsertPromoteToVariable(FBlueprintActionContext const& MenuContext, FGraphActionListBuilderBase& OutAllActions)
{
    // If we can promote this to a variable add a menu entry to do so.
    const UEdGraphSchema_OdysseyBrush* K2Schema = Cast<const UEdGraphSchema_OdysseyBrush>(GraphObj->GetSchema());
    if ((K2Schema != nullptr) && (MenuContext.Pins.Num() > 0))
    {
        if (K2Schema->CanPromotePinToVariable(*MenuContext.Pins[0], false))
        {
            TSharedPtr<FOdysseyBrushAction_PromoteVariable> PromoteAction = TSharedPtr<FOdysseyBrushAction_PromoteVariable>(new FOdysseyBrushAction_PromoteVariable(true));
            PromoteAction->MyOdysseyBrushEditor = EditorPtr;
            OutAllActions.AddAction(PromoteAction);
        }

        if (MenuContext.Graphs.Num() == 1 && FBlueprintEditorUtils::DoesSupportLocalVariables(MenuContext.Graphs[0]) && K2Schema->CanPromotePinToVariable(*MenuContext.Pins[0], true))
        {
            TSharedPtr<FOdysseyBrushAction_PromoteVariable> LocalPromoteAction = TSharedPtr<FOdysseyBrushAction_PromoteVariable>(new FOdysseyBrushAction_PromoteVariable(false));
            LocalPromoteAction->MyOdysseyBrushEditor = EditorPtr;
            OutAllActions.AddAction( LocalPromoteAction );
        }
    }
}

/*******************************************************************************
* SOdysseyBrushActionMenuExpander
*******************************************************************************/

void SOdysseyBrushActionMenuExpander::Construct(const FArguments& InArgs, const FCustomExpanderData& ActionMenuData)
{
    OwnerRowPtr  = ActionMenuData.TableRow;
    IndentAmount = InArgs._IndentAmount;
    ActionPtr    = ActionMenuData.RowAction;

    if (!ActionPtr.IsValid())
    {
        SExpanderArrow::FArguments SuperArgs;
        SuperArgs._IndentAmount = InArgs._IndentAmount;

        SExpanderArrow::Construct(SuperArgs, ActionMenuData.TableRow);
    }
    else
    {
        ChildSlot
            .Padding(TAttribute<FMargin>(this, &SOdysseyBrushActionMenuExpander::GetCustomIndentPadding))
            [
                SNew(SOdysseyBrushActionFavoriteToggle, ActionMenuData)
            ];
    }
}

FMargin SOdysseyBrushActionMenuExpander::GetCustomIndentPadding() const
{
    FMargin CustomPadding = SExpanderArrow::GetExpanderPadding();
    // if this is a action row (not a category or separator)
    if (ActionPtr.IsValid())
    {
        // flip the left/right margins (we want the favorite toggle aligned to the far left)
        //CustomPadding = FMargin(CustomPadding.Right, CustomPadding.Top, CustomPadding.Left, CustomPadding.Bottom);
    }
    return CustomPadding;
}

#undef LOCTEXT_NAMESPACE
