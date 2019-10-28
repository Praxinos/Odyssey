// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EdGraphSchema_K2.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraphSchema_OdysseyBrush.generated.h"

UCLASS(config=Editor)
class ODYSSEYBRUSHEDITOR_API UEdGraphSchema_OdysseyBrush : public UEdGraphSchema_K2
{
    GENERATED_UCLASS_BODY()

public:
    virtual  bool  DoesSupportEventDispatcher()  const  override { return  false; }
    virtual void GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const override;
};
