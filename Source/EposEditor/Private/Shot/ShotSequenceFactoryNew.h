// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

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
