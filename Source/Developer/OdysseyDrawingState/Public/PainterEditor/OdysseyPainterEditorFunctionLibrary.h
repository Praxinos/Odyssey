// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "Proxies/OdysseyBrushBlock.h"

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
    UFUNCTION( BlueprintPure, Category = "Odyssey|Canvas", meta = (DefaultToSelf="BrushContext") )
    static float GetViewportZoom( UOdysseyBrushAssetBase* BrushContext );
    
    //Gets rotation value from ILIAD viewport.
    UFUNCTION( BlueprintPure, Category = "Odyssey|Canvas", meta = (DefaultToSelf="BrushContext") )
    static float GetViewportRotation( UOdysseyBrushAssetBase* BrushContext );
    
    //Gets pan value from ILIAD viewport.
    UFUNCTION( BlueprintPure, Category = "Odyssey|Canvas", meta = (DefaultToSelf="BrushContext") )
    static FVector2D GetViewportPan( UOdysseyBrushAssetBase* BrushContext );
};
