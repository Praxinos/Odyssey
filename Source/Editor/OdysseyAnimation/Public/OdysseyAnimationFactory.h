// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "Widgets/SOdysseyAnimationConfigureWindow.h"

#include "OdysseyAnimationFactory.generated.h"

UCLASS()
class ODYSSEYANIMATION_API UOdysseyAnimationFactory 
    : public UFactory
{
    GENERATED_UCLASS_BODY()

private:
    virtual UObject* FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn ) override;

    virtual bool ConfigureProperties() override;

    virtual FString GetDefaultNewAssetName() const;

private:
    FOdysseyAnimationConfiguration mConfiguration;
    bool mConfigured = false;
};
