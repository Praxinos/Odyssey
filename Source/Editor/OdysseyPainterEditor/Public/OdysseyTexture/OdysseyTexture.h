// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "Engine/Texture.h"
#include "TextureResource.h"
#include "OdysseyLayerStack.h"

#include "OdysseyTexture.generated.h"


UCLASS(hidecategories=Object, MinimalAPI, BlueprintType)
class UOdysseyTexture : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	//~ Begin UObject Interface.
	virtual void Serialize(FArchive& Ar) override;
	//~ End UObject Interface.
    
public:
    UTexture2D* GetResultTexture2D();
    void SetResultTexture2D( UTexture2D* iTexture );
    
    FOdysseyLayerStack* GetLayerStack();
    
private:
    FOdysseyLayerStack* mLayerStack;
    
    UPROPERTY()
    UTexture2D* mResultTexture;
};
