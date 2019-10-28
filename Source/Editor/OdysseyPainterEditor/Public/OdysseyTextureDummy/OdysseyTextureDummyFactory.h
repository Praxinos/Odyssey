// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "Templates/SubclassOf.h"
#include "UObject/ObjectMacros.h"
#include "OdysseyTextureDummyFactory.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyTextureDummyFactory : public UFactory
{
    GENERATED_UCLASS_BODY()

    virtual  UObject*  FactoryCreateNew(    UClass* Class,
                                            UObject* InParent,
                                            FName Name,
                                            EObjectFlags Flags,
                                            UObject* Context,
                                            FFeedbackContext* Warn)  override;

    virtual bool ConfigureProperties() override;

private:
    int textureWidth;
    int textureHeight;
};
