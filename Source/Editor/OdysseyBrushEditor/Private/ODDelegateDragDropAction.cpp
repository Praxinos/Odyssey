// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "ODDelegateDragDropAction.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Layout/WidgetPath.h"
#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"
#include "EdGraphSchema_OdysseyBrush.h"
#include "K2Node_AddDelegate.h"
#include "K2Node_CallDelegate.h"
#include "K2Node_ClearDelegate.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_RemoveDelegate.h"

#include "Kismet2/BlueprintEditorUtils.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "ODDelegateDragDropAction"

void FKismetDelegateDragDropAction::MakeEvent(FNodeConstructionParams Params)
{
    check(Params.Graph && Params.Property);
    const UMulticastDelegateProperty* MulticastDelegateProperty = Cast<const UMulticastDelegateProperty>(Params.Property);
    const UFunction* SignatureFunction = MulticastDelegateProperty ? MulticastDelegateProperty->SignatureFunction : NULL;
    if(SignatureFunction)
    {
        const FScopedTransaction Transaction( NSLOCTEXT("UnrealEd", "K2_AddNode", "Add Node") );
        Params.Graph->Modify();

        const FString FunctionName = FString::Printf(TEXT("%s_Event"), *Params.Property->GetName());
        UK2Node_CustomEvent* NewNode = UK2Node_CustomEvent::CreateFromFunction(Params.GraphPosition, Params.Graph, FunctionName, SignatureFunction);

        if( NewNode )
        {
            FBlueprintEditorUtils::AnalyticsTrackNewNode( NewNode );
        }

        UBlueprint* OdysseyBrush = FBlueprintEditorUtils::FindBlueprintForGraphChecked(Params.Graph);
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(OdysseyBrush);
        Params.AnalyticCallback.ExecuteIfBound();
    }
}

void FKismetDelegateDragDropAction::AssignEvent(FNodeConstructionParams Params)
{
    check(Params.Graph && Params.Property);
    const UMulticastDelegateProperty* MulticastDelegateProperty = Cast<const UMulticastDelegateProperty>(Params.Property);
    const UFunction* SignatureFunction = MulticastDelegateProperty ? MulticastDelegateProperty->SignatureFunction : NULL;
    if(SignatureFunction)
    {
        UK2Node_AddDelegate* TemplateNode = NewObject<UK2Node_AddDelegate>();
        TemplateNode->SetFromProperty(Params.Property, Params.bSelfContext);
        FEdGraphSchemaAction_K2AssignDelegate::AssignDelegate(TemplateNode, Params.Graph, NULL, Params.GraphPosition, true);
        Params.AnalyticCallback.ExecuteIfBound();
    }
}

FReply FKismetDelegateDragDropAction::DroppedOnPanel(const TSharedRef< SWidget >& Panel, FVector2D ScreenPosition, FVector2D GraphPosition, UEdGraph& Graph)
{
    if(IsValid())
    {
        FNodeConstructionParams NewNodeParams;
        NewNodeParams.Property = GetVariableProperty();
        const UClass* VariableSourceClass = CastChecked<UClass>(NewNodeParams.Property->GetOuter());
        const UBlueprint* DropOnOdysseyBrush = FBlueprintEditorUtils::FindBlueprintForGraph(&Graph);
        NewNodeParams.Graph = &Graph;
        NewNodeParams.GraphPosition = GraphPosition;
        NewNodeParams.bSelfContext = VariableSourceClass == NULL || DropOnOdysseyBrush->SkeletonGeneratedClass->IsChildOf(VariableSourceClass);;
        NewNodeParams.AnalyticCallback = AnalyticCallback;

        FMenuBuilder MenuBuilder(true, NULL);
        const FText VariableNameText = FText::FromName( VariableName );
        MenuBuilder.BeginSection("BPDelegateDroppedOn", VariableNameText );
        {

            const bool bBlueprintCallable = NewNodeParams.Property->HasAllPropertyFlags(CPF_BlueprintCallable);
            if(bBlueprintCallable)
            {
                MenuBuilder.AddMenuEntry(
                    LOCTEXT("CallDelegate", "Call"),
                    FText::Format( LOCTEXT("CallDelegateToolTip", "Call {0}"), VariableNameText ),
                    FSlateIcon(),
                    FUIAction(
                        FExecuteAction::CreateStatic(&FKismetDelegateDragDropAction::MakeMCDelegateNode<UK2Node_CallDelegate>,NewNodeParams)
                    ));
            }

            if(NewNodeParams.Property->HasAllPropertyFlags(CPF_BlueprintAssignable))
            {
                MenuBuilder.AddMenuEntry(
                    LOCTEXT("AddDelegate", "Bind"),
                    FText::Format( LOCTEXT("AddDelegateToolTip", "Bind event to {0}"), VariableNameText ),
                    FSlateIcon(),
                    FUIAction(
                        FExecuteAction::CreateStatic(&FKismetDelegateDragDropAction::MakeMCDelegateNode<UK2Node_AddDelegate>,NewNodeParams)
                    ));

                MenuBuilder.AddMenuEntry(
                    LOCTEXT("AddRemove", "Unbind"),
                    FText::Format( LOCTEXT("RemoveDelegateToolTip", "Unbind event from {0}"), VariableNameText ),
                    FSlateIcon(),
                    FUIAction(
                        FExecuteAction::CreateStatic(&FKismetDelegateDragDropAction::MakeMCDelegateNode<UK2Node_RemoveDelegate>, NewNodeParams)
                    ));

                MenuBuilder.AddMenuEntry(
                    LOCTEXT("AddClear", "Unbind all"),
                    FText::Format( LOCTEXT("ClearDelegateToolTip", "Unbind all events from {0}"), VariableNameText ),
                    FSlateIcon(),
                    FUIAction(
                        FExecuteAction::CreateStatic(&FKismetDelegateDragDropAction::MakeMCDelegateNode<UK2Node_ClearDelegate>, NewNodeParams)
                    ));

                const UEdGraphSchema_OdysseyBrush* Schema = GetDefault<UEdGraphSchema_OdysseyBrush>();
                check(Schema);
                const EGraphType GraphType = Schema->GetGraphType(&Graph);
                const bool bSupportsEventGraphs = (DropOnOdysseyBrush && FBlueprintEditorUtils::DoesSupportEventGraphs(DropOnOdysseyBrush));
                const bool bAllowEvents = ((GraphType == GT_Ubergraph) && bSupportsEventGraphs);
                if(bAllowEvents)
                {
                    MenuBuilder.AddMenuEntry(
                        LOCTEXT("AddEvent", "Event"),
                        FText::Format( LOCTEXT("EventDelegateToolTip", "Create event with the {0} signature"), VariableNameText ),
                        FSlateIcon(),
                        FUIAction(
                            FExecuteAction::CreateStatic(&FKismetDelegateDragDropAction::MakeEvent, NewNodeParams)
                        ));

                    MenuBuilder.AddMenuEntry(
                        LOCTEXT("AssignEvent", "Assign"),
                        LOCTEXT("AssignDelegateToolTip", "Create and bind event"),
                        FSlateIcon(),
                        FUIAction(
                            FExecuteAction::CreateStatic(&FKismetDelegateDragDropAction::AssignEvent, NewNodeParams)
                        ));
                }
            }
        }
        MenuBuilder.EndSection();
        FSlateApplication::Get().PushMenu(Panel, FWidgetPath(), MenuBuilder.MakeWidget(), ScreenPosition, FPopupTransitionEffect( FPopupTransitionEffect::ContextMenu));
    }
    return FReply::Handled();
}

bool FKismetDelegateDragDropAction::IsValid() const
{
    return VariableSource.IsValid() &&
        (VariableName != NAME_None) &&
        (NULL != FindField<UMulticastDelegateProperty>(VariableSource.Get(), VariableName));
}

bool FKismetDelegateDragDropAction::IsSupportedBySchema(const class UEdGraphSchema* Schema) const
{
    auto SchemaK2 = Cast<UEdGraphSchema_OdysseyBrush>(Schema);
    return SchemaK2 && SchemaK2->DoesSupportEventDispatcher();
}

#undef LOCTEXT_NAMESPACE
