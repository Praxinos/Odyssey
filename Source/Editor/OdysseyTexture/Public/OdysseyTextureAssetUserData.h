// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "OdysseyLayerStack.h"
#include "Engine/AssetUserData.h"

#include "OdysseyTextureAssetUserData.generated.h"

UCLASS()
class ODYSSEYTEXTURE_API UOdysseyTextureAssetUserData : public UAssetUserData
{
    GENERATED_UCLASS_BODY()

public:
    //~ Begin UObject Interface.
    virtual void Serialize(FArchive& Ar) override;
    //~ End UObject Interface.

public:
    FOdysseyLayerStack* GetLayerStack();
    void SetLayerStack( FOdysseyLayerStack* iLayerStack );

private:
    FOdysseyLayerStack* mLayerStack;
};

