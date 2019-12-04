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

private:
    virtual UObject* FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn ) override;

    virtual bool ConfigureProperties() override;

private:
    int mTextureWidth;
    int mTextureHeight;
};
