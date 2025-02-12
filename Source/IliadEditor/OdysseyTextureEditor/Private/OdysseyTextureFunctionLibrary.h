// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "OdysseyTextureFunctionLibrary.generated.h"

class UOdysseyLayerStack;
class UTexture2D;

UCLASS()
class UOdysseyTextureFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|Texture", meta = (HideSelfPin="Texture", DefaultToSelf="Texture"))
    static bool HasLayerStack(UTexture2D* Texture);

    UFUNCTION(BlueprintPure, Category="Odyssey|Texture", meta = (HideSelfPin="Texture", DefaultToSelf="Texture"))
    static UOdysseyTextureLayerStack* GetLayerStack(UTexture2D* Texture);
};
