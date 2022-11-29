// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/Blueprint.h"
#include "OdysseyBrushBlueprint.generated.h"

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
#endif // WITH_EDITOR
};
