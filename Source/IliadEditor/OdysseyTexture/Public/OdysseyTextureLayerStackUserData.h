// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "OdysseyTextureLayerStack.h"
#include "Engine/AssetUserData.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "UObject/ObjectSaveContext.h"

#include "OdysseyTextureLayerStackUserData.generated.h"

UCLASS(BlueprintType)
class ODYSSEYTEXTURE_API UOdysseyTextureLayerStackUserData : public UAssetUserData
{
    GENERATED_BODY()

public:
    void InitWithEmptyLayerStack();
    void InitWithDefaultLayerStack();
    void InitWithDuplicateLayerStack(UOdysseyTextureLayerStack* iLayerStack);
    void InitWithEmptyVectorLayer();


    /**
     * @brief Get the Layer Stack object
     * If there is no layerStack, creates it from the texture, with a single raster layer
     *
     * @return UOdysseyTextureLayerStack*
     */
    UOdysseyTextureLayerStack* GetLayerStack();
    UTexture2D* GetTexture();

public:
    UPROPERTY(BlueprintReadOnly, Category="Odyssey|LayerStack")
    TObjectPtr<UOdysseyTextureLayerStack> LayerStack;
};
