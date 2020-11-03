// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "Templates/SubclassOf.h"
#include "UObject/ObjectMacros.h"

#include "OdysseyTextureFactory.generated.h"

UCLASS()
class UOdysseyTextureFactory 
    : public UFactory
{
    GENERATED_UCLASS_BODY()

private:
    virtual UObject* FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn ) override;

    virtual bool ConfigureProperties() override;

    virtual FString GetDefaultNewAssetName() const;

private:
    int                  mTextureWidth;
    int                  mTextureHeight;
    ETextureSourceFormat mTextureFormat;
    FString              mDefaultName;
    FLinearColor         mBackgroundColor;
};
