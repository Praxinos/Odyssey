// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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

private:
    virtual UObject* FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn ) override;

    virtual bool ConfigureProperties() override;

private:
    int mTextureWidth;
    int mTextureHeight;
};
