// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "PaperFlipbookFactory.h"

#include "OdysseyFlipbookFactory.generated.h"

UCLASS()
class UOdysseyFlipbookFactory : public UPaperFlipbookFactory
{
    GENERATED_UCLASS_BODY()

private:
	virtual UObject* FactoryCreateNew(UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn) override;
};
