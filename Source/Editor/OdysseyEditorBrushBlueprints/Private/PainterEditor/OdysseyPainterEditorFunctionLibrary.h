// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "proxies/OdysseyBrushColor.h"
#include "OdysseyPainterEditorFunctionLibrary.generated.h"

class UOdysseyBrushAssetBase;

//---

UCLASS(Blueprintable)
class UOdysseyPainterEditorFunctionLibrary 
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    //TODO: make a GetEditorFromBrush() and 

    //Gets zoom value from ILIAD viewport.
    UFUNCTION( BlueprintPure, Category = "Odyssey|Canvas", meta = (HidePin="BrushInstance", DefaultToSelf="BrushInstance") )
    static float GetViewportZoom( UOdysseyBrushAssetBase* BrushInstance );
    
    //Gets rotation value from ILIAD viewport.
    UFUNCTION( BlueprintPure, Category = "Odyssey|Canvas", meta = (HidePin="BrushInstance", DefaultToSelf="BrushInstance") )
    static float GetViewportRotation( UOdysseyBrushAssetBase* BrushInstance );
    
    //Gets pan value from ILIAD viewport.
    UFUNCTION( BlueprintPure, Category = "Odyssey|Canvas", meta = (HidePin="BrushInstance", DefaultToSelf="BrushInstance") )
    static FVector2D GetViewportPan( UOdysseyBrushAssetBase* BrushInstance );
};
