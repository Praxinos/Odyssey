// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"

#include "Proxies/OdysseyBrushBlock.h"
#include "Proxies/OdysseyBrushCache.h"

#include "OdysseyPainterEditorFunctionLibrary.generated.h"

class UOdysseyBrushAssetBase;

//---

UCLASS(meta=(ScriptName="OdysseyPainterEditorLibrary"))
class ODYSSEYDRAWINGSTATE_API UOdysseyPainterEditorFunctionLibrary 
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    //Gets zoom value from ILIAD viewport.
    UFUNCTION( BlueprintPure, Category = "OdysseyPainterEditorFunctionLibrary", meta = (DefaultToSelf="BrushContext") )
    static float GetViewportZoom( UOdysseyBrushAssetBase* BrushContext );
    
    //Gets rotation value from ILIAD viewport.
    UFUNCTION( BlueprintPure, Category = "OdysseyPainterEditorFunctionLibrary", meta = (DefaultToSelf="BrushContext") )
    static float GetViewportRotation( UOdysseyBrushAssetBase* BrushContext );
    
    //Gets pan value from ILIAD viewport.
    UFUNCTION( BlueprintPure, Category = "OdysseyPainterEditorFunctionLibrary", meta = (DefaultToSelf="BrushContext") )
    static FVector2D GetViewportPan( UOdysseyBrushAssetBase* BrushContext );
};
