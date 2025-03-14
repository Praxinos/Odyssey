// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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

    //Gets zoom value from viewport.
    UFUNCTION( BlueprintPure, Category="Odyssey|Canvas", meta = (HidePin="BrushInstance", DefaultToSelf="BrushInstance") )
    static float GetViewportZoom( UOdysseyBrushAssetBase* BrushInstance );

    //Gets rotation value from viewport.
    UFUNCTION( BlueprintPure, Category="Odyssey|Canvas", meta = (HidePin="BrushInstance", DefaultToSelf="BrushInstance") )
    static float GetViewportRotation( UOdysseyBrushAssetBase* BrushInstance );

    //Gets pan value from viewport.
    UFUNCTION( BlueprintPure, Category="Odyssey|Canvas", meta = (HidePin="BrushInstance", DefaultToSelf="BrushInstance") )
    static FVector2D GetViewportPan( UOdysseyBrushAssetBase* BrushInstance );

    //Gets step from Stroke Options.
    UFUNCTION( BlueprintPure, Category="Odyssey|Modifiers", meta = ( DisplayName = "Get Step Modifier", HidePin = "BrushInstance", DefaultToSelf = "BrushInstance") )
    static float  GetStep( UOdysseyBrushAssetBase* BrushInstance );
};
