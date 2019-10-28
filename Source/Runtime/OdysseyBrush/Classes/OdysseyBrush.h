// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/Blueprint.h"
#include "OdysseyBrush.generated.h"

/**
 * Odyssey Brush
 * Design a brush to paint with.
 */
UCLASS()
class ODYSSEYBRUSH_API UOdysseyBrush : public UBlueprint
{
    GENERATED_UCLASS_BODY()

#if WITH_EDITOR
    // UBlueprint interface
    virtual  bool  SupportedByDefaultBlueprintFactory()  const  override { return  false; }
    virtual  void  GetAssetRegistryTags( TArray< FAssetRegistryTag >&  OutTags )  const  override { return; }
#endif // WITH_EDITOR
};
