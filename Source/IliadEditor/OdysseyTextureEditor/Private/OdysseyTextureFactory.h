// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "Templates/SubclassOf.h"
#include "UObject/ObjectMacros.h"
#include "Texture/SOdysseyTextureConfigureWindow.h"

#include "OdysseyTextureFactory.generated.h"

UCLASS()
class UOdysseyTextureFactory
    : public UFactory
{
    GENERATED_UCLASS_BODY()

public:
    void SetConfiguration(const FOdysseyTextureConfiguration& iConfiguration);

private:
    virtual UObject* FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn ) override;

    virtual bool ConfigureProperties() override;

    virtual FString GetDefaultNewAssetName() const;

    virtual FName GetNewAssetThumbnailOverride() const override;

private:
    FOdysseyTextureConfiguration mTextureConfiguration;
};
