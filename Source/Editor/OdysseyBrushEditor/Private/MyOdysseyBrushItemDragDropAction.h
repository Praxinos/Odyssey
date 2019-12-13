// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"
#include "Input/DragAndDrop.h"
#include "Input/Reply.h"
#include "Engine/Blueprint.h"
#include "OdysseyBrushEditor.h"
#include "GraphEditorDragDropAction.h"

class UEdGraph;
struct FEdGraphSchemaAction;

/** DragDropAction class for drag and dropping an item from the My Blueprints tree (e.g., variable or function) */
class FMyOdysseyBrushItemDragDropAction : public FGraphSchemaActionDragDropAction
{
public:
    DRAG_DROP_OPERATOR_TYPE(FMyOdysseyBrushItemDragDropAction, FGraphSchemaActionDragDropAction)

    // FGraphEditorDragDropAction interface
    virtual FReply DroppedOnAction(TSharedRef<FEdGraphSchemaAction> Action) override;
    virtual FReply DroppedOnCategory(FText Category) override;
    virtual void HoverTargetChanged() override;
    // End of FGraphEditorDragDropAction

    /** Set if operation is modified by alt */
    void SetAltDrag(bool InIsAltDrag) {    bAltDrag = InIsAltDrag; }

    /** Set if operation is modified by the ctrl key */
    void SetCtrlDrag(bool InIsCtrlDrag) { bControlDrag = InIsCtrlDrag; }

protected:
     /** Constructor */
    FMyOdysseyBrushItemDragDropAction();

    virtual UBlueprint* GetSourceOdysseyBrush() const
    {
        return nullptr;
    }

    /** Helper method to see if we're dragging in the same blueprint */
    bool IsFromOdysseyBrush(UBlueprint* InOdysseyBrush) const
    {
        return GetSourceOdysseyBrush() == InOdysseyBrush;
    }

    void SetFeedbackMessageError(const FText& Message);
    void SetFeedbackMessageOK(const FText& Message);

protected:
    /** Was ctrl held down at start of drag */
    bool bControlDrag;
    /** Was alt held down at the start of drag */
    bool bAltDrag;
    /** Analytic delegate to track node creation */
    FNodeCreationAnalytic AnalyticCallback;
};
