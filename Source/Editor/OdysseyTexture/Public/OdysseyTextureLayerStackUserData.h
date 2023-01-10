// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "Engine/AssetUserData.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "UObject/ObjectSaveContext.h"

#include "OdysseyTextureLayerStackUserData.generated.h"

UCLASS()
class ODYSSEYTEXTURE_API UOdysseyTextureLayerStackUserData : public UAssetUserData
{
    GENERATED_BODY()

public:
    void InitWithEmptyLayerStack();
    void InitWithDefaultLayerStack();
    void InitWithDuplicateLayerStack(UOdysseyTextureLayerStack* iLayerStack);

    /**
     * @brief Get the Layer Stack object
     * If there is no layerStack, creates it from the texture, with a single raster layer
     * 
     * @return UOdysseyTextureLayerStack* 
     */
    UOdysseyTextureLayerStack* GetLayerStack();
    UTexture2D* GetTexture();
    
private:
	UPROPERTY()
    TObjectPtr<UOdysseyTextureLayerStack> LayerStack;
};

