// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Factories/Factory.h"
#include "StoryNoteFactoryNew.generated.h"

/**
 * Implements a factory for UStoryNote objects.
 */
UCLASS( hidecategories = Object )
class UStoryNoteFactoryNew : public UFactory
{
    GENERATED_UCLASS_BODY()

public:
    // UFactory Interface
    virtual UObject*    FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn ) override;
    virtual bool        ShouldShowInNewMenu() const override;
};
