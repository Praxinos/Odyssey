// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "Input/DragAndDrop.h"
#include "Input/Reply.h"
#include "Engine/MemberReference.h"
#include "OdysseyBrushEditor.h"
#include "Editor/GraphEditor/Public/GraphEditorDragDropAction.h"
#include "MyOdysseyBrushItemDragDropAction.h"

class UEdGraph;

/*******************************************************************************
* FKismetDragDropAction
*******************************************************************************/

class ODYSSEYBRUSHEDITOR_API FKismetDragDropAction : public FMyOdysseyBrushItemDragDropAction
{
public:
    DRAG_DROP_OPERATOR_TYPE(FKismetDragDropAction, FMyOdysseyBrushItemDragDropAction)

    // FGraphEditorDragDropAction interface
    virtual void HoverTargetChanged() override;
    virtual FReply DroppedOnPanel( const TSharedRef< class SWidget >& Panel, FVector2D ScreenPosition, FVector2D GraphPosition, UEdGraph& Graph) override;
    // End of FGraphSchemaActionDragDropAction

    DECLARE_DELEGATE_RetVal_ThreeParams(bool, FCanBeDroppedDelegate, TSharedPtr<FEdGraphSchemaAction> /*DropAction*/, UEdGraph* /*HoveredGraphIn*/, FText& /*ImpededReasonOut*/);

    static TSharedRef<FKismetDragDropAction> New(TSharedPtr<FEdGraphSchemaAction> InActionNode, FNodeCreationAnalytic AnalyticCallback, FCanBeDroppedDelegate CanBeDroppedDelegate)
    {
        TSharedRef<FKismetDragDropAction> Operation = MakeShareable(new FKismetDragDropAction);
        Operation->SourceAction = InActionNode;
        Operation->AnalyticCallback = AnalyticCallback;
        Operation->CanBeDroppedDelegate = CanBeDroppedDelegate;
        Operation->Construct();
        return Operation;
    }

protected:
    bool ActionWillShowExistingNode() const;

    /** */
    FCanBeDroppedDelegate CanBeDroppedDelegate;
};

/*******************************************************************************
* FKismetFunctionDragDropAction
*******************************************************************************/

class ODYSSEYBRUSHEDITOR_API FKismetFunctionDragDropAction : public FKismetDragDropAction
{
public:
    DRAG_DROP_OPERATOR_TYPE(FKismetFunctionDragDropAction, FKismetDragDropAction)

    FKismetFunctionDragDropAction();

    // FGraphEditorDragDropAction interface
    virtual FReply DroppedOnPanel( const TSharedRef< class SWidget >& Panel, FVector2D ScreenPosition, FVector2D GraphPosition, UEdGraph& Graph) override;
    virtual FReply DroppedOnPin(FVector2D ScreenPosition, FVector2D GraphPosition) override;
    // End of FGraphEditorDragDropAction

    static TSharedRef<FKismetFunctionDragDropAction> New(TSharedPtr<FEdGraphSchemaAction> InActionNode, FName InFunctionName, UClass* InOwningClass, const FMemberReference& InCallOnMember, FNodeCreationAnalytic AnalyticCallback, FCanBeDroppedDelegate CanBeDroppedDelegate = FCanBeDroppedDelegate());

protected:
    /** Name of function being dragged */
    FName FunctionName;
    /** Class that function belongs to */
    UClass* OwningClass;
    /** Call on member reference */
    FMemberReference CallOnMember;

    /** Looks up the functions field on OwningClass using FunctionName */
    UFunction const* GetFunctionProperty() const;

    /** Constructs an action to execute, placing a function call node for the associated function */
    class UBlueprintFunctionNodeSpawner* GetDropAction(UEdGraph& Graph) const;
};

/*******************************************************************************
* FKismetMacroDragDropAction
*******************************************************************************/

class ODYSSEYBRUSHEDITOR_API FKismetMacroDragDropAction : public FKismetDragDropAction
{
public:
    DRAG_DROP_OPERATOR_TYPE(FKismetMacroDragDropAction, FKismetDragDropAction)

    FKismetMacroDragDropAction();

    // FGraphEditorDragDropAction interface
    virtual FReply DroppedOnPanel( const TSharedRef< class SWidget >& Panel, FVector2D ScreenPosition, FVector2D GraphPosition, UEdGraph& Graph) override;
    // End of FGraphEditorDragDropAction

    static TSharedRef<FKismetMacroDragDropAction> New(TSharedPtr<FEdGraphSchemaAction> InActionNode, FName InMacroName, UBlueprint* InOdysseyBrush, UEdGraph* InMacro, FNodeCreationAnalytic AnalyticCallback);

protected:
    // FMyOdysseyBrushItemDragDropAction interface
    virtual UBlueprint* GetSourceOdysseyBrush() const override
    {
        return OdysseyBrush;
    }
    // End of FMyOdysseyBrushItemDragDropAction interface

protected:
    /** Name of macro being dragged */
    FName MacroName;
    /** Graph for the macro being dragged */
    UEdGraph* Macro;
    /** OdysseyBrush we are operating on */
    UBlueprint* OdysseyBrush;
};

