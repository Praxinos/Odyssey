// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "OdysseyLayerStack.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "Engine/AssetUserData.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "UObject/ObjectSaveContext.h"

#include "OdysseyTextureAssetUserData.generated.h"

UCLASS()
class ODYSSEYTEXTURE_API UOdysseyTextureAssetUserData : public UAssetUserData
{
    GENERATED_BODY()

public:
	static UOdysseyTextureAssetUserData* MakeFromTexture(UTexture2D* iTexture);

	UOdysseyTextureAssetUserData();

public:
    //~ Begin UObject Interface.
    virtual void Serialize(FArchive& Ar) override;
    //~ End UObject Interface.

public:
    FOdysseyLayerStack* GetOldLayerStack();
    void SetOldLayerStack( FOdysseyLayerStack* iLayerStack );

    UOdysseyTextureLayerStack* GetLayerStack();
    UTexture2D* GetTexture();

private:
    FOdysseyLayerStack* mLayerStack;
    
private:
	UPROPERTY()
    TObjectPtr<UOdysseyTextureLayerStack> LayerStack;
};

