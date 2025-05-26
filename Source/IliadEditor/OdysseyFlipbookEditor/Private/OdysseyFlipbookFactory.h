// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "PaperFlipbookFactory.h"

#include "OdysseyFlipbookFactory.generated.h"

UCLASS()
class UOdysseyFlipbook : public UObject
{
    GENERATED_BODY()
};

UCLASS()
class UOdysseyFlipbookFactory : public UPaperFlipbookFactory
{
    GENERATED_UCLASS_BODY()

private:
    virtual UObject* FactoryCreateNew(UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn) override;
};
