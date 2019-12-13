// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "EdGraphSchema_OdysseyBrush.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/KismetDebugUtilities.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GraphEditorActions.h"
#include "K2Node.h"
#include "EdGraphSchema_K2_Actions.h"
#include "K2Node_EditablePinBase.h"
#include "K2Node_Event.h"
#include "K2Node_ActorBoundEvent.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Variable.h"
#include "K2Node_BreakStruct.h"
#include "K2Node_CallArrayFunction.h"
#include "K2Node_CallParentFunction.h"
#include "K2Node_ComponentBoundEvent.h"
#include "K2Node_Tunnel.h"
#include "K2Node_Composite.h"
#include "K2Node_CreateDelegate.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_DynamicCast.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_FunctionTerminator.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "K2Node_Knot.h"
#include "K2Node_Literal.h"
#include "K2Node_MacroInstance.h"
#include "K2Node_MakeArray.h"
#include "K2Node_MakeStruct.h"
#include "K2Node_Select.h"
#include "K2Node_SpawnActor.h"
#include "K2Node_SpawnActorFromClass.h"
#include "K2Node_Switch.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"
#include "K2Node_SetFieldsInStruct.h"
#include "Framework/Commands/GenericCommands.h"
#include "BlueprintEditorSettings.h"
#include "Internationalization/Internationalization.h"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "EdGraphSchema_OdysseyBrush"

//////////////////////////////////////////////////////////////////////////
// UEdGraphSchema_OdysseyBrush

UEdGraphSchema_OdysseyBrush::UEdGraphSchema_OdysseyBrush(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}


// if node is a get/set variable and the variable it refers to does not exist
static bool IsUsingOdysseyNonExistantVariable(const UEdGraphNode* InGraphNode, UBlueprint* OwnerBlueprint)
{
    bool bNonExistantVariable = false;
    const bool bBreakOrMakeStruct =
        InGraphNode->IsA(UK2Node_BreakStruct::StaticClass()) ||
        InGraphNode->IsA(UK2Node_MakeStruct::StaticClass());
    if (!bBreakOrMakeStruct)
    {
        if (const UK2Node_Variable* Variable = Cast<const UK2Node_Variable>(InGraphNode))
        {
            if (Variable->VariableReference.IsSelfContext())
            {
                TSet<FName> CurrentVars;
                FBlueprintEditorUtils::GetClassVariableList(OwnerBlueprint, CurrentVars);
                if ( false == CurrentVars.Contains(Variable->GetVarName()) )
                {
                    bNonExistantVariable = true;
                }
            }
            else if(Variable->VariableReference.IsLocalScope())
            {
                // If there is no member scope, or we can't find the local variable in the member scope, then it's non-existant
                UStruct* MemberScope = Variable->VariableReference.GetMemberScope(Variable->GetBlueprintClassFromNode());
                if (MemberScope == nullptr || !FBlueprintEditorUtils::FindLocalVariable(OwnerBlueprint, MemberScope, Variable->GetVarName()))
                {
                    bNonExistantVariable = true;
                }
            }
        }
    }
    return bNonExistantVariable;
}

void UEdGraphSchema_OdysseyBrush::GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, FMenuBuilder* MenuBuilder, bool bIsDebugging) const
{
    check(CurrentGraph);
    UBlueprint* OwnerBlueprint = FBlueprintEditorUtils::FindBlueprintForGraphChecked(CurrentGraph);

    if (InGraphPin != NULL)
    {
        MenuBuilder->BeginSection("EdGraphSchemaPinActions", LOCTEXT("PinActionsMenuHeader", "Pin Actions"));
        {
            if (!bIsDebugging)
            {
                // Break pin links
                if (InGraphPin->LinkedTo.Num() > 1)
                {
                    MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().BreakPinLinks );
                }

                // Add the change pin type action, if this is a select node
                if (InGraphNode->IsA(UK2Node_Select::StaticClass()))
                {
                    MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().ChangePinType);
                }

                // add sub menu for break link to
                if (InGraphPin->LinkedTo.Num() > 0)
                {
                    MenuBuilder->AddMenuEntry(
                        InGraphPin->Direction == EEdGraphPinDirection::EGPD_Input ? LOCTEXT("SelectAllInputNodes", "Select All Input Nodes") : LOCTEXT("SelectAllOutputNodes", "Select All Output Nodes"),
                        InGraphPin->Direction == EEdGraphPinDirection::EGPD_Input ? LOCTEXT("SelectAllInputNodesTooltip", "Adds all input Nodes linked to this Pin to selection") : LOCTEXT("SelectAllOutputNodesTooltip", "Adds all output Nodes linked to this Pin to selection"),
                        FSlateIcon(),
                        FUIAction(FExecuteAction::CreateUObject((UEdGraphSchema_K2*const)this, &UEdGraphSchema_K2::SelectAllNodesInDirection, InGraphPin->Direction, const_cast<UEdGraph*>(CurrentGraph), const_cast<UEdGraphPin*>(InGraphPin))));

                    if(InGraphPin->LinkedTo.Num() > 1)
                    {
                        MenuBuilder->AddSubMenu(
                            LOCTEXT("BreakLinkTo", "Break Link To..."),
                            LOCTEXT("BreakSpecificLinks", "Break a specific link..."),
                            FNewMenuDelegate::CreateUObject( (UEdGraphSchema_K2*const)this, &UEdGraphSchema_K2::GetBreakLinkToSubMenuActions, const_cast<UEdGraphPin*>(InGraphPin)));

                        MenuBuilder->AddSubMenu(
                            LOCTEXT("JumpToConnection", "Jump to Connection..."),
                            LOCTEXT("JumpToSpecificConnection", "Jump to specific connection..."),
                            FNewMenuDelegate::CreateUObject( (UEdGraphSchema_K2*const)this, &UEdGraphSchema_K2::GetJumpToConnectionSubMenuActions, const_cast<UEdGraphPin*>(InGraphPin)));

                        MenuBuilder->AddSubMenu(
                            LOCTEXT("StraightenConnection", "Straighten Connection To..."),
                            LOCTEXT("StraightenConnection_Tip", "Straighten a specific connection"),
                            FNewMenuDelegate::CreateUObject( this, &UEdGraphSchema_K2::GetStraightenConnectionToSubMenuActions, const_cast<UEdGraphPin*>(InGraphPin)));
                    }
                    else
                    {
                        ((UEdGraphSchema_K2*const)this)->GetBreakLinkToSubMenuActions(*MenuBuilder, const_cast<UEdGraphPin*>(InGraphPin));
                        ((UEdGraphSchema_K2*const)this)->GetJumpToConnectionSubMenuActions(*MenuBuilder, const_cast<UEdGraphPin*>(InGraphPin));

                        UEdGraphPin* Pin = InGraphPin->LinkedTo[0];
                        FText PinName = Pin->GetDisplayName();
                        FText NodeName = Pin->GetOwningNode()->GetNodeTitle(ENodeTitleType::ListView);

                        MenuBuilder->AddMenuEntry(
                            FGraphEditorCommands::Get().StraightenConnections,
                            NAME_None,
                            FText::Format(LOCTEXT("StraightenDescription_SinglePin", "Straighten Connection to {0} ({1})"), NodeName, PinName),
                            FText::Format(LOCTEXT("StraightenDescription_SinglePin_Node_Tip", "Straighten the connection between this pin, and {0} ({1})"), NodeName, PinName),
                            FSlateIcon(NAME_None, NAME_None, NAME_None)
                        );
                    }
                }

                // Conditionally add the var promotion pin if this is an output pin and it's not an exec pin
                if (InGraphPin->PinType.PinCategory != PC_Exec)
                {
                    MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().PromoteToVariable );

                    if (FBlueprintEditorUtils::DoesSupportLocalVariables(CurrentGraph))
                    {
                        MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().PromoteToLocalVariable );
                    }
                }

                if (InGraphPin->PinType.PinCategory == PC_Struct && InGraphNode->CanSplitPin(InGraphPin))
                {
                    // If the pin cannot be split, create an error tooltip to use
                    FText Tooltip;
                    if (PinHasSplittableStructType(InGraphPin))
                    {
                        Tooltip = FGraphEditorCommands::Get().SplitStructPin->GetDescription();
                    }
                    else
                    {
                        Tooltip = LOCTEXT("SplitStructPin_Error", "Cannot split the struct pin, it may be missing Blueprint exposed properties!");
                    }
                    MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().SplitStructPin, NAME_None, FGraphEditorCommands::Get().SplitStructPin->GetLabel(), Tooltip );
                }

                if (InGraphPin->ParentPin != NULL)
                {
                    MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().RecombineStructPin );
                }

                // Conditionally add the execution path pin options if this is an execution branching node
                if( InGraphPin->Direction == EGPD_Output && InGraphPin->GetOwningNode())
                {
                    if (CastChecked<UK2Node>(InGraphPin->GetOwningNode())->CanEverInsertExecutionPin())
                    {
                        MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().InsertExecutionPinBefore);
                        MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().InsertExecutionPinAfter);
                    }

                    if (CastChecked<UK2Node>(InGraphPin->GetOwningNode())->CanEverRemoveExecutionPin())
                    {
                        MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().RemoveExecutionPin );
                    }
                }

                if (UK2Node_SetFieldsInStruct::ShowCustomPinActions(InGraphPin, true))
                {
                    MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().RemoveThisStructVarPin);
                    MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().RemoveOtherStructVarPins);
                }

                if (InGraphPin->PinType.PinCategory != PC_Exec && InGraphPin->Direction == EGPD_Input && InGraphPin->LinkedTo.Num() == 0 && !ShouldHidePinDefaultValue(const_cast<UEdGraphPin*>(InGraphPin)))
                {
                    MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().ResetPinToDefaultValue);
                }
            }
        }
        MenuBuilder->EndSection();

        // Add the watch pin / unwatch pin menu items
        MenuBuilder->BeginSection("EdGraphSchemaWatches", LOCTEXT("WatchesHeader", "Watches"));
        {
            if (!IsMetaPin(*InGraphPin))
            {
                const UEdGraphPin* WatchedPin = ((InGraphPin->Direction == EGPD_Input) && (InGraphPin->LinkedTo.Num() > 0)) ? InGraphPin->LinkedTo[0] : InGraphPin;
                if (FKismetDebugUtilities::IsPinBeingWatched(OwnerBlueprint, WatchedPin))
                {
                    MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().StopWatchingPin );
                }
                else
                {
                    MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().StartWatchingPin );
                }
            }
        }
        MenuBuilder->EndSection();
    }
    else if (InGraphNode != NULL)
    {
        if (IsUsingOdysseyNonExistantVariable(InGraphNode, OwnerBlueprint))
        {
            MenuBuilder->BeginSection("EdGraphSchemaNodeActions", LOCTEXT("NodeActionsMenuHeader", "Node Actions"));
            {
                GetNonExistentVariableMenu(InGraphNode,OwnerBlueprint, MenuBuilder);
            }
            MenuBuilder->EndSection();
        }
        else
        {
            MenuBuilder->BeginSection("EdGraphSchemaNodeActions", LOCTEXT("NodeActionsMenuHeader", "Node Actions"));
            {
                if (!bIsDebugging)
                {
                    // Replaceable node display option
                    AddSelectedReplaceableNodes( OwnerBlueprint, InGraphNode, MenuBuilder );

                    // Node contextual actions
                    MenuBuilder->AddMenuEntry( FGenericCommands::Get().Delete );
                    MenuBuilder->AddMenuEntry( FGenericCommands::Get().Cut );
                    MenuBuilder->AddMenuEntry( FGenericCommands::Get().Copy );
                    MenuBuilder->AddMenuEntry( FGenericCommands::Get().Duplicate );
                    MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().ReconstructNodes );
                    MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().BreakNodeLinks );

                    // Conditionally add the action to add an execution pin, if this is an execution node
                    if( InGraphNode->IsA(UK2Node_ExecutionSequence::StaticClass()) || InGraphNode->IsA(UK2Node_Switch::StaticClass()) )
                    {
                        MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().AddExecutionPin );
                    }

                    // Conditionally add the action to create a super function call node, if this is an event or function entry
                    if( InGraphNode->IsA(UK2Node_Event::StaticClass()) || InGraphNode->IsA(UK2Node_FunctionEntry::StaticClass()) )
                    {
                        MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().AddParentNode );
                    }

                    // Conditionally add the actions to add or remove an option pin, if this is a select node
                    if (InGraphNode->IsA(UK2Node_Select::StaticClass()))
                    {
                        MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().AddOptionPin);
                        MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().RemoveOptionPin);
                    }

                    // Don't show the "Assign selected Actor" option if more than one actor is selected
                    if (InGraphNode->IsA(UK2Node_ActorBoundEvent::StaticClass()) && GEditor->GetSelectedActorCount() == 1)
                    {
                        MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().AssignReferencedActor);
                    }
                }

                // If the node has an associated definition (for some loose sense of the word), allow going to it (same action as double-clicking on a node)
                if (InGraphNode->CanJumpToDefinition())
                {
                    MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().GoToDefinition);
                }

                // show search for references for everyone
                MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().FindReferences);

                if (!bIsDebugging)
                {
                    if (InGraphNode->IsA(UK2Node_Variable::StaticClass()))
                    {
                        GetReplaceVariableMenu(InGraphNode, OwnerBlueprint, MenuBuilder, true);
                    }

                    if (InGraphNode->IsA(UK2Node_SetFieldsInStruct::StaticClass()))
                    {
                        MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().RestoreAllStructVarPins);
                    }

                    MenuBuilder->AddMenuEntry(FGenericCommands::Get().Rename, NAME_None, LOCTEXT("Rename", "Rename"), LOCTEXT("Rename_Tooltip", "Renames selected function or variable in blueprint.") );
                }

                // Select referenced actors in the level
                MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().SelectReferenceInLevel );
            }
            MenuBuilder->EndSection(); //EdGraphSchemaNodeActions

            if (!bIsDebugging)
            {
                // Collapse/expand nodes
                MenuBuilder->BeginSection("EdGraphSchemaOrganization", LOCTEXT("OrganizationHeader", "Organization"));
                {
                    MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().CollapseNodes );
                    MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().CollapseSelectionToFunction );
                    MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().CollapseSelectionToMacro );
                    MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().ExpandNodes );

                    if(InGraphNode->IsA(UK2Node_Composite::StaticClass()))
                    {
                        MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().PromoteSelectionToFunction );
                        MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().PromoteSelectionToMacro );
                    }

                    MenuBuilder->AddSubMenu(LOCTEXT("AlignmentHeader", "Alignment"), FText(), FNewMenuDelegate::CreateLambda([](FMenuBuilder& InMenuBuilder){

                        InMenuBuilder.BeginSection("EdGraphSchemaAlignment", LOCTEXT("AlignHeader", "Align"));
                        InMenuBuilder.AddMenuEntry( FGraphEditorCommands::Get().AlignNodesTop );
                        InMenuBuilder.AddMenuEntry( FGraphEditorCommands::Get().AlignNodesMiddle );
                        InMenuBuilder.AddMenuEntry( FGraphEditorCommands::Get().AlignNodesBottom );
                        InMenuBuilder.AddMenuEntry( FGraphEditorCommands::Get().AlignNodesLeft );
                        InMenuBuilder.AddMenuEntry( FGraphEditorCommands::Get().AlignNodesCenter );
                        InMenuBuilder.AddMenuEntry( FGraphEditorCommands::Get().AlignNodesRight );
                        InMenuBuilder.AddMenuEntry( FGraphEditorCommands::Get().StraightenConnections );
                        InMenuBuilder.EndSection();

                        InMenuBuilder.BeginSection("EdGraphSchemaDistribution", LOCTEXT("DistributionHeader", "Distribution"));
                        InMenuBuilder.AddMenuEntry( FGraphEditorCommands::Get().DistributeNodesHorizontally );
                        InMenuBuilder.AddMenuEntry( FGraphEditorCommands::Get().DistributeNodesVertically );
                        InMenuBuilder.EndSection();

                    }));
                }

                MenuBuilder->EndSection();
            }

            if (const UK2Node* K2Node = Cast<const UK2Node>(InGraphNode))
            {
                if (!K2Node->IsNodePure())
                {
                    if (!bIsDebugging && GetDefault<UBlueprintEditorSettings>()->bAllowExplicitImpureNodeDisabling)
                    {
                        // Don't expose the enabled state for disabled nodes that were not explicitly disabled by the user
                        if (!K2Node->IsAutomaticallyPlacedGhostNode())
                        {
                            // Add compile options
                            MenuBuilder->BeginSection("EdGraphSchemaCompileOptions", LOCTEXT("CompileOptionsHeader", "Compile Options"));
                            {
                                MenuBuilder->AddMenuEntry(
                                    FGraphEditorCommands::Get().DisableNodes,
                                    NAME_None,
                                    LOCTEXT("DisableCompile", "Disable (Do Not Compile)"),
                                    LOCTEXT("DisableCompileToolTip", "Selected node(s) will not be compiled."));

                                TSharedPtr<const FUICommandList> MenuCommandList = MenuBuilder->GetTopCommandList();
                                if(ensure(MenuCommandList.IsValid()))
                                {
                                    const FUIAction* SubMenuUIAction = MenuCommandList->GetActionForCommand(FGraphEditorCommands::Get().EnableNodes);
                                    if(ensure(SubMenuUIAction))
                                    {
                                        MenuBuilder->AddSubMenu(
                                            LOCTEXT("EnableCompileSubMenu", "Enable Compile"),
                                            LOCTEXT("EnableCompileSubMenuToolTip", "Options to enable selected node(s) for compile."),
                                            FNewMenuDelegate::CreateLambda([MenuCommandList](FMenuBuilder& SubMenuBuilder)
                                            {
                                                SubMenuBuilder.PushCommandList(MenuCommandList.ToSharedRef());

                                                SubMenuBuilder.AddMenuEntry(
                                                    FGraphEditorCommands::Get().EnableNodes_Always,
                                                    NAME_None,
                                                    LOCTEXT("EnableCompileAlways", "Always"),
                                                    LOCTEXT("EnableCompileAlwaysToolTip", "Always compile selected node(s)."));
                                                SubMenuBuilder.AddMenuEntry(
                                                    FGraphEditorCommands::Get().EnableNodes_DevelopmentOnly,
                                                    NAME_None,
                                                    LOCTEXT("EnableCompileDevelopmentOnly", "Development Only"),
                                                    LOCTEXT("EnableCompileDevelopmentOnlyToolTip", "Compile selected node(s) for development only."));

                                                SubMenuBuilder.PopCommandList();
                                            }),
                                            *SubMenuUIAction,
                                            NAME_None, FGraphEditorCommands::Get().EnableNodes->GetUserInterfaceType());
                                    }
                                }
                            }
                            MenuBuilder->EndSection();
                        }
                    }

                    /*
                    // Add breakpoint actions
                    MenuBuilder->BeginSection("EdGraphSchemaBreakpoints", LOCTEXT("BreakpointsHeader", "Breakpoints"));
                    {
                        MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().ToggleBreakpoint );
                        MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().AddBreakpoint );
                        MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().RemoveBreakpoint );
                        MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().EnableBreakpoint );
                        MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().DisableBreakpoint );
                    }
                    MenuBuilder->EndSection();
                    */
                }
            }

            /*
            MenuBuilder->BeginSection("EdGraphSchemaDocumentation", LOCTEXT("DocumentationHeader", "Documentation"));
            {
                MenuBuilder->AddMenuEntry( FGraphEditorCommands::Get().GoToDocumentation );
            }
            MenuBuilder->EndSection();
            */
        }
    }

    UEdGraphSchema::GetContextMenuActions(CurrentGraph, InGraphNode, InGraphPin, MenuBuilder, bIsDebugging);
}

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
