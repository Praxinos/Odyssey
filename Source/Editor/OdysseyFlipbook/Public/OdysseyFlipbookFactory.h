// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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
