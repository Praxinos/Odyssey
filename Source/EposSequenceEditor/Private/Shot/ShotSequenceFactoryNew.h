// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Factories/Factory.h"
#include "ShotSequenceFactoryNew.generated.h"

/**
 * Implements a factory for UShotSequence objects.
 */
UCLASS( hidecategories = Object )
class UShotSequenceFactoryNew : public UFactory
{
    GENERATED_UCLASS_BODY()

public:
    // UFactory Interface
    virtual UObject*    FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn ) override;
    virtual bool        ShouldShowInNewMenu() const override;
};
